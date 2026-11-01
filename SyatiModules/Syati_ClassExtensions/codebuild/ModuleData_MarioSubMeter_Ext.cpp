#include "kamek/hooks.h"

#include "ModuleData_MarioSubMeter_Ext.h"

kmWrite16(0x80471690 + 0x02, sizeof(MarioSubMeter_Ext)); // New Size Allocation