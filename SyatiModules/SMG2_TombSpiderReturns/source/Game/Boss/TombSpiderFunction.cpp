#include "Game/Boss/TombSpiderFunction.h"
#include "Game/Boss/TombSpider.h"
#include "Game/Boss/TombSpiderAcid.h"
#include "Game/Boss/TombSpiderEnvironment.h"
#include "Game/Boss/TombSpiderParts.h"
#include "Game/Boss/TombSpiderVitalSpot.h"
#include "Game/LiveActor/ActorCameraInfo.h"
#include "Game/LiveActor/HitSensor.h"
#include "Game/LiveActor/ModelObj.h"
#include "Game/Map/PlanetMap.h"
#include "Game/MapObj/SpiderThread.h"
#include "Game/Util/ActorCameraUtil.h"
#include "Game/Util/CameraUtil.h"
#include "Game/Util/EffectUtil.h"
#include "Game/Util/GravityUtil.h"
#include "Game/Util/LiveActorUtil.h"
#include "Game/Util/MathUtil.h"
#include "Game/Util/MtxUtil.h"
#include "Game/Util/ObjUtil.h"
#include "Game/Util/PlayerUtil.h"

#include "Game/Player/MarioActor.h"
#include "Game/Camera/CameraTargetArg.h"
#include "Game/Scene/SceneObjHolder.h"
#include "Game/Util/ActorAnimUtil.h"
#include "Game/Util/DemoUtil.h"

#include "TombSpiderSensor.h"
#include "Util.h"

namespace {
    static const f32 sPlanetInsideRadius = 1800.0f;
}

TombSpiderGland* TombSpiderFunction::getGlandFrontL(const TombSpider* pParent) {
    return pParent->mParts->mGlandFrontL;
}

TombSpiderGland* TombSpiderFunction::getGlandFrontR(const TombSpider* pParent) {
    return pParent->mParts->mGlandFrontR;
}

TombSpiderGland* TombSpiderFunction::getGlandRearL(const TombSpider* pParent) {
    return pParent->mParts->mGlandRearL;
}

TombSpiderGland* TombSpiderFunction::getGlandRearR(const TombSpider* pParent) {
    return pParent->mParts->mGlandRearR;
}

TombSpiderVitalSpot* TombSpiderFunction::getVitalSpotC(const TombSpider* pParent) {
    return pParent->mParts->mVitalSpotC;
}

TombSpiderVitalSpot* TombSpiderFunction::getVitalSpotL(const TombSpider* pParent) {
    return pParent->mParts->mVitalSpotL;
}

TombSpiderVitalSpot* TombSpiderFunction::getVitalSpotR(const TombSpider* pParent) {
    return pParent->mParts->mVitalSpotR;
}

MtxPtr TombSpiderFunction::getMtxEye(const TombSpider* pParent) {
    return pParent->mParts->mMtxEye;
}

MtxPtr TombSpiderFunction::getMtxMouth(const TombSpider* pParent) {
    return pParent->mParts->mMtxMouth;
}

MtxPtr TombSpiderFunction::getMtxHip(const TombSpider* pParent) {
    return pParent->mParts->mMtxHip;
}

MtxPtr TombSpiderFunction::getMtxGlandFrontL(const TombSpider* pParent) {
    return pParent->mParts->mMtxGlandFrontL;
}

MtxPtr TombSpiderFunction::getMtxGlandFrontR(const TombSpider* pParent) {
    return pParent->mParts->mMtxGlandFrontR;
}

MtxPtr TombSpiderFunction::getMtxGlandRearL(const TombSpider* pParent) {
    return pParent->mParts->mMtxGlandRearL;
}

MtxPtr TombSpiderFunction::getMtxGlandRearR(const TombSpider* pParent) {
    return pParent->mParts->mMtxGlandRearR;
}

void TombSpiderFunction::appearThreadAttacherAll(TombSpider* pParent) {
    return pParent->mParts->appearThreadAttacherAll();
}

void TombSpiderFunction::killThreadAttacherAll(TombSpider* pParent) {
    return pParent->mParts->killThreadAttacherAll();
}

void TombSpiderFunction::startAcid(TombSpider* pParent) {
    emitAcidMouth(pParent);
    emitAcidGland(pParent);
}

