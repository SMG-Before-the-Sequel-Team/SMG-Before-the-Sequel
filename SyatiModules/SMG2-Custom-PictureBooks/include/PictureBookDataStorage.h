#pragma once

#include "syati.h"

struct PictureBookEntry {
    const char* mName;
    u32 mChapterReadFlags;
};

class PictureBookDataStorage : public BinaryDataChunkBase {
public:
    PictureBookDataStorage();

    virtual ~PictureBookDataStorage();
    virtual u32 makeHeaderHashCode() const;
    virtual u32 getSignature() const;
    virtual u32 serialize(u8* pPosition, u32 u) const;
    virtual u32 deserialize(const u8* pPosition, u32 size);
    virtual void initializeData();


    MR::Vector<MR::AssignableArray< PictureBookEntry* > > mDataEntries;
};

namespace PictureBookDataUtil {
    void setReadChapter(const char* pBookName, s32 chapter, bool isRead = true);
    bool isReadChapter(const char* pBookName, s32 chapter);

    bool isReadChapterJMapProgress(const JMapInfo* pBCSV, s32 row);
}