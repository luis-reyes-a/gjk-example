#include "platform.h"
#define TARGET_FPS 60
#define DEFAULT_WINDOW_WIDTH  1920
#define DEFAULT_WINDOW_HEIGHT 1920
#define DEFAULT_MULTISAMPLE_COUNT 4

#define DEFAULT_FRAMEBUFFER_WIDTH  1920
#define DEFAULT_FRAMEBUFFER_HEIGHT 1920

#define DEBUG_WRITE_OUT_FONT_ATLASES 1
//#define DEFAULT_FRAMEBUFFER_HEIGHT 1080

static Platform *PLATFORM;
inline Platform *get_platform() {
    assert (PLATFORM);
    return PLATFORM;
}
static Render_Context *get_render_context() {
    assert (PLATFORM);
    return &PLATFORM->rcx;
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
    if (PLATFORM) {
        PLATFORM->game_memory_store = make_memory_arena(PLATFORM->entire_game_memory, sizeof(PLATFORM->entire_game_memory));
        Render_Context *rcx = &PLATFORM->rcx;
        rcx->arena = push_memory_arena(&PLATFORM->game_memory_store, 1024*1024*4);
        Temporary_Memory_Arena = push_memory_arena(&PLATFORM->game_memory_store, 1024*1024*4);
    }
    return PLATFORM;
}

static Texture_Atlas *load_font_atlas_and_metrics(Texture_Atlas_ID atlas_id, Rasterized_Font *font, String filepath, s32 fontheight);
    




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
    
    
    init_game(platform);
    Render_Context *rcx = &platform->rcx;
    
    //load fonts
    {
        String filepath = "C:/Windows/Fonts/consola.ttf"s;
        Texture_Atlas *atlas = load_font_atlas_and_metrics(TEXTURE_ATLAS_UI_FONT, &rcx->ui_font, filepath, 26);
        if (atlas) {
            assert (atlas->type == TEXTURE_8BPP);
            Opengl_Texture_Parameters params = default_texture_params(Opengl_Texture_Parameters::SINGLE_CHANNEL, atlas->width, atlas->height);
            params.pixels = atlas->pixels;
            
            GLuint gpuid;
            if (allocate_texture(params, &gpuid)) {
                atlas->loaded_on_gpu = true;
                atlas->gpuid = (u32)gpuid;
            } else {
                logprintf("Failed to load font %s onto gpu for rendering\n", filepath.str);
            }
        } else {
            logprintf("Failed to load font %s\n", filepath.str);    
        }
        
    }
    
    
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
        temp_memory_high_water_mark = max2(temp_memory_high_water_mark, Temporary_Memory_Arena.used);
        Temporary_Memory_Arena.used = 0;
        
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
        platform->frame_index += 1;
        //debug_printf("Framerate is %f", platform->dt * TARGET_FPS * 60);
    }
    
    debug_printf("~~~~~~~~~~EXITING @%llu~~~~~~~~~~\n", get_frame_index()); 
    debug_printf("Temp memory allocated was %'u\n", Temporary_Memory_Arena.size);
    debug_printf("Temp memory high water mark is %'u (%f%%)\n", temp_memory_high_water_mark, 100.0f * ((f32)temp_memory_high_water_mark / (f32)Temporary_Memory_Arena.size));
}

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "../libs/stb_truetype.h"
constexpr s32 FONT_H_OVERSAMPLE = 1; //not sure what 
constexpr s32 FONT_V_OVERSAMPLE = 1; //these are for honestly...

#if DEBUG_WRITE_OUT_FONT_ATLASES
	#define STB_IMAGE_WRITE_IMPLEMENTATION
	#define STB_IMAGE_WRITE_STATIC static
	#define STBIW_ASSERT(x) assert(x)
	#include "../libs/stb_image_write.h"
#endif

