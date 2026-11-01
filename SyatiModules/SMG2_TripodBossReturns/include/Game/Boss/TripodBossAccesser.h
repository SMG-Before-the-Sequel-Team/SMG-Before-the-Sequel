#pragma once

#include "Game/LiveActor/LiveActor.h"
#include "Game/Boss/TripodBoss.h"

class TripodBossStepPoint;

class TripodBossAccesser : public NameObj {
public:
    TripodBossAccesser(const char *);

    virtual ~TripodBossAccesser();

    void setTriPodBoss(TripodBoss *);
    void addTripodBossParts(LiveActor *);
    void showTripodBossParts();
    void hideTripodBossParts();
    void activeTripodBossParts();
    bool isTripoddBossParts(const NameObj *) const;
    static TripodBossAccesser* createSceneObj();

    TripodBoss* mBoss;                  // 0xC
    LiveActor* mParts[0x100];           // 0x10
    s32 mNumParts;                      // 0x410
};

namespace MR {
    void addTripodBossParts(LiveActor *);
    void addTripodBossPartsMovement(NameObj *);
    bool isTripoddBossParts(const NameObj *);
    void showTripodBossParts();
    void hideTripodBossParts();
    void activeTripodBossParts();
    void requestMovementTripodBossParts();
    bool isCreatedTripodBoss();
    bool isStartedTripodBoss();
    bool isBrokenTripodBoss();
    bool isDemoTripodBoss();
    bool isStartDemoTripodBoss();
    bool isDamageDemoTripodBoss();
    bool isEndBreakDownDemoTripodBoss();
    bool isLeaveMarioNowTripodBoss();
    bool isRideMarioTripodBoss();
    bool tryDamageDemoTripodBoss();
    void requestEndDamageDemoTripodBoss();
    u32 getTripodBossGravityHostID();
    void setTripodBossJointAttachBaseMatrix(const TPos3f &, s32);
    void requestStartTripodBossStepSequence(s32);
    void addTripodBossStepPoint(TripodBossStepPoint *);
    void getTripodBossJointMatrix(TPos3f *, s32);
    void getTripodBossAttachJointMatrix(TPos3f *, s32);
    void concatTripodBossAttachJointMatrix(TPos3f *, s32);
    bool isSteppingTripodBossLegID(s32);
    bool isSteppingTripodBossJointID(s32);
};
