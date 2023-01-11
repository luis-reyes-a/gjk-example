#include "platform.h"
#define TARGET_FPS 60
#define DEFAULT_WINDOW_WIDTH  1920
#define DEFAULT_WINDOW_HEIGHT 1920
#define DEFAULT_MULTISAMPLE_COUNT 4

#define DEFAULT_FRAMEBUFFER_WIDTH  1920
#define DEFAULT_FRAMEBUFFER_HEIGHT 1920
//#define DEFAULT_FRAMEBUFFER_HEIGHT 1080

static Platform *PLATFORM;
inline Platform *get_platform() {
    assert (PLATFORM);
    return PLATFORM;
}
inline u64 get_frame_index() {
    return get_platform()->frame_index;
}


//TODO how does this work with dll? Is having "static thread_local Memory_Arena *Temporary_Memory_Arena" the correct way?
static thread_local Memory_Arena Temporary_Memory_Arena;

internal Memory_Arena *
get_temp_memory() {
    return &Temporary_Memory_Arena;
}

#pragma push_macro("LOGPRINTF_IDENTIFIER")
#define LOGPRINTF_IDENTIFIER "win32"

#define WIN32_INCLUDE_OpenGL 1
#include "win32.h"

//#include "ui.cpp"
//#include "debug.cpp"


