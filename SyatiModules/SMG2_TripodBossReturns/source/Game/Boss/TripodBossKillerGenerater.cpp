#include "Game/Boss/TripodBossAccesser.h"
#include "Game/Boss/TripodBossFixPartsBase.h"
#include "Game/Boss/TripodBossKillerGenerater.h"
#include "Game/Enemy/HomingKiller.h"
#include "Game/LiveActor/HitSensor.h"
#include "Game/Scene/SceneFunction.h"
#include "Game/Util/ActorSwitchUtil.h"
#include "Game/Util/JMapUtil.h"
#include "Game/Util/LiveActorUtil.h"

#include "Game/Util/ActorMovementUtil.h"
#include "Game/Util/ObjUtil.h"
#include "Game/Util/ActorSensorUtil.h"
#include "Game/Util/ActorAnimUtil.h"
#include "Game/Util/EffectUtil.h"
#include "Game/Util/PlayerUtil.h"
#include "Game/Util/SoundUtil.h"

namespace NrvTripodBossKillerGenerater {
    FULL_NERVE(TripodBossKillerGeneraterNrvNonActive, TripodBossKillerGenerater, NonActive);
    FULL_NERVE(TripodBossKillerGeneraterNrvHide, TripodBossKillerGenerater, Hide);
    FULL_NERVE(TripodBossKillerGeneraterNrvShowDemo, TripodBossKillerGenerater, ShowDemo);
    FULL_NERVE(TripodBossKillerGeneraterNrvReady, TripodBossKillerGenerater, Ready);
    FULL_NERVE(TripodBossKillerGeneraterNrvShootSetting, TripodBossKillerGenerater, ShootSetting);
    FULL_NERVE(TripodBossKillerGeneraterNrvShoot, TripodBossKillerGenerater, Shoot);
    FULL_NERVE(TripodBossKillerGeneraterNrvCoolDown, TripodBossKillerGenerater, CoolDown);
    FULL_NERVE(TripodBossKillerGeneraterNrvBreak, TripodBossKillerGenerater, Break);
    FULL_NERVE(TripodBossKillerGeneraterNrvStop, TripodBossKillerGenerater, Stop);
    FULL_NERVE(TripodBossKillerGeneraterNrvRestart, TripodBossKillerGenerater, Restart);
};  // namespace NrvTripodBossKillerGenerater

TripodBossKillerGenerater::TripodBossKillerGenerater(const char* pName) : TripodBossFixPartsBase(pName) {
    mKiller = nullptr;
    _118 = 2000.0f;
    mActiveLabel = false;
    mHasCollision = true;
    _121 = 0;
    _E4.identity();
}

void TripodBossKillerGenerater::init(const JMapInfoIter& rIter) {
    TripodBossFixPartsBase::init(rIter);
    MR::makeMtxTR(_E4, this);
    initModelManagerWithAnm("TripodBossKillerCannon", nullptr, nullptr, false);
    MR::connectToScene(this, MR::MovementType_MapObjDecoration, MR::CalcAnimType_MapObjDecoration, MR::DrawBufferType_TripodBoss, -1);
    MR::getJMapInfoArg1NoInit(rIter, &_118);
    initClippingSphere();
    mKiller = new HomingKiller("ホーミングキラー");
    mKiller->initWithoutIter();
    s32 arg;
    MR::getJMapInfoArg2WithInit(rIter, &arg);

    if (arg == 0) {
        mKiller->mDisableChase = 1;
    }

    if (mHasCollision) {
        initHitSensor(1);
        MR::addHitSensor(this, "body", ATYPE_DELETED_ENEMY_32, 0, 0.0f, TVec3f(0.0f, 0.0f, 0.0f));
        MR::initCollisionParts(this, "TripodBossKillerCannon", getSensor("body"), nullptr);
    }

    initEffectKeeper(0, "TripodBoss", false);
    initSound(4, "TripodBoss", nullptr, TVec3f(0.0f, 0.0f, 0.0f));
    initNerve(&NrvTripodBossKillerGenerater::TripodBossKillerGeneraterNrvNonActive::sInstance, 0);
    MR::useStageSwitchReadAppear(this, rIter);
    if (mActiveLabel > 0) {
        MR::startBck(this, "2ndDemo", nullptr);
        MR::setBckFrameAndStop(this, 0.0f);
    } else {
        MR::startBck(this, "Shoot", nullptr);
        MR::setBckFrameAndStop(this, 0.0f);
    }

    if (mHasCollision) {
        MR::invalidateCollisionParts(this);
    }

    makeActorAppeared();
}

