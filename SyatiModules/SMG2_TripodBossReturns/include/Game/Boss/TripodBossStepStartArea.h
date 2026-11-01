#pragma once

#include "Game/AreaObj/AreaObj.h"

class TripodBossStepStartArea : public AreaObj {
public:
    TripodBossStepStartArea(const char *);

    virtual ~TripodBossStepStartArea();
    virtual void init(const JMapInfoIter &);
    virtual void movement();
    virtual const char* getManagerName() const;

    s32 _3C;
};
