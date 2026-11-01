#include "kamek/hooks.h"

#include "ModuleData_TalkBalloonHolder_Ext.h"

kmWrite16(0x80378FB8 + 0x02, sizeof(TalkBalloonHolder_Ext)); // New Size Allocation