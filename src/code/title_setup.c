#include "global.h"

void TitleSetup_InitImpl(GameState* gameState) {
    /*
    // vanilla
    gameState->running = false;
    SET_NEXT_GAMESTATE(gameState, Title_Init, TitleContext);
    */

    gameState->running = false;
    gSaveContext.gameMode = 0;
    //gSaveContext.linkAge = 1;
    Sram_InitDebugSave();
    //Inventory_ChangeEquipment(EQUIP_TYPE_SHIELD, EQUIP_VALUE_SHIELD_DEKU);
    gSaveContext.dayTime = CLOCK_TIME(12, 0);
    gSaveContext.entranceIndex = ENTR_BMORI1_0;
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
