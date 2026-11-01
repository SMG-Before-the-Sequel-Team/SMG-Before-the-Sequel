#include "Game/Boss/TombSpiderAction2nd.h"
#include "Game/Boss/TombSpider.h"
#include "Game/Boss/TombSpiderActionBase.h"
#include "Game/Boss/TombSpiderFunction.h"
#include "Game/Boss/TombSpiderStateSwoon.h"

#include "Game/Util.h"

namespace {
    static const s32 sStepWaitMin = 30;
    static const s32 sStepWaitMax = 60;
    static const s32 sStepAttackLoopMin = 240;
    static const s32 sStepAttackLoopMax = 360;
    static const f32 sAttackRotateAccel = 0.005f;
    static const f32 sAttackRotateSpeedMax = 0.4f;
    static const s32 sStepChanceWait = 600;
    static const s32 sChanceDamageLastStep = 5;
}  // namespace

namespace NrvTombSpiderAction2nd {
    FULL_NERVE(TombSpiderAction2ndNrvWait, TombSpiderAction2nd, Wait);
    FULL_NERVE(TombSpiderAction2ndNrvAttackStart, TombSpiderAction2nd, AttackStart);
    FULL_NERVE(TombSpiderAction2ndNrvAttackLoop, TombSpiderAction2nd, AttackLoop);
    FULL_NERVE(TombSpiderAction2ndNrvAttackEnd, TombSpiderAction2nd, AttackEnd);
    FULL_NERVE(TombSpiderAction2ndNrvDamageEye, TombSpiderAction2nd, DamageEye);
    FULL_NERVE(TombSpiderAction2ndNrvDamageHip, TombSpiderAction2nd, DamageHip);
    FULL_NERVE(TombSpiderAction2ndNrvDamageGland, TombSpiderAction2nd, DamageGland);
    FULL_NERVE(TombSpiderAction2ndNrvSwoon, TombSpiderAction2nd, Swoon);
    FULL_NERVE(TombSpiderAction2ndNrvChanceStart, TombSpiderAction2nd, ChanceStart);
    FULL_NERVE(TombSpiderAction2ndNrvChanceWait, TombSpiderAction2nd, ChanceWait);
    FULL_NERVE(TombSpiderAction2ndNrvChanceDamage, TombSpiderAction2nd, ChanceDamage);
    FULL_NERVE(TombSpiderAction2ndNrvChanceDamageLast, TombSpiderAction2nd, ChanceDamageLast);
    FULL_NERVE(TombSpiderAction2ndNrvChanceEnd, TombSpiderAction2nd, ChanceEnd);
    FULL_NERVE(TombSpiderAction2ndNrvWaitDemo, TombSpiderAction2nd, WaitDemo);
}  // namespace NrvTombSpiderAction2nd

TombSpiderAction2nd::TombSpiderAction2nd(TombSpider* pParent) : TombSpiderActionBase(pParent, "戦闘２回戦[トゥームスパイダー]") {
}

void TombSpiderAction2nd::init() {
    initNerve(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvWait::sInstance, 16);
    MR::initActorState(this, new TombSpiderStateSwoon(mParent), &NrvTombSpiderAction2nd::TombSpiderAction2ndNrvSwoon::sInstance, "Swoon");
    TombSpiderActionBase::init();
}

void TombSpiderAction2nd::appear() {
    TombSpiderActionBase::appear();
    TombSpiderFunction::validateMainPartsSensor(mParent);
    TombSpiderFunction::validateGlandSensor(mParent);
}

void TombSpiderAction2nd::attackSensor(HitSensor* pSender, HitSensor* pReceiver) {
    if (TombSpiderFunction::isSpiderAttack(pSender) && isNerve(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvAttackLoop::sInstance)) {
        if (MR::isSensorPlayer(pReceiver) || TombSpiderFunction::isSpringAttacker(pReceiver)) {
            if (MR::sendMsgEnemyAttack(pReceiver, pSender)) {
                mHitStep = getNerveStep();
            }
        }
    }
}

