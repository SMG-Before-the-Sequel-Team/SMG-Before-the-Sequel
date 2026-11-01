#include "PictureBookDataStorage.h"
#include "ExtGameDataUtil.h"

#ifdef GALAXY_LEVEL_ENGINE
#include "GalaxyLevelEngine.h"
#endif


// A maximum of 8 unique picture books. That's a total of 72 chapters! (with the vanilla layout that supports 9)
// There is also a hard limit of 32 chapters
#define MAX_PICTURE_BOOK_COUNT 8

PictureBookDataStorage::PictureBookDataStorage() : BinaryDataChunkBase() {
	mDataEntries.init(MAX_PICTURE_BOOK_COUNT);

    // We need to figure out how many PictureBooks there are
    // Thankfully we can check the product table for anything using the PictureBook class
    // Just need to ensure that duplicate entries are ignored...

    // This is fine because the arc is already loaded, and this function will fetch the already loaded archive
    const JMapInfo* pProductTable = MR::createCsvParser("ProductMapObjDataTable.arc", "ProductMapObjDataTable.bcsv");
    s32 countProductTable = MR::getCsvDataElementNum(pProductTable);

    for (s32 i = 0; i < countProductTable; i++) {
        const char* pClass;
        MR::getCsvDataStr(&pClass, pProductTable, "ClassName", i);
        if (!MR::isEqualString(pClass, "PictureBook"))
            continue;
        const char* pName;
        MR::getCsvDataStr(&pName, pProductTable, "ModelName", i);

        // Verify that we don't have a save data with this name already
        for (s32 j = 0; j < mDataEntries.size(); j++) {
            if (MR::isEqualString(pName, mDataEntries[j]->mName))
                goto continue_outer; // Duplicate entry in the product table. Skip this BCSV row
        }

        { // Scoping these variables to prevent a warning with the goto
            if (mDataEntries.size() == mDataEntries.capacity()) {
                OSReport("PictureBook Data Storage is at max (%d/%d)\n", mDataEntries.size(), mDataEntries.capacity());
                break;
            }

            // If we made it here, then we need to add an entry
            PictureBookEntry* pEntry = new PictureBookEntry();
            pEntry->mName = pName;
            pEntry->mChapterReadFlags = 0b00000000000000000000000000000000;
            mDataEntries.push_back(pEntry);
        }

    continue_outer:;
    }
};

PictureBookDataStorage::~PictureBookDataStorage() {

}

u32 PictureBookDataStorage::makeHeaderHashCode() const {
    return MR::getHashCode("PictureBookStorage");
};

u32 PictureBookDataStorage::getSignature() const {
    return 'PBS1';
};

// Format
// 0x00 = u32 EntryCount
// 
// 0x04 = (u32, u32)[] Entry (raw)

u32 PictureBookDataStorage::serialize(u8* pPosition, u32 u) const {
    u32 count = mDataEntries.size();
    u32* pDataPtr = (u32*)pPosition;

    *pDataPtr++ = count;
    for (s32 i = 0; i < mDataEntries.size(); i++) {
        *pDataPtr++ = MR::getHashCode(mDataEntries[i]->mName);
        *pDataPtr++ = mDataEntries[i]->mChapterReadFlags;
    }

    return sizeof(u32) + (count * sizeof(PictureBookEntry));
};

u32 PictureBookDataStorage::deserialize(const u8* pPosition, u32 size) {
    if (size < 4)
        return 0; // Failsafe... I hope

    u32* pDataPtr = (u32*)pPosition;
    u32 count = *pDataPtr;
    pDataPtr++;

    for (s32 i = 0; i < mDataEntries.size(); i++) {
        u32 hash = MR::getHashCode(mDataEntries[i]->mName);
        // Try to load the data of each entry
        for (s32 j = 0; j < count; j++) {
            u32* pEntryPtr = pDataPtr + j;
            u32 entryHash = *pEntryPtr++;

            if (hash != entryHash)
                continue;
            mDataEntries[i]->mChapterReadFlags = *pEntryPtr;
            OSReport("%s << %08d\n", mDataEntries[i]->mName, mDataEntries[i]->mChapterReadFlags);
        }
    }

    return 0;
};

