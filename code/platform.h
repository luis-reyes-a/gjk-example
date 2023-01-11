#ifndef PLATFORM_H //created on January 9th 2023
#define PLATFORM_H

//TODO make these
#define fader_printf(...)   do {} while(0)
#define logprintf_(...)   do {} while(0)
#define logprintf(...)    do {} while(0)
#define debug_printf(...) do {} while(0)

#define DEBUG_BUILD 1
#include "ubiquitous.h"
#include "intrinsics.h"
#include "maths.h"
#include "strings.h"
#include "input.h"
#include "draw.h"


enum Visit_Files_Flags : u32 {
    //better name for this to not confuse with recursive flag
    VISIT_FILES_IGNORE_FOLDERS  = (1 << 0),  //NOTE ignore just means that user procedure won't be
    VISIT_FILES_IGNORE_FILES    = (1 << 1),  //called but it will still be visited if VISIT_FILES_RECURSIVE is set
    
    VISIT_FILES_RECURSIVE       = (1 << 2),
    VISIT_FILES_FOLLOW_SYMLINKS = (1 << 3), //recursive must be set as well
};

struct Visit_Files_Info {
    String directory;
    String filename;
    String filepath;
    //should we include filepath as well?
    bool is_folder;
    bool is_symlink;
    u64 filesize;
};

struct Visit_Files_Result {
    bool couldnt_find_directory;
};



struct Platform_File_Writer {
    u64 handle; //HANDLE for win32
    u32 current_offset; //cursor offset we're writing to
    u32 size_written;
    boolint valid;
};

struct Platform_File_Info {
    String filepath;
    u64 filesize;
    boolint is_directory;
    Platform_File_Info *next_in_list;
};

struct Platform_File_Group { //allocated but must be released
    SLL (Platform_File_Info, next_in_list) files;
};

struct Platform_Heap {
    uintptr handle;
};

struct Platform { 
    //stuff we send to platform
    boolint still_running;
    boolint lock_cursor_to_screen;
    boolint user_wants_to_quit_app;
    
    //stuff we get back
    u32 startup_seed; //NOTE this shouldn't be modified, you're expected to just copy it and use it
    
    f32 dt;
    //TODO render_context is too big and we prob can't leave it 
    Render_Context rcx;
    Vector2i mouse_pos_delta; //from last frame to current mouse_pos
    Vector2i mouse_pos;
    Vector2  mouse_pos_normalized; //not clamped 01
    
    //Keyboard_Input kbinput;
    //Mouse_Input mouse;
    u64 frame_index;
    u64 debug_num_frames_passed_fps;

    // TODO I don't like that one is inline and other is pointer
    // but the thing is that the platform layer wants to use temp memory as well
    // while the game gets it's own private reserve
    Memory_Arena game_memory_store; //entire memory passed to game
    
    u8 entire_game_memory[1024*1024*1024 * 1];
    
    #if 0
    void    (*toggle_fullscreen)(); //don't call this, just use macro
    
    //u32     (*gpu_allocate_texture)(s32 width, s32 height, void *data);
    void    (*logprintf_internal)(String marker, String fileloc, char *fmt, ...); //don't call this, just use macro
    char *  (*open_entire_file_null_terminate)(Memory_Arena *allocator, String filepath, u32 *out_filesize);
    //boolint     (*read_entire_file)(String filepath, void *data, uintptr size);
    u32     (*read_file)(String filepath, void *file, u32 readsize, u32 file_offset);
    boolint     (*write_entire_file)(String filepath, void *data, uintptr size);
    boolint     (*does_file_exist)(String filepath);
    
    //file writer stuff
    Platform_File_Writer (*get_file_writer)(String filepath);
    boolint                  (*write_file)(Platform_File_Writer *writer, void *data, uintptr size);
    void                 (*close_file)(Platform_File_Writer *writer);
    
    Visit_Files_Result (*visit_files)(String directory, u32 flags, void (*proc)(Visit_Files_Info *info, void *userdata), void *userdata);
        
    
    Platform_Heap (*heap_create)(uintptr size);
    void *        (*heap_allocate)(Platform_Heap heap, uintptr size);
    void          (*heap_free)(Platform_Heap heap, void *memory);
    
    //NOTE this works even if the path points to a file/folder that doesn't exist
    String (*get_absolute_filepath)(Memory_Arena *arena, String path);
    
    //of main window
    //User_Input * (*get_user_input)();
    Platform_File_Group (*get_files_in_directory_of_type)(Memory_Arena *arena, String directory, String ext, boolint recursive);
    #endif
};

#endif //PLATFORM_H
