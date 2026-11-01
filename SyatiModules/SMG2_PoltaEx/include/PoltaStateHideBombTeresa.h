#pragma once

#include "Game/LiveActor/ActorStateBase.h"

class Polta;

class PoltaStateHideBombTeresa : public ActorStateBase<Polta> {
public:
	PoltaStateHideBombTeresa(Polta*);

	virtual void appear();

	void exeStart();
	void exeWait();
	void exeEnd();

	Polta* mOwner;
	s32 mAppearType; // 0 = Randomly around the entire arena || 1 = Randomly nearby the player || 2 = At the Player
};

namespace NrvPoltaStateHideBombTeresa
{
	NERVE(PoltaStateHideBombTeresaNrvStart); // Does the hiding sequence
	NERVE(PoltaStateHideBombTeresaNrvWait); // Waits and spawns bomb boos
	NERVE(PoltaStateHideBombTeresaNrvEnd); // Reappears from the ground
}