void TripodBossKillerGenerater::kill() {
    LiveActor::kill();
    MR::emitEffect(this, "ExplosionM");
}

bool TripodBossKillerGenerater::receiveMsgEnemyAttack(u32 msg, HitSensor* pSender, HitSensor* pReceiver) {
    if (MR::isMsgExplosionAttack(msg)) {
        return requestBreak();
    }

    return false;
}

void TripodBossKillerGenerater::calcTripodLocalMatrix(TPos3f* pPos) {
    pPos->set(_E4);
}

void TripodBossKillerGenerater::activateTripodBoss() {
    MR::onCalcAnim(this);
    if (mActiveLabel > 0) {
        setNerve(&NrvTripodBossKillerGenerater::TripodBossKillerGeneraterNrvHide::sInstance);
        if (mHasCollision) {
            MR::invalidateCollisionParts(this);
        }
    } else {
        setNerve(&NrvTripodBossKillerGenerater::TripodBossKillerGeneraterNrvReady::sInstance);
        if (mHasCollision) {
            MR::validateCollisionParts(this);
        }
    }

    _E0 = 1;
    _E1 = 1;
}

void TripodBossKillerGenerater::setLocalMatrix(const TPos3f& pMtx) {
    _E4.set(pMtx);
}

void TripodBossKillerGenerater::setActiveLebel(s32 label) {
    mActiveLabel = label;
}

void TripodBossKillerGenerater::setHasCollision(bool hasCollision) {
    mHasCollision = hasCollision;
}

bool TripodBossKillerGenerater::tryShootSetting() {
    if (MR::isValidSwitchAppear(this) && !MR::isOnSwitchAppear(this)) {
        return false;
    }

    if (MR::isSteppingTripodBossJointID(_CC)) {
        return false;
    }

    TVec3f trans;
    _8C.getTrans(trans);
    if (MR::isDead(mKiller)) {
        if (PSVECDistance((const Vec*)MR::getPlayerPos(), (const Vec*) & trans) < _118) {
            TVec3f front;
            MR::calcFrontVec(&front, this);
            TVec3f v4;
            _8C.getTrans(v4);
            mKiller->appear(v4, front);
            setNerve(&NrvTripodBossKillerGenerater::TripodBossKillerGeneraterNrvShootSetting::sInstance);
            return true;
        }
    }

    return false;
}

bool TripodBossKillerGenerater::tryShoot() {
    if (mKiller->isMoveStart()) {
        setNerve(&NrvTripodBossKillerGenerater::TripodBossKillerGeneraterNrvShoot::sInstance);
        return true;
    }

    return false;
}

bool TripodBossKillerGenerater::tryCoolDown() {
    if (MR::isDead(mKiller)) {
        setNerve(&NrvTripodBossKillerGenerater::TripodBossKillerGeneraterNrvCoolDown::sInstance);
        return true;
    }

    return false;
}

bool TripodBossKillerGenerater::tryReady() {
    if (MR::isGreaterStep(this, 0)) {
        setNerve(&NrvTripodBossKillerGenerater::TripodBossKillerGeneraterNrvReady::sInstance);
        return true;
    }

    return false;
}

bool TripodBossKillerGenerater::tryBreak() {
    if (MR::isEndBreakDownDemoTripodBoss()) {
        setNerve(&NrvTripodBossKillerGenerater::TripodBossKillerGeneraterNrvBreak::sInstance);
        return true;
    }

    return false;
}

bool TripodBossKillerGenerater::tryAbort() {
    if (!MR::isStartedTripodBoss() || MR::isDemoTripodBoss()) {
        if (!MR::isDead(mKiller)) {
            mKiller->kill();
        }

        setNerve(&NrvTripodBossKillerGenerater::TripodBossKillerGeneraterNrvReady::sInstance);
        return true;
    } else {
        return false;
    }
}

bool TripodBossKillerGenerater::requestBreak() {
    if (isNerve(&NrvTripodBossKillerGenerater::TripodBossKillerGeneraterNrvReady::sInstance) ||
        isNerve(&NrvTripodBossKillerGenerater::TripodBossKillerGeneraterNrvShootSetting::sInstance) ||
        isNerve(&NrvTripodBossKillerGenerater::TripodBossKillerGeneraterNrvShoot::sInstance) ||
        isNerve(&NrvTripodBossKillerGenerater::TripodBossKillerGeneraterNrvCoolDown::sInstance)) {
        if (isNerve(&NrvTripodBossKillerGenerater::TripodBossKillerGeneraterNrvShootSetting::sInstance) && !MR::isDead(mKiller)) {
            mKiller->kill();
        }

        if (mHasCollision) {
            MR::invalidateCollisionParts(this);
        }

        MR::startBck(this, "2ndDemo", nullptr);
        MR::setBckFrameAndStop(this, 0.0f);
        setNerve(&NrvTripodBossKillerGenerater::TripodBossKillerGeneraterNrvStop::sInstance);
        return true;
    }

    return false;
}

