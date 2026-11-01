#pragma once

#include "syati-light.h"
#include "Game/LiveActor/LiveActor.h"

class BrosBase;

class BrosProjectileBase : public LiveActor {
public:
	BrosProjectileBase(const char*);

	virtual void init(const JMapInfoIter& rIter);

	virtual void appear();

	virtual void control();
	virtual void calcAndSetBaseMtx();

	virtual void attackSensor(HitSensor* pSender, HitSensor* pReceiver);

	virtual bool receiveMsgPlayerAttack(u32 msg, HitSensor* pSender, HitSensor* pReceiver);

	virtual bool receiveOtherMsg(u32 msg, HitSensor* pSender, HitSensor* pReceiver);

	virtual void startThrow(const TVec3f*, const TVec3f*) = 0;
	virtual void startHold(BrosBase*);
	virtual bool isNerveHold() = 0;
	virtual bool isNerveYoshiEat() = 0;
	virtual bool isNerveYoshiHold() = 0;
	virtual bool isNerveYoshiThrow() = 0;
	virtual bool tryStartYoshiEat() = 0;
	virtual bool tryStartYoshiHold() = 0;
	virtual bool tryStartYoshiThrow() = 0;


	BrosBase* mHolder; //_90
	AnimScaleController* mAnimScaleCtrl; //_94
	TVec3f _98; //_98
	TVec3f _A4; //_A4
	s32 _B0; //_B0 ThrowStateId?
	bool _B4; //_B4
	YoshiLockOnTarget* mYoshiLockOnTarget; //_B8
};