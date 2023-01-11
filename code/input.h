#ifndef INPUT_H //created on January 9th 2023
#define INPUT_H

enum Keyboard_Key_ID : u32 {
    //NOTE this ids must be in alphabetic order! don't rearrange them!!!
    KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H,
    KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P,
    KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X,
    KEY_Y, KEY_Z,
    
    KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, 
    KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
    
    KEY_F1,  KEY_F2,  KEY_F3,  KEY_F4,   KEY_F5,  KEY_F6, 
    KEY_F7,  KEY_F8,  KEY_F9,  KEY_F10,  KEY_F11, KEY_F12,
    
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
    
    KEY_SPACEBAR,
    KEY_BACKSPACE,
    KEY_DELETE,
    KEY_ENTER,
    KEY_RETURN = KEY_ENTER,
    KEY_HOME,
    KEY_END,
    KEY_PAGEUP,
    KEY_PAGEDOWN,
    KEY_BACKTICK,
    KEY_ESCAPE,
    KEY_TAB,
    KEY_ALT,
    KEY_CONTROL,
    KEY_SHIFT,
    
    KEY_FORWARDSLASH,
    KEY_BACKSLASH,
    
    KEY_COMMA,
    KEY_PERIOD,
    
    KEY_MOUSE_LEFT,
    KEY_MOUSE_MIDDLE,
    KEY_MOUSE_RIGHT,
    
    KEY_COUNT,
    KEY_INVALID = KEY_COUNT,
};

enum Input_Event_Flags : u32 {
    INPUT_EVENT_HANDLED                = (1 << 0),
    INPUT_EVENT_KEY_MOUSE_DOUBLE_CLICK = (1 << 1), //only for mouse key events
};

enum Input_Event_Type : u32 {
    INPUT_EVENT_KEY,
    INPUT_EVENT_MOUSE_POS, //NOTE gets called every frame, this event is kind of a dud, it just gives us a chance to run code in for_unhandled_input_event()
    INPUT_EVENT_MOUSE_SCROLL, //is horizontal scroll a thing?
    INPUT_EVENT_MOUSE_DELTA,  //delta mouse cursor pos change
    INPUT_EVENT_TEXT,
    INPUT_EVENT_FILE_DROP, //user drops file into window
    
    INPUT_EVENT_FOCUS_CHANGE,
    
    //these events occur "after" (after window minimized/maximized/resized), they're not requrests before they occur
    INPUT_EVENT_WINDOW_SIZE_CHANGED,
    INPUT_EVENT_PROGRAM_WILL_EXIT,
};

enum Input_Focus_Type : u32 {
    INPUT_FOCUS_WINDOW,
    INPUT_FOCUS_WINDOW_UI,
    INPUT_FOCUS_SHELL,
    //INPUT_FOCUS_EXTERNAL_PROGRAM, //NOTE keep this last, we don't make an event list for this by making arrays [INPUT_FOCUS_COUNT-1]...
    INPUT_FOCUS_COUNT,
};

struct Input_Event {
    Input_Event_Type type;
    u32 flags;
    
    Input_Event *prev_event;
    Input_Event *next_event;
};

struct Input_Event_Key : Input_Event {
    Keyboard_Key_ID id;
    bool is_down;
    bool on_press;
    bool on_release;
    bool on_release_focus_lost; //TODO get rid of this one
    bool on_autorepeat_press;
    //bool on_release_focus_lost; //don't think we'll ever care about that 
};

struct Input_Event_Mouse_Scroll : Input_Event {
    f32 scroll;
};

struct Input_Event_Mouse_Delta : Input_Event {
    union {
        Vector2i delta;
        struct {
            s32 dx; 
            s32 dy;
        };
    };
};

struct Input_Event_Mouse_Pos : Input_Event {
    Vector2i pos;
    Vector2i last_pos;
};

