#include "Game/Boss/TombSpiderSensorCtrl.h"
#include "Game/Boss/TombSpiderGland.h"
#include "Game/Boss/TombSpiderVitalSpot.h"
#include "Game/Boss/TombSpiderFunction.h"
#include "Game/Boss/TombSpider.h"

#include "revolution.h"
#include "JSystem/JGeometry/TVec.h"
#include "Game/LiveActor/HitSensor.h"
#include "Game/Util/ActorSensorUtil.h"
#include "Game/Util/PlayerUtil.h"
#include "Game/Util/MathUtil.h"
#include "Game/Player/MarioActor.h"

#include "TombSpiderSensor.h"
#include "Util.h"

namespace {
    static const f32 sAttackSensorDistanceOffset = 100.0f;
    static const f32 sEyeSensorPosOffset = -100.0f;
    static const f32 sHipSensorPosOffset = -400.0f;
}

TombSpiderSensorCtrl::TombSpiderSensorCtrl(TombSpider* pOwner) : mParent(pOwner) { }

void TombSpiderSensorCtrl::update(HitSensor* pSensor) {
    if (!updateMainParts(pSensor) && !updateGland(pSensor) && !updateVitalSpot(pSensor) && !updateAttackSensor(pSensor)) {
        // Whatever was here got completely removed
        return;
    }
}

bool TombSpiderSensorCtrl::updateMainParts(HitSensor* pSensor) {
    if (pSensor->isType(ATYPE_TOMB_SPIDER_EYE)) {
        MtxPtr eyeMtxPtr = TombSpiderFunction::getMtxEye(mParent);
        f32 offset = sEyeSensorPosOffset;
        pSensor->mPosition.x = offset * eyeMtxPtr[0][2] + eyeMtxPtr[0][3];
        pSensor->mPosition.y = offset * eyeMtxPtr[1][2] + eyeMtxPtr[1][3];
        pSensor->mPosition.z = MR::getPlayerPos()->z;
        return true;
    }

    if (pSensor->isType(ATYPE_TOMB_SPIDER_HIP)) {
        MtxPtr eyeMtxPtr = TombSpiderFunction::getMtxHip(mParent);
        f32 offset = sHipSensorPosOffset;
        pSensor->mPosition.x = offset * eyeMtxPtr[0][2] + eyeMtxPtr[0][3];
        pSensor->mPosition.y = offset * eyeMtxPtr[1][2] + eyeMtxPtr[1][3];
        pSensor->mPosition.z = MR::getPlayerPos()->z;
        return true;
    }

    return false;
}

namespace {
    inline bool tryUpdateSensor(HitSensor* pSensor, u32 sensorType, PartsModel* part) {
        if (pSensor->isType(sensorType)) {
            pSensor->mPosition.set(part->mTranslation.x, part->mTranslation.y, MR::getPlayerPos()->z);
            return true;
        }
        return false;
    }
}

bool TombSpiderSensorCtrl::updateGland(HitSensor* pSensor) {
    if (tryUpdateSensor(pSensor, ATYPE_TOMB_SPIDER_GLAND_FRONT_L, TombSpiderFunction::getGlandFrontL(mParent)))
        return true;

    if (tryUpdateSensor(pSensor, ATYPE_TOMB_SPIDER_GLAND_FRONT_R, TombSpiderFunction::getGlandFrontR(mParent)))
        return true;

    if (tryUpdateSensor(pSensor, ATYPE_TOMB_SPIDER_GLAND_REAR_L, TombSpiderFunction::getGlandRearL(mParent)))
        return true;

    if (tryUpdateSensor(pSensor, ATYPE_TOMB_SPIDER_GLAND_REAR_R, TombSpiderFunction::getGlandRearR(mParent)))
        return true;

    return false;
}

bool TombSpiderSensorCtrl::updateVitalSpot(HitSensor* pSensor) {
    if (tryUpdateSensor(pSensor, ATYPE_TOMB_SPIDER_VITAL_SPOT_C, TombSpiderFunction::getVitalSpotC(mParent)))
        return true;

    if (tryUpdateSensor(pSensor, ATYPE_TOMB_SPIDER_VITAL_SPOT_L, TombSpiderFunction::getVitalSpotL(mParent)))
        return true;

    if (tryUpdateSensor(pSensor, ATYPE_TOMB_SPIDER_VITAL_SPOT_R, TombSpiderFunction::getVitalSpotR(mParent)))
        return true;

    return false;
}

namespace {
    // TVec2 CTOR heavily misbehaving
    void updateAttackSensorPos(HitSensor* pSensor, const LiveActor* pActor) {
        MtxPtr baseMtx = pActor->getBaseMtx();
        TVec2f pos(baseMtx[0][2], baseMtx[1][2]);
        MR::normalize(&pos);
        f32 dist = MR::calcDistanceXY(pActor->mTranslation, *MR::getPlayerPos()) - sAttackSensorDistanceOffset;
        pos.x *= dist;
        pos.y *= dist;
        pos.x += pActor->mTranslation.x;
        pos.y += pActor->mTranslation.y;
        f32 Z = MR::getPlayerPos()->z;
        pSensor->mPosition.set(pos.x, pos.y, Z);
    }
}

