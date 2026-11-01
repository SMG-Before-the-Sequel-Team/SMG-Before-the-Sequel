#pragma once

#include "ExtMarioSubMeter.h"

{{IncludeList}}

const MarioSubMeterEntry cModuleMarioSubMeterTable[] = {
	//DUMMY - This is never read
	{ "1234567890", "0" },
{{MarioSubMeterList}}
};

const s32 cModuleMarioSubMeterTableCount = sizeof(cModuleMarioSubMeterTable) / sizeof(MarioSubMeterEntry);