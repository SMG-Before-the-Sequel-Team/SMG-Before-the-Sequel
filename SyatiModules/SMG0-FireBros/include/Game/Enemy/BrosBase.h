#pragma once

#include "syati-light.h"
#include "Game/LiveActor/LiveActor.h"
#include "Game/Enemy/BrosProjectileBase.h"
#include "Game/LiveActor/ActorJointCtrl.h"
#include "Game/Player/YoshiLockOnTarget.h"
#include "Game/Enemy/WalkerStateBindStarPointer.h"

#define BROSBASE_MAX_PROJECTILES 5

class BrosBase : public LiveActor {
public:
	BrosBase(const char*);

	virtual ~BrosBase();
	virtual void init(const JMapInfoIter&);

	virtual void calcAndSetBaseMtx();

	virtual TVec3f getHoldPosition() = 0;
	virtual TVec3f getHoldRotation() = 0;
	virtual void throwProjectile(BrosProjectileBase*) = 0;


	void initRewards(const JMapInfoIter&);
	void tryAppearStarPiece();
	void tryAppearCoinPop();
	void getHoldMtx(MtxPtr);
	bool isStand();
	bool isAllowYoshiEat();
	void updateProjectile();

	BrosProjectileBase* mProjectiles[BROSBASE_MAX_PROJECTILES]; //_90, _94, _98, _9C, _A0
	s32 _A4; //_A4
	s32 _A8; //_A8
	TVec3f mFrontVec; //_AC
	bool mUseClosestPoint; //_B8
	bool mHasStartDelay; //_B9
	s32 mRewardType; //_BC
	AnimScaleController* mAnimScaleCtrl; //_C0
	ActorJointCtrl* mActorJointCtrl; //_C4
	YoshiLockOnTarget* mYoshiLockOnTarget; //_C8
	s32 _CC; //_CC
	MapObjConnector* mMapObjConnector; //_D0
	bool _D4; //_D4
	WalkerStateBindStarPointer* mSupportFreezeStand; //_D8
	WalkerStateBindStarPointer* mSupportFreezeSwoon; //_DC
};

static TVec3f sHandPosition;
static TVec3f sHandRotation;

//THIS IS WRONG!!
//These belong to HammerBros
namespace NrvBrosBase {
	NERVE(BrosBaseNrvWait);
	NERVE(BrosBaseNrvStartDelay);
	NERVE(BrosBaseNrvMove);
	NERVE(BrosBaseNrvAttack);
	NERVE(BrosBaseNrvStagger);
	NERVE(BrosBaseNrvBlowDown);
	NERVE(BrosBaseNrvPressDown);
	NERVE(BrosBaseNrvStandPressDown);
	NERVE(BrosBaseNrvGuard);
	NERVE(BrosBaseNrvStandSupportFreeze);
	NERVE(BrosBaseNrvSupportFreeze);
	NERVE(BrosBaseNrvEndFaceCtrl); //Never executed ?
}