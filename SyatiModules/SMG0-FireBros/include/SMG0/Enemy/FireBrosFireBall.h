#pragma once

#include "syati.h"
#include "Game/Enemy/BrosProjectileBase.h"

class FireBrosFireBall : public BrosProjectileBase {
public:
	FireBrosFireBall(const char*);

	virtual void init(const JMapInfoIter& rIter);

	virtual void appear();

	virtual void kill();

	virtual void control();

	virtual void startThrow(const TVec3f*, const TVec3f*);

	virtual bool isNerveHold();
	virtual bool isNerveYoshiEat();
	virtual bool isNerveYoshiHold();
	virtual bool isNerveYoshiThrow();
	virtual bool tryStartYoshiEat();
	virtual bool tryStartYoshiHold();
	virtual bool tryStartYoshiThrow();

	WalkerStateBindStarPointer* _BC; //Alledgedly SupportFreeze
};

namespace NrvFireBrosFireBall {
	NERVE(FireBrosFireBallNrvSupportFreeze);
	NERVE(FireBrosFireBallNrvYoshiThrow);
	NERVE(FireBrosFireBallNrvYoshiHold);
	NERVE(FireBrosFireBallNrvYoshiEat);
	NERVE(FireBrosFireBallNrvMove);
	NERVE(FireBrosFireBallNrvHold);
}