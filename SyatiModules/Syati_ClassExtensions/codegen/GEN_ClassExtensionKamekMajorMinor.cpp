#include "kamek/hooks.h"

#include "ModuleData_{{ClassName}}_Ext.h"

#if defined(SB4W) || defined(SB4K)
kmWrite16({{HookAddrMinor}} + 0x02, sizeof({{ClassName}}_Ext)); // New Size Allocation
#elif defined(SB4E) || defined(SB4P) || defined(SB4J)
kmWrite16({{HookAddrMajor}} + 0x02, sizeof({{ClassName}}_Ext)); // New Size Allocation
#endif