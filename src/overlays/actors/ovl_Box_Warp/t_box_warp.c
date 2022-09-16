#include "t_box_warp.h"
#include "assets/objects/object_box/object_box.h"

#define FLAGS 0

void BoxWarp_Init(Actor* thisx, PlayState* play);
void BoxWarp_Destroy(Actor* thisx, PlayState* play);
void BoxWarp_Update(Actor* thisx, PlayState* play);
void BoxWarp_Draw(Actor* thisx, PlayState* play);

void BoxWarp_WaitOpen(BoxWarp* this, PlayState* play);
void BoxWarp_Open(BoxWarp* this, PlayState* play);

const ActorInit Box_Warp_InitVars = {
    ACTOR_BOX_WARP,
    ACTORCAT_CHEST,
    FLAGS,
    OBJECT_BOX,
    sizeof(BoxWarp),
    (ActorFunc)BoxWarp_Init,
    (ActorFunc)BoxWarp_Destroy,
    (ActorFunc)BoxWarp_Update,
    (ActorFunc)BoxWarp_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 0, ICHAIN_STOP),
};

static AnimationHeader* sAnimations[4] = { &gTreasureChestAnim_00024C, &gTreasureChestAnim_000128,
                                           &gTreasureChestAnim_00043C, &gTreasureChestAnim_00043C };

void BoxWarp_SetupAction(BoxWarp* this, BoxWarpActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void BoxWarp_Init(Actor* thisx, PlayState* play) {
    //PlayState* play = play;
    BoxWarp* this = (BoxWarp*)thisx;
    AnimationHeader* anim;
    CollisionHeader* colHeader;
    f32 animFrameStart;
    f32 endFrame;

    this->type = 0xD;
    animFrameStart = 0.0f;
    anim = sAnimations[((void)0, gSaveContext.linkAge)];

    this->entranceIndex = thisx->params & 0xFFF;
    this->cutsceneIndex = ((thisx->params >> 12) & 0xF) ? ((((thisx->params >> 12) & 0xF) - 1) | 0xFFF0) : 0;

    colHeader = NULL;
    endFrame = Animation_GetLastFrame(anim);

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DPM_UNK);

    CollisionHeader_GetVirtual(&gTreasureChestCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

    SkelAnime_Init(play, &this->skelanime, &gTreasureChestSkel, anim, this->jointTable, this->morphTable, 5);
    Animation_Change(&this->skelanime, anim, 1.5f, animFrameStart, endFrame, ANIMMODE_ONCE, 0.0f);


    Actor_SetScale(&this->dyna.actor, 0.01f);
    Actor_SetFocus(&this->dyna.actor, 40.0f);

    BoxWarp_SetupAction(this, BoxWarp_WaitOpen);
}

void BoxWarp_Destroy(Actor* thisx, PlayState* play) {
    BoxWarp* this = (BoxWarp*)thisx;
    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);

}

void BoxWarp_Update(Actor* thisx, PlayState* play) {
    BoxWarp* this = (BoxWarp*)thisx;

    this->actionFunc(this, play);

    Actor_SetFocus(&this->dyna.actor, 40.0f);

    if (this->actionFunc == BoxWarp_Open && this->skelanime.curFrame > 45) {
        this = NULL;
    }
}

/**
 * Chest is ready to be open
 */
void BoxWarp_WaitOpen(BoxWarp* this, PlayState* play) {
    f32 frameCount;
    AnimationHeader* anim;
    s32 linkAge;
    s32 pad;
    Vec3f sp4C;
    Player* player;


    if (this->unk_1F4 != 0) {
        linkAge = gSaveContext.linkAge;
        anim = sAnimations[(this->unk_1F4 < 0 ? 2 : 0) + linkAge];

        frameCount = Animation_GetLastFrame(anim);
        Animation_Change(&this->skelanime, anim, 1.5f, 0, frameCount, ANIMMODE_ONCE, 0.0f);
        BoxWarp_SetupAction(this, BoxWarp_Open);

        if (this->unk_1F4 > 0) {
            Actor_SpawnAsChild(&play->actorCtx, &this->dyna.actor, play, ACTOR_DEMO_TRE_LGT,
                        this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                        this->dyna.actor.world.pos.z, this->dyna.actor.shape.rot.x,
                        this->dyna.actor.shape.rot.y, this->dyna.actor.shape.rot.z, 0xFFFF);
            Audio_PlayFanfare(NA_BGM_OPEN_TRE_BOX | 0x900);
        }

        Flags_SetTreasure(play, 0x98);
        
    } else {
        player = GET_PLAYER(play);
        func_8002DBD0(&this->dyna.actor, &sp4C, &player->actor.world.pos);
        if (sp4C.z > -50.0f && sp4C.z < 0.0f && fabsf(sp4C.y) < 10.0f && fabsf(sp4C.x) < 20.0f &&
            Player_IsFacingActor(&this->dyna.actor, 0x3000, play)) {
            func_8002F554(&this->dyna.actor, play, 0 - (0x0980 >> 5 & 0x7F));
        }
    }
    
    
    
    


    //osSyncPrintf("Actor_Environment_Tbox_On() %d\n", this->dyna.actor.params & 0x1F);
    //Flags_SetTreasure(play, this->dyna.actor.params & 0x1F);
}

