#include "CrashHelper.h"

NameObj *gCurrentObj;
const JMapInfoIter *gCurrentIter;
s32 gNameObjNum = 0;
s32 gLiveActorNum = 0;
s32 gAreaObjNum = 0;
ObjInfo gObjInfos[4864];

static SRR0ExactDebugMsgPair gExactDebugMsgEntries[] = {
	{ 0x802E1D98, POWERSTAR_NOT_FOUND },
	{ 0x8035BB98, POWERSTAR_SETUP_INVALID },
	{ 0x8063544C, INVALID_STR },
	{ 0x8014A3B4, INVALID_EFFECT },
	{ 0x8014A970, INVALID_EFFECT },
	{ 0x8014AA84, INVALID_EFFECT },
	{ 0x802345F0, INVALID_EFFECT },
	{ 0x804CB940, SCENARIO_DATA_NOT_FOUND },
	{ 0x804CBA98, SCENARIO_DATA_NOT_FOUND },
	{ 0x8023D620, INVALID_PATH },
	{ 0x80245F30, INVALID_PATH },
	{ 0x80266978, INVALID_PATH },
	{ 0x80053810, INVALID_PATH },
	{ 0x80508B24, OUT_OF_MEMORY },
	{ 0x80508B30, OUT_OF_MEMORY },
	{ 0x80501EE0, OUT_OF_MEMORY },
	{ 0x8045BFD4, SPAWN_MISSING },
	{ 0x8050E74C, JUTTEXTURE_ERROR },
	{ 0x8026A140, INVALID_SWITCH },
	{ 0x8026A3B0, INVALID_SW_A },
	{ 0x8026A390, INVALID_SW_A },
	{ 0x8026A3A0, INVALID_SW_A },
	{ 0x8026A3F0, INVALID_SW_B },
	{ 0x8026A3D0, INVALID_SW_B },
	{ 0x8026A3E0, INVALID_SW_B },
	{ 0x8026A410, INVALID_SW_APPEAR },
	{ 0x8026A434, INVALID_SW_DEAD },
	{ 0x8026A440, INVALID_SW_DEAD },
	{ 0x8026A820, INVALID_SW_PARAM },
	{ 0x8048C894, INVALID_MESSAGE },
	{ 0x8048C894, INVALID_MESSAGE },
	{ 0x8023BAA8, INVALID_BTK },
	{ 0x8023BAF8, INVALID_BRK },
	{ 0x8023BB48, INVALID_BTP },
	{ 0x8023BB98, INVALID_BPK },
	{ 0x8023BBD4, INVALID_BVA },
	{ 0x800693A4, INVALID_ANIMATION },
	{ 0x80070AB4, INVALID_AREA_SHAPE },
	{ 0x80232230, MISSING_MODEL }
};
const s32 gExactDebugMsgEntriesCount = sizeof(gExactDebugMsgEntries) / sizeof(SRR0ExactDebugMsgPair);

