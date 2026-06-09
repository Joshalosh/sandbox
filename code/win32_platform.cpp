
#include <windows.h>
#include <stdio.h>

#include "staples.h"
#include "sandbox.h"

GLOBAL bool         g_running;
GLOBAL Win32_Bitmap g_bitmap;

INTERNAL Win32_Window_Dimension Win32GetWindowDimension(HWND window) {
    Win32_Window_Dimension result;
    RECT client_rect;
    ASSERT(window);
    GetClientRect(window, &client_rect);
    result.width = client_rect.right - client_rect.left;
    result.height = client_rect.bottom - client_rect.top;
    return result;
}

INTERNAL void Win32CopyBitmapToWindow(HDC device_context, Win32_Bitmap bitmap,
                                      int window_width, int window_height) {
    StretchDIBits(device_context, 0, 0, window_width, window_height, 0, 0, bitmap.width, bitmap.height,
                  bitmap.memory, &bitmap.info, DIB_RGB_COLORS, SRCCOPY); 
}

LRESULT CALLBACK Win32MainWindowCallback(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
    LRESULT result = 0;
    switch (message) {
        case WM_SIZE: {
        } break;
        case WM_DESTROY:     g_running = false;                      break;
        case WM_CLOSE:       g_running = false;                      break;
        case WM_ACTIVATEAPP: OutputDebugStringA("WM_ACTIVATEAPP\n"); break;
        case WM_PAINT: {
            PAINTSTRUCT paint;
            HDC device_context = BeginPaint(window, &paint);
            Win32_Window_Dimension dimension = Win32GetWindowDimension(window);
            Win32CopyBitmapToWindow(device_context, g_bitmap, dimension.width, dimension.height);
            EndPaint(window, &paint);
        } break;
        default: result = DefWindowProc(window, message, w_param, l_param); break;
    }
    return result;
}

INTERNAL Win32ResizeDIBSection(Win32_Bitmap *bitmap, int width, int height) {
    // Win32 takes LONG for its width and height which is int32 
    // just to make operations easy I should keep things in this functions to 
    // S32 so everything plays nice with the required ints for the API
    S32 bytes_per_pixel = 4;
    S32 bitmap_memory_size = width * height * bytes_per_pixel;
    void *new_memory = VirtualAlloc(0, bitmap_memory_size, MEM_COMMIT, PAGE_READWRITE);
    if (new_memory) {
        if (bitmap->memory) {
            VirtualFree(bitmap->memory, 0, MEM_RELEASE);
        }

        bitmap->width = width;
        bitmap->height = height;
        bitmap->bytes_per_pixel = bytes_per_pixel;

        bitmap->info.bmiHeader.biSize = sizeof(bitmap->info.bmiHeader);
        bitmap->info.bmiHeader.biWidth = bitmap->width;
        bitmap->info.bmiHeader.biHeight = -bitmap->height;
        bitmap->info.bmiHeader.biPlanes = 1;
        bitmap->info.bmiHeader.biBitCount = 32;
        bitmap->info.bmiHeader.biCompression = BI_RGB;

        bitmap->memory = new_memory;
        bitmap->pitch = bitmap->width * bitmap->bytes_per_pixel;
    }
}

// Its hidden in the macro but this function takes the parameters:
// - void *memory
DEBUG_PLATFORM_FREE_FILE_MEMORY(DEBUGPlatformFreeFileMemory) {
    if (memory) {
        VirtualFree(memory, 0, MEM_RELEASE);
    }
}