struct Input_Event_Text : Input_Event {
    String text;
    //we don't store length because handling keychords may remove characters, affecting the length as we're processing input
    //so we just call strlen() when we're about to handle text (it shouldn't be that long anyways...)
    //s32 length; 
};

struct Input_Event_File_Drop : Input_Event {
    String filepath;
};

struct Input_Event_Window_Size_Changed : Input_Event {
    bool was_maximized;
    bool was_minimized;
    s32 new_width;
    s32 new_height;
};

struct Input_Event_Program_Will_Exit : Input_Event {
    //nothing
};





// TODO at the moment we don't do any special guarding against user wanting to change input multiple
// times during the frame, we just assert(). As far as I can tell, changing input multiple times means
// there's a bug (i.e. UI handled multiple times in same frame with different states...)
struct Input_Event_Focus_Change : Input_Event  {
    Input_Focus_Type prev_focus;
    Input_Focus_Type new_focus;
};

//TODO can we inherit event as a struct and not per element ... is that a thing c++ can do?
union Input_Event_Data {
    Input_Event event;
    Input_Event_Key key;
    Input_Event_Mouse_Scroll mouse_scroll;
    Input_Event_Mouse_Delta  mouse_delta;
    Input_Event_Text text;
    Input_Event_File_Drop file_drop;
    Input_Event_Window_Size_Changed window_size_changed;
    Input_Event_Focus_Change input_focus_change;
};




struct User_Input {
    //Input_Focus_Type prev_focus_type;
    //Input_Focus_Type focus_to_release; //whenever a focus change occurs, this gets set and *MUST* be handled in a INPUT_EVENT_FOCUS_LOST event
    Input_Focus_Type focus_type;
    Input_Focus_Type next_focus_type;
    bool on_window_lose_focus; //specifally keyboard focus for Windows
    //bool prev_focus_handled;
    //bool set_next_input_focus_this_frame; //just to check we don't set it multiple times in a frame, becaus we don't handle that correctly
    Input_Event_Data events[64];
    
    char text_event_buffer[32];
    s32 text_event_length;
	//this must be handled every frame, otherwise old events will continue to point to the same text stream...
    //u32 text_event_flags;
    
    Input_Event event_lists[INPUT_FOCUS_COUNT];
    Input_Event free_events_list;
    
    //u64 frame_focus_lost[INPUT_FOCUS_COUNT];
    
    
    // NOTE don't use this for event handling. Use Input_Event_Key instead
    // this is just in wierd cases where you want non-traditional modifiers for key events
    //s32 modifier_count;
    Bit_Array64<KEY_COUNT> modifiers_down;
    Bit_Array64<KEY_COUNT> repeated_down; //for text handling autorepeat key messages
    
    union {
        Vector2i mouse_pos;
        struct  {
            s32 mouse_x, mouse_y;
        };
    };
    
    Vector2 mouse_pos_normalized;
    
    //what's "returned" to main loop
    bool request_quit_program; //if request is accepted, next frame a PROGRAM_WILL_EXIT event will be sent, then program exits
    bool sent_out_program_will_exit_event;
};

internal s32
get_event_index(User_Input *input, Input_Event *event) {
    Input_Event *first         = (Input_Event *)&input->events[0];
    Input_Event *last_plus_one = (Input_Event *)&input->events[countof(input->events)];
    if (event >= first && event < last_plus_one) {
        return (s32)(uintptr)(event - first);
    } else {
        return -1;
    }
}

internal Input_Event *
get_event_at_index(User_Input *input, s32 index) {
    if (index >= 0 && countof(input->events)) {
        return (Input_Event *)(input->events + index);
    } else {
        return null;
    }
}

internal void
insert_to_text_stream(User_Input *input, char c) {
    if (input->text_event_length < (countof(input->text_event_buffer)-1)) { //-1 for zero terminator
        input->text_event_buffer[input->text_event_length++] = c;
        input->text_event_buffer[input->text_event_length] = 0;
    }
}

