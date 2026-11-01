#pragma once

#include "revolution.h"
#include "Game/LiveActor/HitSensor.h"
#include "Game/MapObj/AnmModelObj.h"

class AnmModelBindMove : public AnmModelObj {
public:
	AnmModelBindMove(const char* pName);

	virtual void init(const JMapInfoIter&);

	virtual void control();

	virtual bool receiveOtherMsg(u32 msg, HitSensor* pSender, HitSensor* pReceiver);

	virtual bool isOnStartAnmTrigger() const;

	virtual bool isKilledAtMoveDone() const;
	virtual bool isRepeat() const;
};


namespace MR
{
	bool isBinded(const LiveActor*, HitSensor*);
	bool isBindedGround(const LiveActor*, HitSensor*);
	bool isBindedRoof(const LiveActor*, HitSensor*);
	bool isBindedWall(const LiveActor*, HitSensor*);
}