void TombSpiderFunction::updateAcid(TombSpider* pParent) {
    pParent->mParts->mAcid->update();
}

void TombSpiderFunction::endAcid(TombSpider* pParent) {
    pParent->mParts->mAcid->end();

    MR::deleteEffect(pParent, "Acid");

    if (!MR::isDead(getGlandFrontL(pParent))) {
        MR::deleteEffect(pParent, "AcidFrontL");
    }

    if (!MR::isDead(getGlandFrontR(pParent))) {
        MR::deleteEffect(pParent, "AcidFrontR");
    }

    if (!MR::isDead(getGlandRearL(pParent))) {
        MR::deleteEffect(pParent, "AcidRearL");
    }

    if (!MR::isDead(getGlandRearR(pParent))) {
        MR::deleteEffect(pParent, "AcidRearR");
    }

    pParent->mParts->mAcid->end();
    invalidateAttackSensor(pParent);
}

bool TombSpiderFunction::isAttackEnable(const TombSpiderGland* pGland) {
    if (!pGland->isActive() || pGland->isDamage()) {
        return false;
    }
    return true;
}

bool TombSpiderFunction::isAttackDirection(HitSensor* pSender, HitSensor* pReceiver) {
    TVec3f diff(pReceiver->mPosition);
    diff.sub(pSender->mPosition);
    return diff.dot(pSender->mActor->mVelocity) > 0.0f;
}

bool TombSpiderFunction::tryDamageEye(HitSensor* pSender, HitSensor* pReceiver) {
    if (pReceiver->isType(ATYPE_TOMB_SPIDER_EYE) && TombSpiderFunction::isAttackDirection(pSender, pReceiver)) {
        return true;
    }
    return false;
}

bool TombSpiderFunction::tryDamageHip(HitSensor* pSender, HitSensor* pReceiver) {
    if (pReceiver->isType(ATYPE_TOMB_SPIDER_HIP) && TombSpiderFunction::isAttackDirection(pSender, pReceiver)) {
        return true;
    }
    return false;
}

namespace {
    bool tryDamageGlandEach(TombSpiderGland* pGland, u32 sensorType, HitSensor* pSender, HitSensor* pReceiver) {
        if (!pReceiver->isType(sensorType) || !TombSpiderFunction::isAttackDirection(pSender, pReceiver) || pGland->isDamage()) {
            return false;
        }

        pGland->startDamage();
        pReceiver->invalidate();
        return true;
    }

    bool tryDamageGlandFrontL(TombSpider* pParent, HitSensor* pSender, HitSensor* pReceiver) {
        if (tryDamageGlandEach(TombSpiderFunction::getGlandFrontL(pParent), ATYPE_TOMB_SPIDER_GLAND_FRONT_L, pSender, pReceiver)) {
            pParent->getSensor("front_l_attacker")->invalidate();
            return true;
        }
        return false;
    }

    bool tryDamageGlandFrontR(TombSpider* pParent, HitSensor* pSender, HitSensor* pReceiver) {
        if (tryDamageGlandEach(TombSpiderFunction::getGlandFrontR(pParent), ATYPE_TOMB_SPIDER_GLAND_FRONT_R, pSender, pReceiver)) {
            pParent->getSensor("front_r_attacker")->invalidate();
            return true;
        }
        return false;
    }

    bool tryDamageGlandRearL(TombSpider* pParent, HitSensor* pSender, HitSensor* pReceiver) {
        if (tryDamageGlandEach(TombSpiderFunction::getGlandRearL(pParent), ATYPE_TOMB_SPIDER_GLAND_REAR_L, pSender, pReceiver)) {
            pParent->getSensor("rear_l_attacker")->invalidate();
            return true;
        }
        return false;
    }

    bool tryDamageGlandRearR(TombSpider* pParent, HitSensor* pSender, HitSensor* pReceiver) {
        if (tryDamageGlandEach(TombSpiderFunction::getGlandRearR(pParent), ATYPE_TOMB_SPIDER_GLAND_REAR_R, pSender, pReceiver)) {
            pParent->getSensor("rear_r_attacker")->invalidate();
            return true;
        }
        return false;
    }

}  // namespace

