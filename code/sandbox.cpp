
#include "staples.h"
#include "sandbox.h"

INTERNAL S32 RoundR32ToS32(R32 a) {
    S32 result = (S32)(a + 0.5f);
    return result;
}

INTERNAL void DrawRect(Game_Bitmap *bitmap, V2 min, V2 max, U32 col) {

    S32 min_x = RoundR32ToS32(min.x);
    S32 min_y = RoundR32ToS32(min.y);
    S32 max_x = RoundR32ToS32(max.x);
    S32 max_y = RoundR32ToS32(max.y);

    if (min_x < 0)              min_x = 0;
    if (max_x > bitmap->width)  max_x = bitmap->width;
    if (min_y < 0)              min_y = 0;
    if (max_y > bitmap->height) max_y = bitmap->height;

    U8 *row = (U8 *)bitmap->memory + min_x*bitmap->bytes_per_pixel + min_y*bitmap->pitch; 
    for (S32 y = min_y; y < max_y; y++) {
        U32 *pixel = (U32 *)row;
        for (S32 x = min_x; x < max_x; x++) {
            *pixel++ = col;
        }
        row += bitmap->pitch;
    }
}

// It's hidden but this function takes the parameters 
// Game_Memory *memory and Game_Bitmap *bitmap
extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender) {
    Game_State *game_state = (Game_State *)memory->persisting_storage;
    if (!memory->is_initialised) {
        memory->is_initialised = true;
    }

    V2 min = {250, 260};
    V2 max = {260, 270};
    U32 white = 0xFFFFFFFF;
    DrawRect(bitmap, V2{250.0f, 260.0f}, V2{280.0f, 290.0f}, white);
}
