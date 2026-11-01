#include "kamek/hooks.h"

#include "ModuleData_SpinDriverPathDrawInit_Ext.h"

kmWrite16(0x8045A29C + 0x02, sizeof(SpinDriverPathDrawInit_Ext)); // New Size Allocation