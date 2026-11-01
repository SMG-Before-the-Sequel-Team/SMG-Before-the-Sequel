#pragma once

#include "Game/LiveActor/LiveActor.h"
#include "JSystem/JGeometry/TMatrix.h"
#include "revolution/mtx.h"

class TripodBossBaseJointPosition : public LiveActor {
public:
    TripodBossBaseJointPosition(const char *);

    virtual ~TripodBossBaseJointPosition();
    virtual void init(const JMapInfoIter &);
    virtual void initAfterPlacement();
    virtual MtxPtr getBaseMtx() const;
    virtual void control();

    s32 _8C;
    TPos3f _90;
};
