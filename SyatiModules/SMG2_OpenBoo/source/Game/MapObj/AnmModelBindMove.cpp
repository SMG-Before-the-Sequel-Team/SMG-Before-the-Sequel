#include "Game/MapObj/AnmModelBindMove.h"

#include "Game/Util/ActorSwitchUtil.h"
#include "Game/Util/LiveActorUtil.h"
#include "Game/Util/PlayerUtil.h"
#include "Game/Util/ActorSensorUtil.h"

AnmModelBindMove::AnmModelBindMove(const char* pName) : AnmModelObj(pName)
{

}

void AnmModelBindMove::init(const JMapInfoIter& rIter)
{
	AnmModelObj::init(rIter);

	// SMG1 would hardcode the use of SW_B, but we'll just use InitSwitch in ActorInfo
}

void AnmModelBindMove::control()
{
	if (!MR::isValidSwitchB(this))
		return;

	if (!isNerve(&NrvAnmModelObj::HostTypeMove::sInstance))
		return;

	if (!MR::isFirstStep(this))
		return;

	if (!MR::isOnSwitchB(this))
		MR::onSwitchB(this);
}

bool AnmModelBindMove::receiveOtherMsg(u32 msg, HitSensor* pSender, HitSensor* pReceiver)
{
	// Is this even correct??
	return !(msg != ACTMES_IS_REFLECTION_G_CAPTURE);
}

bool AnmModelBindMove::isOnStartAnmTrigger() const
{
	LiveActor* pRush = MR::getCurrentRushActor();
	if (!pRush)
		return false;

	return MR::isBinded(pRush, getSensor("Body"));
}

bool AnmModelBindMove::isKilledAtMoveDone() const
{
	return false;
}

bool AnmModelBindMove::isRepeat() const
{
	return true;
}

namespace MR
{
	bool isBinded(const LiveActor* pActor, HitSensor* pSensor)
	{
		return MR::isBindedGround(pActor, pSensor) || MR::isBindedRoof(pActor, pSensor) || MR::isBindedWall(pActor, pSensor);
	}

	bool isBindedGround(const LiveActor* pActor, HitSensor* pSensor)
	{
		if (!MR::isBindedGround(pActor))
			return false;
		return MR::getGroundSensor(pActor) == pSensor;
	}

	bool isBindedRoof(const LiveActor* pActor, HitSensor* pSensor)
	{
		if (!MR::isBindedRoof(pActor))
			return false;
		return MR::getRoofSensor(pActor) == pSensor;
	}

	bool isBindedWall(const LiveActor* pActor, HitSensor* pSensor)
	{
		if (!MR::isBindedWall(pActor))
			return false;
		return MR::getWallSensor(pActor) == pSensor;
	}
}