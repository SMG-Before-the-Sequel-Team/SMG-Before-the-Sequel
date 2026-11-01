#include "Game/Boss/TripodBossShell.h"
#include "Game/Boss/TripodBossAccesser.h"
#include "Game/Boss/TripodBossFixPartsBase.h"
#include "Game/LiveActor/HitSensor.h"
#include "Game/LiveActor/ModelObj.h"
#include "Game/Util/ActorSwitchUtil.h"
#include "Game/Util/LiveActorUtil.h"
#include "Game/Util/PlayerUtil.h"
#include "JSystem/JGeometry/TVec.h"

#include "Game/Util.h"
#include "Game/NameObj/NameObjCategories.h"

namespace NrvTripodBossShell {
    void TripodBossShellNrvNonActive::execute(Spine* pSpine) const {
        ((TripodBossShell*)pSpine->mExecutor)->exeNonActive();
    }
    TripodBossShellNrvNonActive(TripodBossShellNrvNonActive::sInstance);

    void TripodBossShellNrvWait::execute(Spine* pSpine) const {
        ((TripodBossShell*)pSpine->mExecutor)->exeWait();
    }
    TripodBossShellNrvWait(TripodBossShellNrvWait::sInstance);

    void TripodBossShellNrvBreak::execute(Spine* pSpine) const {
        ((TripodBossShell*)pSpine->mExecutor)->exeBreak();
    }
    TripodBossShellNrvBreak(TripodBossShellNrvBreak::sInstance);
};

TripodBossShell::~TripodBossShell() {

}

TripodBossShell::TripodBossShell(const char *pName) : TripodBossFixPartsBase(pName) {
    mBreakModel = NULL;
}

void TripodBossShell::init(const JMapInfoIter &rIter) {
    TripodBossFixPartsBase::init(rIter);
    initModelManagerWithAnm("TripodBossShell", NULL, NULL, false);
    initHitSensor(2);
    MR::addHitSensorMapObj(this, "body", 0x10, 900.0f, TVec3f(0.0, 300.0f, 0.0f));
    MR::addHitSensor(this, "killer_terget", ATYPE_BREAKABLE_CAGE, 8, 900.0f * mScale.x, TVec3f(0.0f, 0.0f, 0.0f));
    MR::initCollisionParts(this, "TripodBossShell", getSensor("killer_terget"), NULL);
    MR::connectToScene(this, MR::MovementType_MapObjDecoration, MR::CalcAnimType_MapObjDecoration, MR::DrawBufferType_MapObj, -1);
    initSound(4, nullptr, NULL, TVec3f(0.0f));
    mBreakModel = MR::createModelObjMapObjStrongLight("壊れモデル", "TripodBossShellBreak", getBaseMtx());
    mBreakModel->initWithoutIter();
    MR::invalidateClipping(mBreakModel);
    mBreakModel->makeActorDead();
    MR::addTripodBossPartsMovement(mBreakModel);
    initNerve(&NrvTripodBossShell::TripodBossShellNrvNonActive::sInstance, 0);
    MR::invalidateCollisionParts(this);
    getSensor("body")->invalidate();
    MR::useStageSwitchWriteDead(this, rIter);
    makeActorDead();
}

void TripodBossShell::kill() {
    LiveActor::kill();
    mBreakModel->kill();
}

bool TripodBossShell::receiveMsgEnemyAttack(u32 msg, HitSensor *a2, HitSensor *a3) {
    if (a3 == getSensor("killer_terget") && !isNerve(&NrvTripodBossShell::TripodBossShellNrvBreak::sInstance)) {
        setNerve(&NrvTripodBossShell::TripodBossShellNrvBreak::sInstance);
        return true;
    }

    return false;
}

void TripodBossShell::activateTripodBoss() {
    if (isNerve(&NrvTripodBossShell::TripodBossShellNrvNonActive::sInstance)) {
        MR::onCalcAnim(this);
        MR::validateCollisionParts(this);
        setNerve(&NrvTripodBossShell::TripodBossShellNrvWait::sInstance);
    }
}

// https://decomp.me/scratch/okSIK
void TripodBossShell::exeBreak() {
    updateTripodMatrix();
    if (MR::isFirstStep(this)) {
        MR::startSound(this, "SE_BM_TRIPOD_CORE_BREAK", -1, -1);
        MR::offEntryDrawBuffer(this);
        getSensor("killer_terget")->invalidate();
        MR::invalidateCollisionParts(this);
        MR::invalidateClipping(this);
        mBreakModel->appear();
        MR::requestMovementOn(mBreakModel);
        MR::startBck(mBreakModel, "Break", NULL);
        TVec3f v16(*MR::getPlayerPos());
        v16 -= mTranslation;
        //JGeometry::subInternal(&v16.x, &mTranslation.x, &v16.x);
        f32 v11;
        MR::separateScalarAndDirection(&v11, &v16, v16);
        if (v11 <= 1000.0f) {
            TVec3f v15;
            MR::calcUpVec(&v15, this);
            if (v16.dot(v15) > 0.0f) {
                TVec3f v12(v16);
                v12.x *= 40.0f;
                v12.y *= 40.0f;
                v12.z *= 40.0f;
                MR::forceJumpPlayer(v12);
            }
        }
    }

    if (MR::isStep(this, 4)) {
        MR::stopScene(30);
        if (MR::isValidSwitchDead(this)) {
            MR::onSwitchDead(this);
        }
    }

    if (MR::isBckStopped(mBreakModel)) {
        kill();
    }
}

void TripodBossShell::exeWait() {
    updateTripodMatrix();
}

void TripodBossShell::exeNonActive() {
    
}
