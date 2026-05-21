
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

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR command_line, int show_command_line) {
    WNDCLASS window_class      = {};
    window_class.style         = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    window_class.lpfWndProc    = Win32MainWindowCallback;
    window_class.hInstance     = instance;
    window_class.lpszClassName = "SandboxWindowClass";

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

                /*
                Game_Bitmap bitmap = {};
                bitmap.memory          = g_bitmap.memory;
                bitmap.width           = g_bitmap.width;
                bitmap.height          = g_bitmap.height;
                bitmap.pitch           = g_bitmap.pitch;
                bitmap.bytes_per_pixel = g_bitmap.bytes_per_pixel;
                */

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