// Hidden in this macro the function takes the params:
// - char *filename
DEBUG_PLATFORM_READ_ENTIRE_FILE(DEBUGPlatformReadEntireFile) {
    Debug_Read_File_Result result = {};
    HANDLE file_handle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (file_handle != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER file_size;
        if (GetFileSizeEx(file_handle, &file_size)) {
            U32 file_size32 = SafeU64ToU32(file_size.QuadPart);
            result.content = VirtualAlloc(0, file_size32, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
            if (result.content) {
                DWORD bytes_read;
                if (ReadFile(file_handle, result.content, file_size32, &bytes_read, 0) && 
                    (file_size32 == bytes_read)) {
                    result.content_size = file_size32;
                } else {
                    // TODO: Log the file wasn't successfully read
                    DEBUGPlatformFreeFileMemory(result.content);
                    result.content      = 0;
                    result.content_size = 0;
                }
            } else {
                // TODO: Log the memory allocation failed
            }
        } else {
            // TODO: Log the file handle size wasn't extracted successfully 
        }

        CloseHandle(file_handle);
    } else {
        // TODO: Log the file handle was not successfully created
    }
    return result;
}

// Hidden in this macro the function takes these params:
// - char *filename 
// - U32 memory_size 
// - void *memory
DEBUG_PLATFORM_WRITE_ENTIRE_FILE(DEBUGPlatformWriteEntireFile) {
    B32 result = false;

    HANDLE file_handle = CreateFileA(filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (file_handle != INVALID_HANDLE_VALUE) {
        DWORD bytes_written;
        if (WriteFile(file_handle, memory, memory_size, &bytes_written, 0)) {
            result = (bytes_written == memory_size);
        } else {
            // TODO: Log the file write failed
        }
        CloseHandle(file_handle);
    } else {
        // TODO: Log the file handle was not successfully created
    }
    return result;
}

INTERNAL FILETIME Win32GetLastWriteTime(char *filename) {
    FILETIME last_write_time = {};
    WIN32_FILE_ATTRIBUTE_DATA data;
    if (GetFileAttributesExA(filename, GetFileExInfoStandard, &data)) {
        last_write_time = data.ftLastWriteTime;
    }
    return last_write_time;
}

INTERNAL Win32_Game_Code Win32LoadGameCode(char *source_dll_path, char *temp_dll_path) {
    Win32_Game_Code result = {};
    result.last_dll_write_time = Win32GetLastWriteTime(source_dll_path);
    if (CopyFile(source_dll_path, temp_dll_path, FALSE)) {
        result.game_code_dll = LoadLibraryA(temp_dll_path);
        if (result.game_code_dll) {
            result.update_and_render = (Game_Update_And_Render *)
                                       GetProcAddress(result.game_code_dll, "GameUpdateAndRender");
            result.is_valid = (result.update_and_render != 0);
        }
    } else {
        // TODO: Log the copy failed
    }
    if (!result.is_valid) {
        result.update_and_render = 0;
    }
    return result;
}

INTERNAL void Win32UnloadGameCode(Win32_Game_Code *game_code) {
    if (game_code->game_code_dll) {
        FreeLibrary(game_code->game_code_dll);
        game_code->game_code_dll = 0;
    }
    game_code->is_valid = false;
    game_code->update_and_render = 0;
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR command_line, int show_command_line) {
    WNDCLASS window_class      = {};
    window_class.style         = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    window_class.lpfWndProc    = Win32MainWindowCallback;
    window_class.hInstance     = instance;
    window_class.lpszClassName = "SandboxWindowClass";

    Win32ResizeDIBSection(&g_bitmap, 1280, 720

    if (RegisterClass(&window_class)) {
        HWND window = CreateWindowEx(0, window_class.lpszClassName, "Sandbox",
                                     WS_OVERLAPPEDWINDOW|WS_VISIBLE, CW_USEDEFAULT,
                                     CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                     0, 0, instance, 0);
        if (window) {
            HDC device_context = GetDC(window);

#if DEBUG
            LPVOID base_address = (LPVOID)TERABYTES(2);
#else 
            LPVOID base_address = 0;
#endif
            Game_Memory game_memory = {};
            game_memory.persisting_storage_size      = MEGABYTES(64);
            game_memory.temporary_storage_size       = GIGABYTES(4);
            game_memory.DEBUGPlatformFreeFileMemory  = DEBUGPlatformFreeFileMemory;
            game_memory.DEBUGPlatformReadEntireFile  = DEBUGplatformReadEntireFile;
            game_memory.DEBUGPlatformWriteEntireFile = DEBUGPlatformWriteEntireFile;

            U64 total_memory_size = game_memory.persisting_storage_size + 
                                    game_memory.temporary_storage_size;
            game_memory.persisting_storage = VirtualAlloc(base_address, total_memory_size,
                                                          MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
            game_memory.temporary_storage = (U8 *)game_memory.persisting_storage + 
                                                  game_memory.persisting_storage_size;
            if (game_memory.persisting_storage && game_memory.temporary_storage) {
                g_running = true;

                char exe_filename[MAX_PATH]; // Max path is a little big dodgey
                DWORD size_of_filename = GetModuleFileNameA(0, exe_filename, sizeof(exe_filename));
                if (size_of_filename == 0 || size_of_filename == sizeof(exe_filename)) {
                    // TODO: Log that the executable couldnt be found or path was too long
                    return 0;
                }

                // TODO: one_past_last_slash needs a better backup solution if GetModuleFileNameA fails
                char *one_past_last_slash = exe_filename;
                for (char *scan = exe_filename; *scan; scan++) {
                    if (*scan == '\\') {
                        one_past_last_slash = scan + 1;
                    }
                }

                char source_dll_name[] = "sandbox.dll";
                size_t source_dll_name_length_without_null_terminator = sizeof(source_dll_name) - 1;
                char source_dll_full_path[MAX_PATH];
                size_t current_path_length = one_past_last_slash - exe_filename;
                Concatenate_strings(current_path_length, exe_filename,
                                    source_dll_name_length_without_null_terminator, source_dll_name,
                                    sizeof(source_dll_full_path), source_dll_full_path);


                while (g_running) {
                    MSG message;
                    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
                        if (message.message == WM_QUIT) {
                            g_running = false;
                        }

                        TranslateMessage(&message);
                        DispatchMessageA(&message);
                    }
                    
                    Game_Bitmap bitmap = {};
                    bitmap.memory          = g_bitmap.memory;
                    bitmap.width           = g_bitmap.width;
                    bitmap.height          = g_bitmap.height;
                    bitmap.pitch           = g_bitmap.pitch;
                    bitmap.bytes_per_pixel = g_bitmap.bytes_per_pixel;

                    Win32_Window_Dimension dimension = Win32GetWindowDimension(window);
                    Win32CopyBitmapToWindow(device_context, g_bitmap, dimension.width, dimension.height);
                }
            }
        } else {
            // Error Logging
        }
    } else {
        // Error logging
    }
}

