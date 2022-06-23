#include "global.h"

void TitleSetup_InitImpl(GameState* gameState) {
    /*
    // vanilla
    gameState->running = false;
    SET_NEXT_GAMESTATE(gameState, Title_Init, TitleContext);
    */

    gameState->running = false;
    gSaveContext.gameMode = 0;
    gSaveContext.linkAge = LINK_AGE_CHILD;
    Sram_InitDebugSave();
    Inventory_ChangeEquipment(EQUIP_TYPE_SHIELD, EQUIP_VALUE_SHIELD_DEKU);
    SET_EVENTCHKINF(0xA1);
    gSaveContext.dayTime = CLOCK_TIME(6, 0);
    gSaveContext.entranceIndex = ENTR_TEST01_0;
    gSaveContext.respawnFlag = 0;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex = ENTR_LOAD_OPENING;
    SET_NEXT_GAMESTATE(gameState, Play_Init, PlayState);
}

void TitleSetup_Destroy(GameState* gameState) {
}

void TitleSetup_Init(GameState* gameState) {
    gameState->destroy = TitleSetup_Destroy;
    TitleSetup_InitImpl(gameState);
}