void PictureBookDataStorage::initializeData() {
    for (s32 i = 0; i < mDataEntries.size(); i++) {
        mDataEntries[i]->mChapterReadFlags = 0b00000000000000000000000000000000;
    }
};


namespace {
    PictureBookDataStorage* getPictureBookDataStorage() {
        if (ExtGameDataHolder* pHolder = ExtGameDataUtil::getCurrentGameDataHolder())
            return pHolder->mPictureBookDataStorage;
        return nullptr;
    }
}

namespace PictureBookDataUtil {
    void setReadChapter(const char* pBookName, s32 chapter, bool isRead) {
        if (chapter == 0)
            return;

        PictureBookDataStorage* pStorage = getPictureBookDataStorage();
        for (s32 i = 0; i < pStorage->mDataEntries.size(); i++) {
            if (!MR::isEqualString(pBookName, pStorage->mDataEntries[i]->mName))
                continue;

            // This is definitely cheating LOL
            char mem[sizeof(MR::BitArray)];
            MR::BitArray* tmp = (MR::BitArray*)&mem;
            tmp->mFlagCount = 32;
            tmp->mFlags = (u8*)&pStorage->mDataEntries[i]->mChapterReadFlags;
            return tmp->set(chapter - 1, isRead); // Chapters are offset by 1. Don't want to waste bits tho
        }
    }

    bool isReadChapter(const char* pBookName, s32 chapter) {
        if (chapter == 0)
            return true; // How can you "read" the table of contents???

        PictureBookDataStorage* pStorage = getPictureBookDataStorage();
        for (s32 i = 0; i < pStorage->mDataEntries.size(); i++) {
            if (!MR::isEqualString(pBookName, pStorage->mDataEntries[i]->mName))
                continue;

            // This is definitely cheating LOL
            char mem[sizeof(MR::BitArray)];
            MR::BitArray* tmp = (MR::BitArray*) & mem;
            tmp->mFlagCount = 32;
            tmp->mFlags = (u8*)&pStorage->mDataEntries[i]->mChapterReadFlags;
            return tmp->isOn(chapter - 1); // Chapters are offset by 1. Don't want to waste bits tho
        }
        return false; // Default to being unread
    }

    // -----------------------------------------------------------------

    bool isReadChapterJMapProgress(const JMapInfo* pBCSV, s32 row) {
#ifndef GALAXY_LEVEL_ENGINE
        return true;
#else
        {
            GLE::RepeatableIter startRange, endRange;
            char startBuffer[0x20];
            char endBuffer[0x20];
            GLE::createRepeatableIter(startRange, "PictureBookName%d");
            GLE::createRepeatableIter(endRange, "PictureBookChapter%d");

            while (GLE::goNextRepeatableIter(startRange, startBuffer, 0x20, pBCSV)) {
                if (!GLE::goNextRepeatableIter(endRange, endBuffer, 0x20, pBCSV))
                    break;

                const char* pName;
                MR::getCsvDataStrOrNULL(&pName, pBCSV, startBuffer, row);
                if (pName == nullptr)
                    continue;

                PictureBookDataStorage* pStorage = getPictureBookDataStorage();
                s32 idx = -1;
                for (s32 i = 0; i < pStorage->mDataEntries.size(); i++) {
                    if (!MR::isEqualString(pName, pStorage->mDataEntries[i]->mName))
                        continue;
                    idx = i;
                    break;
                }
                if (idx < 0)
                    return false; // unable to find save data, so we assume this hasn't been read

                s32 chapter;
                MR::getCsvDataS32(&chapter, pBCSV, endBuffer, row);
                if (chapter < 0 || chapter >= 32) // Can't check a non-existant chapter
                    continue;
                if (chapter == 0) { // Check if any chapter has been read before
                    if (!pStorage->mDataEntries[idx]->mChapterReadFlags)
                        return false;
                }
                else if (!isReadChapter(pName, chapter))
                    return false;
            }
        }
        return true;
#endif // !GALAXY_LEVEL_ENGINE
    }
}