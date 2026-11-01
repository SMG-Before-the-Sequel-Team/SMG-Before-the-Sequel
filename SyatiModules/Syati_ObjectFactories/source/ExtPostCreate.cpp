#include "revolution.h"
#include "kamek/hooks.h"
#include "Game/Util/StringUtil.h"
#include "ModuleData_PostCreate.h"

namespace {
    void runPostCreate(const char* pName, const JMapInfoIter& rIter) {
        NameObjFactory::callPostCreation(pName, rIter);

        for (s32 i = 1; i < cModulePostCreateTableCount; i++) {
            const PostCreateEntry entry = cModulePostCreateTable[i];

            if (MR::isEqualString(entry.mName, pName))
                if (entry.mFunc != nullptr)
                    entry.mFunc(rIter);
        }
    }

    kmCall(0x8045641C, runPostCreate); // redirection hook
    kmCall(0x8045645C, runPostCreate);
}
