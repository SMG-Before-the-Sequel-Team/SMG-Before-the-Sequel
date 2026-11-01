#include "Game/Boss/TombSpiderActionCocoon.h"
#include "Game/Boss/TombSpider.h"
#include "Game/Boss/TombSpiderActionBase.h"
#include "Game/Boss/TombSpiderFunction.h"
#include "Game/LiveActor/HitSensor.h"
#include "Game/LiveActor/ModelObj.h"
#include "Game/Util/LiveActorUtil.h"
#include "Game/Util/ObjUtil.h"

#include "Game/Util.h"

namespace {
    static const s32 sBreakStep = 3;
}

namespace NrvTombSpiderActionCocoon {
    FULL_NERVE(TombSpiderActionCocoonNrvWait, TombSpiderActionCocoon, Wait);
    FULL_NERVE(TombSpiderActionCocoonNrvBreak, TombSpiderActionCocoon, Break);
}  // namespace NrvTombSpiderActionCocoon

TombSpiderActionCocoon::TombSpiderActionCocoon(TombSpider* pParent) : TombSpiderActionBase(pParent, "まゆ状態[トゥームスパイダー]") {
}

void TombSpiderActionCocoon::init() {
    initNerve(&NrvTombSpiderActionCocoon::TombSpiderActionCocoonNrvWait::sInstance, 0);
    TombSpiderActionBase::init();
}

void TombSpiderActionCocoon::attackSensor(HitSensor* pSender, HitSensor* pReceiver) {
    if (!MR::isPlayerInBind()) {
        MR::sendMsgPush(pReceiver, pSender);
    }
}

bool TombSpiderActionCocoon::receiveOtherMsg(u32 msg, HitSensor* pSender, HitSensor* pReceiver) {
    if (msg == ACTMES_IS_BROKEN) {
        if (mIsDead || isNerve(&NrvTombSpiderActionCocoon::TombSpiderActionCocoonNrvBreak::sInstance)) {
            return true;
        }
        return false;
    }

    if (msg == ACTMES_SLING_SHOOT_ATTACK) {
        if (!TombSpiderFunction::isAttackDirection(pSender, pReceiver)) {
            return false;
        }

        if (TombSpiderFunction::isSpringAttacker(pSender)) {
            if (isNerve(&NrvTombSpiderActionCocoon::TombSpiderActionCocoonNrvWait::sInstance)) {
                setNerve(&NrvTombSpiderActionCocoon::TombSpiderActionCocoonNrvBreak::sInstance);
                return true;
            }
        } else {
            MR::shakeCameraNormal();
            MR::startSound(TombSpiderFunction::getCocoon(mParent), "SE_OJ_TSPIDER_COCOON_BOUND", -1, -1);
            return true;
        }
    }

    // DEBUG
    //OSReport("Msg: %02X\n");
    //if (msg == ACTMES_SPIN_STORM_RANGE)
    //{
    //    if (isNerve(&NrvTombSpiderActionCocoon::TombSpiderActionCocoonNrvWait::sInstance)) {
    //        setNerve(&NrvTombSpiderActionCocoon::TombSpiderActionCocoonNrvBreak::sInstance);
    //        return true;
    //    }
    //}

    return false;
}

void TombSpiderActionCocoon::exeWait() {
    if (MR::isFirstStep(this)) {
        mParent->getSensor("cocoon")->validate();
        MR::startBck(TombSpiderFunction::getCocoon(mParent), "Wait", nullptr);
    }
}

void TombSpiderActionCocoon::exeBreak() {
    if (MR::isFirstStep(this)) {
        MR::startSound(TombSpiderFunction::getCocoon(mParent), "SE_OJ_TSPIDER_COCOON_BREAK", -1, -1);
    }

    if (MR::isStep(this, sBreakStep)) {
        kill();
    }
}
