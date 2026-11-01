#include "SMG0/Enemy/FireBros.h"
#include "SMG0/Enemy/FireBrosFireBall.h"
#include "Game/Enemy/BrosBaseStateJump.h"
#include "Game/Enemy/HammerBrosStateAttack.h"
#include "Game/Enemy/BrosBaseStateSwoon.h"
#include "Game/Player/YoshiFunction.h"

FireBros::FireBros(const char* pName) : BrosBase(pName)
{
	_E0 = 0.f;
	mStartDelay = 0;
	_E8.identity();
	mStateJump = NULL;
	mStateAttack = NULL;
	mStateSwoon = NULL;
}

void FireBros::init(const JMapInfoIter& rIter)
{
	MR::processInitFunction(this, rIter, "FireBros", false);
	BrosBase::init(rIter);
	for (s32 i = 0; i < BROSBASE_MAX_PROJECTILES; i++)
	{
		mProjectiles[i] = new FireBrosFireBall("FireBall");
		mProjectiles[i]->initWithoutIter();
	}

	if (MR::isExistRail(this))
	{
		MR::moveCoordToRailPoint(this, MR::getRailPointNum(this)-1);
	}

	MR::calcFrontVec(&mFrontVec, this);

	MR::getJMapInfoArg0NoInit(rIter, &mStartDelay);
	if (mStartDelay > 0)
		mHasStartDelay = true;

	bool ObjArg2 = false;
	MR::getJMapInfoArg2NoInit(rIter, &ObjArg2);
	if (ObjArg2 != false)
		mUseClosestPoint = false;

	initActorStates();
	initRewards(rIter);
	MR::useStageSwitchSyncAppear(this, rIter);

	mSupportFreezeStand = new WalkerStateBindStarPointer(this, mAnimScaleCtrl);
	MR::initActorState(this, mSupportFreezeStand, &NrvBrosBase::BrosBaseNrvStandSupportFreeze::sInstance, "SupportFreezeStand");

	mSupportFreezeSwoon = new WalkerStateBindStarPointer(this, mAnimScaleCtrl);
	MR::initActorState(this, mSupportFreezeSwoon, &NrvBrosBase::BrosBaseNrvSupportFreeze::sInstance, "SupportFreezeSwoon");

	MR::validateClipping(this);
}

void FireBros::initActorStates()
{
	initNerve(&NrvBrosBase::BrosBaseNrvWait::sInstance, 0x10);

	mStateJump = new BrosBaseStateJump(this);
	mStateAttack = new HammerBrosStateAttack(this);
	mStateSwoon = new BrosBaseStateSwoon(this);

	MR::initActorState(this, mStateJump, &NrvBrosBase::BrosBaseNrvMove::sInstance, "Jump");
	MR::initActorState(this, mStateAttack, &NrvBrosBase::BrosBaseNrvAttack::sInstance, "Attack");
	MR::initActorState(this, mStateSwoon, &NrvBrosBase::BrosBaseNrvStagger::sInstance, "Swoon");
}