static SRR0RangeDebugMsgPair gRangeDebugMsgEntries[] = {
	{ 0x80004000, 0x80006EA0, CORE_SYSTEM },
	{ 0x80006EA0, 0x80008350, VEC_MTX_QUAT_MATH },
	{ 0x8000C150, 0x8000C170, VEC_MTX_QUAT_MATH },
	{ 0x8000D750, 0x8000D8C0, VEC_MTX_QUAT_MATH },
	{ 0x80021280, 0x80021290, VEC_MTX_QUAT_MATH },
	{ 0x80023780, 0x800237F0, VEC_MTX_QUAT_MATH },
	{ 0x80026A60, 0x80026B60, VEC_MTX_QUAT_MATH },
	{ 0x8002D710, 0x8002D750, VEC_MTX_QUAT_MATH },
	{ 0x80037040, 0x80037080, VEC_MTX_QUAT_MATH },
	{ 0x8003B5E0, 0x8003B820, VEC_MTX_QUAT_MATH },
	{ 0x8003BF50, 0x80040B90, VEC_MTX_QUAT_MATH },
	{ 0x800447A0, 0x80047510, VEC_MTX_QUAT_MATH },
	{ 0x8004AA20, 0x8004AB70, VEC_MTX_QUAT_MATH },
	{ 0x80062C30, 0x80063510, VEC_MTX_QUAT_MATH },
	{ 0x80065DF0, 0x800662A0, VEC_MTX_QUAT_MATH },
	{ 0x80008350, 0x80008680, SOUNDS },
	{ 0x80019D60, 0x80019ED0, SOUNDS },
	{ 0x80032840, 0x800329E0, SOUNDS },
	{ 0x80042240, 0x800426C0, SOUNDS },
	{ 0x8005A5B0, 0x8005BCB0, SOUNDS },
	{ 0x802205A0, 0x80222AD0, SOUNDS },
	{ 0x8022CB10, 0x8022CEE0, SOUNDS },
	{ 0x80269770, 0x80269C80, SOUNDS },
	{ 0x80008680, 0x8000AFF0, ANIMATIONS },
	{ 0x80042D70, 0x80042F80, ANIMATIONS },
	{ 0x8004F5A0, 0x8004F9D0, ANIMATIONS },
	{ 0x800692F0, 0x8006A040, ANIMATIONS },
	{ 0x8006ACC0, 0x8006F810, ANIMATIONS },
	{ 0x80228320, 0x80228D20, ANIMATIONS },
	{ 0x8022DA80, 0x8022DD40, ANIMATIONS },
	{ 0x8000AFF0, 0x8000B000, MATERIALS },
	{ 0x80530F20, 0x805330D0, MATERIALS },
	{ 0x8052F790, 0x8052FB60, MATERIALS },
	{ 0x8051F610, 0x8051FF20, MATERIALS },
	{ 0x800426C0, 0x80042D70, MATERIALS },
	{ 0x8000B000, 0x8000BF70, CAMERA },
	{ 0x8001C530, 0x8001ED20, CAMERA },
	{ 0x80047510, 0x80047A50, CAMERA },
	{ 0x8004E2F0, 0x8004E4F0, CAMERA },
	{ 0x8010BE80, 0x80133C20, CAMERA },
	{ 0x8022AB70, 0x8022AC40, CAMERA },
	{ 0x80462600, 0x80462FB0, CAMERA },
	{ 0x8000BF70, 0x8000C150, BCSV },
	{ 0x8000CFB0, 0x8000D0E0, BCSV },
	{ 0x8002BD90, 0x8002D410, BCSV },
	{ 0x80037080, 0x80037800, BCSV },
	{ 0x8004C6D0, 0x8004D2E0, BCSV },
	{ 0x800674B0, 0x80067BE0, BCSV },
	{ 0x8000C170, 0x8000C2A0, ACTOR_INIT_FUNC },
	{ 0x8000C340, 0x8000CFB0, ACTOR_INIT_FUNC },
	{ 0x8000C2A0, 0x8000C340, ACTOR_INIT_BCSV },
	{ 0x8000D0E0, 0x8000D320, HIT_SENSOR },
	{ 0x80014680, 0x80017070, HIT_SENSOR },
	{ 0x8004E970, 0x8004E9A0, HIT_SENSOR },
	{ 0x80235830, 0x80236520, HIT_SENSOR },
	{ 0x8023DEE0, 0x8023E780, HIT_SENSOR },
	{ 0x8000D320, 0x8000D620, SWITCHES },
	{ 0x8001A2C0, 0x8001AC70, SWITCHES },
	{ 0x8004BB00, 0x8004BE40, SWITCHES },
	{ 0x80060040, 0x80060170, SWITCHES },
	{ 0x80244F10, 0x80245620, SWITCHES },
	{ 0x80255B10, 0x80255EB0, SWITCHES },
	{ 0x8026A040, 0x8026B380, SWITCHES },
	{ 0x8000D620, 0x8000D750, TEXTURES },
	{ 0x8000DAD0, 0x80014680, MATH },
	{ 0x8003B1A0, 0x8003B5E0, MATH },
	{ 0x8003B820, 0x8003BF50, MATH },
	{ 0x800171F0, 0x80019D60, SHADOWS },
	{ 0x8023E780, 0x80242EE0, SHADOWS },
	{ 0x80268900, 0x80268B80, SHADOWS },
	{ 0x803861E0, 0x80386500, SHADOWS },
	{ 0x80019ED0, 0x8001A2C0, NERVES },
	{ 0x80032AE0, 0x80033850, NERVES },
	{ 0x80047A50, 0x80048580, NERVES },
	{ 0x8023C8F0, 0x8023C900, NERVES },
	{ 0x80243150, 0x80243330, NERVES },
	{ 0x8001AC70, 0x8001C430, BGM },
	{ 0x80228D20, 0x80229220, BGM },
	{ 0x8001F330, 0x8001F360, MEMORY },
	{ 0x80040B90, 0x80041380, MEMORY },
	{ 0x8001F360, 0x80021190, CUTSCENE },
	{ 0x80133C20, 0x80148160, CUTSCENE },
	{ 0x80059BE0, 0x80059D30, CUTSCENE },
	{ 0x8023C460, 0x8023C540, CUTSCENE },
	{ 0x80021190, 0x80021280, TEXT },
	{ 0x80041380, 0x80042240, TEXT },
	{ 0x8005A020, 0x8005A120, TEXT },
	{ 0x8005F000, 0x8005FAE0, TEXT },
	{ 0x80060170, 0x80060220, TEXT },
	{ 0x80060820, 0x80061570, TEXT },
	{ 0x803774B0, 0x8037D870, TEXT },
	{ 0x80021290, 0x80022AE0, DRAWING_MATH },
	{ 0x800638C0, 0x80065DF0, DRAWING_MATH },
	{ 0x8037E310, 0x8037E420, DRAWING_MATH },
	{ 0x80022AE0, 0x80023780, EFFECTS },
	{ 0x80148160, 0x8014FD90, EFFECTS },
	{ 0x80233240, 0x802347B0, EFFECTS },
	{ 0x80263590, 0x802638C0, EFFECTS },
	{ 0x8045F760, 0x80460ED0, EFFECTS },
	{ 0x80474690, 0x80474B60, EFFECTS },
	{ 0x80475BC0, 0x804790E0, EFFECTS },
	{ 0x800237F0, 0x80023CA0, COUNTERS },
	{ 0x8005A120, 0x8005A430, COUNTERS },
	{ 0x80464220, 0x80466290, COUNTERS },
	{ 0x80023CA0, 0x80024910, GAMEEVENTFLAG_INFORMATIONMESSAGE },
	{ 0x80059D30, 0x8005A020, GAMEEVENTFLAG_INFORMATIONMESSAGE },
	{ 0x804790E0, 0x80479840, GAMEEVENTFLAG_INFORMATIONMESSAGE },
	{ 0x80024910, 0x80024ED0, POWERSTAR_RACES },
	{ 0x80024ED0, 0x80025520, COMETS_GALAXY_STATES },
	{ 0x80371F20, 0x80372B90, COMETS_GALAXY_STATES },
	{ 0x80025520, 0x80025E80, FILE_READS },
	{ 0x8004BE40, 0x8004C6D0, FILE_READS },
	{ 0x80060220, 0x80060330, FILE_READS },
	{ 0x80026B60, 0x80027060, FUR },
	{ 0x80027160, 0x80027AC0, FUR },
	{ 0x80027B20, 0x80029DC0, FUR },
	{ 0x80029DC0, 0x8002B180, WPAD },
	{ 0x8004E070, 0x8004E2F0, WPAD },
	{ 0x80068B30, 0x80068D90, WPAD },
	{ 0x8002B180, 0x8002B7C0, GRAVITY },
	{ 0x80222AD0, 0x80228320, GRAVITY },
	{ 0x8002B7C0, 0x8002BD90, HASHES },
	{ 0x8002D410, 0x8002D710, MODELS },
	{ 0x8002D750, 0x8002DD50, MODELS },
	{ 0x80042F80, 0x800447A0, MODELS },
	{ 0x8006A190, 0x8006AC90, MODELS },
	{ 0x802315E0, 0x802322E0, MODELS },
	{ 0x802396E0, 0x8023A580, MODELS },
	{ 0x8023A680, 0x8023C460, MODELS },
	{ 0x8023C540, 0x8023C6D0, MODELS },
	{ 0x80242EE0, 0x80243150, MODELS },
	{ 0x803845B0, 0x803861E0, MODELS },
	{ 0x80409470, 0x80409770, MODELS },
	{ 0x8002DD50, 0x80030E70, LAYOUTS },
	{ 0x80059970, 0x80059BA0, LAYOUTS },
	{ 0x8006A040, 0x8006A190, LAYOUTS },
	{ 0x8045D340, 0x8045F760, LAYOUTS },
	{ 0x80460ED0, 0x80462600, LAYOUTS },
	{ 0x80466290, 0x8046CFE0, LAYOUTS },
	{ 0x8046EBC0, 0x80470240, LAYOUTS },
	{ 0x80470240, 0x80471240, LAYOUTS },
	{ 0x80472950, 0x80474690, LAYOUTS },
	{ 0x804755D0, 0x80475BC0, LAYOUTS },
	{ 0x8047A220, 0x8047ED40, LAYOUTS },
	{ 0x80030E70, 0x80031170, LIGHTING },
	{ 0x800329E0, 0x80032AE0, LIGHTING },
	{ 0x8022BF50, 0x8022C570, LIGHTING },
	{ 0x802354C0, 0x80235830, LIGHTING },
	{ 0x80243330, 0x80243580, LIGHTING },
	{ 0x80255EB0, 0x80256A30, LIGHTING },
	{ 0x8025A330, 0x8025C040, LIGHTING },
	{ 0x80031170, 0x80032840, GENERAL_PURPOSE },
	{ 0x80033850, 0x80034AD0, GENERAL_PURPOSE },
	{ 0x80034DA0, 0x80034F50, GENERAL_PURPOSE },
	{ 0x80035880, 0x80037040, GENERAL_PURPOSE },
	{ 0x8004AB70, 0x8004BB00, GENERAL_PURPOSE },
	{ 0x8004E4F0, 0x8004E530, GENERAL_PURPOSE },
	{ 0x8004EFA0, 0x8004F5A0, GENERAL_PURPOSE },
	{ 0x80057560, 0x80057BB0, GENERAL_PURPOSE },
	{ 0x8014FD90, 0x80150C20, GENERAL_PURPOSE },
	{ 0x802195D0, 0x8021ACB0, GENERAL_PURPOSE },
	{ 0x80229220, 0x8022AB70, GENERAL_PURPOSE },
	{ 0x8022AC40, 0x8022BCA0, GENERAL_PURPOSE },
	{ 0x8022C570, 0x8022CB10, GENERAL_PURPOSE },
	{ 0x8022CEE0, 0x8022DA80, GENERAL_PURPOSE },
	{ 0x8022F3B0, 0x8022F9D0, GENERAL_PURPOSE },
	{ 0x80034AD0, 0x80034DA0, OBJ_GROUP },
	{ 0x80060330, 0x80060410, OBJ_GROUP },
	{ 0x802384F0, 0x80238A70, OBJ_GROUP },
	{ 0x80380A60, 0x80380CB0, OBJ_GROUP },
	{ 0x80034F50, 0x80035880, COLLISION },
	{ 0x80038280, 0x80038610, COLLISION },
	{ 0x800390B0, 0x8003B1A0, COLLISION },
	{ 0x80063800, 0x800638C0, COLLISION },
	{ 0x8023C6D0, 0x8023C830, COLLISION },
	{ 0x80247B40, 0x8024B920, COLLISION },
	{ 0x80256A30, 0x80259F80, COLLISION },
	{ 0x80037800, 0x80038280, POLYGON_MATH },
	{ 0x80038610, 0x800390B0, POLYGON_MATH },
	{ 0x80048580, 0x80048650, COLORS },
	{ 0x80048650, 0x8004AA20, NPC },
	{ 0x8034B3E0, 0x80354580, NPC },
	{ 0x80355E90, 0x8035B1E0, NPC },
	{ 0x8035DA60, 0x8035F620, NPC },
	{ 0x803659C0, 0x8036EDB0, NPC },
	{ 0x80373280, 0x80376160, NPC },
	{ 0x8004D2E0, 0x8004D5D0, POWERSTAR },
	{ 0x802DF4F0, 0x802E1E10, POWERSTAR },
	{ 0x8004D5D0, 0x8004D8A0, COINS },
	{ 0x8004D8A0, 0x8004DC10, STAR_PIECE },
	{ 0x8004DC10, 0x8004DFF0, KINOKO },
	{ 0x8004DFF0, 0x8004E070, SCENE_PLACEMENT },
	{ 0x80058030, 0x800583F0, SCENE_PLACEMENT },
	{ 0x80451340, 0x80456940, SCENE_PLACEMENT },
	{ 0x804579B0, 0x8045B070, SCENE_PLACEMENT },
	{ 0x8045CFF0, 0x8045D340, SCENE_PLACEMENT },
	{ 0x80471240, 0x804725C0, SCENE_PLACEMENT },
	{ 0x8004E530, 0x8004E970, GENERAL_POS },
	{ 0x8025C040, 0x8025C450, GENERAL_POS },
	{ 0x8004EA20, 0x8004EFA0, RAIL },
	{ 0x80051970, 0x800564C0, RAIL },
	{ 0x80057D80, 0x80058030, RAIL },
	{ 0x8023D100, 0x8023DEE0, RAIL },
	{ 0x80245620, 0x802467D0, RAIL },
	{ 0x80265EE0, 0x80266AE0, RAIL },
	{ 0x8004F9D0, 0x80051970, PLAYER },
	{ 0x8037E420, 0x8037EC50, PLAYER },
	{ 0x80386500, 0x804092C0, PLAYER },
	{ 0x8040B760, 0x80422990, PLAYER },
	{ 0x80479840, 0x8047A220, PLAYER },
	{ 0x800564C0, 0x80057420, STAGE_SCENARIO },
	{ 0x80057BB0, 0x80057D80, STAGE_SCENARIO },
	{ 0x8005A430, 0x8005A4E0, STAGE_SCENARIO },
	{ 0x8045B070, 0x8045CFF0, STAGE_SCENARIO },
	{ 0x80057420, 0x80057560, SCENE_INIT },
	{ 0x80058490, 0x80059970, SCREEN },
	{ 0x80462FB0, 0x80464220, SCREEN },
	{ 0x8005BCB0, 0x8005F000, STAR_POINTER },
	{ 0x8005FAE0, 0x80060040, ASSIST_TICO },
	{ 0x80362380, 0x803659C0, ASSIST_TICO },
	{ 0x80060490, 0x80060720, FUNCTION_THREAD_EXECUTION },
	{ 0x80060770, 0x80060820, SYSTEM },
	{ 0x80061570, 0x80062C30, AREA_OBJ },
	{ 0x8006F810, 0x80077500, AREA_OBJ },
	{ 0x8025C450, 0x8025F540, AREA_OBJ },
	{ 0x800662A0, 0x80066E70, PLAYER_BIND },
	{ 0x80066E70, 0x800674B0, JOINT },
	{ 0x80067BE0, 0x800681D0, JOINT },
	{ 0x8022BCA0, 0x8022BF50, JOINT },
	{ 0x802322E0, 0x80233240, JOINT },
	{ 0x802347B0, 0x802353E0, JOINT },
	{ 0x80236520, 0x80237320, JOINT },
	{ 0x8037D870, 0x8037E310, JOINT },
	{ 0x800681D0, 0x80068B30, PAD_RECORDER },
	{ 0x80077500, 0x8008A650, AUDIO },
	{ 0x80422990, 0x8042A960, AUDIO },
	{ 0x8008A650, 0x8010BE80, BOSS },
	{ 0x80150C20, 0x802195D0, ENEMY },
	{ 0x8021ACB0, 0x802205A0, ENEMY },
	{ 0x8022DD40, 0x8022F3B0, BINDER },
	{ 0x802559E0, 0x80255B10, BINDER },
	{ 0x8022F9D0, 0x802315E0, CLIPPING },
	{ 0x80237320, 0x802384F0, REGULAR_OBJ },
	{ 0x80254110, 0x802559E0, REGULAR_OBJ },
	{ 0x80259F80, 0x8025A330, REGULAR_OBJ },
	{ 0x80266AE0, 0x80266D70, REGULAR_OBJ },
	{ 0x80267AB0, 0x80268900, REGULAR_OBJ },
	{ 0x8026E320, 0x802C7670, REGULAR_OBJ },
	{ 0x802C9B00, 0x8032F950, REGULAR_OBJ },
	{ 0x80339390, 0x80344930, REGULAR_OBJ },
	{ 0x8042A960, 0x8042ED40, REGULAR_OBJ },
	{ 0x80433310, 0x80440710, REGULAR_OBJ },
	{ 0x804410B0, 0x804442D0, REGULAR_OBJ },
	{ 0x80448F60, 0x8044B940, REGULAR_OBJ },
	{ 0x80238A70, 0x802396E0, LOD },
	{ 0x8023CF60, 0x8023D100, LOD },
	{ 0x8023C900, 0x8023CF60, PARTS_MODEL },
	{ 0x80243580, 0x80243990, VIEW_GROUP },
	{ 0x80244260, 0x80244F10, AIR },
	{ 0x802467D0, 0x80247B40, BUTTERFLY },
	{ 0x8024B920, 0x80253360, FILE_SELECT },
	{ 0x8046CFE0, 0x8046EBC0, FILE_SELECT },
	{ 0x80253360, 0x80254110, FISH },
	{ 0x8025F540, 0x8025FA40, SKY },
	{ 0x80268B80, 0x80269770, SKY },
	{ 0x80269C80, 0x80269F90, SKY },
	{ 0x8025FA40, 0x80263590, PRODUCT_MAP_OBJ },
	{ 0x802638C0, 0x80265E50, RACE },
	{ 0x80376160, 0x80376530, RACE },
	{ 0x80266D70, 0x80267AB0, SCENARIO_SELECT },
	{ 0x80456940, 0x804579B0, SCENARIO_SELECT },
	{ 0x8026B380, 0x8026E320, WATER },
	{ 0x802C7670, 0x802C9B00, MARIO_FACE_SHIP },
	{ 0x8032F950, 0x80339390, MAP_PARTS },
	{ 0x80344930, 0x80347C70, CARETAKER },
	{ 0x80347C70, 0x80348660, STRAY_TICO },
	{ 0x8035F620, 0x80360C50, STRAY_TICO },
	{ 0x80348660, 0x803492B0, DREAMER },
	{ 0x803492B0, 0x8034B3E0, GHOST },
	{ 0x8037EC50, 0x80380A60, GHOST },
	{ 0x80354580, 0x80355E90, MII },
	{ 0x80376530, 0x803774B0, CHIMP },
	{ 0x8035B1E0, 0x8035BF10, EVENT },
	{ 0x80372B90, 0x80373280, EVENT },
	{ 0x80409770, 0x8040B760, EVENT },
	{ 0x8035BF10, 0x8035DA60, RABBIT },
	{ 0x8036EDB0, 0x80371F20, RABBIT },
	{ 0x80360C50, 0x80362380, SUPER_DREAMER },
	{ 0x80380CB0, 0x803845B0, INFERNO_MARIO },
	{ 0x8042ED40, 0x80433310, GLI_BIRD },
	{ 0x804725C0, 0x80472950, GLI_BIRD },
	{ 0x80440710, 0x80440FE0, TAMAKORO },
	{ 0x804442D0, 0x80448F60, TAMAKORO },
	{ 0x8044B940, 0x80451340, TUBE_SLIDING },
	{ 0x80474B60, 0x804755D0, HOME },
	{ 0x807F0000, 0, CUSTOM_CODE }
};
const s32 gRangeDebugMsgEntriesCount = sizeof(gRangeDebugMsgEntries) / sizeof(SRR0RangeDebugMsgPair);

