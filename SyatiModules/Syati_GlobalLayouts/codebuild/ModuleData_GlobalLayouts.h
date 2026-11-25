#pragma once
#include "revolution/types.h"
#include "GlobalLayouts.h"

#include "TimerLayout.h"


const CreateLayoutEntry cModuleCreateLayoutTable[] = {
	//DUMMY - This is never read
	{ "", "", NULL},
{ "TimerLayout", "/LayoutData/TimerLayout.arc", createExtLayout<TimerLayout> },

};

const s32 cModuleCreateLayoutTableCount = sizeof(cModuleCreateLayoutTable) / sizeof(CreateLayoutEntry);