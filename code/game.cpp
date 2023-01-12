#include "platform.h"

static Platform *PLATFORM;
inline Platform *get_platform() {
    assert (PLATFORM);
    return PLATFORM;
}
inline u64 get_frame_index() {
    return PLATFORM->frame_index;
}

void init_game(Platform *platform) {
    PLATFORM = platform;
    platform->rcx.xfov_t = 80.0f / 360.0f;
}
static Render_Context *get_render_context() {
    return &PLATFORM->rcx;
}
void update_game(Platform *platform) {
    platform->still_running = true;
    
    static u16 sprite = 0;
    static u32 tick = 0;
    tick += 1;
    if (tick > 15) {
        tick = 0;
        sprite += 1;
        if (sprite >= CHARSET_COUNT) sprite = 0;
    }
    draw_quad(v2(600, 600), 300, 300, V4(1,0,0,1), SPRITEID(0, sprite));
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
            }
        } else if (event->type == INPUT_EVENT_MOUSE_SCROLL) {
            Input_Event_Mouse_Scroll *scroll = (Input_Event_Mouse_Scroll *)event; 
            rcx->xfov_t += scroll->scroll/360.0f;
        }
    }
}

static Memory_Arena *get_temp_memory(){
    return null;
}