void BoxWarp_Open(BoxWarp* this, PlayState* play) {
    u16 sfxId;
    Player* player = GET_PLAYER(play);
    LinkAnimationHeader* anim;

    
    
    anim = (LINK_IS_CHILD) ? &gPlayerAnim_clink_demo_DDbox_open  : &gPlayerAnim_link_demo_DDbox_open;

    osSyncPrintf("chestwarp->1F4=%d\n", this->unk_1F4);

    this->dyna.actor.flags &= ~ACTOR_FLAG_7;

    if (SkelAnime_Update(&this->skelanime)) {
        if (Animation_OnFrame(&player->skelAnime, 82.0f)) {
            LinkAnimation_PlayOnceSetSpeed(play, &player->skelAnime, anim, 2.0f / 3.0f);
            }
        
        if (this->unk_1F4 > 0) {
            if (this->unk_1F4 < 120) {
                this->unk_1F4++;
            } else {
                Math_StepToF(&this->unk_1B0, 0.0f, 0.05f);
            }
            if (((this->unk_1F4 == 30) && LINK_IS_CHILD) || (this->unk_1F4 == 40)) {
                play->nextEntranceIndex = this->entranceIndex;
                gSaveContext.nextCutsceneIndex = this->cutsceneIndex;
                play->transitionTrigger = TRANS_TRIGGER_START;
                play->transitionType = TRANS_TYPE_FADE_BLACK_FAST;
                //gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK;
            }
        } else {
            if (this->unk_1F4 > -120) {
                this->unk_1F4--;
            } else {
                Math_StepToF(&this->unk_1B0, 0.0f, 0.05f);
            }
        }
    } else {
        sfxId = 0;
        
        if (LINK_IS_CHILD) {
            if (Animation_OnFrame(&player->skelAnime, 75.0f)) {
            LinkAnimation_PlayOnceSetSpeed(play, &player->skelAnime, anim, 2.0f / 3.0f);
            }
        }
        
        if (Animation_OnFrame(&this->skelanime, 30.0f)) {
            sfxId = NA_SE_EV_TBOX_UNLOCK;
        } else if (Animation_OnFrame(&this->skelanime, 90.0f)) {
            sfxId = NA_SE_EV_TBOX_OPEN;
        } 

        if (sfxId != 0) {
            Audio_PlaySfxGeneral(sfxId, &this->dyna.actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        }

        if (this->skelanime.jointTable[3].z > 0) {
            this->unk_1B0 = (0x7D00 - this->skelanime.jointTable[3].z) * 0.00006f;
            if (this->unk_1B0 < 0.0f) {
                this->unk_1B0 = 0.0f;
            } else if (this->unk_1B0 > 1.0f) {
                this->unk_1B0 = 1.0f;
            }
        }
    }
}

void BoxWarp_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    BoxWarp* this = (BoxWarp*)thisx;
    s32 pad;

    if (limbIndex == 1) {
        gSPMatrix((*gfx)++, Matrix_NewMtx(play->state.gfxCtx, "../z_en_box.c", 1492),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList((*gfx)++, gTreasureChestChestFrontDL);
        
    } else if (limbIndex == 3) {
        gSPMatrix((*gfx)++, Matrix_NewMtx(play->state.gfxCtx, "../z_en_box.c", 1502),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList((*gfx)++, gTreasureChestChestSideAndLidDL);
    }
}


Gfx* BoxWarp_EmptyDList(GraphicsContext* gfxCtx) {
    Gfx* dListHead;
    Gfx* dList;

    dList = Graph_Alloc(gfxCtx, sizeof(Gfx));
    ASSERT(dList != NULL, "gfxp != NULL", "../z_en_box.c", 1528);

    dListHead = dList;
    gSPEndDisplayList(dListHead++);

    return dList;
}

void BoxWarp_Draw(Actor* thisx, PlayState* play) {
    BoxWarp* this = (BoxWarp*)thisx;
    /*
    GfxPrint printer;
    Gfx* gfx;
    
    
    OPEN_DISPS(play->state.gfxCtx, __FILE__, __LINE__);

    gfx = POLY_OPA_DISP + 1;
    gSPDisplayList(OVERLAY_DISP++, gfx);

    GfxPrint_Init(&printer);
    GfxPrint_Open(&printer, gfx);

    GfxPrint_SetColor(&printer, 255, 0, 255, 255);
    GfxPrint_SetPos(&printer, 10, 10);
    GfxPrint_Printf(&printer,"this->entranceIndex = %02x", this->entranceIndex);
    GfxPrint_SetPos(&printer, 10, 11);
    GfxPrint_Printf(&printer,"this->cutsceneIndex = %02x", this->cutsceneIndex);
    GfxPrint_SetPos(&printer, 10, 12);
    GfxPrint_Printf(&printer,"thisx->params = %02x", thisx->params);
    GfxPrint_SetPos(&printer, 10, 13);
    GfxPrint_Printf(&printer,"dyna params = %04x", this->dyna.actor.params);
    
    

    gfx = GfxPrint_Close(&printer);
    GfxPrint_Destroy(&printer);

    gSPEndDisplayList(gfx++);
    gSPBranchList(POLY_OPA_DISP, gfx);
    POLY_OPA_DISP = gfx;

    CLOSE_DISPS(play->state.gfxCtx, __FILE__, __LINE__);
    */

    OPEN_DISPS(play->state.gfxCtx, __FILE__, __LINE__);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x08, BoxWarp_EmptyDList(play->state.gfxCtx));
    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    POLY_OPA_DISP = SkelAnime_Draw(play, this->skelanime.skeleton, this->skelanime.jointTable, NULL,
                                    BoxWarp_PostLimbDraw, this, POLY_OPA_DISP);

    CLOSE_DISPS(play->state.gfxCtx, __FILE__, __LINE__);
}