const char *getEnglishObjectName (const char *pJapaneseName) {
    StageDataHolder *pStageDataHolder = MR::getStageDataHolder();
    JMapInfoIter objectIter = pStageDataHolder->mObjNameTbl->findElement<const char *>("jp_name", pJapaneseName, 0);
    if (objectIter == pStageDataHolder->mObjNameTbl->end()) 
        return "(isn't exist)";
    const char *pEnglishName;
    objectIter.getValue<const char *>("en_name", &pEnglishName);
    return pEnglishName;
}

const char *getClassName (const char *pJapaneseName) {
	ProductMapCreator *pCreator = (ProductMapCreator *)MR::getSceneObjHolder()->getObj(SCENE_OBJ_PRODUCT_MAP_CREATOR);
	const char *pObjName = getEnglishObjectName(pJapaneseName);
	if (MR::isEqualString(pObjName, "(isn't exist)")) 
		pObjName = pJapaneseName;
	ProductMapEntry *pEntry = pCreator->getProductMapEntry(pObjName);
	return pEntry ? pEntry->mClassName : pObjName;
}

void reportCurrentInitObj (NameObj *pObj, const JMapInfoIter &rIter) {
	pObj->setLinkedInfo(rIter);
    gCurrentObj = pObj;
    gCurrentIter = &rIter;
	ObjInfo &info = gObjInfos[gNameObjNum];
	info.pObj = pObj;
	info.zoneId = MR::getPlacedZoneId(rIter);
	rIter.getValue("pos_x", &info.position.x);
	rIter.getValue("pos_y", &info.position.y);
	rIter.getValue("pos_z", &info.position.z);

    OSReport("------- Object Init #%d -------\n", gNameObjNum + 1);
	OSReport("Address	: %X\nNameJPN	: %s\nNameENG	: %s\nClassName	: %s\n", pObj, pObj->mName, getEnglishObjectName(pObj->mName), getClassName(pObj->mName));
	OSReport("Zone	: %s\nPosX	: %f\nPosY	: %f\nPosZ	: %f\n", MR::getZoneNameFromZoneId(info.zoneId), info.position.x, info.position.y, info.position.z);
	gNameObjNum++;
}
kmCall(0x8004AB8C, reportCurrentInitObj);

