#include "KinokoTrampoline.h"

KinokoTrampoline::KinokoTrampoline (const char *pName) : RailMoveObj(pName) {

}

KinokoTrampoline::~KinokoTrampoline () {

}

void KinokoTrampoline::init (const JMapInfoIter &rIter) {
    RailMoveObj::init(rIter);
    mLodCtrl->syncMaterialAnimation();
    mBounceCollision = MR::createCollisionPartsFromLiveActor(this, "Bounce", getSensor("Body"), MR::CollisionScaleType_Default);
    mIdleCollision = MR::createCollisionPartsFromLiveActor(this, "Idle", getSensor("Idle"), MR::CollisionScaleType_Default);
    
    if (MR::isValidSwitchA(this)) {
        MR::listenStageSwitchOnOffA(this, MR::Functor(this, &activate), MR::Functor(this, &deactivate));
        deactivate();
    }
    else 
        activate();
    
}

void KinokoTrampoline::control () {
    if (MR::isBckOneTimeAndStopped(this)) 
        if (isActive()) 
            MR::startAction(this, "Wait");
        else 
            MR::startAction(this, "Idle");
    
    if (!isActive() && MR::isBckPlaying(this, "Wait") && MR::isBckLooped(this)) 
        MR::startAction(this, "Idle");

    mBounceCollision->setMtx(*(TPos3f *)getBaseMtx());
    mIdleCollision->setMtx(*(TPos3f *)getBaseMtx());
    mBounceCollision->updateMtx();
    mIdleCollision->updateMtx();
    MapObjActor::control();
}

bool KinokoTrampoline::receiveOtherMsg(u32 msg, HitSensor *pSender, HitSensor *pReceiver) {
    if (isActive())
    {
        if (MR::isSensorPlayer(pSender) && !MR::isSensorEye(pSender))
        {
            if (isMsgNeedBounce(msg))
                MR::startAction(this, "Reaction");
        }
        //else
        //{
        //    MR::startAction(this, "Reaction");
        //    LiveActor* pOther = pSender->mActor;
        //    if (MR::isCalcGravity(pOther))
        //    {
        //        /*TVec3f upvec;
        //        MR::calcUpVec(&upvec, this);*/
        //        MR::addVelocityJump(pOther, 50.f);
        //    }
        //}
    }
    return RailMoveObj::receiveOtherMsg(msg, pSender, pReceiver);
}

void KinokoTrampoline::activate () {
    MR::validateHitSensor(this, "Body");
    MR::validateCollisionParts(mBounceCollision);
    MR::invalidateCollisionParts(mIdleCollision);
    MR::startAction(this, "Wait");
    MR::startBrk(this, "On");
    if (mLodCtrl->mIsValid) {
        MR::startBrk(mLodCtrl->mModelObjMiddle, "On");
        MR::startBrk(mLodCtrl->mModelObjLow, "On");
    }
}

void KinokoTrampoline::deactivate () {
    MR::invalidateHitSensor(this, "Body");
    MR::validateCollisionParts(mIdleCollision);
    MR::invalidateCollisionParts(mBounceCollision);
    MR::startBrk(this, "Off");
    if (mLodCtrl->mIsValid) {
        MR::startBrk(mLodCtrl->mModelObjMiddle, "Off");
        MR::startBrk(mLodCtrl->mModelObjLow, "Off");
    }
}

bool KinokoTrampoline::isActive () {
    return MR::isSensorValid(getSensor("Body"));
}