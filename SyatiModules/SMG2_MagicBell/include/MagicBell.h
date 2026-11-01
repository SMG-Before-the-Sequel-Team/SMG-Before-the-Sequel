#pragma once

#include "Game/LiveActor/LiveActor.h"
#include "Swinger.h"

class MagicBell : public LiveActor {
public:
    MagicBell(const char*);

    virtual ~MagicBell();
    virtual void init(const JMapInfoIter&);
    virtual MtxPtr getBaseMtx() const;
    virtual void attackSensor(HitSensor*, HitSensor*);
    virtual bool receiveMsgPlayerAttack(u32, HitSensor*, HitSensor*);

    void exeWait();
    void exeRing();
    bool tryRing();
    void startRing(const TVec3f&, const TVec3f&);
    void updateMtx();

    //These offsets were translated to their SMG2 offsets
    Swinger* mBellSwinger;      // 0x90
    MtxPtr mSurface2Mtx;        // 0x94
    Swinger* mBellRodSwinger;   // 0x98
    MtxPtr mSurface1Mtx;        // 0x9C
    TVec3f mHitMarkTranslation; // 0xA0
};

namespace NrvMagicBell {
    NERVE(MagicBellNrvWait);
    NERVE(MagicBellNrvRing);
};