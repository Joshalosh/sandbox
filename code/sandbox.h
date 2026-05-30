
// Services that the platform layer provides to the game


// Services that the game provides to the platform layer

struct Game_Bitmap {
    void *memory;
    int   widht;
    int   height;
    int   pitch;
    int   bytes_per_pixel;
};

#define GAME_UPDATE_AND_RENDER(name) void name(Game_Memory *memory, Game_Bitmap *bitmap)
typedef GAME_UPDATE_AND_RENDER(Game_Update_And_Render)
