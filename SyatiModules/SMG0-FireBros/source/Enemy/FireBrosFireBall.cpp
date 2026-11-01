#include "SMG0/Enemy/FireBrosFireBall.h"

FireBrosFireBall::FireBrosFireBall(const char* pName) : BrosProjectileBase(pName)
{
	_BC = NULL;
}

void FireBrosFireBall::init(const JMapInfoIter& rIter)
{
	MR::processInitFunction(this, rIter, "FireBrosFireBall", false);
	BrosProjectileBase::init(rIter);
	initNerve(&NrvFireBrosFireBall::FireBrosFireBallNrvHold::sInstance, 1);
	_BC = new WalkerStateBindStarPointer(this, mAnimScaleCtrl);
	MR::initActorState(this, _BC, &NrvFireBrosFireBall::FireBrosFireBallNrvSupportFreeze::sInstance, "SupportFreeze");
	MR::invalidateClipping(this);
	makeActorDead();
}

void decideMsgAttack(HitSensor* pReciever, HitSensor* pSender)
{
	if (MR::isEqualString(pSender->mActor->mName, "FireBall"))
		MR::sendMsgEnemyAttackFireStrong(pReciever, pSender);
	else
		MR::sendMsgEnemyAttack(pReciever, pSender);
}

kmCall(0x8018BB34, decideMsgAttack);
kmCall(0x8018BBCC, decideMsgAttack);

void decideGravity(LiveActor* pActor)
{
	MR::calcGravity(pActor);
	if (MR::isEqualString(pActor->mName, "FireBall"))
		pActor->mGravity *= 0.1f;
}

kmCall(0x8018B808, decideGravity);

f32 decideSpeed()
{
	register LiveActor* pActor;
	__asm {
		mr pActor, r28
	}
	if (MR::isEqualString(pActor->mName, "FireBall"))
		return 50.f;
	return 18.f;
}

kmCall(0x80188D6C, decideSpeed);