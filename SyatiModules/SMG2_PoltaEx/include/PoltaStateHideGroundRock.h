#pragma once

#include "Game/LiveActor/ActorStateBase.h"

class Polta;

class PoltaStateHideGroundRock : public ActorStateBase<Polta> {
public:
	PoltaStateHideGroundRock(Polta*);

	virtual void appear();

	void exeStart();
	void exeWait();
	void exeEnd();

	Polta* mOwner;
	s32 mAppearType; // 0 = Randomly around the entire arena || 1 = At the Player
};

namespace NrvPoltaStateHideGroundRock
{
	NERVE(PoltaStateHideGroundRockNrvStart); // Does the hiding sequence
	NERVE(PoltaStateHideGroundRockNrvWait); // Waits and spawns rocks
	NERVE(PoltaStateHideGroundRockNrvEnd); // Reappears from the ground
}