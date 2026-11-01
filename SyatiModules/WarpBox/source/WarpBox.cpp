#include "WarpBox.h"
#include "syati.h"

WarpBox::WarpBox (const char *pName) : LiveActor(pName) {
    mHorizontalForce = 5.0f;
    mVerticalForce = 25.0f;
    mEnterAfterExit = false;
    mColorFrame = 0;
    mBgmIndexLeft = -1;
    mBgmStateExited = -1;
}

WarpBox::~WarpBox () {

}

void WarpBox::init (const JMapInfoIter &rIter) {
    MR::initDefaultPos(this, rIter);
    MR::processInitFunction(this, rIter, false);
    MR::connectToSceneMapObj(this);
    MR::getJMapInfoArg0NoInit(rIter, &mHorizontalForce);
    MR::getJMapInfoArg1NoInit(rIter, &mVerticalForce);
    MR::getJMapInfoArg2NoInit(rIter, &mEnterAfterExit);
    MR::getJMapInfoArg3NoInit(rIter, &mColorFrame);
    MR::setBGMParams(&mBgmIndexLeft, &mBgmStateExited, rIter);
    MR::getJMapInfoArg6NoInit(rIter, &mWarpAnyActor);
    MR::useStageSwitchWriteB(this, rIter);
    
    if (!MR::isExistSceneObj(EXT_SCENE_OBJ_WARP_CUBE_MANAGER)) 
        MR::createSceneObj(EXT_SCENE_OBJ_WARP_CUBE_MANAGER);
    WarpBoxMgr::getManager()->entry(this, rIter);

    initHitSensor(1);
    MR::addHitSensorMapObj(this, "Body", 8, 70.0f, TVec3f(0, 50.0f, 0));
    initEffectKeeper(1, "WarpBox", false);
    makeActorAppeared();
    initNerve(&NrvWarpBox::NrvWait::sInstance, 0);
}

void WarpBox::attackSensor (HitSensor *pReceiver, HitSensor *pSender) {
    if (isNerve(&NrvWarpBox::NrvWait::sInstance)) {
        if (MR::isSensorPlayerOrYoshi(pSender)) 
            setNerve(&NrvWarpBox::NrvIn::sInstance);
        else if (mWarpAnyActor) 
            warpToOppositeWarpBox(&pSender->mActor->mTranslation);
    }
}

void WarpBox::warpPlayerToOppositeWarpBox () {
    warpToOppositeWarpBox(MR::getPlayerPos());
    MR::endDemo(MarioAccess::getPlayerActor(), "WarpBox");
}

void WarpBox::warpToOppositeWarpBox (TVec3f *pWarpPos) {
    WarpBox *pOppositeCube = WarpBoxMgr::getManager()->getOppositeCube(this);
    if (!pOppositeCube) {
        WarpBoxExitPoint *pExitPoint = WarpBoxMgr::getManager()->getExitPoint(this);
        pWarpPos->set(pExitPoint->mPosition);
        MR::showPlayer();
        setNerve(&NrvWarpBox::NrvWait::sInstance);
        return;
    }
    pWarpPos->set(pOppositeCube->mTranslation);
    pOppositeCube->setNerve(&NrvWarpBox::NrvOut::sInstance);
    setNerve(&NrvWarpBox::NrvWait::sInstance);
}

void WarpBox::exeWait () {
    if (MR::isFirstStep(this)) {
        MR::startAction(this, "WarpBox");
        MR::validateHitSensors(this);
    }
}

void WarpBox::exeIn () {
    MarioAccess::getPlayerActor()->mVelocity = TVec3f(0);
    if (MR::isFirstStep(this)) {
        MR::hidePlayer();
        MR::requestStartDemoWithoutCinemaFrame(this, "WarpBox", NULL, NULL);
        MR::emitEffect(this, "PlayerInSmoke");
        MR::startAction(this, "In");
        MR::invalidateHitSensors(this);
    }
    if (!MR::isBckPlaying(this, "In")) {
        MR::deleteEffect(this, "PlayerInSmoke");
        warpPlayerToOppositeWarpBox();
    }
}

void WarpBox::exeOut () {
    if (MR::isFirstStep(this)) {
        MR::startAction(this, "Out");
        MR::invalidateHitSensors(this);
        MR::requestStartDemoWithoutCinemaFrame(this, "WarpBox", NULL, NULL);
    }
    if (MR::isStep(this, 24)) {
        MR::emitEffect(this, "PlayerInSmoke");
        launchPlayerOut();
    }
    if (MR::isStep(this, 46)) {
        MR::deleteEffect(this, "PlayerInSmoke");
        setNerve(&NrvWarpBox::NrvWait::sInstance);
        if (!mEnterAfterExit) 
            kill();
    }
}

void WarpBox::launchPlayerOut () {
    MR::showPlayer();
    MR::endDemo(MarioAccess::getPlayerActor(), "WarpBox");
    MR::startSoundPlayer("SE_PV_JUMP_STAND_AUTO", -1, -1);
    MR::startBckPlayer("Jump", 0);

    TVec3f jumpVec;
    MR::calcFrontVec(&jumpVec, this);
    jumpVec.x *= mHorizontalForce;
    jumpVec.y = mVerticalForce;
    jumpVec.z *= mHorizontalForce;
    MR::forceJumpPlayer(jumpVec);
}

void WarpBox::control () {
    if (MR::testSubPadButtonC(0)) {
        TVec3f jumpVec;
        MR::calcFrontVec(&jumpVec, this);
        jumpVec.x *= mHorizontalForce;
        jumpVec.y = mVerticalForce;
        jumpVec.z *= mHorizontalForce;
        MR::forceJumpPlayer(jumpVec);
    }
}

namespace NrvWarpBox {
    void NrvWait::execute (Spine *pSpine) const {
        WarpBox *pActor = (WarpBox *)pSpine->mExecutor;
        pActor->exeWait();
    }

    void NrvIn::execute (Spine *pSpine) const {
        WarpBox *pActor = (WarpBox *)pSpine->mExecutor;
        pActor->exeIn();
    }

    void NrvOut::execute (Spine *pSpine) const {
        WarpBox *pActor = (WarpBox *)pSpine->mExecutor;
        pActor->exeOut();
    }

    NrvWait(NrvWait::sInstance);
    NrvIn(NrvIn::sInstance);
    NrvOut(NrvOut::sInstance);
}