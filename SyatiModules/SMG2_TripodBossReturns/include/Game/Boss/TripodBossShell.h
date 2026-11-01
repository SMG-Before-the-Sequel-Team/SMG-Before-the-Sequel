#pragma once

#include "Game/Boss/TripodBossFixPartsBase.h"
#include "Game/LiveActor/HitSensor.h"
#include "Game/LiveActor/ModelObj.h"

class TripodBossShell : public TripodBossFixPartsBase {
public:
    TripodBossShell(const char *);

    virtual ~TripodBossShell();
    virtual void init(const JMapInfoIter &);
    virtual void kill();
    virtual bool receiveMsgEnemyAttack(u32, HitSensor *, HitSensor *);
    virtual void activateTripodBoss();

    void exeBreak();
    inline void exeWait();
    inline void exeNonActive();
    
    ModelObj* mBreakModel;          // 0xE4
};

namespace NrvTripodBossShell {
    NERVE(TripodBossShellNrvNonActive);
    NERVE(TripodBossShellNrvWait);
    NERVE(TripodBossShellNrvBreak);
};