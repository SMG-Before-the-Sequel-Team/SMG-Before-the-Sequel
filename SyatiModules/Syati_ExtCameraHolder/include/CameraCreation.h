#pragma once

#include "revolution.h"
#include "Game/Camera/Camera.h"

namespace {
    template<typename T>
    Camera* createExtCamera() {
        return new T();
    }

    struct CameraTableEntry {
        const char* mName;
        const char* mExplain;
        Camera* (*mCreationFunc)(void);
        bool mIsPublic;
    };
};