void TripodBossKillerGenerater::exeShowDemo() {
    if (MR::isStep(this, 90)) {
        if (PSVECDistance((const Vec*)MR::getPlayerPos(), (const Vec*)&mTranslation) < 230.0f) {
            setNerve(&NrvTripodBossKillerGenerater::TripodBossKillerGeneraterNrvRestart::sInstance);
            return;
        }

        MR::startBck(this, "2ndDemo", nullptr);

        if (mHasCollision) {
            MR::validateCollisionParts(this);
        }
    }

    updateTripodMatrix();
    MR::isStep(this, 110);
    if (!MR::isDamageDemoTripodBoss()) {
        setNerve(&NrvTripodBossKillerGenerater::TripodBossKillerGeneraterNrvReady::sInstance);
    }
}

void TripodBossKillerGenerater::exeReady() {
    updateTripodMatrix();
    if (!tryBreak() && !tryAbort() && !tryShootSetting()) {
        return;
    }
}

void TripodBossKillerGenerater::exeShootSetting() {
    updateTripodMatrix();
    TVec3f front;
    MR::calcFrontVec(&front, this);
    TVec3f trans;
    _8C.getTrans(trans);
    HomingKiller* killer = mKiller;
    killer->mBasePos.set(trans);
    killer->mBaseFront.set(front);
    if (!tryBreak() && !tryAbort() && !tryShoot()) {
        return;
    }
}

void TripodBossKillerGenerater::exeShoot() {
    if (MR::isFirstStep(this)) {
        MR::startBck(this, "Shoot", nullptr);
        PSVECDistance((const Vec*)&mTranslation, (const Vec*)MR::getPlayerPos());
    }

    updateTripodMatrix();
    if (!tryBreak() && !tryAbort() && !tryCoolDown()) {
        return;
    }
}

void TripodBossKillerGenerater::exeCoolDown() {
    updateTripodMatrix();
    if (!tryBreak() && !tryAbort() && !tryReady()) {
        return;
    }
}

void TripodBossKillerGenerater::exeStop() {
    if (MR::isFirstStep(this)) {
        MR::emitEffect(this, "ExplosionS");
        MR::startSound(this, "SE_BM_TRIPOD_CANNON_BREAK", -1, -1);
        if (mHasCollision) {
            MR::invalidateCollisionParts(this);
        }
    }
    updateTripodMatrix();

    if (MR::isGreaterStep(this, 300)) {
        setNerve(&NrvTripodBossKillerGenerater::TripodBossKillerGeneraterNrvRestart::sInstance);
    }
}

void TripodBossKillerGenerater::exeRestart() {
    if (MR::isFirstStep(this)) {
        _121 = 1;
    }

    if (_121) {
        if (PSVECDistance((const Vec*)MR::getPlayerPos(), (const Vec*)&mTranslation) >= 230.0f) {
            MR::startBck(this, "2ndDemo", nullptr);
            MR::startSound(this, "SE_BM_TRIPOD_CANNON_RESTART", -1, -1);
            if (mHasCollision) {
                MR::validateCollisionParts(this);
            }

            _121 = 0;
        }
    }

    updateTripodMatrix();
    if (!_121) {
        if (MR::isBckStopped(this)) {
            setNerve(&NrvTripodBossKillerGenerater::TripodBossKillerGeneraterNrvReady::sInstance);
        }
    }
}

void TripodBossKillerGenerater::exeBreak() {
    if (MR::isFirstStep(this)) {
        if (!MR::isDead(mKiller)) {
            mKiller->kill();
        }
        kill();
    }
}

void TripodBossKillerGenerater::exeHide() {
    if (MR::isDamageDemoTripodBoss()) {
        setNerve(&NrvTripodBossKillerGenerater::TripodBossKillerGeneraterNrvShowDemo::sInstance);
    }

    updateTripodMatrix();
}

void TripodBossKillerGenerater::exeNonActive() {
}

TripodBossKillerGenerater::~TripodBossKillerGenerater() {
}
