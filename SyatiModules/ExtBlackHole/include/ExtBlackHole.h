#pragma once
#include "syati.h"

class ExtBlackHole : public BlackHole {
public:
    bool mIsTeleport;
    s32 mFadeTime;
    s32 mScreenType;
    bool mUseScream;
};

void closeType(s32 mScreenType);
void openType(s32 mScreenType);