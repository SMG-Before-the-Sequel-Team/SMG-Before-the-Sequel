#pragma once

#include "Game/Boss/Polta.h"
#include "Game/Boss/PoltaFunction.h"

#define BATTLE_ARENA_RADIUS 2500.0f
#define BATTLE_MOVE_RADIUS 2100.0f

namespace PoltaFunction
{
	bool appearRedRockCircle(Polta*, const TVec3f&, f32, s32, s32);

	bool isSensorAnyPolta(const Polta*, const HitSensor*);
	bool isSensorPoltaRock(const Polta*, const HitSensor*);
	bool isSensorPoltaBombTeresa(const Polta*, const HitSensor*);

	// Spawns a bomb-boo at a random point within a circle
	bool appearBombTeresaShadowRange(const Polta*, f32, f32, const TVec3f&); // Uses appearShadow
	bool appearBombTeresaNormalRange(const Polta*, f32, f32, const TVec3f&); // Uses appearNormal

	bool appearGroundRockRange(Polta*, f32, f32, const TVec3f&);

	void rotateToHome(Polta* pPolta);
	void rotateToPoint(Polta* pPolta, const TVec3f& target);
	f32 calcToPointRotateSpeed(Polta* pPolta, const TVec3f& target);

	void offAllSensor(Polta* pPolta);
	void clampToArena(TVec3f* pOutput, const TVec3f& rCenter, f32 radius);

	void offArmRepair(Polta* pPolta);
	void onArmRepair(Polta* pPolta);
}