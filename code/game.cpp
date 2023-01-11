#include "platform.h"

static Platform *PLATFORM;
inline Platform *get_platform() {
    assert (PLATFORM);
    return PLATFORM;
}

void init_game(Platform *platform) {
    PLATFORM = platform;
}
void update_game(Platform *platform) {
    platform->still_running = true;
}

void handle_input_game(User_Input *input) {
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
            }
        }
    }
}

static Memory_Arena *get_temp_memory(){
    return null;
}