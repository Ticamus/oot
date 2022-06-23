#ifndef Z_OBJ_SHUTTER_H
#define Z_OBJ_SHUTTER_H

#include "ultra64.h"
#include "global.h"

struct ObjShutter;

typedef struct ObjShutter {
    /* 0x0000 */ DynaPolyActor dyna;
                 bool open;                
} ObjShutter; // size = 0x014C

#endif
