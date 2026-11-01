#pragma once

#include "ExtCometScreenFilter.h"

const CometScreenFilterEntry cModuleCometScreenFilterTable[] = {
	//DUMMY - This is never read
	{ "1234567890", 0 },
    { "Red", 1 },
    { "Dark", 2 },
    { "Purple", 3 },
    { "Exterminate", 4 },
    { "Mimic", 5 },
    { "Ghost", 6 },
    { "Quick", 7 },
    { "Horror", 8 },
    { "Rainbow", 9 },

};

const s32 cModuleCometScreenFilterTableCount = sizeof(cModuleCometScreenFilterTable) / sizeof(CometScreenFilterEntry);