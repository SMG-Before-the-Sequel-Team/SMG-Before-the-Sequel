#pragma once

#include "JSystem/JGeometry/TVec.h"

class LiveActor;
class StarPointerOnOffController;

void setupCommandStream_StarPointerOnOffController(StarPointerOnOffController*);

namespace MR {
	bool tryStartStarPointerCommandStream(const LiveActor* pActor, const TVec3f* pVec, s32 padChan, bool b);
	bool tryEndStarPointerCommandStream(const LiveActor* pActor, s32 padChan);
	bool isStarPointerCommandStream(const LiveActor* pActor, s32 padChan);
}