#include "Game/Boss/TripodBossFixPartsBase.h"
#include "Game/Boss/TripodBossAccesser.h"
#include "Game/Boss/TripodBossBreakMovement.h"
#include "Game/LiveActor/HitSensor.h"
#include "Game/Util/LiveActorUtil.h"
#include "Game/Util/ObjUtil.h"
#include "JSystem/JMath.h"

#include "Game/Util/ActorInitUtil.h"
#include "Game/Util/ActorMovementUtil.h"
#include "Game/Util/JMapUtil.h"
#include "Game/Util/MtxUtil.h"
#include "Game/Util/ModelUtil.h"

TripodBossFixPartsBase::TripodBossFixPartsBase(const char *pName) : LiveActor(pName) {
    _BC.x = 0.0f;
    _BC.y = 0.0f;
    _BC.z = 0.0f;
    mBreakMovement = NULL;
    _CC = -1;
    _D0.x = 0.0f;
    _D0.y = 0.0f;
    _D0.z = 0.0f;
    mClippingDistance = 0.0f;
    _E0 = 0;
    _E1 = 0;
    _8C.identity();
}

void TripodBossFixPartsBase::init(const JMapInfoIter &rIter) {
    MR::initDefaultPos(this, rIter);
    _BC = mTranslation;
    MR::getJMapInfoArg0NoInit(rIter, &_CC);
    MR::addTripodBossParts(this);
    MR::invalidateClipping(this);
}

void TripodBossFixPartsBase::initAfterPlacement() {
    updateTripodMatrix();
    calcAnim();
    MR::offCalcAnim(this);
}

void TripodBossFixPartsBase::initClippingSphere() {
    TBox3f box;
    MR::calcModelBoundingBox(&box, this);
    TVec3f v9(box.mPointMin);
    JMathInlineVEC::PSVECAdd((const Vec*)&v9, (const Vec*)&box.mPointMax, (Vec*)&v9);
    TVec3f v10(v9);
    v10.x *= 0.5f;
    v10.y *= 0.5f;
    v10.z *= 0.5f;
    _D0 = v10;
    mClippingDistance = PSVECDistance((const Vec*)&_D0, (const Vec*)&box.mPointMin);
}

void TripodBossFixPartsBase::setClippingSphere(const TVec3f &a1, f32 dist) {
    _D0 = a1;
    mClippingDistance = dist;
}

void TripodBossFixPartsBase::makeActorDead() {
    LiveActor::makeActorDead();
    if (mBreakMovement != NULL) {
        mBreakMovement->makeActorDead();
    }
}

void TripodBossFixPartsBase::calcAndSetBaseMtx() {
    MR::setBaseTRMtx(this, _8C);
}

bool TripodBossFixPartsBase::receiveOtherMsg(u32 msg, HitSensor *a2, HitSensor *a3) {
    if (msg == 170) {
        activateTripodBoss();
        return true;
    }

    return false;
}

void TripodBossFixPartsBase::calcTripodLocalMatrix(TPos3f *pPos) {
    MR::makeMtxTR((MtxPtr)pPos, _BC, mRotation);
}

void TripodBossFixPartsBase::activateTripodBoss() {

}

void TripodBossFixPartsBase::startBreakMovement() {
    TPos3f mtx;
    mtx.set(getBaseMtx());
    mBreakMovement->start(mtx, _CC);
}

/* matrix copy is wrong */
void TripodBossFixPartsBase::updateBreakMovementMatrix() {
    mBreakMovement->movement();
    _8C = mBreakMovement->_8C;
    _8C.getTrans(mTranslation);
}

void TripodBossFixPartsBase::updateTripodMatrix() {
    TVec3f mul;
    calcTripodLocalMatrix(&_8C);
    MR::concatTripodBossAttachJointMatrix(&_8C, _CC);
    _8C.getTrans(mTranslation);

    if (_E0) {
        _8C.mult(_D0, mul);
        if (MR::isJudgedToClipFrustum(mul, mClippingDistance)) {
            MR::hideModelAndOnCalcAnim(this);
        }
        else {
            MR::showModel(this);
        }
    }
}

void TripodBossFixPartsBase::initBreakMovement(s32 level) {
    mBreakMovement = new TripodBossBreakMovement("三脚ボスパーツ破壊挙動");
    mBreakMovement->setBreakDownLevel(level);
}
