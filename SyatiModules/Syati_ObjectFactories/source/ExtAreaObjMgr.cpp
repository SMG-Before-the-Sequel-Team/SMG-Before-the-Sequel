#include "Game/AreaObj/AreaObjContainer.h"
#include "ModuleData_ExtAreaObjMgr.h"
void ExtAreaObjManagers(AreaObjMgr* pManager, AreaObjContainer* pContainer, const JMapInfoIter& rIter, s32 l) {
    pManager->init(rIter);
    pContainer->mManagers.assign(pManager, pContainer->mNumManagers++);

    if (l == 94) {
        for (s32 i = 1; i < cModuleCreateAreaObjMgrTableCount; i++) {
            ManagerEntry* entry = &cModuleCreateAreaObjMgrTable[i];
            pManager = entry->mCreationFunc(entry->mMaxAreas, entry->pManagerName);
            pManager->init(rIter);
            pContainer->mManagers.assign(pManager, pContainer->mNumManagers++);
        }
    }
}

kmWrite32(0x80071318, PPC_LI(3, 0x17C + cModuleCreateAreaObjMgrTableCount*4)); // li r3, 0x17C + cModuleCreateAreaObjMgrTableCount*4
kmWrite32(0x80071320, PPC_LI(0, 0x5F + cModuleCreateAreaObjMgrTableCount)); // li r0, 0x5F + cModuleCreateAreaObjMgrTableCount

kmWrite32(0x8007136C, PPC_MR(4, 25)); // mr r4, r25
kmWrite32(0x80071370, PPC_ADDI(5, 1, 0x8)); // addi r5, r1, 8
kmWrite32(0x80071374, PPC_MR(6, 27)); // mr r6, r27
kmCall(0x80071378, ExtAreaObjManagers); // bl ExtAreaObjManagers
kmWrite32(0x8007137C, PPC_B(0x18)); // b 0x18

kmWrite32(0x80071394, PPC_ADDI(27, 27, 1)); // addi r27, r27, 1
kmWrite32(0x80071398, PPC_CMPWI(27, 0x5F)); // cmpwi r27, 0x5F
kmWrite32(0x8007139C, PPC_ADDI(31, 31, 0xC)); // addi r31, r31, 0xC