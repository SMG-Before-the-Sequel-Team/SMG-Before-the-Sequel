#include "Game/Boss/TripodBossCore.h"
#include "Game/Boss/TripodBossFixPartsBase.h"
#include "Game/Boss/TripodBossAccesser.h"
#include "Game/LiveActor/HitSensor.h"
#include "Game/LiveActor/ModelObj.h"
#include "Game/Util/ActorSwitchUtil.h"
#include "Game/Util/LiveActorUtil.h"
#include "Game/Util/ObjUtil.h"

#include "Game/Util/SoundUtil.h"
#include "Game/Util/EffectUtil.h"
#include "Game/Util/ActorSensorUtil.h"
#include "Game/Util/ActorAnimUtil.h"
#include "Game/NameObj/NameObjCategories.h"
#include "revolution.h"
#include "Game/Util/Color8.h"

#include "Game/Util/JMapUtil.h"

namespace NrvTripodBossCore {
    void TripodBossCoreNrvNonActive::execute(Spine* pSpine) const {
        ((TripodBossCore*)pSpine->mExecutor)->exeNonActive();
    }
    TripodBossCoreNrvNonActive(TripodBossCoreNrvNonActive::sInstance);

    void TripodBossCoreNrvWait::execute(Spine* pSpine) const {
        ((TripodBossCore*)pSpine->mExecutor)->exeWait();
    }
    TripodBossCoreNrvWait(TripodBossCoreNrvWait::sInstance);

    void TripodBossCoreNrvDamageDemo::execute(Spine* pSpine) const {
        ((TripodBossCore*)pSpine->mExecutor)->exeDamageDemo();
    }
    TripodBossCoreNrvDamageDemo(TripodBossCoreNrvDamageDemo::sInstance);

    void TripodBossCoreNrvWarning::execute(Spine* pSpine) const {
        ((TripodBossCore*)pSpine->mExecutor)->exeWarning();
    }
    TripodBossCoreNrvWarning(TripodBossCoreNrvWarning::sInstance);

    void TripodBossCoreNrvBreak::execute(Spine* pSpine) const {
        ((TripodBossCore*)pSpine->mExecutor)->exeBreak();
    }
    TripodBossCoreNrvBreak(TripodBossCoreNrvBreak::sInstance);
};

TripodBossCore::TripodBossCore(const char *pName) : TripodBossFixPartsBase(pName) {
    mBreakModel = NULL;
    mBloomModel = NULL;
    mIsIndestructable = false;
}

void TripodBossCore::init(const JMapInfoIter &rIter) {
    TripodBossFixPartsBase::init(rIter);
    initModelManagerWithAnm("TripodBossCore", NULL, NULL, false);
    MR::connectToScene(this, MR::MovementType_MapObjDecoration, MR::CalcAnimType_MapObjDecoration, MR::DrawBufferType_MapObj, -1);
    initClippingSphere();
    initHitSensor(1);
    MR::addHitSensor(this, "body", ATYPE_BREAKABLE_CAGE, 8, 300.0f * mScale.x, TVec3f(0.0f, 0.0f, 0.0f));
    MR::initCollisionParts(this, "TripodBossCore", getSensor("body"), NULL);
    initSound(4, false, NULL, TVec3f(0.0f));
    mBreakModel = MR::createModelObjMapObjStrongLight("壊れモデル", "TripodBossCoreBreak", getBaseMtx());
    mBreakModel->initWithoutIter();
    MR::invalidateClipping(mBreakModel);
    mBreakModel->makeActorDead();
    MR::addTripodBossPartsMovement(mBreakModel);
    mBloomModel = MR::createModelObjBloomModel("ブルームモデル", "TripodBossCoreBloom", getBaseMtx(), false);
    mBloomModel->initWithoutIter();
    MR::invalidateClipping(mBloomModel);
    mBloomModel->makeActorDead();
    MR::addTripodBossPartsMovement(mBloomModel);
    initNerve(&NrvTripodBossCore::TripodBossCoreNrvNonActive::sInstance, 0);
    initEffectKeeper(0, "TripodBossCore", false);
    MR::setEffectHostMtx(this, "BlackSmoke", getBaseMtx());
    MR::useStageSwitchWriteDead(this, rIter);
    MR::invalidateCollisionParts(this);
    makeActorDead();

    MR::getJMapInfoArg7NoInit(rIter, &mIsIndestructable);
}

void TripodBossCore::kill() {
    LiveActor::kill();
    mBreakModel->kill();
}

bool TripodBossCore::receiveMsgEnemyAttack(u32 msg, HitSensor *a2, HitSensor *a3) {
    if (mIsIndestructable)
        return false;

    if (!isNerve(&NrvTripodBossCore::TripodBossCoreNrvBreak::sInstance)) {
        setNerve(&NrvTripodBossCore::TripodBossCoreNrvBreak::sInstance);
        return true;
    }

    return false;
}

void TripodBossCore::activateTripodBoss() {
    if (isNerve(&NrvTripodBossCore::TripodBossCoreNrvNonActive::sInstance)) {
        MR::onCalcAnim(this);
        MR::validateCollisionParts(this);
        setNerve(&NrvTripodBossCore::TripodBossCoreNrvWait::sInstance);
        mBloomModel->makeActorAppeared();
    }
}

void TripodBossCore::exeWait() {
    if (MR::isFirstStep(this)) {
        MR::tryStartAllAnim(this, "Wait");
        MR::tryStartAllAnim(mBloomModel, "Wait");
    }

    updateTripodMatrix();
    MR::requestPointLight(this, mTranslation, Color8(0x96, 0x96, 0x96, 0xFF), 1.0f, -1);
    if (MR::isDamageDemoTripodBoss()) {
        setNerve(&NrvTripodBossCore::TripodBossCoreNrvDamageDemo::sInstance);
    }
}

void TripodBossCore::exeDamageDemo() {
    if (MR::isFirstStep(this)) {
        MR::tryStartAllAnim(this, "2ndDemo");
        MR::tryStartAllAnim(mBloomModel, "2ndDemo");
    }

    MR::requestPointLight(this, mTranslation, Color8(0xFF, 0x96, 0x96, 0xFF), 1.0f, -1);
    if (MR::isDamageDemoTripodBoss()) {
        setNerve(&NrvTripodBossCore::TripodBossCoreNrvWarning::sInstance);
    }
}

void TripodBossCore::exeWarning() {
    updateTripodMatrix();
    MR::requestPointLight(this, mTranslation, Color8(0xFF, 0x96, 0x96, 0xFF), 1.0f, -1);
}

void TripodBossCore::exeBreak() {
    updateTripodMatrix();
    if (MR::isFirstStep(this)) {
        MR::startSound(this, "SE_BM_TRIPOD_CORE_BREAK", -1, -1);
        MR::emitEffect(this, "BlackSmoke");
        MR::hideModelAndOnCalcAnim(this);
        getSensor("body")->invalidate();
        MR::invalidateCollisionParts(this);
        MR::invalidateClipping(this);
        mBloomModel->kill();
        mBreakModel->appear();
        MR::startBck(mBreakModel, "Break", NULL);
        MR::requestMovementOn(mBreakModel);
        if (MR::isValidSwitchDead(this)) {
            MR::onSwitchDead(this);
        }
    }    

    if (MR::isStep(this, 2)) {
        MR::stopScene(16);
        MR::shakeCameraNormal();
    }

    if (MR::isEndBreakDownDemoTripodBoss()) {
        kill();
        MR::deleteEffect(this, "BlackSmoke");
    }
}

void TripodBossCore::exeNonActive() {
    
}

TripodBossCore::~TripodBossCore() {
    
}