bool TombSpiderSensorCtrl::updateAttackSensor(HitSensor* pSensor) {
    if (pSensor->isType(ATYPE_ENEMY_ATTACK)) {
        TVec3f* pPos = &mParent->mTranslation;
        f32 dist = MR::calcDistanceXY(*pPos, *MR::getPlayerPos()) - sAttackSensorDistanceOffset;

        TVec2f pos(1.0f, 0.0f);

        MR::rotateVecDegree(&pos, mParent->getRotate() - 90.0f);
        pos.x *= dist;
        pos.y *= dist;
        pos.x += mParent->mTranslation.x;
        pos.y += mParent->mTranslation.y;
        f32 Z = MR::getPlayerPos()->z;
        pSensor->mPosition.set(pos.x, pos.y, Z);
        return true;
    }

    if (pSensor->isType(ATYPE_TOMB_SPIDER_GLAND_FRONT_L_ATTACKER)) {
        updateAttackSensorPos(pSensor, TombSpiderFunction::getGlandFrontL(mParent));
        return true;
    }

    if (pSensor->isType(ATYPE_TOMB_SPIDER_GLAND_FRONT_R_ATTACKER)) {
        updateAttackSensorPos(pSensor, TombSpiderFunction::getGlandFrontR(mParent));
        return true;
    }

    if (pSensor->isType(ATYPE_TOMB_SPIDER_GLAND_REAR_L_ATTACKER)) {
        updateAttackSensorPos(pSensor, TombSpiderFunction::getGlandRearL(mParent));
        return true;
    }

    if (pSensor->isType(ATYPE_TOMB_SPIDER_GLAND_REAR_R_ATTACKER)) {
        updateAttackSensorPos(pSensor, TombSpiderFunction::getGlandRearR(mParent));
        return true;
    }

    return false;
}

void TombSpiderSensorCtrl::initSensor() {
    MR::addHitSensor(mParent, "body", ATYPE_TOMB_SPIDER_BODY, 8, 600.0f, TVec3f(0.0f, -100.0f, 0.0f));

    MR::addHitSensorCallback(mParent, "eye", ATYPE_TOMB_SPIDER_EYE, 8, 280.0f);
    MR::addHitSensorCallback(mParent, "hip", ATYPE_TOMB_SPIDER_HIP, 8, 600.0f);
    MR::addHitSensorCallback(mParent, "mouth", ATYPE_TOMB_SPIDER_MOUTH, 8, 150.0f);
    MR::addHitSensorCallback(mParent, "gland_front_l", ATYPE_TOMB_SPIDER_GLAND_FRONT_L, 8, 200.0f);
    MR::addHitSensorCallback(mParent, "gland_front_r", ATYPE_TOMB_SPIDER_GLAND_FRONT_R, 8, 200.0f);
    MR::addHitSensorCallback(mParent, "gland_rear_l", ATYPE_TOMB_SPIDER_GLAND_REAR_L, 8, 200.0f);
    MR::addHitSensorCallback(mParent, "gland_rear_r", ATYPE_TOMB_SPIDER_GLAND_REAR_R, 8, 200.0f);
    MR::addHitSensorCallback(mParent, "attacker", ATYPE_ENEMY_ATTACK, 8, 100.0f);
    MR::addHitSensorCallback(mParent, "front_l_attacker", ATYPE_TOMB_SPIDER_GLAND_FRONT_L_ATTACKER, 8, 100.0f);
    MR::addHitSensorCallback(mParent, "front_r_attacker", ATYPE_TOMB_SPIDER_GLAND_FRONT_R_ATTACKER, 8, 100.0f);
    MR::addHitSensorCallback(mParent, "rear_l_attacker", ATYPE_TOMB_SPIDER_GLAND_REAR_L_ATTACKER, 8, 100.0f);
    MR::addHitSensorCallback(mParent, "rear_r_attacker", ATYPE_TOMB_SPIDER_GLAND_REAR_R_ATTACKER, 8, 100.0f);
    MR::addHitSensorCallback(mParent, "vital_spot_c", ATYPE_TOMB_SPIDER_VITAL_SPOT_C, 8, 200.0f);
    MR::addHitSensorCallback(mParent, "vital_spot_l", ATYPE_TOMB_SPIDER_VITAL_SPOT_L, 8, 200.0f);
    MR::addHitSensorCallback(mParent, "vital_spot_r", ATYPE_TOMB_SPIDER_VITAL_SPOT_R, 8, 200.0f);

    MR::addHitSensor(mParent, "cocoon", ATYPE_TOMB_SPIDER_COCOON, 8, 750.0f, TVec3f(0.0f, -50.0f, 0.0f));

    TombSpiderFunction::invalidateAllPartsSensor(mParent);
}