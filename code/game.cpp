#include "platform.h"
#include "gjk.cpp"

enum Game_Mode {
    GAME_GJK_2D,
    GAME_GJK_3D,
    GAME_MODE_COUNT,
};

struct Game_State {
    Game_Mode mode;
    u32 seed;
    Shape shapes_2d[2];
};

static Game_State *g_game_state;
static Platform *g_platform;

inline Platform *get_platform() {
    assert (g_platform);
    return g_platform;
}

inline Game_State *get_game_state() {
    assert (g_game_state);
    return g_game_state;
}

inline u64 get_frame_index() {
    return g_platform->frame_index;
}

//TODO this function will have to change if any thread can call from it
inline Memory_Arena *get_temp_memory(){
    return get_platform()->main_thread_temp_memory;
}


void init_game(Platform *platform) {
    g_platform = platform;
    g_game_state = push_struct(&platform->game_memory_store, Game_State);
    assert (g_game_state);
    
    
    Render_Context *rcx = &platform->rcx;
    rcx->cam_pos = {0, 0, 0};
    rcx->xfov_t = 80.0f / 360.0f;
    
    g_game_state->seed = 0xcafebabe;
    g_game_state->shapes_2d[0] = {SHAPE_CIRCLE, V2(821, 403), V2(150, 0)}; 
    g_game_state->shapes_2d[1] = {SHAPE_RECT,   V2(800, 800), V2(600, 600), -1.0f/16.0f};
}
static Render_Context *get_render_context() {
    return &g_platform->rcx;
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


static Vector4 make_random_color_v4(u32 *seed) {
    Color col;
    col.argb = get_random_u32(seed);
    col.a = 255;
    return unpack_v4(col);
}

void update_game(Platform *platform) {
    Game_State *state = get_game_state();
    switch (state->mode) {
    case GAME_GJK_2D: {
        draw_quad(V2(800, 800), 400, 400, V4(1,1,1,1));
        
        Shape *shape1 = state->shapes_2d + 0;
        Shape *shape2 = state->shapes_2d + 1;
        GJK_Result gjk = gjk_get_distance_apart(*shape1, *shape2);
        shape1->angle_t += 0.001f;
        
        
        Vector4 col = (gjk.closest_dist <= 0) ? V4(1,0,0,1) : V4(1,1,1,1);
        draw(shape1, col);
        draw(shape2, col);
        
        if (gjk.closest_dist > 0) {
            Vector2 dir = normalize(gjk.closest_delta);
            Vector2 contact_point_on_s1 = get_furthest_point_generic(*shape1, dir);
            draw_line_delta(contact_point_on_s1, gjk.closest_delta, 6, V4(0,1,0,1));
        } else if (gjk.closest_dist < 0) {
            Vector2 dir = normalize(gjk.closest_delta);
            Vector2 contact_point_on_s1 = get_furthest_point_generic(*shape1, dir);
            draw_line_delta(contact_point_on_s1, -gjk.closest_delta, 6, V4(0,1,0,1));
        }
        
        Vector2 origin = V2(800, 800);
        draw_quad(origin, 6, 6, V4(1,0,0,1)); 
        gjk_draw_minkowski_difference(origin, *shape1, *shape2, 64, V4(0,0,1,1), 6);
        
        
        f32 lineheight = 24;
        Vector2 line_pos = V2(800, 600); 
        {
            String str = tprintf("Shape1 Pos: (%.3f, %.3f)", shape1->pos.x, shape1->pos.y);
            draw_string(line_pos, str, 24, V4(1,1,1,1));
            line_pos.y -= lineheight;
        }
        
        {
            String str = tprintf("Shape2 Pos: (%.3f, %.3f)", shape2->pos.x, shape2->pos.y);
            draw_string(line_pos, str, 24, V4(1,1,1,1));
            line_pos.y -= lineheight;
        }
        
        {
            String str = tprintf("Dist is %f", gjk.closest_dist);
            draw_string(line_pos, str, 24, V4(1,1,1,1));
            line_pos.y -= lineheight;
        }    
    } break;
    case GAME_GJK_3D: {
        static f32 angle_t = 0;
        Quaternion rot = quaternion_axis_angle_t(V3(1,0,0), angle_t);
        angle_t += 0.004f;
        draw_cube(V3(0,0,0), .5f, .5f, .5f, rot, V4(1,1,0,1));
        
        
        draw_line(V3(1,0,1), V3(3,0,1), 0.01f, V4(1,0,0,1));
        
        
        
        
        s32 num_axis_cubes = 20;
        f32 axis_cube_size = 0.25f;
        for (int i = 0; i < num_axis_cubes; i += 1) {
            draw_cube(v3((f32)i+1, 0, 0), axis_cube_size, axis_cube_size, axis_cube_size, V4(1,0,0,1));
            draw_cube(v3(0, (f32)i+1, 0), axis_cube_size, axis_cube_size, axis_cube_size, V4(0,1,0,1));
            draw_cube(v3(0, 0, (f32)i+1), axis_cube_size, axis_cube_size, axis_cube_size, V4(0,0,1,1));
        }    
    } break;
    }
}

static void handle_input_gjk_2d(User_Input *input) {
    Game_State *state = get_game_state();
    for_unhandled_input_event (input, INPUT_FOCUS_WINDOW) {
        if (event->type == INPUT_EVENT_KEY) {
            Input_Event_Key *key = (Input_Event_Key *)event;
            switch (key->id) {
            case KEY_MOUSE_LEFT: 
            case KEY_MOUSE_RIGHT: {
                if (key->is_down) {
                    mark_handled(input, event);
                    s32 index = (key->id == KEY_MOUSE_LEFT) ? 0 : 1;
                    Shape *shape = state->shapes_2d + index;
                    shape->pos = v2(input->mouse_pos);
                }
            } break;
            }
        } else if (event->type == INPUT_EVENT_MOUSE_SCROLL) {
            Input_Event_Mouse_Scroll *scroll = (Input_Event_Mouse_Scroll *)event;
            mark_handled(input, event);
            
            f32 scale = 4;
            if (has_modifier(input, KEY_MOUSE_LEFT)) {
                Shape *shape = state->shapes_2d + 0;
                shape->dim.x += scale*scroll->scroll;
                shape->dim.y += scale*scroll->scroll;
            } else if (has_modifier(input, KEY_MOUSE_LEFT)) {
                Shape *shape = state->shapes_2d + 1;
                shape->dim.x += scale*scroll->scroll;
            }
            //rcx->xfov_t += scroll->scroll/360.0f;
        }
    }
}

static void handle_input_gjk_3d(User_Input *input) {
    Render_Context *rcx = &get_platform()->rcx;
    f32 cam_move_speed = 0.1f;
    f32 cam_rot_speed = 0.003f;
    
    for_unhandled_input_event (input, INPUT_FOCUS_WINDOW) {
        if (event->type == INPUT_EVENT_KEY) {
            Input_Event_Key *key = (Input_Event_Key *)event;
            switch (key->id) {
            
            case KEY_Q: 
            case KEY_E: {
                if (key->is_down) {
                    mark_handled(input, event);
                    f32 sign = (key->id==KEY_E) ? 1.0f : -1.0f;
                    rcx->cam_pos.y += sign*cam_move_speed; 
                }
            } break;
            
            case KEY_W: 
            case KEY_S: {
                Vector3 cam_x, cam_y, cam_z;
                get_camera_local_axis(&cam_x, &cam_y, &cam_z);
                if (key->is_down) {
                    mark_handled(input, event);
                    f32 sign = (key->id==KEY_W) ? 1.0f : -1.0f;
                    rcx->cam_pos -= cam_z*cam_move_speed*sign; 
                }
            } break;
            
            case KEY_A: 
            case KEY_D: {
                Vector3 cam_x, cam_y, cam_z;
                get_camera_local_axis(&cam_x, &cam_y, &cam_z);
                if (key->is_down) {
                    mark_handled(input, event);
                    f32 sign = (key->id==KEY_D) ? 1.0f : -1.0f;
                    rcx->cam_pos += cam_x*cam_move_speed*sign; 
                }
            } break;
            
            case KEY_UP: 
            case KEY_DOWN: {
                if (key->is_down) {
                    mark_handled(input, event);
                    f32 sign = (key->id==KEY_UP) ? 1.0f : -1.0f;
                    rcx->cam_pitch_t += sign*cam_rot_speed;
                }
            } break;
            
            case KEY_LEFT:
            case KEY_RIGHT: {
                if (key->is_down) {
                    mark_handled(input, event);
                    f32 sign = (key->id==KEY_RIGHT) ? 1.0f : -1.0f;
                    rcx->cam_yaw_t += sign*cam_rot_speed;
                }
            } break;
            
            }
        } 
    }
}

void handle_input_game(User_Input *input) {
    Game_State *state = get_game_state();
    
    //global key bindings
    for_unhandled_input_event (input, INPUT_FOCUS_WINDOW) {
        if (event->type == INPUT_EVENT_KEY) {
            Input_Event_Key *key = (Input_Event_Key *)event;
            switch (key->id) {
            case KEY_TAB: {
                mark_handled(input, key);
                if (key->on_press) {
                    state->mode = (Game_Mode)(state->mode + 1);
                    if (state->mode == GAME_MODE_COUNT) {
                        state->mode = (Game_Mode)0;
                    }    
                }
            } break;
            case KEY_F4: {
                mark_handled(input, event);
                if (key->on_press && has_modifier(input, KEY_ALT)) {
                    input->request_quit_program = true;
                }
            } break;
            }
        }
    }
    
    switch (state->mode) {
    case GAME_GJK_2D: handle_input_gjk_2d(input); break;
    case GAME_GJK_3D: handle_input_gjk_3d(input); break;
    }
}