#define for_unhandled_input_event(input, focus) \
	for (Input_Event_Iterator input_it((input), (focus)); input_it.event; input_it.get_next_event())  \
	if (Input_Event *event = input_it.event) 

struct Input_Event_Iterator { //need this so we can remove elements from list as we're iterating
    Input_Event *sentinel = null;
    Input_Event *event    = null;
    Input_Event *next     = null;
    
    Input_Event_Text text_event = {};
    bool sent_text_event_already = false;
    Input_Focus_Type focus;
    User_Input *input;
    
    Input_Event_Focus_Change focus_change_event = {};
    
    
    
    Input_Event_Iterator(User_Input *input, Input_Focus_Type focus) : input(input), focus(focus) {
        //assert (focus != INPUT_FOCUS_EXTERNAL_PROGRAM); //we should never be looking at these right?.. otherwise our focus_to_release will need an additinal boolean...
        if ((input->focus_type == focus) && (input->on_window_lose_focus ||
            (input->focus_type != input->next_focus_type))) { //NOTE so for_unhandled_input_event() gets called multiple times, we will continue getting focus change events
            focus_change_event.type = INPUT_EVENT_FOCUS_CHANGE;
            focus_change_event.prev_focus = input->focus_type;
            focus_change_event.new_focus  = input->on_window_lose_focus? INPUT_FOCUS_COUNT : input->next_focus_type; //invalid focus type
            event = &focus_change_event; //next time get_next_event() gets called, we cosume the input and ignore remaining events
            return;
        }
        #if 0
        if (focus == input->focus_to_release && (focus != INPUT_FOCUS_EXTERNAL_PROGRAM)) { 
            assert (input->focus_type != input->focus_to_release);
            focus_change_event.type = INPUT_EVENT_FOCUS_CHANGE;
            focus_change_event.prev_focus = input->focus_to_release;
            focus_change_event.new_focus  = input->focus_type;    
            event = &focus_change_event;
            return;
            //NOTE because the current focus isn't what was passed in, we should not be getting *ANY* other
            //messages so we don't bother to setup of the linked list iteration, we just return this event and go
        }
        #endif
        
        if ((focus >= 0) && (focus < countof(input->event_lists))) {
            sentinel = input->event_lists + focus;
            if (sentinel->next_event != sentinel) {
                event = sentinel->next_event;
                
                if (event->next_event != sentinel) {
                    next = event->next_event;
                }
            }
        }    
        
        
        if (!event && (focus == input->focus_type) && !sent_text_event_already &&
            (input->text_event_length > 0)) { //pseudo-inject text event as last iteration if we have text
            text_event.type = INPUT_EVENT_TEXT;
            text_event.text.str = input->text_event_buffer;
            text_event.text.length = input->text_event_length;
            text_event.text.size   = input->text_event_length;
            event = &text_event;
            sent_text_event_already = true;
        }
    }
    
    inline Input_Event *get_next_event() {
        Input_Event *next_next = null;
        if (next && (next->next_event != sentinel)) {
            next_next = next->next_event;
        }
        
        event = next;
        next  = next_next;
        
        if (!event && (focus == input->focus_type) && !sent_text_event_already &&
            (input->text_event_length > 0)) { //pseudo-inject text event as last iteration if we have text
            text_event.type = INPUT_EVENT_TEXT;
            text_event.text.str = input->text_event_buffer;
            text_event.text.length = input->text_event_length;
            text_event.text.size   = input->text_event_length;
            event = &text_event;
            sent_text_event_already = true;
        }
        return event;
    }
    
    void set_input_focus(Input_Focus_Type new_focus) {
        if (new_focus == input->focus_type) return; //nothing to do
        assert (input->next_focus_type != new_focus); //otherwise set_input_focus() was already called ...
        input->next_focus_type = new_focus;
        
        //set event and make next point to it
        focus_change_event.type = INPUT_EVENT_FOCUS_CHANGE;
        focus_change_event.prev_focus = input->focus_type;
        focus_change_event.new_focus  = new_focus; 
        assert (event);
        next = &focus_change_event; //next time get_next_event() gets called, we cosume the input and ignore remaining events
    }
};

