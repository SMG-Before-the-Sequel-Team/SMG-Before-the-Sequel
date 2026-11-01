#pragma once
#include "syati.h"

class LavaSteam : public LiveActor {
public:
    LavaSteam(const char* pName);
    virtual ~LavaSteam();
    virtual void init(const JMapInfoIter& rIter);
    virtual void initAfterPlacement();
    virtual void attackSensor(HitSensor* pSender, HitSensor* pReceiver);
    void startSteam();
    void startSteamSign();
    void setOff();
    void exeWait();
    void exeSteam();
    void exeSteamEnd();
    void exeOff();

    s32 mInterval;
    s32 mSignTime;
    s32 mSteamTime;
    TVec3f _9C;
    TVec3f mEffectSRTVec;
    bool mSteamForeverMode;
};

namespace NrvLavaSteam {
    NERVE(HostTypeWait);
    NERVE(HostTypeWaitForSwitchOn);
    NERVE(HostTypeSteam);
    NERVE(HostTypeSteamEnd);
    NERVE(HostTypeOff);
};