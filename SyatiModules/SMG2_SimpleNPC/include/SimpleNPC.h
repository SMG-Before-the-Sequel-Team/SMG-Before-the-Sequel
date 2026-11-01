#pragma once

#include "syati.h"

// ObjArg0 Color Frame
// ObjArg1 Texture Frame
// ObjArg2 Behaviour
// ObjArg3 NpcGoods Equipment
// ObjArg4 Instance Settings

class SimpleNPC : public NPCActor {
public:
	SimpleNPC(const char* pName);

	virtual void init(const JMapInfoIter& rIter);
	virtual void control();
	virtual void calcAndSetBaseMtx();

	virtual void initNPCData(const JMapInfoIter& rIter);
	virtual void initBehaviourData(const JMapInfoIter& rIter);
	virtual void setBehaviour(s32 index);

	TakeOutStar* mTakeOutStar;
	ActorJointCtrl* mJointCtrl;
	const JMapInfo* mBehaviourData;
	s32 mRailMoveDelayTimer; // If greater than 0, the NPCs rail movement will be stopped. Decrements every frame when not talking to the NPC.
	bool mIsDynamicJointActive;
	bool mIsFaceJointActive;
	bool mIsPreventReaction; // If true, this NPC will not be able to recieve reactions to players
};

namespace NrvSimpleNPC
{
	NERVE(NrvNull);
	NERVE(NrvTalk);
	NERVE(NrvWait);
	NERVE(NrvReaction);
}

namespace MR {
	void getParamDataS32(s32* pDest, const JMapInfo* pCsvData, s32 ParamID, s32 Row);
	void getParamDataF32(f32* pDest, const JMapInfo* pCsvData, s32 ParamID, s32 Row);
	void getParamDataStrOrNULL(const char** pDest, const JMapInfo* pCsvData, s32 ParamID, s32 Row);
}