#define SET_INPUT_FOCUS(new_focus) input_it.set_input_focus(new_focus)


internal Input_Event *
new_input_event(User_Input *input, Input_Event_Type type) {
    assert (type != INPUT_EVENT_TEXT); //never injected into stream, we get this by calling for_unhandled_input_event();
    assert (type != INPUT_EVENT_FOCUS_CHANGE);
    if (input->focus_type < 0)                            return null;
    if (input->focus_type >= countof(input->event_lists)) return null;
    
    Input_Event *freelist_sentinel = &input->free_events_list; 
    if (freelist_sentinel->next_event != freelist_sentinel) {
        Input_Event *event = freelist_sentinel->next_event;
        event->next_event->prev_event = event->prev_event; //remove from freelist
        event->prev_event->next_event = event->next_event;
        
        memzero(event, sizeof Input_Event_Data);
        event->type = type;
        
        Input_Event *sentinel = input->event_lists + input->focus_type;
        event->prev_event = sentinel->prev_event;
        event->next_event = sentinel;
        event->prev_event->next_event = event;
        event->next_event->prev_event = event;
        return event;
    }
    else {
        for_unhandled_input_event(input, input->focus_type) switch(event->type) {
        case INPUT_EVENT_KEY: {
            Input_Event_Key *key = (Input_Event_Key *)event; 
            debug_printf("Key event %c", key->id);
            if (key->on_press)    debug_printf(" (pressed)");
            if (key->is_down)     debug_printf(" (down)");
            if (key->on_release)  debug_printf(" (released)");       
            debug_printf(" [%llu]\n", get_frame_index());
            
        } break;
        case INPUT_EVENT_MOUSE_SCROLL: {
            Input_Event_Mouse_Scroll *scroll = (Input_Event_Mouse_Scroll *)event; 
            debug_printf("Scroll event %f [%llu]\n", scroll->scroll, get_frame_index());
        } break;
        case INPUT_EVENT_MOUSE_DELTA: {
            Input_Event_Mouse_Delta *delta = (Input_Event_Mouse_Delta *)event; 
            debug_printf("Mouse delta event (%d, %d) [%llu]\n", delta->dx, delta->dy, get_frame_index());
        } break;//delta mouse cursor pos change
        case INPUT_EVENT_TEXT: {
            Input_Event_Text *text = (Input_Event_Text *)event; 
            debug_printf("Text event %s [%llu]\n", text->text, get_frame_index());
        } break;
        case INPUT_EVENT_FOCUS_CHANGE: {
            Input_Event_Focus_Change *change = (Input_Event_Focus_Change *)event; 
            debug_printf("Focus changed from %u to %u  [%llu]\n", 
                         change->prev_focus, change->new_focus, get_frame_index());
        } break;
        case INPUT_EVENT_FILE_DROP: {
            Input_Event_File_Drop *drop = (Input_Event_File_Drop *)event;
            debug_printf("Event file drop %.*s [%llu]\n", drop->filepath.length, drop->filepath.str, get_frame_index());
        } break;
        case INPUT_EVENT_WINDOW_SIZE_CHANGED: {
            Input_Event_Window_Size_Changed *change = (Input_Event_Window_Size_Changed *)event; 
            debug_printf("Event window size change to (%d, %d) [%llu]\n", change->new_width, change->new_height, get_frame_index());
        } break;
        case INPUT_EVENT_PROGRAM_WILL_EXIT: {
            debug_printf("Event program will exit [%llu]\n", get_frame_index());
        } break;
        
        default: breakpoint;
        }
        
        assert (0); //should never happen
        return null;
    }
}

