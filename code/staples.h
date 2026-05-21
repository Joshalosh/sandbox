
#include <stdint.h>

#define GLOBAL        static // Explicit for global variables
#define LOCAL_PERSIST static // Explicit for locally persisting variables
#define INTERNAL      static // Explicit for functions internal to the translation unit

typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef int8_t   S8;
typedef int16_t  S16;
typedef int32_t  S32;
typedef int64_t  S64;

typedef int32_t  B32;
typedef float    R32;
typedef double   R64;

#define KILOBYTES(value) ((value)*1024LL)
#define MEGABYTES(value) (KILOBYTES(value)*1024LL)
#define GIGABYTES(value) (MEGABYTES(value)*1024LL)
#define TERABYTES(value) (GIGABYTES(value)*1024LL)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ABS(a)    ((a) < (0) ? -(a) : (a))

#if DEBUG
#define ASSERT(expression) if(!(expression)) {*(int *)0 = 0;}
#else 
#define ASSERT(expression)
#endif

INTERNAL U32 SafeU64ToU32(U64 input) {
    ASSERT(input <= 0xFFFFFFFF)
    U32 result = (U32)input;
    return result;
}