bool reportGetJMapInfoArgNoInitS32 (const JMapInfoIter &rIter, const char *pArgName, s32 *pDest) {
	s32 dest;
	const bool result = rIter.getValue<s32>(pArgName, &dest);
	if (&rIter == gCurrentIter) 
		OSReport("* Getting %s = %d (s32, %s)\n", pArgName, dest, result ? "success" : "fail");
	if (result && dest != -1) {
		*pDest = dest;
		return true;
	}
	if (*pDest != dest) 
		OSReport("   Using default value %d\n", *pDest);
	return false;
}
kmBranch(0x8002BE90, reportGetJMapInfoArgNoInitS32);

bool reportGetJMapInfoArgNoInitF32 (const JMapInfoIter &rIter, const char *pArgName, f32 *pDest) {
	s32 dest;
	const bool result = rIter.getValue<s32>(pArgName, &dest);
	if (&rIter == gCurrentIter) 
		OSReport("* Getting %s = %f (f32, %s)\n", pArgName, (f32)dest, result ? "success" : "fail");
	if (result && dest != -1) {
		*pDest = (f32)dest;
		return true;
    }
    if (*pDest != dest) 
		OSReport("   Using default value %f\n", *pDest);
	return false;
}
kmBranch(0x8002BEF0, reportGetJMapInfoArgNoInitF32);

bool reportGetJMapInfoArgNoInitBOOL (const JMapInfoIter &rIter, const char *pArgName, bool *pDest) {
	s32 dest;
	const bool result = rIter.getValue<s32>(pArgName, &dest);
	if (&rIter == gCurrentIter) 
		OSReport("* Getting %s = %s (bool, %s)\n", pArgName, dest == -1 ? "false" : "true", result ? "success" : "fail");
	if (result && dest == -1) {
		*pDest = false;
		return true;
	} else if (result) {
		*pDest = true;
		return true;
	}
	if (*pDest != dest) 
		OSReport("   Using default value %s\n", *pDest ? "true" : "false");
	return false;
}
kmBranch(0x8002BF60, reportGetJMapInfoArgNoInitBOOL);

SceneObjHolder *reportDeclarePowerStar () {
	OSReport("* Declaring a Power Star\n");
	return MR::getSceneObjHolder();
}
kmCall(0x80372DFC, reportDeclarePowerStar);