//tell nvidia driver to please use it's gpu on a multigpu system...
extern "C" {
    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

internal Memory_Arena *
init_thread_temporary_memory(u32 size = 1024*1024*32) {
    void *data = win32_allocate(size); 
    if (data) {
        Temporary_Memory_Arena = make_memory_arena(data, size);
        return &Temporary_Memory_Arena;
    }
    else return null;
}


internal Platform *
init_platform() {
    PLATFORM = (Platform *)win32_allocate(sizeof(Platform));
    PLATFORM->game_memory_store = make_memory_arena(PLATFORM->entire_game_memory, sizeof(PLATFORM->entire_game_memory));
    return PLATFORM;
}

//NOTE only things game.cpp is "exporting" for this module to link to
void init_game(Platform *platform);
void update_game(Platform *platform);
void handle_input_game(User_Input *input);

int CALLBACK 
WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line_args, int show_code) {     
    Platform *platform = init_platform();
    if (!platform) {
        MessageBoxA(null, "Couldn't allocate enough memory to run the game", "ERROR", MB_OK);
        ExitProcess(0); 
    }
    
    if (!init_thread_temporary_memory()) {
        MessageBoxA(null, "Couldn't allocate enough temporary memory for main thread", "ERROR", MB_OK);
        ExitProcess(0); 
    }
    
    Win32 *win32 = make_single_window_app(instance, "Mage");
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
    
    #if DEFAULT_START_WINDOW_FULLSCREEN
    win32_toggle_fullscreen(win32->window.handle);
    #endif
    
    //TODO we need a better way of figuring this out
    
    
    //set schedular granularity so that Sleep() is more accurate
    nil sleep_is_granular = (timeBeginPeriod(1) == TIMERR_NOERROR);
    assert (sleep_is_granular); //TODO should we allow the game to run without this?
    
    setup_opengl(win32);
    init_opengl();
    
    platform->still_running = true;
    platform->startup_seed = win32_make_random_seed();
    
    
    
    //platform->rcx.font = &pak->fonts[0];
    //assert (platform->rcx.registered_cam_count == 0);
    //register_camera("screen camera default");
    
    init_game(platform);
	#if 0 && DEBUG_BUILD
    DEBUG_init_internal();
    if (OpenGL.flags & OpenGL_SUPPORTS_SRGB_FRAMEBUFFER)
        debug_printf("Opengl suports srgb framebuffers\n");
    if (OpenGL.flags & OpenGL_SUPPORTS_SRGB_TEXTURE)
        debug_printf("Opengl suports srgb textures\n");
	#endif
    
    u64 timestamps[2];
    timestamps[0] = get_timestamp();
	platform->dt = 1.0f / TARGET_FPS; //TODO do we want to keep this?
    
    u32 temp_memory_high_water_mark = 0;
    
    while (platform->still_running) {
        //TODO how will I reset temp arenas for other threads?...well threads don't operate per frame right?, They just keeping running
        defer { 
            temp_memory_high_water_mark = max2(temp_memory_high_water_mark, Temporary_Memory_Arena.used);
            Temporary_Memory_Arena.used = 0;
            platform->frame_index += 1;
        };
        //NOTE is there a better way to see if we are the foreground window?
        #if 0
        if (platform->lock_cursor_to_screen && win32->window == GetForegroundWindow())
        {
            RECT rect;
            rect.left   = rcx->output_window_region.min.x;
            rect.right  = rcx->output_window_region.max.x;
            rect.top    = rcx->output_window_region.min.y;
            rect.bottom = rcx->output_window_region.max.y;
            ClientToScreen(win32->window, (POINT *)&rect.left);  //TODO this isn't safe in case windows moves
            ClientToScreen(win32->window, (POINT *)&rect.right); //member variables around!
            ClipCursor(&rect);
        }
        #endif
        Window *window = &W32.window;
        // NOTE although we update client_width/height on default_message_handler(), we have to get up-to-date values here
        // since default_message_handler() gets called after we do the normalization (or maybe we should normalize mouse pos right before on_input() call?)
        window->client_dim = get_window_client_dim(window->handle);

        Vector2i last_mouse_pos = platform->mouse_pos;
        {
            POINT point;
            if (GetCursorPos(&point) && ScreenToClient(win32->window.handle, &point)) {
                point.y = window->client_height - point.y; //invert y so it positive is going up
                s32 new_mouse_x = point.x - 0 - 1; //TODO what was the -1 for again? Is it for the window border thickness?
                s32 new_mouse_y = point.y - 0 - 1;
                
                platform->mouse_pos.x  = new_mouse_x;
                platform->mouse_pos.y  = new_mouse_y;
                
                platform->mouse_pos_normalized.x = divide((f32)new_mouse_x, (f32)window->client_width-1,  0.0f); //NOTE -1 is so that the max pixel (dim-1) will be 1.0f
                platform->mouse_pos_normalized.y = divide((f32)new_mouse_y, (f32)window->client_height-1, 0.0f);
            }
        }
        platform->mouse_pos_delta = platform->mouse_pos - last_mouse_pos;
        
        Render_Context *rcx = &platform->rcx;
        render_context_begin_frame(rcx, window->client_width, window->client_height, platform->mouse_pos);
        handle_input_events(win32, platform, &handle_input_game);
        if (!platform->still_running) break; 
        //if (input_result.quit_program) break; //anything special we want to do?..
        
        
        //NOTE this is the client drawing area, not actually the window
        update_game(platform);
        
        HDC dc = GetDC(win32->window.handle);
        render_opengl(platform, &platform->rcx);
        
        
        //NOTE SwapBuffers only stalls this thread when it can't handle drawing more frames
        //meaning the first couple of times SwapBuffers won't wait but once it's busy it will start to sleep
        //so we still always need to have Sleep() below even with vysync enabled
        SwapBuffers(dc);
        ReleaseDC(win32->window.handle, dc);
        
        //sleep until next frame boundry, measure dt
        timestamps[1]  = get_timestamp();
        f32 elapsed = get_secs_elapsed(win32, timestamps[1], timestamps[0]);
        f32 target_dt = (1.0f / TARGET_FPS); 
        if (elapsed < target_dt) {
            DWORD msecs_left = (DWORD)((target_dt - elapsed) * 1000.0f);
            Sleep(msecs_left);
        }
        #if DEBUG_BUILD
        //else if (elapsed > target_dt) {
            //platform->debug_num_frames_passed_fps += 1;
        //}
        #endif
        timestamps[1] = get_timestamp();
        platform->dt = get_secs_elapsed(win32, timestamps[1], timestamps[0]);
        timestamps[0] = timestamps[1];
        //debug_printf("Framerate is %f", platform->dt * TARGET_FPS * 60);
    }
    
    debug_printf("~~~~~~~~~~EXITING @%llu~~~~~~~~~~\n", get_frame_index()); 
    debug_printf("Temp memory allocated was %'u\n", Temporary_Memory_Arena.size);
    debug_printf("Temp memory high water mark is %'u (%f%%)\n", temp_memory_high_water_mark, 100.0f * ((f32)temp_memory_high_water_mark / (f32)Temporary_Memory_Arena.size));
}

#pragma pop_macro("LOGPRINTF_IDENTIFIER")