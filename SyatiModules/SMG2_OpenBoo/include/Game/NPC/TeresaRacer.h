#pragma once

#include "revolution.h"
#include "Game/LiveActor/ActorCameraInfo.h"
#include "Game/LiveActor/LiveActor.h"
#include "Game/NPC/NPCActor.h"
#include "Game/Map/RaceManager.h"
#include "Game/Util/NPCUtil.h"

/*
* Spooky speedster wheeee
* 
* Author: Nintendo, duh
* Ported by SuperHackio
* 
* 
* This port works a bit differently to vanilla, as there are more ObjArgs
* 
* obj_arg0: Player Appearance mode. -1 = Both, 1 = Mario Only, 0 = Luigi Only
* obj_arg1: Use Multi Event Camera. -1 = false, 0 = true
* obj_arg2: Raster Scroll Distance. The distance to start the Raster scrolling used in Boo's Boneyard. 0 means Disabled. Defaults to 0. (Boo's Boneyard uses 2500 btw)
*/

class TeresaRacer : public NPCActor, public AbstractRacer {
public:
	TeresaRacer(const char* pName);

	virtual void init(const JMapInfoIter& rIter);

	virtual void control();

	virtual void initRacer();
	//What are the str functions...?
	virtual void prepRacer(const RaceManager*);
	virtual void startRacer();
	virtual bool updateRacer(const RaceManager*);
	virtual bool goalRacer();
	virtual void loseRacer();
	virtual void resetRacer(const RaceManager*);
	virtual void exitRacer();
	virtual void requestMovementOn();

	bool branchFunc(u32);
	bool animeFunc(u32);
	bool eventFunc(u32);

	void exeWait();
	void exePre();
	void exePost();
	void exeMove();
	void exeGoal();
	void changeBgmState();

	//These are SMG1 offsets XP
	TakeOutStar* mTakeOutStar; //_180
	ActorCameraInfo* mActorCameraInfo; //_184
	bool mIsAtGoal; //_188
	bool mObjArg1; // NEW, yes I am putting this here to save memory.
	s32 mBgmState; //_18C
	s32 mObjArg0; //_190
	f32 mObjArg2; // NEW
};

namespace NrvTeresaRacer {
	NERVE(TeresaRacerNrvWait);
	NERVE(TeresaRacerNrvPre);
	NERVE(TeresaRacerNrvReady);
	NERVE(TeresaRacerNrvMove);
	NERVE(TeresaRacerNrvGoal);
	NERVE(TeresaRacerNrvPost);
	NERVE(TeresaRacerNrvTalk);
	NERVE(TeresaRacerNrvTakeOutStar);
}

namespace MR {
	void setRasterScroll(s32, s32, s32);
	bool isRaceManagerGoalFromPathArg6(const RaceManager*, const AbstractRacer*);
	bool isRaceManagerGoalAxis(const RaceManager*, const AbstractRacer*, s32, bool);
	bool isRaceManagerGoalFull(const RaceManager*, const AbstractRacer*);
}