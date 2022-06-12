#ifndef BOXWARP_H
#define BOXWARP_H

#include "ultra64.h"
#include "global.h"

struct BoxWarp;

typedef void (*BoxWarpActionFunc)(struct BoxWarp*, PlayState*);

typedef struct BoxWarp {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ SkelAnime skelanime;
    /* 0x01B4 */ BoxWarpActionFunc actionFunc;
    /* 0x01F9 */ u8 type;
    /* 0x01B8 */ Vec3s jointTable[5];
    /* 0x01D6 */ Vec3s morphTable[5];
    /* 0x01F4 */ s16 unk_1F4; // probably a frame count? set by player code
    /* 0x01B0 */ f32 unk_1B0; // 0-1, rotation-related, apparently unused (in z_en_box.c at least)
                 s16 entranceIndex;
                 u16 cutsceneIndex;
} BoxWarp;

#endif
