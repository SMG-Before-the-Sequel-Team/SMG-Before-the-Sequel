#pragma once

#include "Game/LiveActor/ActorStateBase.h"

class Polta;

class PoltaStateHideChase : public ActorStateBase<Polta> {
public:
	PoltaStateHideChase(Polta*);

	virtual void appear();

	inline void setup(s32 a, s32 b, s32 c, f32 d, bool e) {
		mChaseStepLength = a;
		mPopDelay = b;
		mPopNum = c;
		mMoveSpeed = d;
		mUseSW_B = e;
	}

	void moveToPlayer(f32 Accel, f32 Max);

	void exeStart();
	void exeFollow();
	void exePop();
	void exeReturn();
	void exeEnd();

	Polta* mOwner;
	s32 mChaseStepLength;
	s32 mPopDelay;
	s32 mPopNum; // Counts down until it's Zero.
	f32 mMoveSpeed;
	f32 mAccel;
	bool mUseSW_B;
	bool mPopPhase;
};

namespace NrvPoltaStateHideChase
{
	NERVE(PoltaStateHideChaseNrvStart); // Does the hiding sequence
	NERVE(PoltaStateHideChaseNrvFollow); // Starts moving towards mario
	NERVE(PoltaStateHideChaseNrvPop); // Pops out of the ground
	NERVE(PoltaStateHideChaseNrvReturn); // Pops out of the ground
	NERVE(PoltaStateHideChaseNrvEnd); // Reappears from the ground
}