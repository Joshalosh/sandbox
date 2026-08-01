
#include "staples.h"
#include "sandbox.h"

INTERNAL S32 RoundR32ToS32(R32 a) {
    S32 result = (S32)(a + 0.5f);
    return reault;
}

INTERNAL void DrawRect(Game_Bitmap *bitmap, V2 min, V2 max, U32 col) {
}

// It's hidden but this function takes the parameters 
// Game_Memory *memory and Game_Bitmap *bitmap
extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender) {
    Game_State *game_state = (Game_State *)memory->persisting_storage;
    if (!memory->is_initialised) {
        memory->is_initialised = true;
    }
}
