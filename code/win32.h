#ifndef WIN32_H //Created on October 18th 2020 8:27 pm
#define WIN32_H


#include "windows.h" //NOTE this includes string.h and bunch of other stuff probably
//#include "shellapi.h" //NOTE for drag and drop files

//WORD/SHORT 16 bits, DWORD 32bits

#define fatalprintf(fmt, ...) fatalprintf_internal(UNIQUE_FILE_LOCATION_STRING, fmt, __VA_ARGS__)
#define DEBUG_PRINTF_TO_DEBUGGER_OUTPUT_WINDOW_ONLY true




//TODO clean up this file, maybe break it up into smaller files
//there are some utiltiies here that only work with platform.h included
//and others that only work with graphic windows, 
//the xbox controller stuff should be in both windows and platform
//but now it's only tied to platform... 

#include "xinput.h"

struct Window {
    HWND handle;
    User_Input user_input;

    union {
        Vector2i client_dim;
        struct {
            s32 client_width;
            s32 client_height; 
        }; 
    };
    
    bool is_resizing;
    bool is_maximized;
    bool is_minimized;
};

struct Win32 {
    Window window;
    u64 query_performance_frequency;
    
    //when scaling the window, this aspect ratio will be retained. 
    //if set to zero, window is resized freely as normal
    f32 forced_window_aspect_ratio;
    //nil user_wants_to_quit_app;
    char exe_directory_buffer[MAX_PATH];
    String directory;
    
    Date_Time startup_date_time;
    HANDLE log_file_handle;
    //u32 log_right_alignment_offset;
};

global Win32 W32;





internal void
win32_check_last_error() {
    DWORD error = GetLastError();
    if (error != ERROR_SUCCESS) breakpoint;
}

internal int
fprintf_valist(HANDLE output, char *fmt, va_list list) {
    assert (output != INVALID_HANDLE_VALUE);
    assert (output);
    char buffer[8192];
    int length = stbsp_vsnprintf(buffer, sizeof buffer, fmt, list);
    buffer[length] = 0;
    if (!WriteConsole(output, buffer, length, 0, NULL))
    {
        DWORD error = GetLastError();
        assert (0);
    }
    
    return length;
}