bool TombSpiderFunction::tryDamageGland(TombSpider* pParent, HitSensor* pSender, HitSensor* pReceiver) {
    if (tryDamageGlandFrontL(pParent, pSender, pReceiver) || tryDamageGlandFrontR(pParent, pSender, pReceiver) ||
        tryDamageGlandRearL(pParent, pSender, pReceiver) || tryDamageGlandRearR(pParent, pSender, pReceiver)) {
        return true;
    }
    return false;
}

namespace {
    bool tryDamageVitalSpotEach(TombSpiderVitalSpot* pVitalSpot, u32 sensorType, HitSensor* pSender, HitSensor* pReceiver) {
        if (!pReceiver->isType(sensorType) || !TombSpiderFunction::isAttackDirection(pSender, pReceiver) || pVitalSpot->isDamage()) {
            return false;
        }

        pVitalSpot->startDamage();
        pReceiver->invalidate();
        return true;
    }
}  // namespace

bool TombSpiderFunction::tryDamageVitalSpot(TombSpider* pParent, HitSensor* pSender, HitSensor* pReceiver) {
    if (tryDamageVitalSpotEach(getVitalSpotC(pParent), ATYPE_TOMB_SPIDER_VITAL_SPOT_C, pSender, pReceiver)) {
        return true;
    }

    if (tryDamageVitalSpotEach(getVitalSpotL(pParent), ATYPE_TOMB_SPIDER_VITAL_SPOT_L, pSender, pReceiver)) {
        return true;
    }

    if (tryDamageVitalSpotEach(getVitalSpotR(pParent), ATYPE_TOMB_SPIDER_VITAL_SPOT_R, pSender, pReceiver)) {
        return true;
    }

    return false;
}

void TombSpiderFunction::invalidateAllPartsSensor(TombSpider* pParent) {
    invalidateMainPartsSensor(pParent);
    invalidateGlandSensor(pParent);
    invalidateVitalSpotSensor(pParent);
}

void TombSpiderFunction::invalidateMainPartsSensor(TombSpider* pParent) {
    pParent->getSensor("body")->invalidate();
    pParent->getSensor("eye")->invalidate();
    pParent->getSensor("hip")->invalidate();
    pParent->getSensor("mouth")->invalidate();
}

void TombSpiderFunction::invalidateGlandSensor(TombSpider* pParent) {
    pParent->getSensor("gland_front_l")->invalidate();
    pParent->getSensor("gland_front_r")->invalidate();
    pParent->getSensor("gland_rear_l")->invalidate();
    pParent->getSensor("gland_rear_r")->invalidate();
}

void TombSpiderFunction::invalidateVitalSpotSensor(TombSpider* pParent) {
    pParent->getSensor("vital_spot_c")->invalidate();
    pParent->getSensor("vital_spot_l")->invalidate();
    pParent->getSensor("vital_spot_r")->invalidate();
}

void TombSpiderFunction::invalidateAttackSensor(TombSpider* pParent) {
    pParent->getSensor("attacker")->invalidate();
    pParent->getSensor("front_l_attacker")->invalidate();
    pParent->getSensor("front_r_attacker")->invalidate();
    pParent->getSensor("rear_l_attacker")->invalidate();
    pParent->getSensor("rear_r_attacker")->invalidate();
}

bool TombSpiderFunction::isSpringAttacker(const HitSensor* pSensor) {
    return pSensor->isType(ATYPE_SPRING_ATTACKER);
}

bool TombSpiderFunction::isEnemyAttack(const HitSensor* pSensor) {
    return pSensor->isType(ATYPE_ENEMY_ATTACK);
}

bool TombSpiderFunction::isSpiderBody(const HitSensor* pSensor) {
    return pSensor->isType(ATYPE_TOMB_SPIDER_BODY);
}

bool TombSpiderFunction::isSpiderAttack(const HitSensor* pSensor) {
    if (pSensor->isType(ATYPE_ENEMY_ATTACK) || pSensor->isType(ATYPE_TOMB_SPIDER_GLAND_FRONT_L_ATTACKER) ||
        pSensor->isType(ATYPE_TOMB_SPIDER_GLAND_FRONT_R_ATTACKER) || pSensor->isType(ATYPE_TOMB_SPIDER_GLAND_REAR_L_ATTACKER) ||
        pSensor->isType(ATYPE_TOMB_SPIDER_GLAND_REAR_R_ATTACKER)) {
        return true;
    }
    return false;
}

