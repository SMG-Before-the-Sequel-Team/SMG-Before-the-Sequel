#include "Game/Boss/TripodBossGuardWallPart.h"
#include "Game/Boss/TripodBossAccesser.h"
#include "Game/LiveActor/HitSensor.h"
#include "Game/Util.h"
#include "Game/Util/ActorSensorUtil.h"
#include "Game/Util/LiveActorUtil.h"

#include "Game/NameObj/NameObjCategories.h"

namespace NrvTripodBossGuardWallPart {
    void TripodBossGuardWallPartNrvNonActive::execute(Spine* pSpine) const {
        ((TripodBossGuardWallPart*)pSpine->mExecutor)->exeNonActive();
    }
    TripodBossGuardWallPartNrvNonActive(TripodBossGuardWallPartNrvNonActive::sInstance);

    void TripodBossGuardWallPartNrvDemo::execute(Spine* pSpine) const {
        ((TripodBossGuardWallPart*)pSpine->mExecutor)->exeDemo();
    }
    TripodBossGuardWallPartNrvDemo(TripodBossGuardWallPartNrvDemo::sInstance);

    void TripodBossGuardWallPartNrvActive::execute(Spine* pSpine) const {
        ((TripodBossGuardWallPart*)pSpine->mExecutor)->exeActive();
    }
    TripodBossGuardWallPartNrvActive(TripodBossGuardWallPartNrvActive::sInstance);

    void TripodBossGuardWallPartNrvBreak::execute(Spine* pSpine) const {
        ((TripodBossGuardWallPart*)pSpine->mExecutor)->exeBreak();
    }
    TripodBossGuardWallPartNrvBreak(TripodBossGuardWallPartNrvBreak::sInstance);

    void TripodBossGuardWallPartNrvRepair::execute(Spine* pSpine) const {
        ((TripodBossGuardWallPart*)pSpine->mExecutor)->exeRepair();
    }
    TripodBossGuardWallPartNrvRepair(TripodBossGuardWallPartNrvRepair::sInstance);
};

TripodBossGuardWallPart::TripodBossGuardWallPart(const char *pName) : LiveActor(pName) {
    mHostMtx = NULL;
    mPlacementAngle = 0.0f;
    mStartTiming = 0;
}

void TripodBossGuardWallPart::init(const JMapInfoIter &rIter) {
    initModelManagerWithAnm("TripodBossGuardWall", NULL, NULL, false);
    MR::connectToScene(this, 0x23, 0xB, MR::DrawBufferType_MapObj, MR::CategoryList_Null);
    initHitSensor(2);
    TVec3f sensorOffs;
    sensorOffs.z = sensorOffs.y = sensorOffs.x = 0.0f;
    MR::initCollisionParts(this, "TripodBossGuardWall", MR::addHitSensor(this, "collision", ATYPE_DELETED_ENEMY_31, 0, 1000.0f, sensorOffs), NULL);
    initSound(4, false, NULL, TVec3f(0.0f));
    initEffectKeeper(0, "TripodBoss", false);
    MR::invalidateClipping(this);
    MR::startBck(this, "2ndDemo", NULL);
    MR::setBckFrameAndStop(this, 0.0f);
    initNerve(&NrvTripodBossGuardWallPart::TripodBossGuardWallPartNrvNonActive::sInstance, 0);
    MR::addTripodBossPartsMovement(this);
    makeActorDead();
}

void TripodBossGuardWallPart::makeActorAppeared() {
    LiveActor::makeActorAppeared();
    MR::invalidateCollisionParts(this);
}

void TripodBossGuardWallPart::kill() {
    LiveActor::kill();
    MR::emitEffect(this, "ExplosionM");
}

void TripodBossGuardWallPart::control() {
    mTranslation.set(mHostMtx->mMtx[0][3], mHostMtx->mMtx[1][3], mHostMtx->mMtx[2][3]);
}

