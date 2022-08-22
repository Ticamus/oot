/*
 * File: z_en_vase.c
 * Overlay: ovl_En_Vase
 * Description: An unused, orange pot based on ALTTP. Lacks collision.
 */

#include "z_obj_shutter.h"
#include "assets/objects/object_f53_obj/object_f53_obj.h"

#define FLAGS ACTOR_FLAG_4

void ObjShutter_Init(Actor* thisx, PlayState* play);
void ObjShutter_Destroy(Actor* thisx, PlayState* play);
void ObjShutter_Update(Actor* thisx, PlayState* play);
void ObjShutter_Draw(Actor* thisx, PlayState* play);

const ActorInit Obj_Shutter_InitVars = {
    ACTOR_OBJ_SHUTTER,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_F53_OBJ,
    sizeof(ObjShutter),
    (ActorFunc)ObjShutter_Init,
    (ActorFunc)ObjShutter_Destroy,
    (ActorFunc)ObjShutter_Update,
    (ActorFunc)ObjShutter_Draw,
};

void ObjShutter_InitDynaPoly(ObjShutter* this, PlayState* play) {
    static CollisionHeader* collisionHeaders = &gObjShutterCol;
    s32 pad;
    CollisionHeader* colHeader = NULL;
    s32 pad2[2];
    

    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(collisionHeaders, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
}

void ObjShutter_Init(Actor* thisx, PlayState* play) {
    ObjShutter* this = (ObjShutter*)thisx;

    ObjShutter_InitDynaPoly(this, play);

    this->open = (Flags_GetSwitch(play, 0x01)) ? true : false;
    if (this->open) {
        this->dyna.actor.world.pos.y += 55.0f;
    }
    Actor_SetScale(&this->dyna.actor, 0.1f);
    this->dyna.actor.focus.pos = this->dyna.actor.world.pos;
}

void ObjShutter_Destroy(Actor* thisx, PlayState* play) {
    ObjShutter* this = (ObjShutter*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void ObjShutter_Open(Actor* thisx, PlayState* play) {
    ObjShutter* this = (ObjShutter*)thisx;

    this->open = true;
    Audio_PlayActorSfx2(&this->dyna.actor, NA_SE_EV_SLIDE_DOOR_OPEN);
}


void ObjShutter_Update(Actor* thisx, PlayState* play) {
    ObjShutter* this = (ObjShutter*)thisx;
    
    //BgCheck_DrawDynaCollision(play, this->dyna.bgId);
    if (Flags_GetSwitch(play, 0x01) && !this->open) {
        ObjShutter_Open(thisx, play);
    }
    if ((this->open) && (this->dyna.actor.world.pos.y < this->dyna.actor.home.pos.y + 55.0f)) {
        Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 55.0f, 2.0f);
    }
}

void ObjShutter_Draw(Actor* thisx, PlayState* play) {
    ObjShutter* this = (ObjShutter*)thisx;
    
    
    Gfx_DrawDListOpa(play, object_f53_obj_DL_0011E0);
}
