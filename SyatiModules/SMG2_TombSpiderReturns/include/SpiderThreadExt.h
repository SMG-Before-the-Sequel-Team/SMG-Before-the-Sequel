#pragma once

#include "revolution.h"
#include "JSystem/JGeometry/TVec.h"

// Galaxy 2 didn't include a bunch of functions related to spider string, so we gotta restore these too
// Though, it would be nicer if you could specify your own threading patterns... Project for later.

class LiveActor;
class HitSensor;

struct CutPointEntry {
    s32 mRadialIdx;
    s32 mPointIdx;
};

namespace MR {
    void initSpiderThread(const TVec3f&);
    void appearSpiderThread();
    void startSpiderThreadBattle();
    void startSpiderThreadChance();
    void startSpiderThreadBattleEnd();
    void pauseOffSpiderThread();
    f32 getSpiderThreadPosZ();
    void startActorBindToSpiderThread(LiveActor*, const TVec3f**, const TVec3f**, const TVec3f**, const TVec3f&, const TVec3f&);
    bool sendMsgToSpiderThread(u32, HitSensor*);
    bool touchActorToSpiderThread(const TVec3f&, const TVec3f&);
    void tryPushSpiderThread(const TVec3f&, f32);
    void onSpiderThreadBloom();
}