bool TombSpiderAction2nd::receiveOtherMsg(u32 msg, HitSensor* pSender, HitSensor* pReceiver) {
    if (msg == ACTMES_SLING_SHOOT_IS_LAST_DAMAGE) {
        return mIsDead;
    }

    if (msg == ACTMES_IS_BROKEN) {
        return !TombSpiderFunction::isSpiderBody(pReceiver);
    }

    if (msg == ACTMES_SLING_SHOOT_ATTACK) {
        if (!isNerve(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvSwoon::sInstance) && TombSpiderFunction::tryDamageEye(pSender, pReceiver)) {
            setNerve(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvDamageEye::sInstance);
            return true;
        }

        if (TombSpiderFunction::tryDamageHip(pSender, pReceiver)) {
            setNerve(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvDamageHip::sInstance);
            return true;
        }

        if (TombSpiderFunction::tryDamageGland(mParent, pSender, pReceiver)) {
            setNerve(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvDamageGland::sInstance);
            return true;
        }

        if (isNerve(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvChanceWait::sInstance) &&
            TombSpiderFunction::tryDamageVitalSpot(mParent, pSender, pReceiver)) {
            startChanceDamage();
            if (mEnergy <= 0) {
                setNerve(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvChanceDamageLast::sInstance);
            } else {
                setNerve(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvChanceDamage::sInstance);
            }
            return true;
        }

        if (TombSpiderFunction::isSpiderBody(pReceiver)) {
            return true;
        }
    }

    if (msg == ACTMES_SLING_SHOOT_PASS_THROUGH_ENABLE) {
        return isNerve(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvChanceDamage::sInstance);
    }

    return false;
}

void TombSpiderAction2nd::exeWait() {
    if (updateWait(sStepWaitMin, sStepWaitMax)) {
        setNerve(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvAttackStart::sInstance);
    }
}

void TombSpiderAction2nd::exeAttackStart() {
    if (updateAttackStart()) {
        setNerve(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvAttackLoop::sInstance);
    }
}

void TombSpiderAction2nd::exeAttackLoop() {
    if (updateAttackLoop(sStepAttackLoopMin, sStepAttackLoopMax, sAttackRotateAccel, sAttackRotateSpeedMax)) {
        setNerve(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvAttackEnd::sInstance);
    }
}

void TombSpiderAction2nd::exeAttackEnd() {
    if (updateAttackEnd()) {
        setNerve(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvWait::sInstance);
    }
}

void TombSpiderAction2nd::exeDamageEye() {
    if (updateDamageEye()) {
        setNerve(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvSwoon::sInstance);
    }
}

void TombSpiderAction2nd::exeDamageHip() {
    if (updateDamageHip()) {
        setNerve(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvWaitDemo::sInstance);
        if (!tryWaitChanceStartDemo(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvChanceStart::sInstance)) {
            setNerve(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvChanceStart::sInstance);
        }
    }
}

void TombSpiderAction2nd::exeDamageGland() {
    if (updateDamageGland()) {
        setNerve(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvSwoon::sInstance);
    }
}

void TombSpiderAction2nd::exeSwoon() {
    MR::updateActorStateAndNextNerve(this, &NrvTombSpiderAction2nd::TombSpiderAction2ndNrvWait::sInstance);
}

void TombSpiderAction2nd::exeChanceStart() {
    if (updateChanceStart()) {
        setNerve(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvChanceWait::sInstance);
    }
}

void TombSpiderAction2nd::exeChanceWait() {
    if (updateChanceWait(sStepChanceWait)) {
        setNerve(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvWaitDemo::sInstance);
        if (!tryWaitChanceEndDemo(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvChanceEnd::sInstance)) {
            setNerve(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvChanceEnd::sInstance);
        }
    }
}

void TombSpiderAction2nd::exeChanceDamage() {
    if (updateChanceDamage()) {
        setNerve(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvChanceWait::sInstance);
    }
}

void TombSpiderAction2nd::exeChanceDamageLast() {
    if (MR::isStep(this, sChanceDamageLastStep)) {
        kill();
    }
}

void TombSpiderAction2nd::exeChanceEnd() {
    if (updateChanceEnd()) {
        setNerve(&NrvTombSpiderAction2nd::TombSpiderAction2ndNrvWait::sInstance);
    }
}

void TombSpiderAction2nd::exeWaitDemo() {
}
