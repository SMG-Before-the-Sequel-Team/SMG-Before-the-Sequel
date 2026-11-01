#pragma once

#include "syati.h"
#include "Game/Scene/StageDataHolder.h"

const char *getEnglishObjectName(const char *pJapaneseName);
void reportGameSceneStart();
void reportGameSceneDestroy();

struct ObjInfo {
    NameObj *pObj;      // _0
    TVec3f position;    // _4
    s32 zoneId;         // _8
};

enum SRR0ExactDebugMsg {
    POWERSTAR_NOT_FOUND,
    POWERSTAR_SETUP_INVALID,
    INVALID_STR,
    INVALID_EFFECT,
    SCENARIO_DATA_NOT_FOUND,
    INVALID_PATH,
    OUT_OF_MEMORY,
    SPAWN_MISSING,
    JUTTEXTURE_ERROR,
    INVALID_SWITCH,
    INVALID_SW_A,
    INVALID_SW_B,
    INVALID_SW_APPEAR,
    INVALID_SW_DEAD,
    INVALID_SW_PARAM,
    INVALID_MESSAGE,
    INVALID_BTK,
    INVALID_BRK,
    INVALID_BTP,
    INVALID_BPK,
    INVALID_BVA,
    INVALID_ANIMATION,
    INVALID_AREA_SHAPE,
    MISSING_MODEL
};

struct SRR0ExactDebugMsgPair {
    u32 srr0;
    s32 msgId;
};

enum SRR0RangeDebugMsg {
    CORE_SYSTEM,
    VEC_MTX_QUAT_MATH,
    SOUNDS,
    ANIMATIONS,
    MATERIALS,
    CAMERA,
    BCSV,
    ACTOR_INIT_FUNC,
    ACTOR_INIT_BCSV,
    HIT_SENSOR,
    SWITCHES,
    TEXTURES,
    MATH,
    SHADOWS,
    NERVES,
    BGM,
    MEMORY,
    CUTSCENE,
    TEXT,
    DRAWING_MATH,
    EFFECTS,
    COUNTERS,
    GAMEEVENTFLAG_INFORMATIONMESSAGE,
    POWERSTAR_RACES,
    COMETS_GALAXY_STATES,
    FILE_READS,
    FUR,
    WPAD,
    GRAVITY,
    HASHES,
    MODELS,
    LAYOUTS,
    LIGHTING,
    GENERAL_PURPOSE,
    OBJ_GROUP,
    COLLISION,
    POLYGON_MATH,
    COLORS,
    NPC,
    POWERSTAR,
    COINS,
    STAR_PIECE,
    KINOKO,
    SCENE_PLACEMENT,
    GENERAL_POS,
    RAIL,
    PLAYER,
    STAGE_SCENARIO,
    SCENE_INIT,
    SCREEN,
    STAR_POINTER,
    ASSIST_TICO,
    FUNCTION_THREAD_EXECUTION,
    SYSTEM,
    AREA_OBJ,
    PLAYER_BIND,
    JOINT,
    PAD_RECORDER,
    AUDIO,
    BOSS,
    ENEMY,
    BINDER,
    CLIPPING,
    REGULAR_OBJ,
    LOD,
    PARTS_MODEL,
    VIEW_GROUP,
    AIR,
    BUTTERFLY,
    FILE_SELECT,
    FISH,
    SKY,
    PRODUCT_MAP_OBJ,
    RACE,
    SCENARIO_SELECT,
    WATER,
    MARIO_FACE_SHIP,
    MAP_PARTS,
    CARETAKER,
    STRAY_TICO,
    DREAMER,
    GHOST,
    MII,
    CHIMP,
    EVENT,
    RABBIT,
    SUPER_DREAMER,
    INFERNO_MARIO,
    GLI_BIRD,
    TAMAKORO,
    TUBE_SLIDING,
    HOME,
    CUSTOM_CODE
};

struct SRR0RangeDebugMsgPair {
    u32 srr0Start;
    u32 srr0End;
    s32 msgId;
};

namespace MR {
    void actorSoundMovement(LiveActor *);
}