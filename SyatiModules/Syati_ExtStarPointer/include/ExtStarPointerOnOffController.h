#pragma once

#include "ModuleData_ExtStarPointer.h"
#include "Game/System/NerveExecutor.h"
#include "Game/Util/Array.h"
#include "Game/System/StarPointerOnOffController.h"

class ExtStarPointerOnOffController : public NerveExecutor {
public:
	struct RequestInfo {
	public:
		/* 0x00 */ void* mRequester;
		/* 0x04 */ s32 mMode;
	};
	// !!!DO NOT USE THIS!!!
	// This function is not required at all
	inline ExtStarPointerOnOffController() : NerveExecutor(nullptr) {} // This will never be run... right?
	

	bool _8; // 0x08
	MR::FixedArray< RequestInfo*, 16 > mRequests; // 0x0C
	s32 mModeCounter[ExtStarPointerMode_Count]; // 0x4C
	bool mRequested[ExtStarPointerMode_Count];
	s32 mMode;
};