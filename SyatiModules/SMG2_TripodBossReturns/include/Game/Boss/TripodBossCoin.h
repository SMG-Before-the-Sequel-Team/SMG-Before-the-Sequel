#pragma once

#include "Game/LiveActor/LiveActor.h"
#include "Game/MapObj/Coin.h"

class TripodBossCoin : public NameObj {
public:
    TripodBossCoin(const char *);

    virtual ~TripodBossCoin();
    virtual void init(const JMapInfoIter &);
    virtual void movement();

    void exeNonActive();
    void exeActive();
    void exeEnd() { }

    Coin* mCoin;        // 0xC
    Spine* mSpine;      // 0x10
    TPos3f _14;
    s32 _44;
};

namespace NrvTripodBossCoin {
    NERVE(TripodBossCoinNrvNonActive);
    NERVE(TripodBossCoinNrvActive);
    NERVE(TripodBossCoinNrvEnd);
};