PlanetMap* TombSpiderFunction::getPlanet(const TombSpider* pParent) {
    return pParent->mEnvironment->mPlanet;
}

ModelObj* TombSpiderFunction::getCocoon(const TombSpider* pParent) {
    return pParent->mEnvironment->mCocoon;
}

void TombSpiderFunction::startTombSpiderDemo(TombSpider* pParent, const char* pDemoName, const char* pCameraName) {
    if (pCameraName == nullptr) {
        pCameraName = pDemoName;
    }

    ActorCameraInfo* info = pParent->mCameraInfo;  // ok

    MR::startEventCamera(info, pCameraName, CameraTargetArg(pParent), -1);
    pauseOffTombSpiderParts(pParent);
    MR::setBckRate(MR::getPlayerDemoActor(), 0.0f);
}

void TombSpiderFunction::startTombSpiderAnimCameraDemo(TombSpider* pParent, const char* pDemoName, const char* pCameraName, s32 a1) {
    if (pCameraName == nullptr) {
        pCameraName = pDemoName;
    }

    pauseOffTombSpiderParts(pParent);
    MR::setBckRate(MR::getPlayerDemoActor(), 0.0f);
    MR::startAnimCameraTargetSelf(getPlanet(pParent), pParent->mCameraInfo, pCameraName, a1, false, 1.0f);
}

void TombSpiderFunction::endTombSpiderDemo(TombSpider* pParent, const char* pDemoName, const char* pCameraName) {
    if (pCameraName == nullptr) {
        pCameraName = pDemoName;
    }

    MR::endEventCamera(pParent->mCameraInfo, pCameraName, false, -1);
    MR::endDemo(pParent, pDemoName);
}

void TombSpiderFunction::endTombSpiderAnimCameraDemo(TombSpider* pParent, const char* pDemoName, const char* pCameraName) {
    if (pCameraName == nullptr) {
        pCameraName = pDemoName;
    }

    MR::endAnimCamera(getPlanet(pParent), pParent->mCameraInfo, pCameraName, -1, true);
    MR::endDemo(pParent, pDemoName);
}

void TombSpiderFunction::pauseOffTombSpiderParts(const TombSpider* pParent) {
    // MR::pauseOffSpiderThread();
    {
        LiveActor* p = (LiveActor*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_SPIDER_THREAD);
        MR::requestMovementOn(p);
    }
    MR::requestMovementOn(getPlanet(pParent));
    MR::requestMovementOn(getCocoon(pParent));
    MR::requestMovementOn(getGlandFrontL(pParent));
    MR::requestMovementOn(getGlandFrontR(pParent));
    MR::requestMovementOn(getGlandRearL(pParent));
    MR::requestMovementOn(getGlandRearR(pParent));
    MR::requestMovementOn(getVitalSpotC(pParent));
    MR::requestMovementOn(getVitalSpotL(pParent));
    MR::requestMovementOn(getVitalSpotR(pParent));
    pParent->mParts->pauseOffThreadAttacherAll();
}

void TombSpiderFunction::resetPlayerPosTombSpider(const TombSpider* pParent, bool useCurrentPos) {
    TPos3f mtx;
    mtx.identity();
    TVec3f pos(pParent->mTranslation.x, pParent->mTranslation.y - sPlanetInsideRadius, pParent->mTranslation.z);
    TVec3f dirOut(*MR::getPlayerPos());
    dirOut.sub(pParent->mTranslation);

    if (useCurrentPos && !MR::isNearZero(dirOut, 0.001f)) {
        MR::normalize(&dirOut);
        dirOut.scale(sPlanetInsideRadius - 200.0f);
        pos.set(pParent->mTranslation + dirOut);
    }

    TVec3f up;
    MR::calcGravityVector(pParent, pos, &up, nullptr, 0);
    up.scale(-1.0f);

    MR::makeMtxFrontUpPos(&mtx, TVec3f(0.0f, 0.0f, 1.0f), up, pos);
    MR::setPlayerBaseMtx(mtx);
}

