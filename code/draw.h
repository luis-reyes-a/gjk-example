#ifndef DRAW_H //created on January 9th 2023
#define DRAW_H

struct Color {
    union {
        //NOTE the order of these members will have to be flipped on big-endian
        struct { u8 a, r, g, b; };
        u32 argb;
    };
};

struct Camera2D {
    Vector2 pos;
    Vector2 view_dim;
};

enum Texture_Type : u32 {
    TEXTURE_8BPP,
   TEXTURE_ARGB,
};

struct Texture_Atlas {
    bool loaded_on_ram;
    bool loaded_on_gpu;
    u32 first_sprite_id;
    u32 width;
    u32 height;
    u32 gpuid; //if 0, never uploaded to gpu
    Texture_Type type;
    s32 rect_count;
    Rect2i *rects; 
    void *pixels;
};

constexpr s32 CHARSET_START = '!';
constexpr s32 CHARSET_END   = '~';
constexpr s32 CHARSET_COUNT = (CHARSET_END - CHARSET_START + 1);

struct Glyph {
    f32 width; //texture width, texture_height = (ascender + descender)
    f32 ascender; 
    f32 descender; //positive value
    f32 left_side_bearing; //added to advance to add some style, TODO can this just be backed into advance?
    f32 advance; //amount to move to the next glyph's edge
    f32 kern[CHARSET_COUNT]; //added to advance to space out certain pairs of characters 
};

struct Rasterized_Font {
    s32 pixel_height; //what we used to razterize font with, same as ascender + descender
    s32 texture_atlas; //index into texture atlas
    f32 ascender;    //max ascender for all glyphs
    f32 descender;    //max descender for all glyphs
    f32 linegap;
    f32 space_advance; //TODO do fonts store kerning info with ' ' chars? They shouldn't right ... 
    u32 texture_atlas_id; 
   
    Glyph glyphs[CHARSET_COUNT];
};

enum Texture_Atlas_ID : u32 {
    TEXTURE_ATLAS_UI_FONT,
    TEXTURE_ATLAS_COUNT,
};

struct Sprite_ID {
    u16 atlas_id;
    u16 offset; //sprite offset in atlas
};

constexpr Sprite_ID SPRITEID(u16 a, u16 b) {
    return {a, b};
}

constexpr s32 MAX_QUAD_COUNT = 2048;
struct Textured_Quad {
    Vector2 pos;
    f32 w, h;
    f32 rotation_t;
    Color color;
    Sprite_ID sprite_id;
};

constexpr s32 MAX_CIRCLE_COUNT = 32;
struct Draw_Circle {
    Vector2 pos;
    f32 inner_r;
    f32 outer_r;
    Color color;
};


struct Render_Context {
    Vector2i window_dim; //does not include border and title stuff. Just dimensions of window we can render into
    Camera2D screen_cam;
    Vector3 cam_pos;
    f32 xfov_t; //in degrees
    
    Rasterized_Font ui_font;
    Texture_Atlas atlas[TEXTURE_ATLAS_COUNT];
    
    Memory_Arena arena; //for textures right now
    
    s32 quad_count;
    Textured_Quad quads[MAX_QUAD_COUNT];
    
    s32 circle_count;
    Draw_Circle circles[MAX_CIRCLE_COUNT];
};

static Render_Context *get_render_context();

static void
render_context_begin_frame(Render_Context *rcx, s32 window_w, s32 window_h, Vector2i mouse_pos) {
    assert (rcx->arena.size > 0);
    rcx->window_dim = v2i(window_w, window_h);
    rcx->quad_count   = 0;
    rcx->circle_count = 0;
    
    Camera2D *screen_cam = &rcx->screen_cam;
    screen_cam->view_dim = v2(window_w, window_h);
    screen_cam->pos = 0.5f*screen_cam->view_dim;
}

inline Vector4
unpack_v4(Color color) {
    Vector4 result;
    result.a = color.a / 255.0f;
    result.r = color.r / 255.0f;
    result.g = color.g / 255.0f;
    result.b = color.b / 255.0f;
    return result;
}

inline Color
pack_color(Vector4 v) {
    Color col;
    col.r = (u8)(255.0f*clamp(v.x, 0.0f, 1.0f));
    col.g = (u8)(255.0f*clamp(v.y, 0.0f, 1.0f));
    col.b = (u8)(255.0f*clamp(v.z, 0.0f, 1.0f));
    col.a = (u8)(255.0f*clamp(v.w, 0.0f, 1.0f));
    return col;
}



static void
draw_circle(Vector2 pos, f32 outer_radius, Vector4 col, f32 inner_radius = 0) {
    assert (inner_radius < outer_radius);
    Render_Context *rcx = get_render_context();
    if (rcx->circle_count >= MAX_CIRCLE_COUNT) return;
    
    
    Draw_Circle *cir = rcx->circles + rcx->circle_count++;
    cir->pos = pos;
    cir->inner_r = inner_radius;
    cir->outer_r = outer_radius;
    cir->color = pack_color(col);
}