bool reportStageSwitchAppear (const JMapInfoIter &rIter) {
	if (!rIter.isValid()) 
		return false;
	s32 dest = -1;
	const bool result = rIter.getValue<s32>("SW_APPEAR", &dest);
	if (&rIter == gCurrentIter) 
		OSReport("* Using SW_APPEAR %d (%s)\n", dest, result ? "success" : "fail");
	return dest != -1;
}
kmBranch(0x8002CB10, reportStageSwitchAppear);

bool reportStageSwitchDead (const JMapInfoIter &rIter) {
	if (!rIter.isValid()) 
		return false;
	s32 dest = -1;
	const bool result = rIter.getValue<s32>("SW_DEAD", &dest);
	if (&rIter == gCurrentIter) 
		OSReport("* Using SW_DEAD %d (%s)\n", dest, result ? "success" : "fail");
	return dest != -1;
}
kmBranch(0x8002CB80, reportStageSwitchDead);

bool reportStageSwitchAwake (const JMapInfoIter &rIter) {
	if (!rIter.isValid()) 
		return false;
	s32 dest = -1;
	const bool result = rIter.getValue<s32>("SW_AWAKE", &dest);
	if (&rIter == gCurrentIter) 
		OSReport("* Using SW_AWAKE %d (%s)\n", dest, result ? "success" : "fail");
	return dest != -1;
}
kmBranch(0x8002CBF0, reportStageSwitchAwake);

bool reportStageSwitchParam (const JMapInfoIter &rIter) {
	if (!rIter.isValid()) 
		return false;
	s32 dest = -1;
	const bool result = rIter.getValue<s32>("SW_PARAM", &dest);
	if (&rIter == gCurrentIter) 
		OSReport("* Using SW_PARAM %d (%s)\n", dest, result ? "success" : "fail");
	return dest != -1;
}
kmBranch(0x8002CC60, reportStageSwitchParam);

bool reportStageSwitchA (const JMapInfoIter &rIter) {
	if (!rIter.isValid()) 
		return false;
	s32 dest = -1;
	const bool result = rIter.getValue<s32>("SW_A", &dest);
	if (&rIter == gCurrentIter) 
		OSReport("* Using SW_A %d (%s)\n", dest, result ? "success" : "fail");
	return dest != -1;
}
kmBranch(0x8002CA30, reportStageSwitchA);

bool reportStageSwitchB (const JMapInfoIter &rIter) {
	if (!rIter.isValid()) 
		return false;
	s32 dest = -1;
	const bool result = rIter.getValue<s32>("SW_B", &dest);
	if (&rIter == gCurrentIter) 
		OSReport("* Using SW_B %d (%s)\n", dest, result ? "success" : "fail");
	return dest != -1;
}
kmBranch(0x8002CAA0, reportStageSwitchB);

bool reportMessageId (const JMapInfoIter &rIter) {
	if (!rIter.isValid()) 
		return false;
	s32 dest = -1;
	const bool result = rIter.getValue<s32>("MessageId", &dest);
	if (&rIter == gCurrentIter) 
		OSReport("* Using MessageId %d (%s)\n", dest, result ? "success" : "fail");
	return dest != -1;
}
kmBranch(0x8002CCD0, reportMessageId);

void reportGameSceneStart () {
    OSReport("--- GAME SCENE START ---\n");
	gCurrentObj = 0;
}

void reportGameSceneDestroy () {
	OSReport("--- GAME SCENE DESTROY ---\n");
    gNameObjNum = 0;
    gLiveActorNum = 0;
    gAreaObjNum = 0;
	gCurrentObj = 0;
}

void reportRelatedObj (JUTConsole *pConsole) {
	ObjInfo *currentObjInfo = 0;
	for (s32 i = 0; i < gNameObjNum; i++) {
		if (gObjInfos[i].pObj == gCurrentObj) {
			currentObjInfo = &gObjInfos[i];
			break;
		}
	}
	OSReport("* Object likely related to the crash:\n   Address	: %X\n   NameJPN	: %s\n   NameENG	: %s\n   ClassName	: %s\n", gCurrentObj, gCurrentObj->mName, getEnglishObjectName(gCurrentObj->mName), getClassName(gCurrentObj->mName));
	pConsole->print_f("Object likely related to the crash:\n  Address	: %X\n  NameENG	: %s\n  ClassName	: %s\n", gCurrentObj, getEnglishObjectName(gCurrentObj->mName), getClassName(gCurrentObj->mName));
	if (currentObjInfo) {
		OSReport("   Zone	: %s\n   PosX	: %f\n   PosY	: %f\n   PosZ	: %f\n", MR::getZoneNameFromZoneId(currentObjInfo->zoneId), currentObjInfo->position.x, currentObjInfo->position.y, currentObjInfo->position.z);
		pConsole->print_f("  Zone	: %s\n  PosX	: %f\n  PosY	: %f\n  PosZ	: %f\n", MR::getZoneNameFromZoneId(currentObjInfo->zoneId), currentObjInfo->position.x, currentObjInfo->position.y, currentObjInfo->position.z);
	}
}

