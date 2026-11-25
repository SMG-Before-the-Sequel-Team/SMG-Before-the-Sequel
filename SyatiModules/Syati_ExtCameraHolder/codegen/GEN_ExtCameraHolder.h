#pragma once

#include "CameraCreation.h"

{{IncludeList}}


const CameraTableEntry cModuleCreateCameraTable[] = {
	//DUMMY - This is never read
	{ "1234567890", "1234567890", NULL, false},
{{ExtCameraHolderList}}
};

const s32 cModuleCreateCameraTableCount = sizeof(cModuleCreateCameraTable) / sizeof(CameraTableEntry);