static void
draw_quad(Vector2 pos, f32 w, f32 h, Vector4 col, Sprite_ID sprite_id, f32 rotation_t = 0.0f) {
    Render_Context *rcx = get_render_context();
    if (rcx->quad_count >= MAX_QUAD_COUNT) return;
    
    Textured_Quad *quad = rcx->quads + rcx->quad_count++;
    quad->pos = pos;
    quad->w = w;
    quad->h = h;
    quad->rotation_t = rotation_t;
    quad->color = pack_color(col);
    quad->sprite_id = sprite_id;
}

static void draw_quad(Vector2 pos, f32 w, f32 h, Vector4 col, f32 rotation_t = 0.0f) {
    Sprite_ID sprite_id = {};
    sprite_id.atlas_id = 0;
    sprite_id.offset = CHARSET_COUNT; 
    draw_quad(pos, w, h, col, sprite_id, rotation_t);
}

static void draw_line(Vector2 l1, Vector2 l2, f32 thickness, Vector4 col) {
    Vector2 dir = l2 - l1;
    if (normsq(dir) < 0.001f) return;
    
    f32 angle_t = atan2(dir.y, dir.x) / TAU;
    
    Vector2 pos = l1 + 0.5f*dir;
    draw_quad(pos, norm(dir), thickness, col, angle_t);
}

static void draw_line_delta(Vector2 l1, Vector2 delta, f32 thickness, Vector4 col) {
    draw_line(l1, l1 + delta, thickness, col);
}

static void draw_bounding_box(Vector2 pos, f32 w, f32 h, f32 thickness, Vector4 col, f32 rotation_t = 0.0f) {
    Vector2 half_x_basis = eit2(rotation_t);
    Vector2 half_y_basis = perp(half_x_basis);
    half_x_basis *= 0.5f*(w + thickness);
    half_y_basis *= 0.5f*(h + thickness);
    
    Vector2 p;
    
    p = pos + half_x_basis;
    draw_quad(p, thickness, h+(2*thickness), col, rotation_t);
    
    p = pos - half_x_basis;
    draw_quad(p, thickness, h+(2*thickness), col, rotation_t);
    
    //p = pos + half_y_basis;
    //draw_quad(p, thickness, w+(2*thickness), col, rotation_t + 0.25f);
    
    //p = pos - half_y_basis;
    //draw_quad(p, thickness, w+(2*thickness), col, rotation_t + 0.25f);
    
    p = pos + half_y_basis;
    draw_quad(p, w+(2*thickness), thickness, col, rotation_t);
    
    p = pos - half_y_basis;
    draw_quad(p, w+(2*thickness), thickness, col, rotation_t);
}

static Glyph *
get_glyph(Rasterized_Font *font, u32 codepoint) {
    Glyph *glyph = 0;
    if (codepoint >= CHARSET_START && codepoint <= CHARSET_END) {
        glyph = font->glyphs + codepoint - CHARSET_START;
    }
    return glyph;
}

static void
draw_string(Vector2 start_pos, String str, f32 lineheight, Vector4 col) {
    Render_Context *rcx = get_render_context();
    Rasterized_Font *font = &rcx->ui_font;
    
    Vector2 at = start_pos;
    f32 scale = (lineheight / (f32)font->pixel_height); //amount to scale all glyphs by to fit lineheight
    for (int i = 0; i < str.length; i += 1) {
        char c = str.str[i];
        if (c == ' ') {
            at.x += scale*font->space_advance;
            continue;
        } else if ((c < CHARSET_START) || (c > CHARSET_END)) {
            //can't render these chars
            f32 space_width = scale*font->space_advance;
            Vector2 p = {at.x + space_width*0.5f, at.y + space_width*0.5f};
            
            Sprite_ID sprite_id = {};
            sprite_id.atlas_id = 0;
            sprite_id.offset = CHARSET_COUNT; 
            draw_quad(p, space_width, space_width, col, sprite_id);
            
            at.x += space_width;
            continue;    
        }
        
        Glyph *glyph = get_glyph(font, c);
        f32 lsb = glyph->left_side_bearing * scale;
        f32 w = glyph->width  * scale;
        f32 ascender  = glyph->ascender  * scale; 
        f32 descender = glyph->descender * scale;
        
        f32 advance = glyph->advance;
        if ((i + 1) < str.length) {
            char c2 = str.str[i+1];
            if ((c2 >= CHARSET_START) && (c2 <= CHARSET_END)) {
                advance += glyph->kern[c2 - CHARSET_START];
            }
        }
        advance *= scale;
        
        Vector2 p = at;
        p.x += lsb + 0.5f*w;
        f32 ymin = p.y - descender;
        f32 ymax = p.y + ascender;
        p.y = ymin + 0.5f*(ymax - ymin);
        
        Sprite_ID sprite_id = {};
        sprite_id.atlas_id = 0;
        sprite_id.offset = c - CHARSET_START; //sprite offset in atlas
        draw_quad(p, w, ymax-ymin, col, sprite_id);
        
        at.x += advance;
    }
}



#endif //DRAW_H
