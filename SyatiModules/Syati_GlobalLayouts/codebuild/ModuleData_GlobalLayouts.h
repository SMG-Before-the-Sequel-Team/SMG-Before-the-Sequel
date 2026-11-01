#pragma once
#include "revolution/types.h"
#include "GlobalLayouts.h"

#include "FPS.h"
#include "TimerLayout.h"


const CreateLayoutEntry cModuleCreateLayoutTable[] = {
	//DUMMY - This is never read
	{ "", "", NULL},
{ "FPSCounter", "/LayoutData/FPSCounter.arc", createExtLayout<FPSCounter> },
{ "TimerLayout", "/LayoutData/TimerLayout.arc", createExtLayout<TimerLayout> },

};

const s32 cModuleCreateLayoutTableCount = sizeof(cModuleCreateLayoutTable) / sizeof(CreateLayoutEntry);