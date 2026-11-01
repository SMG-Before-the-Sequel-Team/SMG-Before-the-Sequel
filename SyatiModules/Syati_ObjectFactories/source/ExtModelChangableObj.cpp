#include "revolution.h"
#include "kamek/hooks.h"
#include "Game/Util/StringUtil.h"
#include "ModuleData_ModelChangableObj.h"

namespace {
    extern "C"
    {
        void* __kAutoMap_80339410(const char*); // The symbol maps do not have this function labeled correctly
    }

    void* getModelChangableObjCreator(const char* pName) {
        void* creator = __kAutoMap_80339410(pName);

        if (!creator) {
            for (s32 i = 1; i < cNewModelChangableObjTableCount; i++) {
                const ChangableObjEntry func = cNewModelChangableObjTable[i];

                bool v7;
                if (func.mName != NULL) {
                    v7 = strncmp(pName, func.mName, strlen(func.mName)) == 0;
                }
                else {
                    v7 = strcmp(pName, func.mArchiveName) == 0;
                }

                if (v7) {
                    creator = func.mCreationFunc;
                    break;
                }
            }
        }

        return creator;
    }

    kmCall(0x804564C8, getModelChangableObjCreator); // redirection hook
}
