#include "global.h"

#define MEMERRMSG_WIDTH 320
#define MEMERRMSG_HEIGHT 240
#define NUMBEROF_MEMERRMSGS 1

// End of RDRAM without the Expansion Pak installed
#define NORMAL_RDRAM_END 0x80400000
// End of RDRAM with the Expansion Pak installed
#define EXPANDED_RDRAM_END 0x80800000
// Address at the end of normal RDRAM after which is room for a screen buffer
#define FAULT_FB_ADDRESS (NORMAL_RDRAM_END - sizeof(u16[SCREEN_HEIGHT][SCREEN_WIDTH]))

#define SIZEOF_MEMERRMSG (MEMERRMSG_WIDTH * MEMERRMSG_HEIGHT * 4 )
// Address with enough room after to load either of the error message image files before the fault screen buffer at the
// end of RDRAM
#define CHECK_ERRMSG_STATIC_SEGMENT (u8*)(FAULT_FB_ADDRESS - SIZEOF_MEMERRMSG)

u32 sSysCfbFbPtr[2];
u32 sSysCfbEnd;

void Check_WriteRGBA16Pixel(u16* buffer, u32 x, u32 y, u32 value) {
    if (value & 1) {
        (&buffer[x])[y * SCREEN_WIDTH] = value;
    }
}

void Check_ClearRGBA16(u16* buffer) {
    u32 x;
    u32 y;

    for (y = 0; y < SCREEN_HEIGHT; y++) {
        for (x = 0; x < SCREEN_WIDTH; x++) {
            Check_WriteRGBA16Pixel(buffer, x, y, 1);
        }
    }
}

/**
 * x and y are the coordinates of the bottom-left corner.
 */
void Check_DrawRGBA16Texture(u16* buffer, s32 x, s32 y, s32 width, s32 height, u8* texture) {
    s32 v;
    s32 u;
    u8 pixelr;
    u8 pixelg;
    u8 pixelb;
    u8* pixelPtr = texture;

    // memerrms is actually RGBA32 so colors are bad and the game writes more in the ram, after what is shown
    // TODO : Write a proper RGB16 gestion. -> RRRR RGGG GGBB BBBA
    for (v = 0; v < height; v++) {
        for (u = 0; u < width; u += 1, pixelPtr++) {
            pixelr = *pixelPtr;
            pixelPtr++;
            pixelg = *pixelPtr;
            pixelPtr++;
            pixelb = *pixelPtr;
            pixelPtr++;
        

            
            Check_WriteRGBA16Pixel(buffer, x + u, y + v, GPACK_RGBA5551(pixelr,pixelg,pixelb,1));
        }
    }
}


void Check_DrawExpansionPakErrorMessage(void) {
    DmaMgr_SendRequest0(CHECK_ERRMSG_STATIC_SEGMENT, (u32)_memerrmsgSegmentRomStart, (u32)_memerrmsgSegmentRomEnd - (u32)_memerrmsgSegmentRomStart);
    Check_ClearRGBA16((u16*)FAULT_FB_ADDRESS);
    Check_DrawRGBA16Texture((u16*)FAULT_FB_ADDRESS, 0, 0, MEMERRMSG_WIDTH, MEMERRMSG_HEIGHT, CHECK_ERRMSG_STATIC_SEGMENT);
    osWritebackDCacheAll();
    osViSwapBuffer((u16*)FAULT_FB_ADDRESS);
    osViBlack(false);
}

void Check_ExpansionPak() {
    if (osMemSize >= 0x800000) {
        return;
    }

    Check_DrawExpansionPakErrorMessage();
    osDestroyThread(NULL);
    while (true) {}
}

void SysCfb_Init(s32 n64dd) {
    u32 screenSize;
    u32 tmpFbEnd;

    if (osMemSize >= 0x800000) {
        // "8MB or more memory is installed"
        osSyncPrintf("８Ｍバイト以上のメモリが搭載されています\n");
        tmpFbEnd = 0x8074BE80; //
        if (n64dd == 1) {
            osSyncPrintf("RAM 8M mode (N64DD対応)\n"); // "RAM 8M mode (N64DD compatible)"
            sSysCfbEnd = 0x805FB000;
        } else {
            // "The margin for this version is %dK bytes"
            osSyncPrintf("このバージョンのマージンは %dK バイトです\n", (0x4BC00 / 1024));
            sSysCfbEnd = tmpFbEnd;
        }
    } else if (osMemSize >= 0x400000) {
        osSyncPrintf("RAM4M mode\n");
        sSysCfbEnd = 0x80400000;
    } else {
        LogUtils_HungupThread("../sys_cfb.c", 354);
    }

    screenSize = SCREEN_WIDTH * SCREEN_HEIGHT;
    sSysCfbEnd &= ~0x3F;
    // "The final address used by the system is %08x"
    osSyncPrintf("システムが使用する最終アドレスは %08x です\n", sSysCfbEnd);
    sSysCfbFbPtr[0] = sSysCfbEnd - (screenSize * 4);
    sSysCfbFbPtr[1] = sSysCfbEnd - (screenSize * 2);
    // "Frame buffer addresses are %08x and %08x"
    osSyncPrintf("フレームバッファのアドレスは %08x と %08x です\n", sSysCfbFbPtr[0], sSysCfbFbPtr[1]);
}

void SysCfb_Reset(void) {
    sSysCfbFbPtr[0] = 0;
    sSysCfbFbPtr[1] = 0;
    sSysCfbEnd = 0;
}

u32 SysCfb_GetFbPtr(s32 idx) {
    if (idx < 2) {
        return sSysCfbFbPtr[idx];
    }
    return 0;
}

u32 SysCfb_GetFbEnd(void) {
    return sSysCfbEnd;
}