static Texture_Atlas *
load_font_atlas_and_metrics(Texture_Atlas_ID atlas_id, Rasterized_Font *font, String filepath, s32 fontheight) {
    Memory_Arena *temp = get_temp_memory();
    RESTORE_MEMORY_ARENA_ON_SCOPE_EXIT(temp);
    const unsigned char *file = (const unsigned char *)win32_open_entire_file_null_terminate(temp, filepath, 0);
    if (!file) {
        logprintf("Couldn't open %s to load font atlas\n", filepath);
        return 0;
    }
    
    Render_Context *rcx = get_render_context();
    assert (atlas_id >= 0 && atlas_id < TEXTURE_ATLAS_COUNT);
    Texture_Atlas *atlas = rcx->atlas + atlas_id;
    assert (!atlas->loaded_on_ram); //NOTE this atlas needs to be freed first, otherwise we will leak memory
    assert (!atlas->loaded_on_gpu); //NOTE this atlas needs to be freed first, otherwise we will leak memory
    memzero(atlas, sizeof (*atlas));
    memzero(font, sizeof  (*font));
    
    atlas->width  = 256;
    atlas->height = 256;
    s64 target_atlas_area = (fontheight*2)*(fontheight*2);
    //NOTE theres some bit magic you can do to make this fast, but I don't know it
    while (((s64)atlas->width*(s64)atlas->height) < target_atlas_area) {
        if (atlas->width < atlas->height) atlas->width  *= 2;
        else                             atlas->height *= 2;
    }
    
    s32 num_sprites = CHARSET_COUNT + 1;
    
    atlas->type   = TEXTURE_8BPP;
    atlas->pixels = push_size(&rcx->arena, sizeof(u8)*(u32)atlas->width*(u32)atlas->height);
    atlas->rects  = push_array(&rcx->arena, Rect2i, num_sprites);
    if (!atlas->pixels) {
        logprintf("Not enough memory to allocate atlas pixels\n");
    }
    if (!atlas->rects) {
        logprintf("Not enough memory to allocate atlas rects\n");
    }
    
    stbtt_pack_context ctx;
    if (!stbtt_PackBegin(&ctx, (unsigned char *)atlas->pixels, atlas->width, atlas->height, 0, 1, NULL)) {
        logprintf("stbtt_PackBegin failed somehow...\n");
        return 0;
    }
    stbtt_PackSetOversampling(&ctx, FONT_H_OVERSAMPLE, FONT_V_OVERSAMPLE);
    
    // NOTE we add an extra rect to pack at end to rasterize a white texture square to
    stbrp_rect stbrects[CHARSET_COUNT + 1]; 
    stbtt_packedchar chardata[CHARSET_COUNT];
    for(int j = 0; j < CHARSET_COUNT; ++j) {
        chardata[j].x0 = chardata[j].y0 = chardata[j].x1 = chardata[j].y1 = 0;    
    }
    
    
    stbtt_fontinfo info;
    info.userdata = ctx.user_allocator_context;
    stbtt_InitFont(&info, file, stbtt_GetFontOffsetForIndex(file, 0));
    
    stbtt_pack_range range_to_pass_in;
    range_to_pass_in.first_unicode_codepoint_in_range = CHARSET_START;
    range_to_pass_in.array_of_unicode_codepoints = NULL;
    range_to_pass_in.num_chars                   = CHARSET_COUNT; //NOT NUM_SPRITES, since this uses fontinfo and CHARSET_START to get glyph dimensions
    range_to_pass_in.chardata_for_range          = chardata;
    range_to_pass_in.font_size                   = (float)fontheight;
    
   //NOTE here we take every rect and set its .w and .h 
    stbtt_PackFontRangesGatherRects(&ctx, &info, &range_to_pass_in, 1, stbrects);
    stbrects[num_sprites-1].w = stbrects[num_sprites-1].h = fontheight;
    
   //NOTE these actually finds a location for the rects, lays them out
    stbtt_PackFontRangesPackRects(&ctx, stbrects, num_sprites); //NOTE we actuallya ask to pack the extra square here
    
    if (!stbtt_PackFontRangesRenderIntoRects(&ctx, &info, &range_to_pass_in, 1, stbrects)) {
        logprintf("stbtt_PackFontRangesRenderIntoRects failed to ratserize font glyphs into rect atlas->..\n");
        return 0;
    } else {
        stbrp_rect r = stbrects[num_sprites-1]; //render white square
        u8 *start = (u8 *)atlas->pixels + r.x + r.y*ctx.stride_in_bytes;
        for (s32 y = 0; y < r.h; y += 1) {
            for (s32 x = 0; x < r.w; x += 1) {
                u8 *p = start + x + y*ctx.stride_in_bytes;
                *p = 0xff;
            }
        }
    }
    
    //convert stbrects into our own rect array
    for_index_inc (s32, i, num_sprites) {
        stbrp_rect stbr = stbrects[i];
        Rect2i *dst = atlas->rects + i;
        
        if (!stbr.was_packed) {
            logprintf("Failed to pack glyph index %d (%c?)\n", i, (char)(CHARSET_START + i));
            continue;
        }
        
        dst->min.x = stbr.x;
        dst->min.y = stbr.y;
        dst->max.x = stbr.x + stbr.w;
        dst->max.y = stbr.y + stbr.h;
    }
    
    //NOTE here we get the white square is and stick it directly in the atlas struct
    //atlas->white_square_rect.min.x = stbrects[CHARSET_COUNT].x;
    //atlas->white_square_rect.min.y = stbrects[CHARSET_COUNT].y;
    //atlas->white_square_rect.max.x = stbrects[CHARSET_COUNT].x + stbrects[CHARSET_COUNT].w;
    //atlas->white_square_rect.max.y = stbrects[CHARSET_COUNT].y + stbrects[CHARSET_COUNT].h;
    
    //TODO does stbtt have option to do this automatically when rasterizing?...
    #if 1 //flip each glyph vertically
    for_index_inc(s32, i, CHARSET_COUNT) { //we don't have to flip our other textures I don't think, so we only go to CHARSET_COUNT
        stbrp_rect rect = stbrects[i];
        if (!rect.was_packed) continue;
        
        u32 stride = atlas->width;
        u8 *lower_bottom = ((u8 *)atlas->pixels) + rect.x + rect.y*stride;
        for_index_inc(s32, y, rect.h/2) {
            for_index_inc(s32, x, rect.w) {
                u8 *lower = lower_bottom + x + y*stride;
                u8 *upper = lower_bottom + x + (rect.h - y - 1)*stride;
                SWAP(*lower, *upper);
            }   
        }   
    }
    #endif
    
    stbtt_PackEnd(&ctx);
    
    #if DEBUG_WRITE_OUT_FONT_ATLASES
    String filename = get_file_base_name(filepath);
    String out_filename = push_stringf(temp, "../debug_output_%s.png", filename.str);
    stbi_flip_vertically_on_write(true);
    stbi_write_png(out_filename.str, atlas->width, atlas->height, 1, atlas->pixels, atlas->width*sizeof(u8));
    #endif
    
    atlas->loaded_on_ram = true;
    atlas->rect_count = num_sprites;
    
    //do font metrics stuff
    font->texture_atlas_id = atlas_id;
    font->pixel_height = fontheight;
    
    float scale =  stbtt_ScaleForPixelHeight(&info, (float)font->pixel_height);
    int ascender, descender, linegap;
    stbtt_GetFontVMetrics(&info, &ascender, &descender, &linegap); //unscaled coords
    
    font->descender = -(scale*descender); 
    font->ascender  =  (scale*ascender); 
    font->linegap   =  (scale*linegap);
    
    int advance, left_side_bearing;
    stbtt_GetCodepointHMetrics(&info, ' ', &advance, &left_side_bearing); //unscaled coords
    font->space_advance = (advance * scale);
    
    for (u32 codepoint = CHARSET_START; codepoint <= CHARSET_END; codepoint += 1) {
        int glyph_index = stbtt_FindGlyphIndex(&info, codepoint);
        int advance, lsb, x1, y1, x2, y2;
        stbtt_GetGlyphHMetrics(&info, glyph_index, &advance, &lsb);
        stbtt_GetGlyphBitmapBox(&info, glyph_index, scale * FONT_H_OVERSAMPLE, scale * FONT_V_OVERSAMPLE,
                                &x1,&y1,&x2,&y2);
        
      //Glyph *glyph = get_glyph(font, codepoint);
        Glyph *glyph = font->glyphs + (codepoint - CHARSET_START);
        memzero(glyph, sizeof *glyph);
        
        glyph->ascender  = (f32)-y1;
        glyph->descender = (f32)y2;
        glyph->width = (f32)(x2 - x1);
        
        glyph->advance           = advance * scale;
        glyph->left_side_bearing = lsb * scale;
        
        for(u32 codepoint_next = CHARSET_START; codepoint_next <= CHARSET_END; codepoint_next += 1)
        {
            glyph->kern[codepoint_next - CHARSET_START] = (f32)stbtt_GetGlyphKernAdvance(&info, codepoint, codepoint_next) * scale;
        }
    }
    
    return atlas;
}

#pragma pop_macro("LOGPRINTF_IDENTIFIER")