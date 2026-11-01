#pragma once

#include "revolution.h"
#include "Game/Camera/CameraDirector.h"
#include "Game/Util/PlayerUtil.h"

namespace MR {
	void startGlobalEventCameraTargetPlayer(const char* pName, s32 unk1) {
		CameraTargetArg arg;
		MR::setCameraTargetToPlayer(&arg);
		MR::getCameraDirector()->startEvent(NULL, pName, arg, unk1);
	}
}