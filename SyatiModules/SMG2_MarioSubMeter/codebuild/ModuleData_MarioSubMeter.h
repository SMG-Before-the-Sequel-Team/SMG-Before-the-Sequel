#pragma once

#include "ExtMarioSubMeter.h"

#include "Horror.h"


const MarioSubMeterEntry cModuleMarioSubMeterTable[] = {
	//DUMMY - This is never read
	{ "1234567890", "0" },
    { "SubMeterHorror", "HorrorMeter", 2, MR::initHorrorMeter },

};

const s32 cModuleMarioSubMeterTableCount = sizeof(cModuleMarioSubMeterTable) / sizeof(MarioSubMeterEntry);