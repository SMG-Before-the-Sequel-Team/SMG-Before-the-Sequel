#include "kamek/hooks.h"

#include "ModuleData_SamboHead_Ext.h"

kmWrite16(0x8033DC44 + 0x02, sizeof(SamboHead_Ext)); // New Size Allocation