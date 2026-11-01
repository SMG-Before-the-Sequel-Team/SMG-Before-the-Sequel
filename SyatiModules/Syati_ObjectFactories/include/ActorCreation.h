#pragma once

#include "revolution.h"
#include "Game/AreaObj/AreaObj.h"
#include "Game/NameObj/NameObjFactory.h"

// This is the base stuff for object creation

namespace {
    template<typename T>
    NameObj* createExtActor(const char* pName) {
        return new T(pName);
    }

    struct CreateSceneObjEntry {
        s32 mSlotId;
        NameObj* (*mCreationFunc)(void);
    };

    struct ChangableObjEntry {
        const char* mName;
        const char* mArchiveName;
        NameObj* (*mCreationFunc)(const char*);
    };

    struct PostCreateEntry {
        const char* mName;
        void (*mFunc)(const JMapInfoIter&);
    };
};