internal Input_Event_Key *
new_key_event(User_Input *input, Keyboard_Key_ID id) {
    assert (id >= 0 && id < KEY_COUNT);
    Input_Event_Key *key = (Input_Event_Key *)new_input_event(input, INPUT_EVENT_KEY);
    if (key) key->id = id;
    return key;
}


// NOTE this doesn't maintain the order of the input events ... I haven't hit a situation
// where we care about that yet...
inline void
mark_handled(User_Input *input, Input_Event *event) { 
    
    if (event->type == INPUT_EVENT_TEXT) { //text events are never sent into streams, there 'injected' at request site (for_unhandled_input_event)
        assert (!ptr_in_base_count(event, input->events, countof(input->events)));
        input->text_event_length = 0;
    } else if (event->type == INPUT_EVENT_FOCUS_CHANGE) {
        assert (!ptr_in_base_count(event, input->events, countof(input->events)));
        //we don't do anything, because nothing needs to happen
        //if user decides not to handle INPUT_FOCUS_CHANGE, that's their problem (should notice it)
        //input->focus_to_release = INPUT_FOCUS_EXTERNAL_PROGRAM; //actually release focus here
    } else if (event->type == INPUT_EVENT_PROGRAM_WILL_EXIT) {
        //do nothing, this event is never "handled", so that everyone who wants to read it, can
    } else if (~event->flags & INPUT_EVENT_HANDLED) {
        assert (ptr_in_base_count(event, input->events, countof(input->events)));
        event->flags |= INPUT_EVENT_HANDLED; 
        event->prev_event->next_event = event->next_event;
        event->next_event->prev_event = event->prev_event;
        
        //add to free list
        Input_Event *sentinel = &input->free_events_list;
        event->next_event = sentinel->next_event;
        event->prev_event = sentinel;
        
        event->prev_event->next_event = event;
        event->next_event->prev_event = event;
    }
}


inline nil
was_handled(Input_Event *event) { 
    return (event->flags & INPUT_EVENT_HANDLED); 
}



//internal boolint find_unhandled_textfield_input(Input_Event_Queue *queue, Textfield_Input *result);

internal void
set_modifier(User_Input *input, s32 keyid) {
    if (!input->modifiers_down.is_set(keyid)) {
        input->modifiers_down.set_bit(keyid);
        //input->modifier_count += 1;
    }
}

internal void
clear_modifier(User_Input *input, s32 keyid) {
    if (input->modifiers_down.is_set(keyid)) {
        input->modifiers_down.clear_bit(keyid);
        //input->modifier_count -= 1;
        //assert (input->modifier_count >= 0);
    }
}

internal bool
has_modifier(User_Input *input, s32 keyid) {
    return input->modifiers_down.is_set(keyid);
}






inline nil
is_mouse_double_click(Input_Event_Key *key) {
    assert (key->Input_Event::type == INPUT_EVENT_KEY);
    nil result = (key->flags & INPUT_EVENT_KEY_MOUSE_DOUBLE_CLICK);
    if_assert (result, (key->id == KEY_MOUSE_LEFT) || (key->id == KEY_MOUSE_MIDDLE) || (key->id == KEY_MOUSE_RIGHT));
    return result;
}

internal void
init_user_input(User_Input *input) {
    input->focus_type      = INPUT_FOCUS_WINDOW; //not necessary the current way we do things, but just to be robust for future changes
    input->next_focus_type = INPUT_FOCUS_WINDOW; //just to set it correctly
    
    //input->focus_to_release = INPUT_FOCUS_EXTERNAL_PROGRAM;
    for (s32 i = 0; i < countof(input->event_lists); i += 1) {
        Input_Event *sentinel = input->event_lists + i;
        sentinel->prev_event = sentinel;
        sentinel->next_event = sentinel;
    }    
    
    //setup free list
    Input_Event *sentinel = &input->free_events_list;
    sentinel->prev_event = sentinel;
    sentinel->next_event = sentinel;
    
    for_c_array_forward (data, input->events) {
        Input_Event *event = &data->event;
        event->prev_event = sentinel->prev_event;
        event->next_event = sentinel;
        
        event->prev_event->next_event = event;
        event->next_event->prev_event = event;
    }
}

