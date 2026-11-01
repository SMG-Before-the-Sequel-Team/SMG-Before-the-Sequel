#include "SMG0/RockSpeedArea.h"

namespace SMG0 {
    const char* ManagerName = "TripodBossStepStart";

    RockSpeedArea::RockSpeedArea(const char* pName) : AreaObj(pName) {

    }

    void RockSpeedArea::init(const JMapInfoIter& rIter) {
        AreaObj::init(rIter);
        MR::connectToSceneAreaObj(this);
    }

    void RockSpeedArea::movement() {

    }

    const char* RockSpeedArea::getManagerName() const {
        return ManagerName;
    }

    void RollingRockState_GetMaxSpeed(LiveActor* pActor, f32 DefaultMaxSpeed)
    {
        register RockSpeedArea* CurrentRockSpeedArea = (RockSpeedArea*)MR::getAreaObj(ManagerName, pActor->mTranslation);
        register float MaxSpeed = DefaultMaxSpeed;

        if (CurrentRockSpeedArea == NULL || !MR::isEqualStringCase(CurrentRockSpeedArea->getManagerName(), ManagerName) || CurrentRockSpeedArea->mObjArg0 == -1)
            goto PerformRestrictVelocity; //IT'S NOT SPAGHETII I SWEAR!!

        MaxSpeed = static_cast<f32>(CurrentRockSpeedArea->mObjArg0);

    PerformRestrictVelocity:
        MR::restrictVelocity(pActor, MaxSpeed);
    }

    kmCall(0x8043EEC0, RollingRockState_GetMaxSpeed);
}
