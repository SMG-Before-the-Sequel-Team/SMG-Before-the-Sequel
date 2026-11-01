#pragma once

#include "syati.h"

class KinokoTrampoline : public RailMoveObj {
public:
    KinokoTrampoline(const char *);
    ~KinokoTrampoline();
    virtual void init (const JMapInfoIter &);
    virtual void control();
    virtual bool receiveOtherMsg(u32 msg, HitSensor *pSender, HitSensor *pReceiver);
    
    void activate();
    void deactivate();
    bool isActive();
    inline bool isMsgNeedBounce(u32 msg) {
        return MR::isMsgFloorTouch(msg) || MR::isMsgPlayerHipDropFloor(msg);
    }

    CollisionParts* mBounceCollision;
    CollisionParts* mIdleCollision;
};