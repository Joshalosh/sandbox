
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
        } else {
            // Error Logging
        }
    } else {
        // Error logging
    }
}

