#pragma once

#include "syati.h"
#include "Game/Enemy/BrosBase.h"
#include "Game/Enemy/BrosBaseActionBase.h"

class FireBros : public BrosBase {
public:
	FireBros(const char*);

	virtual void init(const JMapInfoIter&);

	virtual void makeActorAppeared();
	virtual void kill();

	virtual void control();

	virtual void attackSensor(HitSensor* pSender, HitSensor* pReceiver);
	virtual bool receiveMsgPush(HitSensor* pSender, HitSensor* pReceiver);
	virtual bool receiveMsgPlayerAttack(u32 msg, HitSensor* pSender, HitSensor* pReceiver);
	virtual bool receiveMsgEnemyAttack(u32 msg, HitSensor* pSender, HitSensor* pReceiver);
	virtual bool receiveOtherMsg(u32 msg, HitSensor* pSender, HitSensor* pReceiver);

	virtual TVec3f getHoldPosition();
	virtual TVec3f getHoldRotation();
	virtual void throwProjectile(BrosProjectileBase*);

	void initActorStates();




	f32 _E0; //_E0
	s32 mStartDelay; //_E4
	TMtx34f _E8; //_E8
	BrosBaseActionBase* mStateJump; //_118
	BrosBaseActionBase* mStateAttack; //_11C
	BrosBaseActionBase* mStateSwoon; //_120
};