bool TombSpiderFunction::isRotateLeftToPlayer(const TombSpider* pParent) {
    TVec2f frontXY(1.0f, 0.0f);
    MR::rotateVecDegree(&frontXY, pParent->mRotation.z - 90.0f);
    TVec3f front(frontXY.x, frontXY.y, 0.0f);
    TVec3f dir(*MR::getPlayerPos());
    dir.sub(pParent->mTranslation);
    dir.z = 0.0f;
    TVec3f crossed;
    PSVECCrossProduct(front.toVecPtr(), dir.toVecPtr(), crossed.toVecPtr());
    return crossed.z > 0.0f;
}

f32 TombSpiderFunction::calcRotateSpeedToPlayer(TombSpider* pParent, s32 ratio) {
    // TODO: this matches but is probably written differently

    MR::repeatDegree(&pParent->mRotation.z);
    f32 angle = MR::calcRotateZ(pParent->mTranslation, *MR::getPlayerPos()) + 90.0f;
    MR::repeatDegree(&angle);

    if (isRotateLeftToPlayer(pParent)) {
        if (angle < pParent->getRotate()) {
            pParent->mRotation.z -= 360.0f;
        }
    } else {
        if (angle > pParent->getRotate()) {
            pParent->mRotation.z += 360.0f;
        }
    }

    return (angle - pParent->getRotate()) / ratio;
}

void TombSpiderFunction::emitAcidMouth(TombSpider* pParent) {
    MR::emitEffect(pParent, "Acid");
    pParent->mParts->mAcid->start();
}

void TombSpiderFunction::emitAcidGland(TombSpider* pParent) {
    if (getGlandFrontL(pParent)->isActive() && !getGlandFrontL(pParent)->isDamage()) {
        MR::emitEffect(pParent, "AcidFrontL");
    }

    if (getGlandFrontR(pParent)->isActive() && !getGlandFrontR(pParent)->isDamage()) {
        MR::emitEffect(pParent, "AcidFrontR");
    }

    if (getGlandRearL(pParent)->isActive() && !getGlandRearL(pParent)->isDamage()) {
        MR::emitEffect(pParent, "AcidRearL");
    }

    if (getGlandRearR(pParent)->isActive() && !getGlandRearR(pParent)->isDamage()) {
        MR::emitEffect(pParent, "AcidRearR");
    }

    pParent->mParts->mAcid->startGland();
}

void TombSpiderFunction::validateMainPartsSensor(TombSpider* pParent) {
    pParent->getSensor("body")->validate();
    pParent->getSensor("eye")->validate();
    pParent->getSensor("hip")->validate();
    pParent->getSensor("mouth")->validate();
}

void TombSpiderFunction::validateGlandSensor(TombSpider* pParent) {
    if (!getGlandFrontL(pParent)->isDamage()) {
        pParent->getSensor("gland_front_l")->validate();
    }

    if (!getGlandFrontR(pParent)->isDamage()) {
        pParent->getSensor("gland_front_r")->validate();
    }

    if (!getGlandRearL(pParent)->isDamage()) {
        pParent->getSensor("gland_rear_l")->validate();
    }

    if (!getGlandRearR(pParent)->isDamage()) {
        pParent->getSensor("gland_rear_r")->validate();
    }
}

void TombSpiderFunction::validateVitalSpotSensor(TombSpider* pParent) {
    pParent->getSensor("vital_spot_c")->validate();
    pParent->getSensor("vital_spot_l")->validate();
    pParent->getSensor("vital_spot_r")->validate();
}

void TombSpiderFunction::validateAttackSensor(TombSpider* pParent) {
    pParent->getSensor("attacker")->validate();

    if (!getGlandFrontL(pParent)->isDamage()) {
        pParent->getSensor("front_l_attacker")->validate();
    }

    if (!getGlandFrontR(pParent)->isDamage()) {
        pParent->getSensor("front_r_attacker")->validate();
    }

    if (!getGlandRearL(pParent)->isDamage()) {
        pParent->getSensor("rear_l_attacker")->validate();
    }

    if (!getGlandRearR(pParent)->isDamage()) {
        pParent->getSensor("rear_r_attacker")->validate();
    }
}
