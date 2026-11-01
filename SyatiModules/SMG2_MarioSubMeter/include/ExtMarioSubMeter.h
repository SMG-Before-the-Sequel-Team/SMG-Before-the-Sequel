#pragma once

#include "revolution.h"
#include "Game/Screen/GameSceneLayoutHolder.h"

class SubMeterLayout;
class MarioSubMeter_Ext;

namespace {
	struct MarioSubMeterEntry {
	public:
		const char* mRefName;
		const char* mLayoutName;
		// 0 = Fly Meter (Vanishes when full, reappears only when in the air)
		// 1 = Water Meter (Vanishes when full, reappears only when underwater)
		// 2 = Do not auto disappear
		s32 mBehaviour;
		void (*mInitFunc)(SubMeterLayout*);
	};
}

MarioSubMeter_Ext* getExtMarioSubMeter();
SubMeterLayout* getMarioSubMeter(const char* pRefName);
SubMeterLayout* getCurrentMarioSubMeter();
bool activeSubMeterExt(const char* pRefName);
bool deactivateSubMeterExt(const char* pRefName);
bool requestFrameInSubMeterExt(const char* pRefName);
bool isActiveSubMeterExt(const char* pRefName);