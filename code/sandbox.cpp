
#include "staples.h"

// It's hidden but this function takes the parameters 
// Game_Memory *memory and Game_Bitmap *bitmap
extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender) {
    Game_State *game_state = (Game_State *)memory->persisting_storage;
    if (!memory->is_initialised) {
        memory->is_initialised = true;
    }
}
