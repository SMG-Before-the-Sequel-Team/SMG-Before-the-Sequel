#include "kamek/hooks.h"

#include "ModuleData_KameckTurtle_Ext.h"

kmWrite16(0x801A6E00 + 0x02, sizeof(KameckTurtle_Ext)); // New Size Allocation