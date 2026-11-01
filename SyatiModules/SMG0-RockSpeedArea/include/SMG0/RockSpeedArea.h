#pragma once

#include "syati.h"

namespace SMG0 {
    class RockSpeedArea : public AreaObj {
    public:
        RockSpeedArea(const char* pName);

        virtual void init(const JMapInfoIter& rIter);
        virtual void movement();
        virtual const char* getManagerName() const;
    };

    void RollingRockState_GetMaxSpeed(LiveActor* pActor, f32 DefaultMaxSpeed);
};
