#pragma once

#include "Game/Boss/TripodBossFixPartsBase.h"
#include "Game/LiveActor/HitSensor.h"
#include "Game/LiveActor/ModelObj.h"

class TripodBossCore : public TripodBossFixPartsBase {
public:
    TripodBossCore(const char *);

    virtual ~TripodBossCore();
    virtual void init(const JMapInfoIter &);
    virtual void kill();
    virtual bool receiveMsgEnemyAttack(u32, HitSensor *, HitSensor *);
    virtual void activateTripodBoss();

    void exeWait();
    void exeDamageDemo();
    void exeWarning();
    void exeBreak();
    inline void exeNonActive();

    ModelObj* mBreakModel;          // 0xE4
    ModelObj* mBloomModel;          // 0xE8
    bool mIsIndestructable; // NEW to SMG2
};

namespace NrvTripodBossCore {
    NERVE(TripodBossCoreNrvNonActive);
    NERVE(TripodBossCoreNrvWait);
    NERVE(TripodBossCoreNrvDamageDemo);
    NERVE(TripodBossCoreNrvWarning);
    NERVE(TripodBossCoreNrvBreak);
};