internal void
reset_user_input(User_Input *input) {
    //assert (input->focus_to_release == INPUT_FOCUS_EXTERNAL_PROGRAM); //focus was never released, must be released
    //NOTE releasing these events has to occur before changing the focus, because that will 
    //set the focus_to_release, which will immediately get eaten up
    for_index_inc (s32, focus, countof(input->event_lists)) { 
        Input_Event *list = input->event_lists + focus;
        Input_Event *freelist = &input->free_events_list;
        while (list->next_event != list) {
            Input_Event *event = list->next_event;
            //remove from list
            event->prev_event->next_event = event->next_event;
            event->next_event->prev_event = event->prev_event;
            
            //add to free list
            event->next_event = freelist->next_event;
            event->prev_event = freelist;
            
            event->prev_event->next_event = event;
            event->next_event->prev_event = event; 
        }
    } 
    input->text_event_length = 0;
    input->text_event_buffer[0] = 0;
    input->on_window_lose_focus = false;
    
    if (input->next_focus_type != input->focus_type) {
        input->focus_type = input->next_focus_type;
    }
    
    if (input->request_quit_program) {
        input->request_quit_program = false;
        new_input_event(input, INPUT_EVENT_PROGRAM_WILL_EXIT); 
        input->sent_out_program_will_exit_event = true;
    }
    

    
    
    input->repeated_down.clear_all_bits(); //should always be zerored out a start of message loop
}


internal Input_Event *
find_unhandled_event(User_Input *input, Input_Focus_Type focus, Input_Event_Type type) {
    assert (type != INPUT_EVENT_KEY); //you probably want to use find_unhandled_key()
    for_unhandled_input_event (input, focus) {
        if (event->type == type) return event;
    }
    return null;
}

internal Input_Event_Key *
find_unhandled_key(Input_Focus_Type focus, User_Input *input, Keyboard_Key_ID id) {
    for_unhandled_input_event (input, focus) {
        if (event->type == INPUT_EVENT_KEY) {
            let key = (Input_Event_Key *)event;
            if (key->id == id) return key;
        }
    }
    return null;
}

internal Input_Event_Key *
find_unhandled_key_press(User_Input *input, Input_Focus_Type focus, Keyboard_Key_ID id) {
    for_unhandled_input_event (input, focus) {
        if (event->type == INPUT_EVENT_KEY) {
            let key = (Input_Event_Key *)event;
            if ((key->id == id) && key->on_press) return key;
        }
    }
    return null;
}

internal Input_Event_Key *
find_unhandled_mouse_double_click(Input_Focus_Type focus, User_Input *input, Keyboard_Key_ID id) {
    assert ((id == KEY_MOUSE_LEFT) || (id == KEY_MOUSE_MIDDLE) || (id == KEY_MOUSE_RIGHT));
    
    for_unhandled_input_event (input, focus) {
        if (event->type == INPUT_EVENT_KEY) {
            let key = (Input_Event_Key *)event;
            if ((key->id == id) && (key->flags & INPUT_EVENT_KEY_MOUSE_DOUBLE_CLICK)) return key;
        }
    }
    return null;
}

internal Input_Event_Key *
find_unhandled_key_release(User_Input *input, Input_Focus_Type focus, Keyboard_Key_ID id) {
    for_unhandled_input_event (input, focus) {
        if (event->type == INPUT_EVENT_KEY) {
            let key = (Input_Event_Key *)event;
            if ((key->id == id) && key->on_release) return key;
        }
    }
    return null;
}


#endif //INPUT_H
