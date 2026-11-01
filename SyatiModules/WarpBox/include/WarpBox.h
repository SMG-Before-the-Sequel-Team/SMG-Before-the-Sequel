#pragma once

#include "syati.h"
#include "WarpBoxMgr.h"
#include "WarpBoxExitPoint.h"

class WarpBox : public LiveActor {
public:
    WarpBox(const char *pName);
    ~WarpBox();
    virtual void init(const JMapInfoIter &rIter);
    virtual void attackSensor(HitSensor *pReceiver, HitSensor *pSender);
    virtual void control();
    void warpToOppositeWarpBox(TVec3f *pWarpPos);
    void warpPlayerToOppositeWarpBox();
    void exeWait();
    void exeIn();
    void exeOut();
    void launchPlayerOut();

    f32 mHorizontalForce;
    f32 mVerticalForce;
    f32 mColorFrame;
    s32 mBgmIndexLeft;
    s32 mBgmStateExited;
    bool mEnterAfterExit;
    bool mWarpAnyActor;
    WarpBox *mWarpEndPoint;
};

namespace NrvWarpBox {
    NERVE(NrvWait);
    NERVE(NrvIn);
    NERVE(NrvOut);
}