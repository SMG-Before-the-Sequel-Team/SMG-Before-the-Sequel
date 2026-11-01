#pragma once

#include "syati.h"

class SeesawMoveNut : public MapObjActor {
public:
	SeesawMoveNut(const char* pName);

	virtual void init(const JMapInfoIter& rIter);

	void exeHalfway();
	void exeEdge();

	void autoBackHome();
	bool isEnableStart();

	f32 mSpeedModifier; // _C4
	f32 mPrevSpeed; // _C8
	s32 mReturnTimer; // _CC
};

namespace NrvSeesawMoveNut {
	NERVE(SeesawMoveNutNrvHalfway);
	NERVE(SeesawMoveNutNrvEdge);
}
