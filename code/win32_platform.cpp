
#include <windows.h>
#include <stdio.h>

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
        } else {
            // Error Logging
        }
    } else {
        // Error logging
    }
}