internal void
win32_logprintf_internal(String marker, String fileloc, char *fmt, ...) {
    static boolint init = false;
    if (!init) {
        init = true;
        Date_Time time = W32.startup_date_time;
        
        RESTORE_MEMORY_ARENA_ON_SCOPE_EXIT(&Temporary_Memory_Arena);
        //String log_filename = push_stringf(&Temporary_Memory_Arena, "%s_%d.log",  get_month_string(time.month).str, time.day);
        String log_filename = push_stringf(&Temporary_Memory_Arena, "%02d-%02d-%d.log", time.month, time.day, time.year);
        String log_filepath = push_stringf(&Temporary_Memory_Arena, "../logs/%s", log_filename.str);
        W32.log_file_handle = CreateFileA(log_filepath.str, GENERIC_WRITE, FILE_SHARE_READ, NULL, 
                                          OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (W32.log_file_handle == INVALID_HANDLE_VALUE) {
            String error_msg = push_stringf(&Temporary_Memory_Arena, "Could not create log file %s. Abort?", log_filename.str);
            int ret = MessageBoxA(NULL, error_msg.str, "Error...", MB_YESNO);
            if (ret == IDYES) ExitProcess(1);
            W32.log_file_handle = 0;
        } else {
            SetFilePointer(W32.log_file_handle, 0, 0, FILE_END);
            logprintf_(""s, "---------------Log output starting at %02d:%02d:%02d---------------\n", time.hour, time.min, time.sec); 
        }
    }
    
    if (!W32.log_file_handle)	return;
    
    
    
    RESTORE_MEMORY_ARENA_ON_SCOPE_EXIT(&Temporary_Memory_Arena);
    String string = push_string(&Temporary_Memory_Arena, 4096);
    
    let pad_with_spaces_to_multiple = [&string](s32 multiple) {
        s32 remainder = string.length % multiple;
        if (remainder != 0) {
            s32 num_extra_spaces = multiple - remainder;
            appendf(&string, "%.*s", num_extra_spaces, "                                                                                                                                  ");    
        }
    };
    
    if (marker.length > 0) {
        appendf(&string, "[%.*s] ", marker.length, marker.str); 
        pad_with_spaces_to_multiple(4);
    }
    
    va_list list;
    va_start(list, fmt);
    append_valist(&string, fmt, list);
    va_end(list);
    
    trim_right_whitespace(&string);
    
    #if 0
    //set right adjustment if it goes far out, or decrease it if we go inwards too much
    if (builder.current_string_length > W32.log_right_alignment_offset)
        W32.log_right_alignment_offset = builder.current_string_length;
    else if (builder.current_string_length <= (W32.log_right_alignment_offset - 16))
        W32.log_right_alignment_offset = builder.current_string_length;
    
    
    if (builder.current_string_length < W32.log_right_alignment_offset) {
        u32 amount_to_add = W32.log_right_alignment_offset - builder.current_string_length;
        append(&builder, "                                                                                                          ", amount_to_add);
    }
    
    appendf(&builder, "   ...%s\n", fileloc.str);
    String to_write = mkstr(&builder);
    
    DWORD bytes_written;
    WriteFile(W32.log_file_handle, to_write.str, to_write.length, &bytes_written, NULL);
    #else 
    pad_with_spaces_to_multiple(4);
    appendf(&string, " @%s\n", fileloc.str);
    
    DWORD bytes_written;
    WriteFile(W32.log_file_handle, string.str, string.length, &bytes_written, NULL);
    #endif
}


internal void
fatalprintf_internal(String fileloc, char *fmt, ...)
{
    LOCAL_STRING_BUILDER(builder, 1024);
    va_list list;
    va_start(list, fmt);
    append_valist(&builder, fmt, list);
    va_end(list);
    s32 normal_message_length = builder.current_string_length;
    append(&builder, "\t");
    append(&builder, fileloc);
    String text = mkstr(&builder);
    
    HANDLE error_handle = CreateFileA("fatal_error.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, 
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (error_handle != INVALID_HANDLE_VALUE)
    {
        DWORD bytes_written;
        WriteFile(error_handle, text.str, text.length, &bytes_written, NULL);
    }
    
    text.length = normal_message_length;
    text.str[text.length] = 0;
    if (!MessageBoxA(W32.window.handle, text.str, "Fatal Error...", MB_OK))
    {
        DWORD error = GetLastError();
        breakpoint;
    }
    
    ExitProcess(1);
}

internal nil
win32_write_entire_file(String filepath, void *data, uintptr size) {
    nil successful_write = false;
    assert (is_null_terminated(filepath)); //null terminated
    HANDLE handle = CreateFileA(filepath.str, GENERIC_WRITE, FILE_SHARE_READ, NULL, 
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (handle != INVALID_HANDLE_VALUE)
    {
        DWORD num_bytes_written;
        DWORD size32 = to_u32(size);
        BOOL ok = WriteFile(handle, data, size32, &num_bytes_written, NULL);
        successful_write = (ok && (num_bytes_written == size32));
        if (!successful_write)
            logprintf("WriteFile() failed when writing_entire_file() for %s", filepath.str);
        CloseHandle(handle);
    }
    else logprintf("Could not create file handle when writing file to path %s", filepath.str);
    return successful_write;
}



internal Platform_File_Writer
win32_get_platform_file_writer(String filepath)
{
    assert (is_null_terminated(filepath)); //null terminated
    Platform_File_Writer writer = {};
    nil successful_write = false;
    writer.handle = (u64)CreateFileA(filepath.str, GENERIC_WRITE, FILE_SHARE_READ, NULL, 
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    writer.valid = (HANDLE)writer.handle != INVALID_HANDLE_VALUE;
    return writer;
}

internal nil
win32_write_file(Platform_File_Writer *writer, void *data, uintptr size)
{
    assert (writer->valid);
    assert ((HANDLE)writer->handle != INVALID_HANDLE_VALUE);
    
    DWORD num_bytes_written;
    DWORD size32 = to_u32(size);
    BOOL ok = WriteFile((HANDLE)writer->handle, data, size32, &num_bytes_written, NULL);
    nil successful_write = (ok && (num_bytes_written == size32));
    if (!successful_write)
        logprintf("WriteFile() failed when writing_entire_file()");
    if (ok)
        writer->current_offset += num_bytes_written;
    return successful_write;
}

internal void
win32_close_file(Platform_File_Writer *writer)
{
    assert (writer->valid);
    assert ((HANDLE)writer->handle != INVALID_HANDLE_VALUE);
    CloseHandle((HANDLE)writer->handle);
    *writer = {};
}

internal Platform_File_Group
win32_get_files_in_directory_of_type(Memory_Arena *arena, String directory, String ext, nil recursive)
{
    Platform_File_Group group = {};
    assert (recursive == false); //TODO support this
    
    String wildcard = push_stringf(arena, "%s/*.%s", directory.str, ext.str);
    
    WIN32_FIND_DATAA ffd;
    HANDLE handle = FindFirstFileA(wildcard.str, &ffd);
    while (handle != INVALID_HANDLE_VALUE)
    {
        Platform_File_Info *info = push_struct(arena, Platform_File_Info);
        if (!info) break;
        
        info->filepath = push_stringf(arena, "%s/%s", directory.str, ffd.cFileName);
        info->filesize = ((u64)ffd.nFileSizeHigh << (u64)32) | ((u64)ffd.nFileSizeLow);
        info->is_directory = (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
        
        group.files.add_head(info);
        
        if (!FindNextFileA(handle, &ffd)) break;
    }
    FindClose(handle);
    return group;
}

internal String
win32_get_absolute_filepath(Memory_Arena *arena, String path) {
    assert (is_null_terminated(path));
    DWORD size_needed = GetFullPathNameA(path.str, 0, 0, NULL);
    
    String result = {};
    if (size_needed > 0) {
        char *str = push_array(arena, char, size_needed);
        if (str) {
            GetFullPathNameA(path.str, size_needed, str, NULL);
            result.str = str;
            result.length = size_needed-1;
            result.size = size_needed;
            assert (result.length > 0);
        }
    }
    return result;
}


// NOTE windows doesn't accept '~/' to mean home directory (who uses that anymore?)
// not sure if we care to add logic to support that...
internal Visit_Files_Result
win32_visit_files(String first_directory, u32 flags, 
                  void (*proc)(Visit_Files_Info *info, void *userdata), void *userdata = null) {
    Visit_Files_Result result = {};
    
    struct Directory : String {
        Directory *prev;
        Directory *next;
    };
    
    Memory_Arena *scratch = &Temporary_Memory_Arena;
    RESTORE_MEMORY_ARENA_ON_SCOPE_EXIT(scratch);
    
    Directory sentinel = {};
    sentinel.prev = &sentinel;
    sentinel.next = &sentinel;
    
    //TODO get more error information back from this 
    let add_directory = [&](String str) {
        assert (str.length > 0);
        Directory *dir = push_struct(scratch, Directory);
        if (dir) {
            *dir = {}; 
        
            //NOTE bc of msvc nonsense we have to put an asterik here to actually iterate any files
        	//this is why we remove this '*' right after calling FindFirstFileA();
            
            //NOTE we're trying to establish a convention that all directories must end with forward slashes
            char *end = "*";
            if ((str.str[str.length-1] != '/') && 
                str.str[str.length-1] != '\\') then end = "/*";
                
            *(String *)dir = push_stringf(scratch, "%.*s%s", str.length, str.str, end);
            if (dir->str) {
                dir->prev = sentinel.prev;
                dir->next = &sentinel;
                
                dir->prev->next = dir;
                dir->next->prev = dir;    
            }
        }
    };
    
    let pop_directory = [&]() -> Directory * {
        if (sentinel.next != &sentinel) {
            Directory *dir = sentinel.next; 
            dir->prev->next = dir->next;
            dir->next->prev = dir->prev;
            dir->prev = dir->next = null;
            return dir;
        }
        else return null;
    };
    
    add_directory((first_directory.length > 0) ? first_directory : "./"s);
    
    while (Directory *dir = pop_directory()) {
        WIN32_FIND_DATAA ffd;
        HANDLE handle = FindFirstFileA(dir->str, &ffd); 
        if (handle == INVALID_HANDLE_VALUE) {
            result.couldnt_find_directory = true;
            continue;
        }
        
        dir->str[--dir->length] = 0;
        
        do {
        	//info->filepath = push_stringf(arena, "%s/%s", directory.str, ffd.cFileName);
            Visit_Files_Info info = {};
            info.is_folder  = (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
            info.is_symlink = (ffd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT);
            if (info.is_folder) {
                //NOTE for directories cFileName doesn't not contain a trailing slash.
                if (flags & VISIT_FILES_RECURSIVE) {
                    bool okay_to_add = true;
                    if (info.is_symlink && !(flags & VISIT_FILES_FOLLOW_SYMLINKS)) 
                        okay_to_add = false;
                    
                    if (okay_to_add) {
                        String folder_name = mkstr(ffd.cFileName);
                        if (!strmatch(folder_name, "."s) && !strmatch(folder_name, ".."s)) {
                            String folder_dir = push_stringf(scratch, "%s%s/", dir->str, ffd.cFileName);
                            add_directory(folder_dir);    
                        }     
                    }
                }
                if (flags & VISIT_FILES_IGNORE_FOLDERS) continue;
            }
            else {
                if (flags & VISIT_FILES_IGNORE_FILES) continue;
            }
            
            info.filesize = ((u64)ffd.nFileSizeHigh << (u64)32) | ((u64)ffd.nFileSizeLow);
            info.directory = *(String *)dir;
            
            let scratch_mark = get_save_point(scratch);
            
            info.filename = duplicate(scratch, ffd.cFileName); 
            info.filepath = push_stringf(scratch, "%s%s", dir->str, ffd.cFileName);
            proc(&info, userdata);
            
            restore(scratch, scratch_mark); 
            
        } while (FindNextFileA(handle, &ffd));
        FindClose(handle);
    }
    return result;
}


internal nil
win32_does_file_exist(String filepath)
{
    //NOTE we could ust PathFileExistsA, but I didn't want to includ shlwapi.lib just for that
    assert (is_null_terminated(filepath));
    nil exits = false;
    WIN32_FIND_DATAA ffd;
    HANDLE handle = FindFirstFileA(filepath.str, &ffd);
    if (handle != INVALID_HANDLE_VALUE)
    {
        exits = true;
        FindClose(handle);
    }
    return exits;
}

internal void
output(HANDLE handle, char *buffer, s32 buffer_size)
{
    DWORD bytes_written;
    BOOL ok = WriteFile(handle, buffer, buffer_size, &bytes_written, NULL);
    assert (ok);
    assert (bytes_written == (DWORD)buffer_size);
}

internal void
outputf(HANDLE handle, char *fmt, ...)
{
    assert (handle != INVALID_HANDLE_VALUE);
    va_list list;
    va_start(list, fmt);
    
    #if 1
    char buffer[8192];
    int buffer_count = stbsp_vsnprintf(buffer, sizeof buffer, fmt, list);
    
    DWORD bytes_written;
    BOOL ok = WriteFile(handle, buffer, buffer_count, &bytes_written, NULL);
    assert (ok);
    assert (bytes_written == (DWORD)buffer_count);
    #else
    persistent HANDLE STDOUT = GetStdHandle(STD_OUTPUT_HANDLE);
    fprintf_valist(STDOUT, fmt, list);
    #endif
    
    va_end(list);
}

internal void
win32_debug_printf_valist_internal(char *fmt, va_list list)
{
    LOCAL_STRING(string, 1024);
    append_valist(&string, fmt, list);
    //remove_trailing_whitespace(&builder);
    //append(&builder, "\n");
    
    #if DEBUG_PRINTF_TO_DEBUGGER_OUTPUT_WINDOW_ONLY
    OutputDebugStringA(string.str);
    #else
    
    static nil init_console_already = false;
    static HANDLE stdout = NULL;
    if (!init_console_already)
    {
        init_console_already = true;
        AllocConsole(); 
        stdout = GetStdHandle(STD_OUTPUT_HANDLE);
        SetForegroundWindow(win32->window); //AllocConsole make console foreground
        
        DWORD mode;
        if (GetConsoleMode(stdout, &mode))
        {
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(win32->stdout, mode);
        }
    }
    
    if (stdout)
    {
        DWORD num_characters_written;
        WriteConsoleA(stdout, string.str, string.length, &num_characters_written, NULL);    
    }
    #endif
}

internal void
win32_debug_printf_internal(char *fmt, ...)
{
    va_list list;
    va_start(list, fmt);
    win32_debug_printf_valist_internal(fmt, list);
    va_end(list);
}








internal void
printf(char *fmt, ...)
{
    static HANDLE STDOUT = GetStdHandle(STD_OUTPUT_HANDLE);
    assert (STDOUT);
    
    va_list list;
    va_start(list, fmt);
    fprintf_valist(STDOUT, fmt, list);
    va_end(list);
}

internal void
errorf(char *fmt, ...)
{
    static HANDLE STDOUT = GetStdHandle(STD_ERROR_HANDLE);
    assert (STDOUT);
    
    va_list list;
    va_start(list, fmt);
    fprintf_valist(STDOUT, fmt, list);
    va_end(list);
}


internal void logf(char *fmt, ...);
//TODO this causes hiccups when playing the game
//TODO maybe do this async? or just build something better for yourself!
internal void
logf_valist(char *fmt, va_list list)
{
    static HWND notepad = 0;
    static HWND edit = 0;
    if (!notepad)   notepad = FindWindowA(NULL, "log.txt - Notepad");
    if (notepad && !edit)
    {
        edit = FindWindowExA(notepad, NULL, "Edit", NULL);
        if (edit)
        {
            SYSTEMTIME systime;
            GetLocalTime(&systime);
            logf("LOG - %s %d, %d %d::%d", get_month_string(systime.wMonth), systime.wDay, systime.wYear, 
                systime.wHour, systime.wMinute);
            logf("\n================================================================================\n\n");         
        }
    }  
    
    if (edit)
    {
        char buffer[4096];
        int length = stbsp_vsnprintf(buffer, sizeof buffer, fmt, list);
        #if 0
        for(s32 i = 0; i < length; i += 1)
        {
            SendMessage(edit, WM_CHAR, buffer[i], 0);
            //         SendMessage(DEBUG_LOG_OUTPUT_HANDLE, WM_CHAR, buffer[i], 1 << 31);
        }
        #else
        SendMessage(edit, EM_REPLACESEL, true, (LPARAM)buffer);
        #endif
    }   
}

internal void
logf(char *fmt, ...)
{
    va_list list;
    va_start(list, fmt);
    logf_valist(fmt, list);
    va_end(list);      
}

internal void 
win32_toggle_fullscreen(HWND window)
{
    //NOTE this was taken from a blog by Raymond Chen, thanks!!!
    //NOTE this will only work with ONE window, because placement below stores window state
    static WINDOWPLACEMENT placement = { sizeof(placement) };
    
    DWORD style = GetWindowLong(window, GWL_STYLE);
    if (style & WS_OVERLAPPEDWINDOW) 
    {
        //make window full screen
        MONITORINFO info = { sizeof(info) };
        if (GetWindowPlacement(window, &placement) &&
            GetMonitorInfo(MonitorFromWindow(window,
                    MONITOR_DEFAULTTOPRIMARY), &info)) 
        {
            SetWindowLong(window, GWL_STYLE,
                style & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(window, HWND_TOP,
                info.rcMonitor.left, info.rcMonitor.top,
                info.rcMonitor.right - info.rcMonitor.left,
                info.rcMonitor.bottom - info.rcMonitor.top,
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    } 
    else 
    {
        //restore to previous placement
        SetWindowLong(window, GWL_STYLE,
            style | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(window, &placement);
        SetWindowPos(window, NULL, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

internal void 
win32_toggle_fullscreen() {
    win32_toggle_fullscreen(W32.window.handle);
}

internal Vector2i
get_window_client_dim(HWND window) {
    RECT rect;
    GetClientRect(window, &rect);
    return v2i(rect.right, rect.bottom);
}

internal Vector2i
get_monitor_dim()
{
    RECT rect;
    GetWindowRect(GetDesktopWindow(), &rect);
    return v2i(rect.right, rect.bottom);
}

internal LRESULT CALLBACK //windows calls us here whenever an event below occurs
default_message_handler(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam) {
    //NOTE typedef UINT_PTR WPARAM, basically unsigned size_t;
    //NOTE typedef LONG_PTR LPARAM, int64 on x64, long on x32 ... for somereason;
    Platform *platform = get_platform();
    //NOTE this is lame. Basically windows will call us here in the middle of CreateWindowEX so I can't initialize windows here as we get startup message (have to poll after)
    Window *window = &W32.window;
    if_assert (window->handle, window->handle == window_handle);
    
    User_Input *input = &window->user_input;
    
    LRESULT result = 0; //NOTE is this a default value this should be set to?..
    switch(message) {
    case WM_CLOSE: {
        //NOTE in multi-window programs, we would wait until all windows are closed to terminated
        //but where single window so we just do this
        new_input_event(input, INPUT_EVENT_PROGRAM_WILL_EXIT);
        input->sent_out_program_will_exit_event = true; 
    } break;
    
    case WM_KILLFOCUS: {
        //set_next_input_focus(input, INPUT_FOCUS_EXTERNAL_PROGRAM);
        input->on_window_lose_focus = true;
        input->modifiers_down.clear_all_bits();
    } break;
    
    case WM_SETFOCUS: {
    } break;
    
    //NOTE this runs whenever user is holding onto the frames of the window regardless if the size had been changed or not
    
    case WM_SIZING: {
        result = DefWindowProcA(window_handle, message, wparam, lparam); 
    } break;
    
    #if 0 //TODO
    case WM_SIZE: {
        //BOOL zoomed = IsZoomed(window_handle);
        window->client_width  = (lparam >>  0) & 0xffff;
        window->client_height = (lparam >> 16) & 0xffff;
        platform->rcx.window_dim.x = window->client_width;
        platform->rcx.window_dim.y = window->client_height;
        debug_printf("WM_SIZE updated rcx window dim (%d, %d)\n", platform->rcx.window_dim.x, platform->rcx.window_dim.y);
        window->is_resizing = (wparam == SIZE_RESTORED);
        
        let new_size_changed_event = [&]() {
            let event = (Input_Event_Window_Size_Changed *)new_input_event(input, INPUT_EVENT_WINDOW_SIZE_CHANGED);
            if (event) {
                event->new_width  = window->client_width;
                event->new_height = window->client_height;     
            }
            return event;
        };
        
        if (window->is_maximized) {
            if (wparam != SIZE_MAXIMIZED) { //I don't think we can get this event if we're maximized... but just in case...
                window->is_maximized = false;
                let event = new_size_changed_event();
                if (event) {
                    event->was_minimized = (wparam == SIZE_MINIMIZED);
                    window->is_minimized = event->was_minimized;     
                }
            }
        } else if (window->is_minimized) {
            if (wparam != SIZE_MINIMIZED) { //I don't think we can get this event if we're maximized... but just in case...
                window->is_minimized = false;
                let event = new_size_changed_event();
                if (event) {
                    event->was_maximized = (wparam == SIZE_MAXIMIZED);
                    window->is_maximized = event->was_maximized;    
                }
            }
        } else {
            if (wparam == SIZE_MAXIMIZED) {
                window->is_maximized = true;
                let event = new_size_changed_event();
                event->was_maximized = true;    
            } else if (wparam == SIZE_MINIMIZED) {
                window->is_minimized = true;
                let event = new_size_changed_event();
                if (event) {
                    assert (event->new_width  == 0);
                    assert (event->new_height == 0);
                    event->was_minimized = true;     
                }
            }
        }
        
        result = DefWindowProcA(window_handle, message, wparam, lparam); 
    } break;
    #endif
    case WM_EXITSIZEMOVE: {
        if (window->is_resizing) {
            window->is_resizing = false;
            //if (window->handle) debug_printf("exited %llu\n", __rdtsc());
            let event = (Input_Event_Window_Size_Changed *)new_input_event(input, INPUT_EVENT_WINDOW_SIZE_CHANGED);
            if (event) {
                event->new_width  = window->client_width;
                event->new_height = window->client_height;    
            }
        }
        result = DefWindowProcA(window_handle, message, wparam, lparam); 
    } break;
    
 
    default:
    result = DefWindowProcA(window_handle, message, wparam, lparam); 
    }
    
    return result;
}


/* NOTE why did I have this here again?
internal void
copy_string_to_clipboard(Win32 *win32, char *string, s32 length)
{
   if (OpenClipboard(win32->window) && EmptyClipboard())
   {
      char *to_write = (char *)GlobalLock(win32->clipboard_buffer);
      if (to_write)
      {
         s32 safe_length = MIN(length, win32->clipboard_buffer_size - 1);
         copy_memory(to_write, string, safe_length);
         to_write[safe_length] = 0;
         SetClipboardData(CF_TEXT, to_write);
         GlobalUnlock(win32->clipboard_buffer);
         CloseClipboard();                  
      }
      
   }
} 
*/



internal void *
win32_allocate(uintptr size)
{
    //MEM_RESERVE just reserves a region of VIRTUAL memory
    //MEM_COMMIT actually maps physical pages to that region, returns zeroed pages
    //MEM_RESET AND MEM_RESET_UNDO do NOT zero out memory again, they serve to LOCK
    //the memory ranges but it doesn't actually decommit and commit again
    void *result = VirtualAlloc(0, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    return result;
}

internal Platform_Heap 
win32_heap_create(uintptr size)
{
    Platform_Heap heap = {};
    u32 flags = 0;
    SIZE_T init_size = size;
    SIZE_T max_size = size;
    heap.handle = (uintptr)HeapCreate(flags, init_size, max_size);
    //NOTE we can also set this to be a low-fragmentation heap (LFH)... maybe the code will run slower though?...
    return heap;
}
internal void *
win32_heap_allocate(Platform_Heap heap, uintptr size)
{
    assert (heap.handle != NULL);
    u32 flags = 0; //there is a flag to set this to zero for us
    void *mem = HeapAlloc((HANDLE)heap.handle, flags, size);
    return mem;
}
internal void 
win32_heap_free(Platform_Heap heap, void *memory)
{
    assert (heap.handle != NULL);
    u32 flags = 0;
    BOOL ok = HeapFree((HANDLE)heap.handle, flags, memory);
    #if DEBUG_BUILD
    if (!ok)
    {
        DWORD error = GetLastError();
        breakpoint;
    }
    #endif
}

internal void
win32_release(void *memory)
{
    BOOL worked = VirtualFree(memory, 0, MEM_RELEASE);
    assert (worked);
}

internal Date_Time
get_date_time() {
    SYSTEMTIME systime;
    GetLocalTime(&systime);
    Date_Time date_time = {};
    //date_time.month = (Month)(systime.wMonth - 1);
    date_time.month = (Month)systime.wMonth;
    date_time.day   = systime.wDay;
    date_time.hour  = systime.wHour;
    date_time.min   = systime.wMinute;
    date_time.sec   = systime.wSecond;// + ((real32)systime.wMilliseconds) / 1000.0f;
    date_time.year   = systime.wYear;// + ((real32)systime.wMilliseconds) / 1000.0f;
    return date_time;
}

internal Win32 *
make_single_window_app(HINSTANCE instance, char *title) {
    Win32 *win32 = &W32; //global alrady initialized to zero
    WNDCLASSA win32_class = {};
    win32_class.style = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
    
    #if DEVELOPER_BUILD 
    win32_class.style |= CS_DBLCLKS; //might be annoying for user menus in final build, dunno
    //NOTE 500 is default (in milliseconds);
    SetDoubleClickTime(250); 
    #endif
    
    win32_class.lpfnWndProc   = default_message_handler;
    win32_class.hInstance     = instance;
    win32_class.lpszClassName = title;
    win32_class.hCursor = LoadCursor(NULL, IDC_ARROW); //otherwise WM_SETCURSOR just never updates cursor icon in DefWindowProcA
    if (!RegisterClassA(&win32_class))	
    {
        MessageBoxA(NULL, "RegisterClassA failed somehow...", "Error...", MB_OK);
        ExitProcess(1);
    }
    
    
    DWORD styles    = (WS_OVERLAPPEDWINDOW|WS_VISIBLE);// & (~WS_MAXIMIZEBOX);
    DWORD ex_styles = 0;
    
    int window_xpos = CW_USEDEFAULT;
    int window_ypos = CW_USEDEFAULT;
    POINT zero_point = { 0, 0 }; //Thank you Raymond Chen, as always
    HMONITOR monitor = MonitorFromPoint(zero_point, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO monitor_info = {sizeof(monitor_info)};
    if (GetMonitorInfoA(monitor, &monitor_info))
    {
        //window pos so that we don't create inside the taskbar
        window_xpos = monitor_info.rcWork.left + 64;
        window_ypos = monitor_info.rcWork.top  + 64;
    }
    else
    {
        DWORD error = GetLastError();
        logprintf("GetMonitorInfoA failed with error code %d", error);
    }
    
    win32->window = {};
    
    
    init_user_input(&win32->window.user_input);
    
    
    win32->window.handle = NULL;
    win32->window.handle = CreateWindowExA(ex_styles, win32_class.lpszClassName,
                                           title, styles,
                                           window_xpos, window_ypos,
                                           DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT,
                                           0,0,instance,0);
    
    if (!win32->window.handle)
    {
        MessageBoxA(NULL, "CreateWindowExA failed somehow...", "Error...", MB_OK);
        ExitProcess(1);
    }
    
    DragAcceptFiles(win32->window.handle, TRUE);
    
    //this returns the full path of the executable, so we have to shave that off
    win32->directory = mkstr(win32->exe_directory_buffer, 0, sizeof(win32->exe_directory_buffer));
    win32->directory.length = GetModuleFileNameA(NULL, win32->directory.str, win32->directory.size);
    if (win32->directory.length)
    {
        s32 at_last_backslash = 0;
        for_index_dec(s32, i, win32->directory.length)
        {
            //TODO windows always uses backslashes right...?
            if (win32->directory.str[i] == '\\')
            {
                at_last_backslash = i;
                break;
            }
        }
        win32->directory.length = at_last_backslash + 1;
        win32->directory.str[win32->directory.length] = 0;
        
        if (!SetCurrentDirectoryA(win32->directory.str))
        {
            MessageBoxA(NULL, "Could not SetCurrentDirectory()", "Error...", MB_OK);   
        }
    }
    
    
    
    
    #if 0 //what does this do again?
    RECT window_rect;	GetWindowRect(win32->window, &window_rect);
    RECT client_rect;	GetClientRect(win32->window, &client_rect);
    s32 extra_x = (window_rect.right - window_rect.left) - (client_rect.right - client_rect.left);
    s32 extra_y = (window_rect.bottom - window_rect.top) - (client_rect.bottom - client_rect.top);
    
    BOOL ok = SetWindowPos(win32->window, HWND_TOP,
        window_rect.left, window_rect.top,	
        DEFAULT_WINDOW_WIDTH  + extra_x,
        DEFAULT_WINDOW_HEIGHT + extra_y,
        SWP_NOMOVE);
    assert (ok);
    //GetClientRect(win32->window, &client_rect);
    //NOTE for some reason this still doesn't work well...
    //assert ((client_rect.right - client_rect.left) == DEFAULT_WINDOW_WIDTH);
    //assert ((client_rect.bottom - client_rect.top) == DEFAULT_WINDOW_HEIGHT);
    #endif
    
    
    LARGE_INTEGER freq;
    if (!QueryPerformanceFrequency(&freq))	win32_check_last_error();
    win32->query_performance_frequency = freq.QuadPart;
    
    //win32->clipboard_buffer_size = 4096;
    //win32->clipboard_buffer =  GlobalAlloc(GMEM_MOVEABLE, win32->clipboard_buffer_size);
    
    
    
    
    
    
    //WriteConsoleA(win32->stdout, "Hello Luis\n", sizeof("Hello Luis\n")-1, &num_characters_written, NULL);
    //SetConsoleTextAttribute(win32->stdout, FOREGROUND_RED|FOREGROUND_GREEN);
    //WriteConsoleA(win32->stdout, "DIE\n", sizeof("DIE\n")-1, &num_characters_written, NULL);
    
    //try to find stdout 
    #if 0
    DWORD num_characters_written = 0;
	#define SUCCESSFULY_WRITE() WriteConsoleA(win32->stdout, litargs("Dev Console\n"), &num_characters_written, NULL)
    win32->stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!win32->stdout) //try find console that called us
    {
        AttachConsole(ATTACH_PARENT_PROCESS);
        win32->stdout = GetStdHandle(STD_OUTPUT_HANDLE);
        if (!SUCCESSFULY_WRITE())
            win32->stdout =  0;
    }
    if (!win32->stdout) //find any open console
    {
        HWND console_window_handle = FindWindowA("ConsoleWindowClass", NULL);//"Command Prompt"
        //DWORD console_window_pid;
        //GetWindowThreadProcessId(console_window_handle, &console_window_pid);
        //if (console_window_pid)
        {
            SetStdHandle(STD_OUTPUT_HANDLE, console_window_handle);
            win32->stdout = GetStdHandle(STD_OUTPUT_HANDLE);
        }
        if (!SUCCESSFULY_WRITE())
            win32->stdout =  0;
    }
    
    if (!win32->stdout) //just make one
    {
        AllocConsole();
        win32->stdout = GetStdHandle(STD_OUTPUT_HANDLE);
        if (!SUCCESSFULY_WRITE())
            win32->stdout =  0;
    }
    #endif
    
    win32->startup_date_time = get_date_time();
   
    return win32;
}





//#if WIN32_INCLUDE_PLATFORM
#if !defined(PLATFORM_H)
#error win32.h needs platform.h in order to retrieve the input...
#endif"


internal void
print_input_event_queue(User_Input *input) {
    for_unhandled_input_event(input, input->focus_type) switch(event->type) {
    case INPUT_EVENT_KEY: {
        Input_Event_Key *key = (Input_Event_Key *)event; 
        debug_printf("Key event %c", key->id);
        if (key->on_press)    debug_printf(" (pressed)");
        if (key->is_down)     debug_printf(" (down)");
        if (key->on_release)  debug_printf(" (released)");       
        debug_printf(" [%llu]\n", FRAME_INDEX);
        
    } break;
    case INPUT_EVENT_MOUSE_SCROLL: {
        Input_Event_Mouse_Scroll *scroll = (Input_Event_Mouse_Scroll *)event; 
        debug_printf("Scroll event %f [%llu]\n", scroll->scroll, FRAME_INDEX);
    } break;
    case INPUT_EVENT_MOUSE_DELTA: {
        Input_Event_Mouse_Delta *delta = (Input_Event_Mouse_Delta *)event; 
        debug_printf("Mouse delta event (%d, %d) [%llu]\n", delta->dx, delta->dy, FRAME_INDEX);
    } break;//delta mouse cursor pos change
    case INPUT_EVENT_TEXT: {
        Input_Event_Text *text = (Input_Event_Text *)event; 
        debug_printf("Text event %s [%llu]\n", text->text, FRAME_INDEX);
    } break;
    case INPUT_EVENT_FOCUS_CHANGE: {
        Input_Event_Focus_Change *change = (Input_Event_Focus_Change *)event; 
        debug_printf("Focus changed from %u to %u  [%llu]\n", 
                     change->prev_focus, change->new_focus, FRAME_INDEX);
    } break;
    case INPUT_EVENT_FILE_DROP: {
        Input_Event_File_Drop *drop = (Input_Event_File_Drop *)event;
        debug_printf("Event file drop %.*s [%llu]\n", drop->filepath.length, drop->filepath.str, FRAME_INDEX);
    } break;
    case INPUT_EVENT_WINDOW_SIZE_CHANGED: {
        Input_Event_Window_Size_Changed *change = (Input_Event_Window_Size_Changed *)event; 
        debug_printf("Event window size change to (%d, %d) [%llu]\n", change->new_width, change->new_height, FRAME_INDEX);
    } break;
    case INPUT_EVENT_PROGRAM_WILL_EXIT: {
        debug_printf("Event program will exit [%llu]\n", FRAME_INDEX);
    } break;
    
    default: assert (0);
    }
}

internal u32 
win32_make_random_seed() {
    u32 result = 0;
    BCryptGenRandom(0, (unsigned char *)&result, sizeof(result), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    return result;
}


internal void
handle_input_events(Win32 *win32, Platform *platform, void (*on_handle_event)(User_Input *)) {
    
    Memory_Arena *scratch = &Temporary_Memory_Arena;
    RESTORE_MEMORY_ARENA_ON_SCOPE_EXIT(scratch);
    
    //NOTE not sure if this is a good idea. What if the messages don't come in the same order...
    //wait what if we stop getting keydown events.....
    //TODO for multiple windows with their own input streams, we may want to store these per window
    Window *window = &win32->window;
    
    //TODO I pulled this out to window in case of multi window, so that maybe each processes input separately...
    //but that probably wouldn't work like that. Maybe this should just be here in the stack instead...
    
    User_Input *input = &window->user_input;
    reset_user_input(input);
    input->mouse_pos = platform->mouse_pos;
    input->mouse_pos_normalized = platform->mouse_pos_normalized;
    
    
    {
        let *event = (Input_Event_Mouse_Pos *)new_input_event(input, INPUT_EVENT_MOUSE_POS);
        if (event) {
            event->pos = platform->mouse_pos;
            event->last_pos = platform->mouse_pos - platform->mouse_pos_delta;
        }
    }
    
    //only called if there is a change in pos
    if (platform->mouse_pos_delta.x || platform->mouse_pos_delta.y) {
        let *event = (Input_Event_Mouse_Delta *)new_input_event(input, INPUT_EVENT_MOUSE_DELTA);
        if (event) {
            event->delta = platform->mouse_pos_delta;
        }
    }
    
    
    
    MSG message;
    //TODO should we care about ignoring WM_MOUSEMOVE and WM_PAINT since PeekMessage will keep generating them if possible?
    while (PeekMessage(&message, window->handle, 0, 0, PM_REMOVE)) {
        
        //i'm declaring it here toplevel, so I can do some stuff to key events specifially
        
        switch (message.message) {
        case WM_KEYDOWN:    case WM_KEYUP:
        case WM_SYSKEYDOWN: case WM_SYSKEYUP:
        {
            Keyboard_Key_ID key_id = KEY_INVALID;
            
            u32 keycode = (u32)message.wParam;
            
            if      (keycode >= 'A' && keycode <= 'Z')
                key_id = (Keyboard_Key_ID)(KEY_A +  (keycode - 'A'));
            else if (keycode >= '0' && keycode <= '9')
                key_id = (Keyboard_Key_ID)(KEY_0 +  (keycode - '0'));
            else if (keycode >= VK_F1 && keycode <= VK_F12) {//goes up to VK_F24!
                key_id = (Keyboard_Key_ID)(KEY_F1 + (keycode - VK_F1)); 
            }
            else switch(keycode) {
            case VK_UP:        key_id = KEY_UP; break;
            case VK_DOWN:      key_id = KEY_DOWN; break;
            case VK_RIGHT:     key_id = KEY_RIGHT; break;
            case VK_LEFT:      key_id = KEY_LEFT; break;
            case VK_SPACE:     key_id = KEY_SPACEBAR; break;
            case VK_BACK:      key_id = KEY_BACKSPACE; break;
            case VK_DELETE:    key_id = KEY_DELETE; break;
            case VK_RETURN:    key_id = KEY_ENTER; break;
            case VK_HOME:      key_id = KEY_HOME; break;
            case VK_END:       key_id = KEY_END; break;
            case VK_PRIOR:     key_id = KEY_PAGEUP; break;
            case VK_NEXT:      key_id = KEY_PAGEDOWN; break;
            case VK_OEM_3:     key_id = KEY_BACKTICK; break;
            case VK_ESCAPE:    key_id = KEY_ESCAPE; break;
            case VK_TAB:       key_id = KEY_TAB; break;
            case VK_CONTROL:   key_id = KEY_CONTROL; break;
            case VK_OEM_5:     key_id = KEY_BACKSLASH; break;
            case VK_OEM_2:     key_id = KEY_FORWARDSLASH; break;
            
            case VK_OEM_COMMA:  key_id = KEY_COMMA; break;
            case VK_OEM_PERIOD: key_id = KEY_PERIOD; break;
            
            //modifier special cases
            case VK_MENU:     key_id = KEY_ALT; break;
            case VK_LCONTROL: key_id = KEY_CONTROL; break;
            case VK_SHIFT:    key_id = KEY_SHIFT;   break;
            } 
            
            
            nil is_down   = (message.lParam & (1 << 31)) == 0;
            nil was_down  = (message.lParam & (1 << 30)) != 0;
            u32 repeat_count = (message.lParam & 0xffff);
            
            if (key_id == KEY_A && is_down) {
                fader_printf("A received %d repeats", repeat_count);
            }
            
            if (key_id != KEY_INVALID) {
                //window->key_button_down[key_id] = is_down != 0; 
                if (is_down) set_modifier(input, key_id);
                else         clear_modifier(input, key_id);
                  
                
                //only make events for key presses and releases7
                if (is_down && !was_down) { //press event
                    let key = new_key_event(input, key_id);
                    if (key) {
                        key->on_press = true;
                        key->is_down  = true;    
                    }
                }
                else if(!is_down && was_down) { //release event
                    let key = new_key_event(input, key_id);
                    if (key) {
                        key->on_release = true;    
                    }
                }
                
                input->repeated_down.set_bit(key_id); //get autorepeat messages for each key
                if (is_down && repeat_count == 1) {
                    input->repeated_down.set_bit(key_id); //get autorepeat messages for each key
                }
            }
            
            #if 0  
            //special text event flags. We use repeat count here to simualte delay when pressing keys
            if (is_down && repeat_count == 1) {
                //we can add to key_text_events?....
                switch (keycode){
                case VK_LEFT:   event_text_flags |= INPUT_TEXT_EVENT_MOVE_LEFT;  break;
                case VK_RIGHT:  event_text_flags |= INPUT_TEXT_EVENT_MOVE_RIGHT; break;
                case VK_UP:     event_text_flags |= INPUT_TEXT_EVENT_MOVE_UP;    break;
                case VK_DOWN:   event_text_flags |= INPUT_TEXT_EVENT_MOVE_DOWN;  break;
                    //case VK_BACK:   event_text_flags |= INPUT_TEXT_EVENT_BACKSPACE;  break;
                case VK_DELETE: event_text_flags |= INPUT_TEXT_EVENT_DELETE;     break;
                case VK_BACK:   event_text_flags |= INPUT_TEXT_EVENT_BACKSPACE;  break;
                
                case VK_OEM_COMMA: {
                    //if (window->key_button_down[KEY_CONTROL])
                    if (has_modifier(input, KEY_CONTROL)) {
                        event_text_flags |= INPUT_TEXT_EVENT_UNDERSCORE;    
                    } 
                } break;
                
                
                //case VK_RETURN: event_text_flags |= INPUT_TEXT_EVENT_ENTER;      break;
                
                };
            }
            #endif
            
            nil convert_message_to_wm_char = true;
            if (keycode >= VK_F1 && keycode <= VK_F24) {
                convert_message_to_wm_char = false;
            } else switch (keycode) { 
            case 'J': {
                //CTRL+J creates a '\n' character or enter press
                if (has_modifier(input, KEY_CONTROL)) convert_message_to_wm_char = false;
            }break;
            case VK_BACK: {
                convert_message_to_wm_char = false;
                //CTRL+J sends ascii 127 or delete
                //if (has_modifier(input, KEY_CONTROL)) convert_message_to_wm_char = false;
            } break;
            }
            
            if (convert_message_to_wm_char) {
                TranslateMessage(&message);
                DispatchMessageA(&message);    
            }
        } break;
        
        case WM_LBUTTONDOWN: case WM_LBUTTONUP: case WM_LBUTTONDBLCLK:
        case WM_MBUTTONDOWN: case WM_MBUTTONUP: case WM_MBUTTONDBLCLK:
        case WM_RBUTTONDOWN: case WM_RBUTTONUP: case WM_RBUTTONDBLCLK: {
            Keyboard_Key_ID keyid = {};
            bool is_down = false;
            bool is_double_click = false;
            switch(message.message){
            case WM_LBUTTONDBLCLK: is_double_click = true; //NOTE fallthrough on purpose
            case WM_LBUTTONDOWN:   keyid = KEY_MOUSE_LEFT; is_down = true; break;
            case WM_LBUTTONUP:     keyid = KEY_MOUSE_LEFT;                 break;
            
            case WM_MBUTTONDBLCLK: is_double_click = true; //NOTE fallthrough on purpose
            case WM_MBUTTONDOWN: keyid = KEY_MOUSE_MIDDLE; is_down = true; break;
            case WM_MBUTTONUP:   keyid = KEY_MOUSE_MIDDLE;                 break;
            
            case WM_RBUTTONDBLCLK: is_double_click = true; //NOTE fallthrough on purpose
            case WM_RBUTTONDOWN: keyid = KEY_MOUSE_RIGHT;  is_down = true; break;
            case WM_RBUTTONUP:   keyid = KEY_MOUSE_RIGHT;                  break;
            
            default: assert (0);
            }
            
            
            if (is_down) {
                let key = new_key_event(input, keyid);
                if (key) {
                    key->is_down = true;
                    
                    if (!has_modifier(input, key->id)) 
                    key->on_press = true;
                    set_modifier(input, key->id);
                    
                    if (is_double_click)
                    key->flags |= INPUT_EVENT_KEY_MOUSE_DOUBLE_CLICK;    
                }
            } else {
                let key = new_key_event(input, keyid);
                if (key) {
                    key->on_release = true;
                    assert (has_modifier(input, key->id));
                    clear_modifier(input, key->id);    
                }
            }
        } break;
        
        
        case WM_MOUSEWHEEL:
        {
            let event = (Input_Event_Mouse_Scroll *)new_input_event(input, INPUT_EVENT_MOUSE_SCROLL);
            if (event) {
                event->scroll = GET_WHEEL_DELTA_WPARAM(message.wParam) / 120.0f;
                //event->mouse_x = current_mouse_pos.x;
            	//event->mouse_y = current_mouse_pos.y;
            }
        } break;
        
        case WM_CHAR: { //doesn't actually output event until after we get all wm_char
            u32 utf16_char = (u32)message.wParam; 
            if ((utf16_char >= ' ' && utf16_char <= '~') ||
                utf16_char == '\t') { //NOTE do we want to handle tab separately or no?
                char c = (utf16_char == '\r') ? '\n' :  (char)utf16_char;
                insert_to_text_stream(input, c);
            }
        } break;
        
        case WM_PAINT: //has to be handled otherwise we just keep getting this message over and over
        {
            //ValidateRect(window->handle, NULL); //apparently this isn't necessary 
            DefWindowProcA(window->handle, message.message, message.wParam, message.lParam);
        } break;
        
        case WM_DROPFILES: {
            HDROP handle = (HDROP)message.wParam;
            UINT file_count = DragQueryFileA(handle, 0xffffffff, NULL, 0);
            for (UINT i = 0; i < file_count; i += 1) {
                // NOTE if you want to converve memory, passing NULL for buffer returns length of the string, or we could just manually handle the string data oursevles
                // ehh who cares, just allocate max path for each string
                let event = (Input_Event_File_Drop *)new_input_event(input, INPUT_EVENT_FILE_DROP);
                if (!event) break;
                
                event->filepath.size = MAX_PATH;
                event->filepath.str = push_array(scratch, char, event->filepath.size);
                if (!event->filepath.str) {
                    assert (0); //TODO either make a null event type or return event to the queue stack
                    break;
                }
                
                event->filepath.length = DragQueryFileA(handle, i, event->filepath.str, event->filepath.size); 
            } 
            DragFinish(handle);
            //DragAcceptFiles
        } break;
        
        default: {
            DispatchMessageA(&message);     
        } break;
        }
    }
    
    //NOTE windows stops sending us wm_keybuttondown events if more than one key are pressed 
    //so we have to check the modifiers_down (keys_down) state and generate these events always
    {
        s32 keyid;
        let modifiers_down = input->modifiers_down;
        while (modifiers_down.find_least_significant_bit(&keyid)) {
            //NOTE we only search on current focus_type (because that's where all new input key events should have been added to right?...)
            Input_Event_Key *key = null;
            for_unhandled_input_event(input, input->focus_type) {
                if (event->type == INPUT_EVENT_KEY) {
                    Input_Event_Key *k = (Input_Event_Key *)event;
                    if (k->id == (u32)keyid) {
                        key = k;
                        assert (!key->on_release);
                        break;    
                    }
                }    
            }
            
            
            if (!key) {
                key = new_key_event(input, (Keyboard_Key_ID)keyid);
            }
            
            if (key) {
                assert (!key->on_release);
                key->is_down = true;
                if (input->repeated_down.is_set(keyid)) {
                    key->on_autorepeat_press = true; 
                }
            }
            modifiers_down.clear_bit(keyid);
        }
    }
    
	//print_input_event_queue(input);
    on_handle_event(input);
    if (input->sent_out_program_will_exit_event) {
        platform->still_running = false;
    }
}



internal char *
win32_open_entire_file_null_terminate(Memory_Arena *arena, String filename, u32 *out_filesize = 0) {
    char *file = 0;
    assert (is_null_terminated(filename));
    HANDLE handle = (void *)CreateFileA(filename.str, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (handle != INVALID_HANDLE_VALUE)
    {
        DWORD high;
        DWORD low = GetFileSize(handle, &high);
        assert (high == 0); //NOTE our memory arenas aren't big enough to handle this 
        if (low)
        {
            assert (low < U32_MAX); //ensure enough space for null terminator
            
            //file = arena ? push_array(arena, char, low + 1) : (char *)win32_allocate(low + 1);
            file = push_array(arena, char, low + 1);
            
            if (file)
            {
                DWORD bytes_read;
                if (ReadFile(handle, file, low, &bytes_read, NULL))
                {
                    assert (bytes_read == low); //@TODO more handling in case of failure
                    file[low] = 0; 
                    if (out_filesize)	*out_filesize = low;
                }
                else
                {
                    file = 0;
                    win32_check_last_error();
                }
            }
        }
        else	win32_check_last_error();
        CloseHandle(handle);
    }
    return file;
}

internal u32 
win32_read_file(String filename, void *file, u32 readsize, u32 file_offset = 0) {
    assert (file && (readsize > 0));
    assert (is_null_terminated(filename));
    HANDLE handle = (void *)CreateFileA(filename.str, GENERIC_READ, FILE_SHARE_READ, NULL,
                                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (handle == INVALID_HANDLE_VALUE) return 0;
    
    defer { CloseHandle(handle); };
    
    DWORD filesize_hi;
    DWORD filesize_lo = GetFileSize(handle, &filesize_hi);
    assert (filesize_lo > 0); //TODO is this allowed if file doesn't exist?...
    if (filesize_hi > 0) return 0; //TODO support 64bit filesizes
    
    
    if (file_offset > 0) {
        DWORD amount_moved = SetFilePointer(handle, file_offset, 0, FILE_BEGIN);
        if (amount_moved != file_offset) {
            win32_check_last_error();
            return 0;
        }   
    }
    
    DWORD bytes_to_read = min2(filesize_lo, readsize);
    DWORD bytes_read_dword;
    u32 result = 0;
    if (ReadFile(handle, file, bytes_to_read, &bytes_read_dword, NULL)) {
        result = bytes_read_dword;
    } else {
        win32_check_last_error();
    }
    
    
    return result;
}

//#define STB_TRUETYPE_IMPLEMENTATION
//#define STBTT_STATIC //make all functions static/internal
//#define STBTT_assert (x) assert (x)
//#include "libs/stb_truetype.h"

#if 0
internal Rasterized_Font 
rasterize_font(Memory_Arena *arena, char *filename, s32 pixel_height)
{
    //NOTE pixel_height is how tall in pixels a line of text will be when we rasterize it to this size...
    Rasterized_Font result = {}; //push_type(arena, Rasterized_Font);
    
    char *file = allocate_file_null_terminate(0, filename);
    assert (file);
    
    result.pixel_height = pixel_height;
    
    stbtt_fontinfo font;
    stbtt_InitFont(&font, (u8 *)file, stbtt_GetFontOffsetForIndex((u8 *)file,0));
    
    float scale    = stbtt_ScaleForPixelHeight(&font, (float)pixel_height);
    //float em_scale = stbtt_ScaleForMappingEmToPixels(&font, (float)pixel_height);
    
    //NOTE I don't understand why ttf files have these values for vector graphics
    //we haven't rasterized, is this just some random scaling the font file uses?
    //why isn't the api just rasterize_everything_to(128), then get all the values in pixels...
    int ascender, descender, linegap;
    stbtt_GetFontVMetrics(&font, &ascender, &descender, &linegap); //unscaled coords
    //float scale = (float)pixel_height / (ascender - descender);
    
    result.descender = -(descender*scale); 
    result.ascender  = (ascender*scale); 
    result.linegap   = (linegap*scale); 
    
    //TODO I don't exactly understand what left_side_bearing is for, or if we really need it?
    //is that just the initial offset to move a character left edge when drawing the start of a line?
    int advance, left_side_bearing;
    stbtt_GetCodepointHMetrics(&font, ' ', &advance, &left_side_bearing); //unscaled coords
    result.space_advance = (advance * scale);
    
    //{
    //int x1, x2, y1, y2;
    //stbtt_GetFontBoundingBox(&font, &x1, &y1, &x2, &y2);
    //result.max_ascender  = -(y1*scale);
    //result.max_descender =  (y2*scale);
    //}
    
    for(u32 c = CHARSET_START; c <= CHARSET_END; ++c)
    {
        int x1, x2, y1, y2;
        stbtt_GetCodepointBitmapBox(&font, c, scale, scale, &x1, &y1, &x2, &y2);
        
        //NOTE the yaxis increases going down
        Glyph *glyph = get_glyph(&result, c);
        
        glyph->width  = x2 - x1;
        glyph->height = y2 - y1;
        
        glyph->ascender  = (f32)-y1;
        glyph->descender = (f32)y2;
        
        stbtt_GetCodepointHMetrics(&font, c, &advance, &left_side_bearing); //unscaled coords
        glyph->advance           = advance * scale;
        glyph->left_side_bearing = left_side_bearing * scale;
        //glyph->advance = (advance - left_side_bearing)*scale;
        
        for(u32 c2 = CHARSET_START; c2 <= CHARSET_END; ++c2)
        {
            glyph->kern[c2 - CHARSET_START] = stbtt_GetCodepointKernAdvance(&font, c, c2) * scale;
        }
        
        glyph->pixels = ALLOCATE(arena, u8, glyph->width * glyph->height);
        stbtt_MakeCodepointBitmap(&font, (u8 *)glyph->pixels, glyph->width, glyph->height,
            glyph->width/*stride*/,scale, scale, c);
        //TODO is there an stb call to do flip automatically?
        //flip vertically
        for(u32 y = 0; y < u32(glyph->height*0.5f); ++y)
        {
            for(s32 x = 0; x < glyph->width; ++x)
            {
                u8 *lower = glyph->pixels + y*glyph->width + x;
                u8 *upper = glyph->pixels + (glyph->height - y - 1)*glyph->width + x;
                u8 temp_pixel = *lower;
                *lower = *upper;
                *upper = temp_pixel;
            }
        }
    }
    win32_release(file);
    //restore_and_zero(arena, restore_point);
    return result;
}
#endif

//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_STATIC static
//#define STBIW_ASSERT(x) assert (x)
//#include "libs/stb_image_write.h"

#if 0
internal Texture_Atlas 
rasterize_font_to_texture_atlas(Linear_Allocator *arena, char *filename, s32 pixel_height, Rasterized_Font *out_font)
{
    Texture_Atlas atlas = {};
    out_font->pixel_height = pixel_height;
    
    const unsigned char *file = (const unsigned char *)open_entire_file_null_terminate(0, filename);
    assert (file);
    
    atlas.width  = pixel_height*8;
    atlas.height = pixel_height*8;
    atlas.pixels = tn_allocate(arena, u8, atlas.width*atlas.height);
    atlas.rect_count = CHARSET_COUNT + 1; //first one is a white texture...
    atlas.rects  = tn_allocate(arena, Rect2i, atlas.rect_count);
    atlas.type = TEXTURE_8BPP;
    
    stbtt_pack_context ctx;
    void *alloc_context = NULL;
    int ok = stbtt_PackBegin(&ctx, (unsigned char *)atlas.pixels, atlas.width, atlas.height, 0, 1, alloc_context);
    assert (ok);
    
    //ok = stbtt_PackFontRange(&ctx, file, 0, (float)pixel_height,
    //CHARSET_START, CHARSET_COUNT, chardata);
    //NOTE adapted from stbtt_PackFontRanges simply to get stbtt_fontinfo back... we could've just asked for it I guess...
    stbtt_fontinfo info;
    
    stbtt_packedchar chardata[256];
    for(int j = 0; j < CHARSET_COUNT; ++j)
        chardata[j].x0 = chardata[j].y0 = chardata[j].x1 = chardata[j].y1 = 0;
    
    //NOTE we reserve one rect for a white texture
    stbrp_rect *rects = (stbrp_rect *) STBTT_malloc(sizeof(*rects) * (CHARSET_COUNT + 1), ctx.user_allocator_context);
    if (rects)
    {
        info.userdata = ctx.user_allocator_context;
        stbtt_InitFont(&info, file, stbtt_GetFontOffsetForIndex(file, 0));
        
        stbtt_pack_range range_to_pass_in;
        range_to_pass_in.first_unicode_codepoint_in_range = CHARSET_START;
        range_to_pass_in.array_of_unicode_codepoints = NULL;
        range_to_pass_in.num_chars                   = CHARSET_COUNT;
        range_to_pass_in.chardata_for_range          = chardata;
        range_to_pass_in.font_size                   = (float)pixel_height; 
        //NOTE n is the total amount of rects packed for all ranges passed in..
        int n = stbtt_PackFontRangesGatherRects(&ctx, &info, &range_to_pass_in, 1, rects + 1);
        rects[0] = {};
        rects[0].w = rects[0].h = 30;
        
        stbtt_PackFontRangesPackRects(&ctx, rects, n + 1);
        
        int ok = stbtt_PackFontRangesRenderIntoRects(&ctx, &info, &range_to_pass_in, 1, rects + 1);
        foreach_index_inc(s32, x_index, rects[0].w)
        {
            foreach_index_inc(s32, y_index, rects[0].h)
            {
                s32 x = rects[0].x + x_index;
                s32 y = rects[0].y + y_index;
                ctx.pixels[x + y*ctx.stride_in_bytes] = 0xff;
            }
        }
        assert (ok);
        
        STBTT_free(rects, ctx.user_allocator_context);   
    }
    else assert (0);
    
    
    float scale =  stbtt_ScaleForPixelHeight(&info, (float)pixel_height);
    
    int ascender, descender, linegap;
    stbtt_GetFontVMetrics(&info, &ascender, &descender, &linegap); //unscaled coords
    //float scale = (float)pixel_height / (ascender - descender);
    
    out_font->descender = -(descender*scale); 
    out_font->ascender  = (ascender*scale); 
    out_font->linegap   = (linegap*scale); 
    
    //TODO I don't exactly understand what left_side_bearing is for, or if we really need it?
    //is that just the initial offset to move a character left edge when drawing the start of a line?
    int advance, left_side_bearing;
    stbtt_GetCodepointHMetrics(&info, ' ', &advance, &left_side_bearing); //unscaled coords
    out_font->space_advance = (advance * scale);
    
    atlas.rects[0].min.x = 0;
    atlas.rects[0].min.y = 0;
    atlas.rects[0].max.x = 30;
    atlas.rects[0].max.y = 30;
    
    //foreach_index_inc(s32, codepoint_index, CHARSET_COUNT)
    for(u32 codepoint = CHARSET_START; codepoint <= CHARSET_END; codepoint += 1)
    {
        int glyph_index = stbtt_FindGlyphIndex(&info, codepoint);
        int advance, lsb, x1, y1, x2, y2;
        stbtt_GetGlyphHMetrics(&info, glyph_index, &advance, &lsb);
        stbtt_GetGlyphBitmapBox(&info, glyph_index,
            scale * ctx.h_oversample,
            scale * ctx.v_oversample,
            &x1,&y1,&x2,&y2);
        
        Glyph *glyph = get_glyph(out_font, codepoint);
        
        //stbrp_rect rect = rects[codepoint - CHARSET_START];
        stbtt_packedchar *pc = chardata + codepoint - CHARSET_START;
        glyph->width = (f32)(pc->x1 - pc->x0);
        atlas.rects[codepoint - CHARSET_START + 1].min.x = pc->x0;
        atlas.rects[codepoint - CHARSET_START + 1].min.y = pc->y0;
        atlas.rects[codepoint - CHARSET_START + 1].max.x = pc->x1;
        atlas.rects[codepoint - CHARSET_START + 1].max.y = pc->y1;
        
        glyph->ascender  = (f32)-y1;
        glyph->descender = (f32)y2;
        
        glyph->advance           = advance * scale;
        glyph->left_side_bearing = lsb * scale;
        
        for(u32 codepoint_next = CHARSET_START; codepoint_next <= CHARSET_END; ++codepoint_next)
        {
            glyph->kern[codepoint_next - CHARSET_START] = stbtt_GetGlyphKernAdvance(&info, codepoint, codepoint_next) * scale;
        }
        
        //flip vertically for opengl ...
        
        //for(u32 y = glyph->rect_in_atlas.min.y; y < glyph->rect_in_atlas.max.y; ++y)
        //{
        //for(u32 x = glyph->rect_in_atlas.min.x; x < glyph->rect_in_atlas.max.x; ++x)
        //{
        //u8 *lower = result.atlas + y*result.atlas_width + x;
        //u8 *upper = result.atlas + glyph->rect_in_atlas.min.x + (glyph_height - y - 1)*result.atlas_width + x;
        //SWAP(*lower, *upper, u8);
        //}
        //}
        
        //flip each glyph upside down
        #if 1
        u32 glyph_width  = pc->x1 - pc->x0;
        u32 glyph_height = pc->y1 - pc->y0;
        u32 stride = atlas.width;
        u8 *lower_bottom = ((u8 *)atlas.pixels) + pc->x0 + pc->y0*stride;
        foreach_index_inc_u32(y, glyph_height/2)
        {
            foreach_index_inc_u32(x, glyph_width)
            {
                u8 *lower = lower_bottom + x + y*stride;
                u8 *upper = lower_bottom + x + (glyph_height - y - 1)*stride;
                SWAP(*lower, *upper, u8);
            }   
        }
        #endif
        
    }
    
    stbtt_PackEnd(&ctx);
    win32_release((void *)file);
    
    
    stbi_write_png("../font_atlas_test.png", atlas.width, atlas.height, 1, atlas.pixels, atlas.width);
    return atlas;
}
#endif



//#endif //WIN32_INCLUDE_PLATFORM



internal u64
get_timestamp() {
    LARGE_INTEGER counter;
    BOOL ok = QueryPerformanceCounter(&counter);
    assert (ok);
    return counter.QuadPart;
}

internal f32
get_secs_elapsed(Win32 *win32, u64 t2, u64 t1)
{
    assert (t2 >= t1); //NOTE according to windows, these values will NOT wrap in less than 100 years since computer was booted... 
    f32 result = (f32)(t2 - t1) / (f32)win32->query_performance_frequency;
    return result;
}











#if WIN32_INCLUDE_OpenGL //code to get basic opengl 3.3 context set up
#include "opengl.h"

#define WGL_DRAW_TO_WINDOW_ARB                  0x2001
#define WGL_ACCELERATION_ARB                    0x2003
#define WGL_FULL_ACCELERATION_ARB               0x2027
#define WGL_SUPPORT_OpenGL_ARB                  0x2010
#define WGL_DOUBLE_BUFFER_ARB                   0x2011
#define WGL_PIXEL_TYPE_ARB                      0x2013
#define WGL_TYPE_RGBA_ARB                       0x202B
#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB        0x20A9
#define WGL_RED_BITS_ARB                        0x2015
#define WGL_GREEN_BITS_ARB                      0x2017
#define WGL_BLUE_BITS_ARB                       0x2019
#define WGL_ALPHA_BITS_ARB                      0x201b
#define WGL_DEPTH_BITS_ARB                      0x2022
#define WGL_STENCIL_BITS_ARB                    0x2023 

//wglCreateContextAttribsARB stuff
#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB             0x2093
#define WGL_CONTEXT_FLAGS_ARB                   0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126

#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB    0x0002
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002


#if OpenGL_DEBUG_BUILD
#define WGL_CONTEXT_DEBUG_BIT_ARB               0x0001
#else
#define WGL_CONTEXT_DEBUG_BIT_ARB               0
#endif

internal int
get_regular_pixel_format_index(HDC dc) {
    int pindex;
    PIXELFORMATDESCRIPTOR pformat = {};
    pformat.nSize = sizeof(pformat);
    pformat.nVersion = 1;
    pformat.iPixelType = PFD_TYPE_RGBA;
    //NOTE obs has problems streaming with PFD_DOUBLEBUFFER set 
    pformat.dwFlags = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER;
    pformat.cColorBits = 32;
    pformat.cAlphaBits = 8;
    pformat.cDepthBits = 24; //TODO should we ask for 32-bits?
    pformat.iLayerType = PFD_MAIN_PLANE;
    pindex = ChoosePixelFormat(dc, &pformat);
    return pindex;
}

internal void
set_pixel_format(HDC dc, int pindex) {
    PIXELFORMATDESCRIPTOR pformat;
    DescribePixelFormat(dc, pindex, sizeof(pformat), &pformat); //@TODO check ok?
    SetPixelFormat(dc, pindex, &pformat);
}

internal u32
figure_out_monitor_refresh_rate(HDC dc) {
    int result = GetDeviceCaps(dc, VREFRESH);
    if (result == 0 || result == 1)
    {
        logprintf("Need to do some rendering tests to figure out what the refresh rate is for monitor...");
        assert (0);
    }
    return (u32)result;
}

internal Opengl *
setup_opengl(Win32 *win32) {
    //this creates an opengl compatible dc context and fills out the opengl struct
    //we attempt to make an srgb framebuffer
    //we require opengl 3.3 or later
    OpenGL = {};
    
    //NOTE this is very, very stupid...
    //we need to set an opengl context before we can call wglGetProcAddress
    //but we need wglGetProcAddress to get wgoChoosePixelFormatARB and we can only set the context ONCE
    //therefore we make a dummy window just to get these functions ...
    typedef BOOL wglChoosePixelFormatARB_(HDC dc, const int *attrib1, const FLOAT *attrib2, 
        UINT nMaxFormats, int *formats, UINT *nNumFormats_supported); 
    typedef HGLRC wglCreateContextAttribsARB_(HDC dc, HGLRC context, const int *attribs);
    wglChoosePixelFormatARB_ *wglChoosePixelFormatARB;
    wglCreateContextAttribsARB_ *wglCreateContextAttribsARB;
    {
        WNDCLASSA wclass = {};
        wclass.lpfnWndProc = DefWindowProcA;
        wclass.hInstance = GetModuleHandle(0);
        wclass.lpszClassName = "wgl extensions loader";
        if (!RegisterClassA(&wclass))	win32_check_last_error();
        
        HWND window = CreateWindowExA(0, wclass.lpszClassName, "opengl dummy window",
            0,
            CW_USEDEFAULT, CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT,
            0, 0, wclass.hInstance, 0);
        
        HDC dc = GetDC(window);
        set_pixel_format(dc, get_regular_pixel_format_index(dc));
        
        HGLRC context = wglCreateContext(dc); 
        if (!wglMakeCurrent(dc, context))	
        {
            DWORD error = GetLastError();
            fatalprintf("wglMakeCurrent() failed with error code %d", error);   
        }
        
        glGetStringi               = (glGetStringi_ *)wglGetProcAddress("glGetStringi");
        wglChoosePixelFormatARB    = (wglChoosePixelFormatARB_*)wglGetProcAddress("wglChoosePixelFormatARB");
        wglCreateContextAttribsARB = (wglCreateContextAttribsARB_*)wglGetProcAddress("wglCreateContextAttribsARB");
        SwapIntervalEXT = (SwapIntervalEXT_*)wglGetProcAddress("wglSwapIntervalEXT");
        
        //TODO check these values?
        wglMakeCurrent(dc, 0);
        wglDeleteContext(context);
        ReleaseDC(window, dc);
        DestroyWindow(window);
    }
    if (!glGetStringi || !wglChoosePixelFormatARB || !wglCreateContextAttribsARB)
        fatalprintf("Failed to load necessary opengl procedures like wglChoosePixelFormatARB");
    
    //find the best pixel format index
    HDC dc = GetDC(win32->window.handle);
    int pindex = get_regular_pixel_format_index(dc);
    //if (wglChoosePixelFormatARB)
    {
        int attribs[] =
        {
            WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, GL_TRUE, 
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE, // 0
            WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB, 
            WGL_SUPPORT_OpenGL_ARB, GL_TRUE, 
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE, 
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_RED_BITS_ARB,   8,
            WGL_GREEN_BITS_ARB, 8,
            WGL_BLUE_BITS_ARB,  8,
            WGL_ALPHA_BITS_ARB, 8,
            WGL_DEPTH_BITS_ARB, 24,
            0,
        };
        
        //TODO does the returned index match exactly to what we asked for? Or is it just close enough???
        //the 1 before index represents length of index. We only care about the only one exact match, so we pass 1
        //choose_pixel_format_worked is a misnomer. Really it's the length of returned &index. 1 means we got an exact match...
        int index;
        GLuint choose_pixel_format_worked = false;
        BOOL ok = wglChoosePixelFormatARB(dc, attribs, 0, 1, &index, &choose_pixel_format_worked);
        if (ok && choose_pixel_format_worked) 
            pindex = index; //we got a valid match
        else
        {
            logprintf("WglChoosePixelFormatARB failed.");
        } 
    }
    set_pixel_format(dc, pindex);
    
    //create best context
    HGLRC context = 0;
    //if (wglCreateContextAttribsARB)
    {
        int attribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | WGL_CONTEXT_DEBUG_BIT_ARB, 
            WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB, //@TODO compatibility version
            0
        };
        context = wglCreateContextAttribsARB(dc, 0, attribs);
        if (context == NULL)
        {
            DWORD code = GetLastError();
            fatalprintf("wglCreateContextAttribsARB failed to give us an opengl context");
        }
    }
    if (!wglMakeCurrent(dc, context))	
        logprintf("wglMakeCurrent failed.");
    
    // get function pointers
    /**/#define X(name) name = (name##_ *)wglGetProcAddress(#name); assert (name)
    X(glCreateShader);
    X(glDeleteShader);
    X(glShaderSource);
    X(glCompileShader);
    X(glCreateProgram);
    X(glDeleteProgram);
    X(glAttachShader);
    X(glLinkProgram);
    X(glValidateProgram);
    
    X(glGetProgramiv);
    X(glGetProgramInfoLog);
    X(glGetShaderInfoLog);
    
    X(glGetAttribLocation);
    X(glBindAttribLocation);
    X(glGetUniformLocation);
    X(glGetUniformBlockIndex);
    
    X(glBindBufferBase);
    X(glUniformBlockBinding);
    
    X(glUniform1ui);
    X(glUniform1i);
    X(glUniform2i);
    X(glUniform1f);
    X(glUniform2f);
    X(glUniform3f);
    X(glUniform4f);
    X(glUniform4fv);
    X(glUniformMatrix4fv);
    X(glUniformMatrix3fv);
    X(glUseProgram);
    X(glGetShaderInfoLog);
    
    X(glGenFramebuffers);
    X(glBindFramebuffer);
    X(glDeleteFramebuffers);
    X(glFramebufferTexture2D);
    X(glCheckFramebufferStatus);
    X(glBlitFramebuffer);
    X(glActiveTexture);
    
    X(glGenBuffers);
    X(glBindBuffer);
    X(glBufferData);
    X(glBufferSubData);
    
    X(glTexImage2DMultisample);
    
    X(glEnableVertexAttribArray);
    X(glDisableVertexAttribArray);
    X(glVertexAttribPointer);
    X(glVertexAttribIPointer);
    X(glVertexAttribDivisor);
    
    
    X(glGenVertexArrays);
    X(glBindVertexArray);
    X(glDrawElementsInstanced);
    
    #if 0
    X(glDebugMessageCallback);
    X(glDebugMessageControl);
    X(glGetDebugMessageLog);
    #endif
    #undef X
    
    //TODO shouldn't we check these extensions before we ask for and SRGB framebuffer...
    GLint extension_count;
    glGetIntegerv(GL_NUM_EXTENSIONS, &extension_count);
    for(GLint i = 0; i < extension_count; i += 1)
    {
        char *string_ = (char *)glGetStringi(GL_EXTENSIONS, i);
        String string = mkstr(string_);
        
        if (strmatch(string, "GL_ARB_framebuffer_sRGB"s) || strmatch(string, "GL_EXT_framebuffer_sRGB"s))
            OpenGL.flags |= OpenGL_SUPPORTS_SRGB_FRAMEBUFFER;
        else if (strmatch(string, "GL_EXT_texture_sRGB"s))
            OpenGL.flags |= OpenGL_SUPPORTS_SRGB_TEXTURE;
        else if (strmatch(string, "GL_ARB_framebuffer_object"s))
            OpenGL.flags |= OpenGL_SUPPORTS_FRAMEBUFFER_OBJECT;
    }
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint *)&OpenGL.max_texture_size);
    
    //NOTE apparently you're not supposed to use glGetString because it's not 'officially' supported
    //but glGetStringi doesn't work with these, so how?!?!
    #if OpenGL_DEBUG_BUILD
    OpenGL.version      = (char *)glGetString(GL_VERSION);
    OpenGL.glsl_version = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
    OpenGL.vendor       = (char *)glGetString(GL_VENDOR);
    OpenGL.renderer     = (char *)glGetString(GL_RENDERER);
    #endif
    
    
    
    ReleaseDC(win32->window.handle, dc);
    return &OpenGL;
    
}

#endif //WIN32_INCLUDE_OpenGL
#endif //WIN32_H
