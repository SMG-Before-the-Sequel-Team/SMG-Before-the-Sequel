#include "revolution.h"
#include "kamek/hooks.h"
#include "Game/Camera/Camera.h"
#include "Game/Camera/CameraHolder.h"
#include "Game/Util/StringUtil.h"
#include "ModuleData_ExtCameraHolder.h"

extern const u8 __ct__12CameraHolderFPCc;
extern const u8 getIndexOf__12CameraHolderCFPCc;
extern const u8 getIndexOf__12CameraHolderCFP6Camera;
extern const u8 createCameras__12CameraHolderFv;

namespace {
#if defined(RMG)
    #define BASE_CAMERA_NUM 45
#elif defined(SB4)
    #define BASE_CAMERA_NUM 44
#else
    #define BASE_CAMERA_NUM 0
#endif
    kmWrite16(&createCameras__12CameraHolderFv + 0x18 + 2, (BASE_CAMERA_NUM * 4) + ((cModuleCreateCameraTableCount - 1) * 4)); // minus one for the dummy entry
    kmWrite16(&createCameras__12CameraHolderFv + 0x24 + 2, (BASE_CAMERA_NUM * 4) + ((cModuleCreateCameraTableCount - 1) * 4)); // minus one for the dummy entry

    void createExtCameras(CameraHolder* pHolder)
    {
        // Create vanilla cameras
        pHolder->createCameras();

        // Create custom cameras
        for (s32 i = 1; i < cModuleCreateCameraTableCount; i++) {
            const CameraTableEntry entry = cModuleCreateCameraTable[i];
            const s32 CameraListIdx = BASE_CAMERA_NUM + (i - 1);
            pHolder->mCameras[CameraListIdx] = entry.mCreationFunc();
            pHolder->mTranslators[CameraListIdx] = pHolder->mCameras[CameraListIdx]->createTranslator();
        }
    }
    kmCall(&__ct__12CameraHolderFPCc + 0x28, createExtCameras);

    s32 getExtIndexOf()
    {
        register const char* pName;
        __asm {
            mr pName, r28
        }

        for (s32 i = 1; i < cModuleCreateCameraTableCount; i++) {
            const CameraTableEntry entry = cModuleCreateCameraTable[i];
            const s32 CameraListIdx = BASE_CAMERA_NUM + (i - 1);
            if (MR::isEqualString(pName, entry.mName))
                return CameraListIdx;
        }
        return -1;
    }
    kmCall(&getIndexOf__12CameraHolderCFPCc + 0x54, getExtIndexOf);

    kmWrite16(&getIndexOf__12CameraHolderCFP6Camera + 0x02, BASE_CAMERA_NUM + (cModuleCreateCameraTableCount - 1));
}
