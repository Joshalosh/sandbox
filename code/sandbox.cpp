
#include "staples.h"

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender) {
    Game_State *game_state = (Game_State *)memory->persisting_storage;
    if (!memory->is_initialised) {
    }
}