void reportCrash (JUTConsole *pConsole, const char *pFormatter, u32 srr0, u32 srr1) {
	pConsole->print_f(pFormatter, srr0, srr1);
	OSReport("--- CRASH ---\n");
	pConsole->print("--------------------------------------------------DEBUG HELP:\n");
	
	if (gCurrentObj) 
		reportRelatedObj(pConsole);
	if (!srr0) {
		OSReport("[The SRR0 is corrupt]\n");
		pConsole->print_f("[The SRR0 is corrupt]\n");
	} else {
		s32 msgId = -1;
		for (s32 i = 0; i < gExactDebugMsgEntriesCount; i++) {
			SRR0ExactDebugMsgPair pair = gExactDebugMsgEntries[i];
			if (pair.srr0 == srr0) {
				msgId = pair.msgId;
				break;
			}
		}
		if (msgId != -1) {
			switch (msgId) {
				case POWERSTAR_NOT_FOUND:
					OSReport("The Power Star cannot be found.\nCheck your Scenario file and Star placement.\nMake sure that it uses a valid Power Star ID.\n");
					pConsole->print_f("The Power Star cannot be found.\nCheck your Scenario file and Star placement.\nMake sure that it uses a valid Power Star ID.\n");
					break;
				case POWERSTAR_SETUP_INVALID:
					OSReport("There is an issue with your Power Star setup.\nCheck if you set the correct Power Star Trigger in your Scenario file.\nCheck if your Power Star is set up correctly according to the Scenario settings.\nCheck if you have placed your Power Star properly.\nMake sure you do not have a duplicate Power Star.\n");
					pConsole->print_f("There is an issue with your Power Star setup.\nCheck if you set the correct Power Star Trigger\nin your Scenario file.\nCheck if your Power Star is set up correctly\naccording to the Scenario settings.\nCheck if you have placed your Power Star properly.Make sure you do not have a duplicate Power Star.\n");
					break;
				case INVALID_STR:
					OSReport("The game is comparing to an invalid string.\nIn the vanilla game this might mean a path is missing.\n");
					pConsole->print_f("The game is comparing to an invalid string.\nIn the vanilla game this might mean a path\nis missing.\n");
					break;
				case INVALID_EFFECT:
					OSReport("An effect cannot be found or is invalid.\nPlease check your Effect.arc in ParticleData.\n");
					pConsole->print_f("An effect cannot be found or is invalid.\nPlease check your Effect.arc in ParticleData.\n");
					break;
				case SCENARIO_DATA_NOT_FOUND:
					OSReport("The game cannot find Scenario data for the current Galaxy.\nCheck if your Scenario.arc file exists and is valid.\nIf you are using GLE: Check if you added a .reg file for the Galaxy in ScenarioData.\n");
					pConsole->print_f("The game cannot find Scenario data for the\ncurrent Galaxy.\nCheck if your Scenario.arc file exists\nand is valid.\nIf you are using GLE: Check if you added a\n.reg file for the Galaxy in ScenarioData.\n");
					break;
				case INVALID_PATH:
					OSReport("A Path is likely missing or malformed.\n");
					pConsole->print_f("A Path is likely missing or malformed.\n");
					break;
				case OUT_OF_MEMORY:
					OSReport("You might be out of memory.\nConsider increasing the HeapSizeExcept value of your Galaxy.\n");
					pConsole->print_f("You might be out of memory.\nConsider increasing the HeapSizeExcept value\nof your Galaxy.\n");
					break;
				case SPAWN_MISSING:
					OSReport("You might be missing a Spawn point.\n");
					pConsole->print_f("You might be missing a Spawn point.\n");
					break;
				case JUTTEXTURE_ERROR:
					OSReport("An object failed to create a JUTTexture.\nThis may be caused by objects being incompatible.\n");
					pConsole->print_f("An object failed to create a JUTTexture.\nThis may be caused by objects being incompatible.");
					break;
				case INVALID_SWITCH:
					OSReport("An invalid switch was used.\n");
					pConsole->print_f("An invalid switch was used.\n");
					break;
				case INVALID_SW_A:
					OSReport("A required SW_A has not been set or is invalid.\n");
					pConsole->print_f("A required SW_A has not been set or is invalid.\n");
					break;
				case INVALID_SW_B:
					OSReport("A required SW_B has not been set or is invalid.\n");
					pConsole->print_f("A required SW_B has not been set or is invalid.\n");
					break;
				case INVALID_SW_APPEAR:
					OSReport("A required SW_APPEAR has not been set or is invalid.\n");
					pConsole->print_f("A required SW_APPEAR has not been set\nor is invalid.\n");
					break;
				case INVALID_SW_DEAD:
					OSReport("A required SW_DEAD has not been set or is invalid.\n");
					pConsole->print_f("A required SW_DEAD has not been set or is invalid.");
					break;
				case INVALID_SW_PARAM:
					OSReport("A required SW_PARAM has not been set or is invalid.\n");
					pConsole->print_f("A required SW_PARAM has not been set or is invalid");
					break;
				case INVALID_MESSAGE:
					OSReport("A message entry might be missing or mislabeled.\nIf an NPC is causing the crash: Make sure its Message ID leads to a valid message entry.\n");
					pConsole->print_f("A message entry might be missing or mislabeled.\nIf an NPC is causing the crash: Make sure its\nMessage ID leads to a valid message entry.\n");
					break;
				case INVALID_BTK:
					OSReport("A .btk animation is missing or invalid.\n");
					pConsole->print_f("A .btk animation is missing or invalid.\n");
					break;
				case INVALID_BRK:
					OSReport("A .brk animation is missing or invalid.\n");
					pConsole->print_f("A .brk animation is missing or invalid.\n");
					break;
				case INVALID_BTP:
					OSReport("A .btp animation is missing or invalid.\n");
					pConsole->print_f("A .btp animation is missing or invalid.\n");
					break;
				case INVALID_BPK:
					OSReport("A .bpk animation is missing or invalid.\n");
					pConsole->print_f("A .bpk animation is missing or invalid.\n");
					break;
				case INVALID_BVA:
					OSReport("A .bva animation is missing or invalid.\n");
					pConsole->print_f("A .bva animation is missing or invalid.\n");
					break;
				case INVALID_ANIMATION:
					OSReport("An animation might be broken or missing.\n");
					pConsole->print_f("An animation might be broken or missing.\n");
					break;
				case INVALID_AREA_SHAPE:
					OSReport("An Area tried to use an invalid area shape.\nMake sure your Areas use valid Shape IDs.\n");
					pConsole->print_f("An Area tried to use an invalid area shape.\nMake sure your Areas use valid Shape IDs.\n");
					break;
				case MISSING_MODEL:
					OSReport("A Model cannot be found.\nMake sure the model name matches the object name.\nIf you are using InitActor.bcsv: Ensure that the\n\"Model\" row name matches with the model file name.\n");
					pConsole->print_f("A Model cannot be found.\nMake sure the model name matches the object name.\nIf you are using InitActor.bcsv: Ensure that the\n\"Model\" row name matches with the model file name.");
					break;
			}
		} else {
			for (s32 i = 0; i < gRangeDebugMsgEntriesCount; i++) {
				SRR0RangeDebugMsgPair pair = gRangeDebugMsgEntries[i];
				if (srr0 >= pair.srr0Start && (pair.srr0End == 0 || srr0 < pair.srr0End)) {
					msgId = pair.msgId;
					break;
				}
			}
			if (msgId != -1) {
				switch (msgId) {
					case CORE_SYSTEM:
						OSReport("[This crash happens in core system functions,\nthis is bad]\n");
						pConsole->print_f("[This crash happens in core system functions,\nthis is bad]\n");
						break;
					case VEC_MTX_QUAT_MATH:
						OSReport("[This crash is related to vector, matrix\nor quaternion math]\n");
						pConsole->print_f("[This crash is related to vector, matrix\nor quaternion math]\n");
						break;
					case SOUNDS:
						OSReport("[This crash is related to sounds]\n");
						pConsole->print_f("[This crash is related to sounds]\n");
						break;
					case ANIMATIONS:
						OSReport("This crash is related to animations]\n");
						pConsole->print_f("This crash is related to animations]\n");
						break;
					case MATERIALS:
						OSReport("[This crash is related to materials of a 3D Model]\n");
						pConsole->print_f("[This crash is related to materials of a 3D Model]");
						break;
					case CAMERA:
						OSReport("[This crash is related to the camera]\n");
						pConsole->print_f("[This crash is related to the camera]\n");
						break;
					case BCSV:
						OSReport("[This crash is related to a BCSV]\n");
						pConsole->print_f("[This crash is related to a BCSV]\n");
						break;
					case ACTOR_INIT_FUNC:
						OSReport("[This crash happens while initializing an actor]\n[MR::processInitFunction or MR::initDefaultPos]\n");
						pConsole->print_f("[This crash happens while initializing an actor]\n[MR::processInitFunction or MR::initDefaultPos]\n");
						break;
					case ACTOR_INIT_BCSV:
						OSReport("[This crash happens while initializing an actor]\n[InitActor.bcsv]\n");
						pConsole->print_f("[This crash happens while initializing an actor]\n[InitActor.bcsv]\n");
						break;
					case HIT_SENSOR:
						OSReport("[This crash is related to HitSensors]\n");
						pConsole->print_f("[This crash is related to HitSensors]\n");
						break;
					case SWITCHES:
						OSReport("[This crash is related to switches]\n");
						pConsole->print_f("[This crash is related to switches]\n");
						break;
					case TEXTURES:
						OSReport("[This crash is related to textures]\n");
						pConsole->print_f("[This crash is related to textures]\n");
						break;
					case MATH:
						OSReport("[This crash is related to math]\n");
						pConsole->print_f("[This crash is related to math]\n");
						break;
					case SHADOWS:
						OSReport("[This crash is related to shadows]\n");
						pConsole->print_f("[This crash is related to shadows]\n");
						break;
					case NERVES:
						OSReport("[This crash is related to nerves]\n");
						pConsole->print_f("[This crash is related to nerves]\n");
						break;
					case BGM:
						OSReport("[This crash is related to background music]\n");
						pConsole->print_f("[This crash is related to background music]\n");
						break;
					case MEMORY:
						OSReport("[This crash is related to memory]\n");
						pConsole->print_f("[This crash is related to memory]\n");
						break;
					case CUTSCENE:
						OSReport("[This crash is related to a cutscene]\n");
						pConsole->print_f("[This crash is related to a cutscene]\n");
						break;
					case TEXT:
						OSReport("[This crash is related to text]\n");
						pConsole->print_f("[This crash is related to text]\n");
						break;
					case DRAWING_MATH:
						OSReport("[This crash is related to drawing math]\n");
						pConsole->print_f("[This crash is related to drawing math]\n");
						break;
					case EFFECTS:
						OSReport("[This crash is related to effects]\n");
						pConsole->print_f("[This crash is related to effects]\n");
						break;
					case COUNTERS:
						OSReport("[This crash is related to\nLife/Coin/Starbit Counters]\n");
						pConsole->print_f("[This crash is related to\nLife/Coin/Starbit Counters]\n");
						break;
					case GAMEEVENTFLAG_INFORMATIONMESSAGE:
						OSReport("[This crash is related to GameEventFlags\nand InformationMessage]\n");
						pConsole->print_f("[This crash is related to GameEventFlags\nand InformationMessage]\n");
						break;
					case POWERSTAR_RACES:
						OSReport("[This crash is related to PowerStar or Races]\n");
						pConsole->print_f("[This crash is related to PowerStar or Races]\n");
						break;
					case COMETS_GALAXY_STATES:
						OSReport("[This crash is related to Comets or Galaxy states]\n");
						pConsole->print_f("[This crash is related to Comets or Galaxy states]");
						break;
					case FILE_READS:
						OSReport("[This crash is related to File reads]\n");
						pConsole->print_f("[This crash is related to File reads]\n");
						break;
					case FUR:
						OSReport("[This crash is related to Fur]\n");
						pConsole->print_f("[This crash is related to Fur]\n");
						break;
					case WPAD:
						OSReport("[This crash is related to WPad (Wiimote)]\n");
						pConsole->print_f("[This crash is related to WPad (Wiimote)]\n");
						break;
					case GRAVITY:
						OSReport("[This crash is related to Gravity]\n");
						pConsole->print_f("[This crash is related to Gravity]\n");
						break;
					case HASHES:
						OSReport("[This crash is related to Hashes]\n");
						pConsole->print_f("[This crash is related to Hashes]\n");
						break;
					case MODELS:
						OSReport("[This crash is related to a 3D Model]\n");
						pConsole->print_f("[This crash is related to a 3D Model]\n");
						break;
					case LAYOUTS:
						OSReport("[This crash is related to layouts]\n");
						pConsole->print_f("[This crash is related to layouts]\n");
						break;
					case LIGHTING:
						OSReport("[This crash is related to lighting]\n");
						pConsole->print_f("[This crash is related to lighting]\n");
						break;
					case GENERAL_PURPOSE:
						OSReport("[This crash happens in a general-purpose function]\n");
						pConsole->print_f("[This crash happens in a general-purpose function]");
						break;
					case OBJ_GROUP:
						OSReport("[This crash happens in a function that is called\nfor all objects in a given group]\n");
						pConsole->print_f("[This crash happens in a function that is called\nfor all objects in a given group]\n");
						break;
					case COLLISION:
						OSReport("[This crash is related to collision]\n");
						pConsole->print_f("[This crash is related to collision]\n");
						break;
					case POLYGON_MATH:
						OSReport("[This crash is related to polygon math]\n");
						pConsole->print_f("[This crash is related to polygon math]\n");
						break;
					case COLORS:
						OSReport("[This crash is related to colors]\n");
						pConsole->print_f("[This crash is related to colors]\n");
						break;
					case NPC:
						OSReport("[This crash is related to an NPC]\n");
						pConsole->print_f("[This crash is related to an NPC]\n");
						break;
					case POWERSTAR:
						OSReport("[This crash is related to a PowerStar]\n");
						pConsole->print_f("[This crash is related to a PowerStar]\n");
						break;
					case COINS:
						OSReport("[This crash is related to Coins]\n");
						pConsole->print_f("[This crash is related to Coins]\n");
						break;
					case STAR_PIECE:
						OSReport("[This crash is related to Starbits]\n");
						pConsole->print_f("[This crash is related to Starbits]\n");
						break;
					case KINOKO:
						OSReport("[This crash is related to a Kinoko (Mushroom)]\n");
						pConsole->print_f("[This crash is related to a Kinoko (Mushroom)]\n");
						break;
					case SCENE_PLACEMENT:
						OSReport("[This crash is related to the scene or placement]\n");
						pConsole->print_f("[This crash is related to the scene or placement]\n");
						break;
					case GENERAL_POS:
						OSReport("[This crash is related to a GeneralPos]\n");
						pConsole->print_f("[This crash is related to a GeneralPos]\n");
						break;
					case RAIL:
						OSReport("[This crash is related to a path]\n");
						pConsole->print_f("[This crash is related to a path]\n");
						break;
					case PLAYER:
						OSReport("[This crash is related to the player]\n");
						pConsole->print_f("[This crash is related to the player]\n");
						break;
					case STAGE_SCENARIO:
						OSReport("[This crash is related to the\ncurrent stage/scenario]\n");
						pConsole->print_f("[This crash is related to the\ncurrent stage/scenario]\n");
						break;
					case SCENE_INIT:
						OSReport("[This crash happens while initializing the scene]\n");
						pConsole->print_f("[This crash happens while initializing the scene]\n");
						break;
					case SCREEN:
						OSReport("[This crash is related to the screen]\n");
						pConsole->print_f("[This crash is related to the screen]\n");
						break;
					case STAR_POINTER:
						OSReport("[This crash is related to the Star Pointer]\n");
						pConsole->print_f("[This crash is related to the Star Pointer]\n");
						break;
					case ASSIST_TICO:
						OSReport("[This crash is related to the Co-Star Luma]\n");
						pConsole->print_f("[This crash is related to the Co-Star Luma]\n");
						break;
					case FUNCTION_THREAD_EXECUTION:
						OSReport("[This crash is related to\nfunction/thread execution]\n");
						pConsole->print_f("[This crash is related to\nfunction/thread execution]\n");
						break;
					case SYSTEM:
						OSReport("[This crash is system-related]\n");
						pConsole->print_f("[This crash is system-related]\n");
						break;
					case AREA_OBJ:
						OSReport("[This crash is related to an Area]\n");
						pConsole->print_f("[This crash is related to an Area]\n");
						break;
					case PLAYER_BIND:
						OSReport("[This crash is related to a player bind]\n");
						pConsole->print_f("[This crash is related to a player bind]\n");
						break;
					case JOINT:
						OSReport("[This crash is related to a joint of a 3D Model]\n");
						pConsole->print_f("[This crash is related to a joint of a 3D Model]\n");
						break;
					case PAD_RECORDER:
						OSReport("[This crash is related to the PadRecorder]\n");
						pConsole->print_f("[This crash is related to the PadRecorder]\n");
						break;
					case AUDIO:
						OSReport("[This crash is related to audio]\n");
						pConsole->print_f("[This crash is related to audio]\n");
						break;
					case BOSS:
						OSReport("[This crash might be related to a boss]\n");
						pConsole->print_f("[This crash might be related to a boss]\n");
						break;
					case ENEMY:
						OSReport("[This crash might be related to an enemy]\n");
						pConsole->print_f("[This crash might be related to an enemy]\n");
						break;
					case BINDER:
						OSReport("[This crash is related to a Binder]\n");
						pConsole->print_f("[This crash is related to a Binder]\n");
						break;
					case CLIPPING:
						OSReport("[This crash is related to Clipping]\n");
						pConsole->print_f("[This crash is related to Clipping]\n");
						break;
					case REGULAR_OBJ:
						OSReport("[This crash might be related to a regular object]\n");
						pConsole->print_f("[This crash might be related to a regular object]\n");
						break;
					case LOD:
						OSReport("[This crash is related to low-detailed rendering]\n");
						pConsole->print_f("[This crash is related to low-detailed rendering]\n");
						break;
					case PARTS_MODEL:
						OSReport("[This crash is related to a PartsModel]\n");
						pConsole->print_f("[This crash is related to a PartsModel]\n");
						break;
					case VIEW_GROUP:
						OSReport("[This crash is related to a View Group]\n");
						pConsole->print_f("[This crash is related to a View Group]\n");
						break;
					case AIR:
						OSReport("[This crash is related to Air]\n");
						pConsole->print_f("[This crash is related to Air]\n");
						break;
					case BUTTERFLY:
						OSReport("[This crash is related to a Butterfly]\n");
						pConsole->print_f("[This crash is related to a Butterfly]\n");
						break;
					case FILE_SELECT:
						OSReport("[This crash is related to FileSelect]\n");
						pConsole->print_f("[This crash is related to FileSelect]\n");
						break;
					case FISH:
						OSReport("[This crash is related to a Fish]\n");
						pConsole->print_f("[This crash is related to a Fish]\n");
						break;
					case SKY:
						OSReport("[This crash is related to a skybox]\n");
						pConsole->print_f("[This crash is related to a skybox]\n");
						break;
					case PRODUCT_MAP_OBJ:
						OSReport("[This crash is related to a ProductMapObj]\n");
						pConsole->print_f("[This crash is related to a ProductMapObj]\n");
						break;
					case RACE:
						OSReport("[This crash is related to a Race]\n");
						pConsole->print_f("[This crash is related to a Race]\n");
						break;
					case SCENARIO_SELECT:
						OSReport("[This crash is related to ScenarioSelect]\n");
						pConsole->print_f("[This crash is related to a View Group]\n");
						break;
					case WATER:
						OSReport("[This crash is related to Water]\n");
						pConsole->print_f("[This crash is related to Water]\n");
						break;
					case MARIO_FACE_SHIP:
						OSReport("[This crash is related to Starship Mario]\n");
						pConsole->print_f("[This crash is related to Starship Mario]\n");
						break;
					case MAP_PARTS:
						OSReport("[This crash might be related to a MapPart]\n");
						pConsole->print_f("[This crash might be related to a MapPart]\n");
						break;
					case CARETAKER:
						OSReport("[This crash is related to a Caretaker (Gearmo)]\n");
						pConsole->print_f("[This crash is related to a Caretaker (Gearmo)]\n");
						break;
					case STRAY_TICO:
						OSReport("[This crash is related to Silver Stars]\n");
						pConsole->print_f("[This crash is related to Silver Stars]\n");
						break;
					case DREAMER:
						OSReport("[This crash is related to Dreamer (Tip Network)]\n");
						pConsole->print_f("[This crash is related to Dreamer (Tip Network)]\n");
						break;
					case GHOST:
						OSReport("[This crash is related to a Ghost]\n");
						pConsole->print_f("[This crash is related to a Ghost]\n");
						break;
					case MII:
						OSReport("[This crash is related to Miis]\n");
						pConsole->print_f("[This crash is related to Miis]\n");
						break;
					case CHIMP:
						OSReport("[This crash is related to a Chimp minigame]\n");
						pConsole->print_f("[This crash is related to a Chimp minigame]\n");
						break;
					case EVENT:
						OSReport("[This crash is related to an Event]\n");
						pConsole->print_f("[This crash is related to an Event]\n");
						break;
					case RABBIT:
						OSReport("[This crash is related to a Rabbit]\n");
						pConsole->print_f("[This crash is related to a Rabbit]\n");
						break;
					case SUPER_DREAMER:
						OSReport("[This crash is related to SuperDreamer\n(Cosmic Spirit)]\n");
						pConsole->print_f("[This crash is related to SuperDreamer\n(Cosmic Spirit)]\n");
						break;
					case INFERNO_MARIO:
						OSReport("[This crash is related to Cosmic Clones]\n");
						pConsole->print_f("[This crash is related to Cosmic Clones]\n");
						break;
					case GLI_BIRD:
						OSReport("[This crash is related to GliBird (Fluzzard)]\n");
						pConsole->print_f("[This crash is related to GliBird (Fluzzard)]\n");
						break;
					case TAMAKORO:
						OSReport("[This crash is related to Tamakoro (Star Ball)]\n");
						pConsole->print_f("[This crash is related to Tamakoro (Star Ball)]\n");
						break;
					case TUBE_SLIDING:
						OSReport("[This crash is related to Tube Sliding]\n");
						pConsole->print_f("[This crash is related to Tube Sliding]\n");
						break;
					case HOME:
						OSReport("[This crash is related to the HOME menu]\n");
						pConsole->print_f("[This crash is related to the HOME menu]\n");
						break;
					case CUSTOM_CODE:
						OSReport("[This crash is related to Custom Code]\n");
						pConsole->print_f("[This crash is related to Custom Code]\n");
						break;
				}
				OSReport("[To get more information, look up the SRR0 and LR Save addresses.]\n");
				pConsole->print_f("[To get more information, look up the SRR0\nand LR Save addresses.]\n");
			} else {
				OSReport("[The SRR0 is unknown]\n");
				pConsole->print_f("[The SRR0 is unknown]\n");
			}
		}
	}
	OSReport("(SRR0: %X)\n", srr0);
	pConsole->print("--------------------------------------------------");
}
#if defined(PAL) || defined(USA) || defined(JPN)
    kmBranch(0x8050FE2C, reportCrash);
#elif defined(TWN) || defined(KOR)
    kmBranch(0x8050FEBC, reportCrash);
#endif

extern "C" {
	void __ptmf_scall();
}
void updateCurrentObj (NameObj *pObj) {
	gCurrentObj = pObj;
	__ptmf_scall(); // Call movement function	
}
kmCall(0x8033A168, updateCurrentObj);