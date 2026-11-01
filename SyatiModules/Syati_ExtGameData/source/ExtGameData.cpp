#include "ModuleData_ExtGameData.h"
#include "ExtGameDataUtil.h"

// Written by VTXG & Evanbowl

#define BASE_CHUNK_COUNT 6

void extCreateChunks(ExtGameDataHolder *pHolder) {
    for (s32 i = 1; i < cExtSaveChunkCount; i++) {
        BinaryDataChunkBase *pChunk = cExtSaveChunkCreateTable[i]();

        if (pChunk) {
            // TODO Find a better way to do this
            u32 memberOffset = (u32)pHolder + sizeof(GameDataHolder) + 4 * (i - 1);
            *(u32 *)memberOffset = (u32)pChunk;

            pHolder->mBinaryDataChunkHolder->addChunk(pChunk);
        }
    }

    pHolder->resetAllData();
}

void extResetAllData(ExtGameDataHolder *pHolder) {
    ((BinaryDataChunkBase*)pHolder->mEventValueChecker)->initializeData();
    BinaryDataChunkHolder* pChunkHolder = pHolder->mBinaryDataChunkHolder;

    for (s32 i = BASE_CHUNK_COUNT; i < pChunkHolder->mNumChunks; i++) {
        pChunkHolder->mChunks[i]->initializeData();
    }
}

#if defined(SB4)
extern const u8 __ct__14GameDataHolderFv;
extern const u8 __ct__8UserFileFv;
extern const u8 resetAllData__14GameDataHolderFv;

kmWrite16(&__ct__14GameDataHolderFv + 0xDA, BASE_CHUNK_COUNT + cExtSaveChunkCount);
kmCall(&__ct__14GameDataHolderFv +0x12C, extCreateChunks);

kmWrite16(&__ct__8UserFileFv +0x2E, sizeof(ExtGameDataHolder));

kmWrite32(&resetAllData__14GameDataHolderFv +0x3C, PPC_MR(3, 31)); // mr r3, r31
kmCall(&resetAllData__14GameDataHolderFv +0x40, extResetAllData);
kmWrite32(&resetAllData__14GameDataHolderFv +0x44, PPC_B(0xC)); // b 0xC
#endif