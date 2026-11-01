#include "Game/Boss/TombSpiderStateSwoon.h"
#include "Game/Boss/TombSpider.h"

#include "Game/Util.h"

namespace {
    static const s32 sLoopStep = 180;
}

namespace NrvTombSpiderStateSwoon {
    FULL_NERVE(TombSpiderStateSwoonNrvStart, TombSpiderStateSwoon, Start);
    FULL_NERVE(TombSpiderStateSwoonNrvLoop, TombSpiderStateSwoon, Loop);
    FULL_NERVE(TombSpiderStateSwoonNrvEnd, TombSpiderStateSwoon, End);
}  // namespace NrvTombSpiderStateSwoon

TombSpiderStateSwoon::TombSpiderStateSwoon(TombSpider* pParent) : ActorStateBase< TombSpider >("State[気絶]"), mParent(pParent){
}

void TombSpiderStateSwoon::init() {
    initNerve(&NrvTombSpiderStateSwoon::TombSpiderStateSwoonNrvStart::sInstance, 0);
    kill();
}

void TombSpiderStateSwoon::appear() {
    mIsDead = false;
    setNerve(&NrvTombSpiderStateSwoon::TombSpiderStateSwoonNrvStart::sInstance);
}

void TombSpiderStateSwoon::exeStart() {
    if (MR::isFirstStep(this)) {
        MR::startAction(mParent, "SwoonStart");
    }

    if (MR::isActionEnd(mParent)) {
        setNerve(&NrvTombSpiderStateSwoon::TombSpiderStateSwoonNrvLoop::sInstance);
    }
}

void TombSpiderStateSwoon::exeLoop() {
    if (MR::isFirstStep(this)) {
        MR::startAction(mParent, "Swoon");
    }

    if (MR::isGreaterStep(this, sLoopStep)) {
        setNerve(&NrvTombSpiderStateSwoon::TombSpiderStateSwoonNrvEnd::sInstance);
    }
}

void TombSpiderStateSwoon::exeEnd() {
    if (MR::isFirstStep(this)) {
        MR::startAction(mParent, "SwoonEnd");
    }

    if (MR::isBckStopped(mParent)) {
        kill();
    }
}