inline void setRotateInline(TPos3f* mtx, f32 r, f32 n) {
    f32 c, s, angle;
    angle = n * r;
    s = sin(angle);
    c = cos(angle);

    mtx->mMtx[0][2] = s;
    mtx->mMtx[1][1] = 1.0f;
    mtx->mMtx[0][0] = c;
    mtx->mMtx[2][0] = -s;
    mtx->mMtx[2][2] = c;
    mtx->mMtx[2][1] = 0.0f;
    mtx->mMtx[1][2] = 0.0f;
    mtx->mMtx[1][0] = 0.0f;
    mtx->mMtx[0][1] = 0.0f;
}

void TripodBossGuardWallPart::calcAndSetBaseMtx() {
    TPos3f mtx;
    mtx.identity();
    setRotateInline(&mtx, mPlacementAngle, 0.017453292f);
    mtx.concat(*mHostMtx, mtx);
    MR::setBaseTRMtx(this, mtx);
}

bool TripodBossGuardWallPart::receiveMsgEnemyAttack(u32 msg, HitSensor *a2, HitSensor *a3) {
    if (MR::isSensorType(a3, ATYPE_DELETED_ENEMY_31) && MR::isMsgExplosionAttack(msg)) {
        return requestBreak();
    }

    return false;
}

void TripodBossGuardWallPart::requestStartDemo() {
    setNerve(&NrvTripodBossGuardWallPart::TripodBossGuardWallPartNrvDemo::sInstance);
}

bool TripodBossGuardWallPart::requestBreak() {
    if (MR::isDead(this) || !isNerve(&NrvTripodBossGuardWallPart::TripodBossGuardWallPartNrvActive::sInstance)) {
        return false;
    }

    setNerve(&NrvTripodBossGuardWallPart::TripodBossGuardWallPartNrvBreak::sInstance);
    return true;
}

bool TripodBossGuardWallPart::isEndDemo() const {
    return isNerve(&NrvTripodBossGuardWallPart::TripodBossGuardWallPartNrvActive::sInstance);
}

void TripodBossGuardWallPart::exeBreak() {
    if (MR::isFirstStep(this)) {
        MR::startSound(this, "SE_BM_TRIPOD_C_COVER_BREAK", -1, -1);
        MR::invalidateCollisionParts(this);
    }

    if (MR::isStep(this, 5)) {
        MR::startBck(this, "2ndDemo", NULL);
        MR::setBckFrameAndStop(this, 0.0f);
    }

    bool isNearPlayer = !MR::isNearPlayer(this, 1000.0f);

    if (isNearPlayer && MR::isGreaterStep(this, 600)) {
        setNerve(&NrvTripodBossGuardWallPart::TripodBossGuardWallPartNrvRepair::sInstance);
    }
}

void TripodBossGuardWallPart::exeRepair() {
    if (MR::isFirstStep(this)) {
        MR::validateCollisionParts(this);
        MR::startBck(this, "2ndDemo", NULL);
    }

    if (MR::isStep(this, 38)) {
        MR::startSound(this, "SE_BM_TRIPOD_WALL_UP_STOP", -1, -1);
    }

    if (MR::isBckStopped(this)) {
        setNerve(&NrvTripodBossGuardWallPart::TripodBossGuardWallPartNrvActive::sInstance);
    }
}

void TripodBossGuardWallPart::exeActive() {

}

void TripodBossGuardWallPart::exeDemo() {
    if (MR::isGreaterStep(this, mStartTiming)) {
        setNerve(&NrvTripodBossGuardWallPart::TripodBossGuardWallPartNrvRepair::sInstance);
    }
}

void TripodBossGuardWallPart::exeNonActive() {

}

void TripodBossGuardWallPart::setHostMatrix(const TPos3f *pPos) {
    mHostMtx = pPos;
}

void TripodBossGuardWallPart::setPlacementAngle(f32 angle) {
    mPlacementAngle = angle;
    MR::repeatDegree(&mPlacementAngle);
}

void TripodBossGuardWallPart::setStartTiming(s32 timing) {
    mStartTiming = timing;
}
