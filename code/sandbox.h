
// Services that the platform layer provides to the game
#if DEBUG
struct Debug_Read_File_Result {
    U32   content_size;
    void *content;
};
#define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) void name(void *memory)
typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(Debug_Platform_Free_File_Memory);

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) Debug_Read_File_Result name(char *filename)
typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(Debug_Platform_Read_Entire_File);

#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) B32 name(char *filename, U32 memory_size, void *memory)
typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(Debug_Platform_Write_Entire_File);
#endif
// Services that the game provides to the platform layer

struct Game_Bitmap {
    void *memory;
    int   width;
    int   height;
    int   pitch;
    int   bytes_per_pixel;
};

struct Game_State {
    U32 *pixel_ptr;
};

struct Game_Memory {
    B32   is_initialised;
    U64   persisting_storage_size;
    void *persisting_storage; // This needs to be cleared to zero at startup
    U64   temporary_storage_size;
    void *temporary_storage;

    Debug_Platform_Free_File_Memory  *DEBUGPlatformFreeFileMemory;
    Debug_Platform_Read_Entire_File  *DEBUGPlatformReadEntireFile;
    Debug_Platform_Write_Entire_File *DEBUGPlatformWriteEntireFile;
};

#define GAME_UPDATE_AND_RENDER(name) void name(Game_Memory *memory, Game_Bitmap *bitmap)
typedef GAME_UPDATE_AND_RENDER(Game_Update_And_Render);
