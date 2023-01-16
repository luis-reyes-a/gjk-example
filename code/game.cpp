#include "platform.h"
#include "gjk.cpp"

static Platform *PLATFORM;
inline Platform *get_platform() {
    assert (PLATFORM);
    return PLATFORM;
}
inline u64 get_frame_index() {
    return PLATFORM->frame_index;
}
static Memory_Arena *get_temp_memory(){
    return get_platform()->main_thread_temp_memory;
}

void init_game(Platform *platform) {
    PLATFORM = platform;
    platform->rcx.xfov_t = 80.0f / 360.0f;
}
static Render_Context *get_render_context() {
    return &PLATFORM->rcx;
}

static void draw(Shape *shape, Vector4 col) {
    f32 thickness = 8;
    switch (shape->type) {
    case SHAPE_CIRCLE: {
        f32 inner_r = max2(0, shape->dim.x - thickness);
        draw_circle(shape->pos, shape->dim.x, col, inner_r);
    } break;
    case SHAPE_AABB: {
        draw_bounding_box(shape->pos, shape->dim.x, shape->dim.y, thickness, col);
    } break;
    case SHAPE_RECT: {
        draw_bounding_box(shape->pos, shape->dim.x, shape->dim.y, thickness, col, shape->angle_t);
    } break;
    }
}

static u32 g_seed = 0xcafebabe;
static u32 g_sel_shape = 0;

//NOTE this configuration was giving us false alerts when closest dist wasn't changing by all that much
//static Shape g_shapes[2] = {
    //{SHAPE_CIRCLE, V2(630, 474), V2(150, 0)}, 
    //{SHAPE_RECT,   V2(400, 400), V2(125, 125), 1.0f/16.0f},
//};

static Shape g_shapes[2] = {
    {SHAPE_CIRCLE, V2(821, 403), V2(150, 0)}, 
    {SHAPE_RECT,   V2(800, 800), V2(600, 600), -1.0f/16.0f},
};

static Vector4 make_random_color_v4(u32 *seed) {
    Color col;
    col.argb = get_random_u32(seed);
    col.a = 255;
    return unpack_v4(col);
}

void update_game(Platform *platform) {
    GJK_Result gjk = gjk_get_distance_apart(g_shapes[0], g_shapes[1]);
    g_shapes[1].angle_t += 0.001f;
    
    
    Vector4 col = (gjk.closest_dist <= 0) ? V4(1,0,0,1) : V4(1,1,1,1);
    draw(&g_shapes[0], col);
    draw(&g_shapes[1], col);
    
    if (gjk.closest_dist > 0) {
        Vector2 dir = normalize(gjk.closest_delta);
        Vector2 contact_point_on_s1 = get_furthest_point_generic(g_shapes[0], dir);
        draw_line_delta(contact_point_on_s1, gjk.closest_delta, 6, V4(0,1,0,1));
    } else if (gjk.closest_dist < 0) {
        Vector2 dir = normalize(gjk.closest_delta);
        Vector2 contact_point_on_s1 = get_furthest_point_generic(g_shapes[0], dir);
        draw_line_delta(contact_point_on_s1, -gjk.closest_delta, 6, V4(0,1,0,1));
    }
    
    Vector2 origin = V2(800, 800);
    draw_quad(origin, 6, 6, V4(1,0,0,1)); 
    gjk_draw_minkowski_difference(origin, g_shapes[0], g_shapes[1], 64, V4(0,0,1,1), 6);
   
    
    f32 lineheight = 24;
    Vector2 line_pos = V2(800, 600); 
    {
        String str = tprintf("Shape1 Pos: (%.3f, %.3f)", g_shapes[0].pos.x, g_shapes[0].pos.y);
        draw_string(line_pos, str, 24, V4(1,1,1,1));
        line_pos.y -= lineheight;
    }
    
    {
        String str = tprintf("Shape2 Pos: (%.3f, %.3f)", g_shapes[1].pos.x, g_shapes[1].pos.y);
        draw_string(line_pos, str, 24, V4(1,1,1,1));
        line_pos.y -= lineheight;
    }
    
    {
        String str = tprintf("Dist is %f", gjk.closest_dist);
        draw_string(line_pos, str, 24, V4(1,1,1,1));
        line_pos.y -= lineheight;
    }
    
}

void handle_input_game(User_Input *input) {
    Render_Context *rcx = &get_platform()->rcx;
    f32 cam_speed = 0.1f;
    for_unhandled_input_event (input, INPUT_FOCUS_WINDOW) {
        if (event->type == INPUT_EVENT_KEY) {
            Input_Event_Key *key = (Input_Event_Key *)event;
            switch (key->id) {
            case KEY_F4: {
                if (key->on_press && has_modifier(input, KEY_ALT)) {
                    mark_handled(input, event);
                    input->request_quit_program = true;
                }
            } break;
            
            case KEY_W: {
                if (key->is_down) {
                    mark_handled(input, event);
                    rcx->cam_pos.y += cam_speed; 
                }
            } break;
            
            case KEY_S: {
                if (key->is_down) {
                    mark_handled(input, event);
                    rcx->cam_pos.y -= cam_speed; 
                }
            } break;
            
            case KEY_D: {
                if (key->is_down) {
                    mark_handled(input, event);
                    rcx->cam_pos.x += cam_speed; 
                }
            } break;
            
            case KEY_A: {
                if (key->is_down) {
                    mark_handled(input, event);
                    rcx->cam_pos.x -= cam_speed; 
                }
            } break;
            
            case KEY_UP: {
                if (key->is_down) {
                    mark_handled(input, event);
                    rcx->cam_pos.z += cam_speed; 
                }
            } break;
            
            case KEY_DOWN: {
                if (key->is_down) {
                    mark_handled(input, event);
                    rcx->cam_pos.z -= cam_speed; 
                }
            } break;
            
            case KEY_MOUSE_LEFT: 
            case KEY_MOUSE_RIGHT: {
                if (key->is_down) {
                    mark_handled(input, event);
                    s32 index = (key->id == KEY_MOUSE_LEFT) ? 0 : 1;
                    Shape *shape = g_shapes + index;
                    shape->pos = v2(input->mouse_pos);
                }
            } break;
            
            }
        } else if (event->type == INPUT_EVENT_MOUSE_SCROLL) {
            Input_Event_Mouse_Scroll *scroll = (Input_Event_Mouse_Scroll *)event;
            mark_handled(input, event);
            
            f32 scale = 4;
            if (has_modifier(input, KEY_MOUSE_LEFT)) {
                Shape *shape = g_shapes + 0;
                shape->dim.x += scale*scroll->scroll;
                shape->dim.y += scale*scroll->scroll;
            } else if (has_modifier(input, KEY_MOUSE_LEFT)) {
                Shape *shape = g_shapes + 1;
                shape->dim.x += scale*scroll->scroll;
            }
            //rcx->xfov_t += scroll->scroll/360.0f;
        }
    }
}

