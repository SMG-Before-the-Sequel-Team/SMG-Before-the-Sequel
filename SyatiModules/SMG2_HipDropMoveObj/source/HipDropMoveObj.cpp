#include "HipDropMoveObj.h"
#include "Game/Player/MarioActor.h"
#include "Game/Util/ActionSoundUtil.h"
#include "Game/Util.h"

HipDropDemoMoveObj::~HipDropDemoMoveObj() {

}

HipDropMoveObj::HipDropMoveObj(const char* pName) : LiveActor(pName) {
}

void HipDropMoveObj::init(const JMapInfoIter& rIter) {
    MR::processInitFunction(this, rIter, false);

    MR::setClippingTypeSphereContainsModelBoundingBox(this, 100.0f);
    initNerve(&NrvHipDropMoveObj::HostTypeWait::sInstance, 0);
    MR::tryRegisterDemoCast(this, rIter);
    makeActorAppeared();
}

bool HipDropMoveObj::receiveMsgPlayerAttack(u32 msg, HitSensor* a2, HitSensor* a3) {
    if (!MR::isMsgPlayerHipDropFloor(msg))
        return false;

    if (!isNerve(&NrvHipDropMoveObj::HostTypeWait::sInstance))
        return false;

    f32 radius = a3->mRadius;
    f32 dist = MR::calcDistanceToPlayer(a3->mPosition);
    if (radius < dist)
        return false;

    setNerve(&NrvHipDropMoveObj::HostTypeMove::sInstance);
    return true;
}

bool HipDropMoveObj::isEndMove() const {
    return MR::isBckStopped(this);
}

void HipDropMoveObj::exeMove() {
    if (MR::isFirstStep(this)) {
        MR::startAction(this, cMoveAnimName);

        MR::startActionSound(this, "Move", -1, -1, -1);

        //MR::StageEffect::rumblePadStart(this, mObjectName);
        //MR::StageEffect::shakeCameraMoving(this, mObjectName);
        MR::tryStartDemoRegistered(this, 0);
        moveStart();
    }

    moving();

    if (isEndMove()) {
        if (MR::isValidSwitchA(this))
            MR::onSwitchA(this);

        //MR::StageEffect::rumblePadStop(this, mObjectName);
        //MR::StageEffect::stopShakingCameraMoving(this, mObjectName);

        MR::startActionSound(this, "Stop", -1, -1, -1);

        moveEnd();
        setNerve(&NrvHipDropMoveObj::HostTypeDone::sInstance);
    }
}

HipDropDemoMoveObj::HipDropDemoMoveObj(const char* pName) : HipDropMoveObj(pName) {
    mMtx.identity();
}

HipDropMoveObj::~HipDropMoveObj() {

}

void HipDropDemoMoveObj::moveStart() {
    MR::startBckPlayer("Wait", NULL);
    MtxPtr mtx = MR::getPlayerDemoActor()->getBaseMtx();
    TMtx34f stack_38;
    stack_38.set(mtx);
    MtxPtr jointMtx = MR::getJointMtx(this, cMoveJointName);
    TMtx34f stack_8;
    stack_8.set(jointMtx);
    stack_8.invert(stack_8);
    mMtx.concat(stack_8, stack_38);
}

void HipDropDemoMoveObj::moving() {
    if (MR::isDemoActive()) {
        MtxPtr jointMtx = MR::getJointMtx(this, cMoveJointName);
        TMtx34f stack_8;
        stack_8.set(jointMtx);
        stack_8.concat(stack_8, mMtx);
        TVec3f vec;
        LiveActor* demoActor = MR::getPlayerDemoActor();
        f32 z = stack_8.mMtx[2][3];
        f32 y = stack_8.mMtx[1][3];
        f32 x = stack_8.mMtx[0][3];
        demoActor->mTranslation.set(x, y, z);
        MR::setPlayerBaseMtx((MtxPtr)&stack_8);
    }
}

namespace NrvHipDropMoveObj {
    HostTypeWait HostTypeWait::sInstance;
    HostTypeMove HostTypeMove::sInstance;
    HostTypeDone HostTypeDone::sInstance;

    void HostTypeDone::execute(Spine* pSpine) const {

    }

    void HostTypeMove::execute(Spine* pSpine) const {
        HipDropMoveObj* obj = reinterpret_cast<HipDropMoveObj*>(pSpine->mExecutor);
        obj->exeMove();
    }

    void HostTypeWait::execute(Spine* pSpine) const {

    }
};

void HipDropMoveObj::moveStart() {

}

void HipDropMoveObj::moving() {

}

void HipDropMoveObj::moveEnd() {

}