
struct Win32_Bitmap {
    BITMAPINFO  info;
    void       *memory;
    int         width;
    int         height;
    int         pitch;
    int         bytes_per_pixel;
};

struct Win32_Window_Dimension {
    int width;
    int height;
};
