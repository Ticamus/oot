#include "global.h"
#include "config.h"

void Setup_InitImpl(SetupState* this) {
    /*osSyncPrintf("ゼルダ共通データ初期化\n"); // "Zelda common data initalization"
    SaveContext_Init();
    this->state.running = false;
    #if (defined BOOT_TO_SCENE || defined BOOT_TO_FILE_SELECT)
        SET_NEXT_GAMESTATE(&this->state, FileSelect_Init, FileSelectState);
    #elif (defined BOOT_TO_MAP_SELECT)
        SET_NEXT_GAMESTATE(&this->state, MapSelect_Init, MapSelectState);
    #else
        SET_NEXT_GAMESTATE(&this->state, ConsoleLogo_Init, ConsoleLogoState);
    #endif
}*/
    
    // vanilla
    osSyncPrintf("ゼルダ共通データ初期化\n"); // "Zelda common data initalization"
    SaveContext_Init();
    this->state.running = false;
    SET_NEXT_GAMESTATE(&this->state, ConsoleLogo_Init, ConsoleLogoState);
    
   
    this->state.running = false;
    gSaveContext.gameMode = 0;
    gSaveContext.linkAge = LINK_AGE_CHILD;
    Sram_InitDebugSave();
    Inventory_ChangeEquipment(EQUIP_TYPE_SHIELD, EQUIP_VALUE_SHIELD_DEKU);
    SET_EVENTCHKINF(0xA1);
    gSaveContext.language = 2;
    gSaveContext.dayTime = CLOCK_TIME(6, 0);
    gSaveContext.entranceIndex = ENTR_YDAN_0;
    gSaveContext.respawnFlag = 0;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex = ENTR_LOAD_OPENING;
    SET_NEXT_GAMESTATE(&this->state, Play_Init, PlayState);
}

void Setup_Destroy(GameState* thisx) {
}

void Setup_Init(GameState* thisx) {
    SetupState* this = (SetupState*)thisx;

    this->state.destroy = Setup_Destroy;
    Setup_InitImpl(this);
}
