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
    Color color;
    Sprite_ID sprite_id;
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
    
};

static Render_Context *get_render_context();

static void
render_context_begin_frame(Render_Context *rcx, s32 window_w, s32 window_h, Vector2i mouse_pos) {
    assert (rcx->arena.size > 0);
    rcx->window_dim = v2i(window_w, window_h);
    rcx->quad_count = 0;
    
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
draw_quad(Vector2 pos, f32 w, f32 h, Vector4 col, Sprite_ID sprite_id) {
    Render_Context *rcx = get_render_context();
    if (rcx->quad_count >= MAX_QUAD_COUNT) return;
    
    Textured_Quad *quad = rcx->quads + rcx->quad_count++;
    quad->pos = pos;
    quad->w = w;
    quad->h = h;
    quad->color = pack_color(col);
    quad->sprite_id = sprite_id;
    
}



#endif //DRAW_H
