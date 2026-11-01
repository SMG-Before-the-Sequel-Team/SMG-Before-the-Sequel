#include "SpaceCocoon.h"
#include "Game/Camera/CameraTargetArg.h"
#include "Game/Camera/CameraTargetMtx.h"
#include "Game/LiveActor/HitSensor.h"
#include "Game/LiveActor/PartsModel.h"
#include "Game/MapObj/PlantPoint.h"
#include "Game/Scene/SceneFunction.h"
#include "Game/Util/ActorCameraUtil.h"
#include "Game/Util/ActorMovementUtil.h"
#include "Game/Util/ActorSensorUtil.h"
#include "Game/Util/ActorSwitchUtil.h"
#include "Game/Util/EffectUtil.h"
#include "Game/Util/GamePadUtil.h"
#include "Game/Util/JMapUtil.h"
#include "Game/Util/LiveActorUtil.h"
#include "Game/Util/MathUtil.h"
#include "Game/Util/PlayerUtil.h"
#include "Game/Util/StarPointerUtil.h"
#include "JSystem/JGeometry/TVec.h"
#include "JSystem/JUtility/JUTTexture.h"
#include "revolution/gx/GXCull.h"
#include "revolution/gx/GXEnum.h"
#include "revolution/gx/GXGeometry.h"
#include "revolution/gx/GXLighting.h"
#include "revolution/gx/GXPixel.h"
#include "revolution/gx/GXTev.h"
#include "revolution/gx/GXTransform.h"
#include "revolution/gx/GXVert.h"
#include "revolution/mtx.h"

#include "Game/Util.h"

#ifdef STAR_POINTER_COMMAND_STREAM
#pragma message(__FILE__ "=> Command Stream Enabled.")
#include "CommandStream.h"
#endif

#define WPAD_CHAN0 0
#define WPAD_CHAN1 1

namespace NrvSpaceCocoon {
    FULL_NERVE(SpaceCocoonNrvStop, SpaceCocoon, Stop);
    FULL_NERVE(SpaceCocoonNrvFree, SpaceCocoon, Free);
    FULL_NERVE(SpaceCocoonNrvFreeInvalid, SpaceCocoon, FreeInvalid);
    FULL_NERVE(SpaceCocoonNrvKinopioLand, SpaceCocoon, BindLand);
    FULL_NERVE(SpaceCocoonNrvKinopioWait, SpaceCocoon, BindWait);
    FULL_NERVE(SpaceCocoonNrvWaitKinopioAimDemo, SpaceCocoon, WaitKinopioAimDemo);
    FULL_NERVE(SpaceCocoonNrvKinopioAim, SpaceCocoon, BindAim);
    FULL_NERVE(SpaceCocoonNrvKinopioAttack, SpaceCocoon, BindAttack);
    FULL_NERVE(SpaceCocoonNrvKinopioAttackSuccess, SpaceCocoon, BindAttackSuccess);
    FULL_NERVE(SpaceCocoonNrvBindLand, SpaceCocoon, BindLand);
    FULL_NERVE(SpaceCocoonNrvBindWait, SpaceCocoon, BindWait);
    FULL_NERVE(SpaceCocoonNrvBindAim, SpaceCocoon, BindAim);
    FULL_NERVE(SpaceCocoonNrvBindAttack, SpaceCocoon, BindAttack);
    FULL_NERVE(SpaceCocoonNrvBindAttackSuccess, SpaceCocoon, BindAttackSuccess);
}  // namespace NrvSpaceCocoon

namespace {
    static Color8 sColor(0xC8, 0xC8, 0xD7, 0xFF);
}

SpaceCocoon::SpaceCocoon(const char* pName)
    : LiveActor(pName), mIsKinopioCameraFocused(false), mNeutralPos(0.0f, 0.0f, 0.0f), mBasePos(0.0f, 0.0f, 0.0f), mSide(1.0f, 0.0f, 0.0f),
      mUp(0.0f, 1.0f, 0.0f), mFront(0.0f, 0.0f, 1.0f), mHeight(100.0f), mPointerPos(0.0f, 0.0f, 0.0f), mPadChannel(-1), mCocoonPos(0.0f, 0.0f, 0.0f),
      mSpringVel(0.0f, 0.0f, 0.0f), mAttackTime(60), mTouchTime(0), mRider(nullptr), mNumPoints(0), mPlantPoints(nullptr), mCameraTargetMtx(nullptr),
      mCameraInfo(nullptr), mCocoonModel(nullptr), mTexture(nullptr) {
    mBaseMtx.identity();
    mTopMtx.identity();
}

void SpaceCocoon::init(const JMapInfoIter& rIter) {
    // FIXME : annoying regswap, idx should be in r30
    // https://decomp.me/scratch/kjwvE
    MR::connectToScene(this, MR::MovementType_Ride, MR::CalcAnimType_Ride, MR::CategoryList_Null, MR::DrawType_SpaceCocoon);

    MR::initDefaultPos(this, rIter);

    MR::getJMapInfoArg0NoInit(rIter, &mAttackTime);
    MR::getJMapInfoArg1NoInit(rIter, &mHeight);
    MR::getJMapInfoArg3NoInit(rIter, &mIsKinopioCameraFocused);

    mBasePos.set(mTranslation);
    MR::calcActorAxis(&mSide, &mUp, &mFront, this);

    mNeutralPos.set(mUp);
    mNeutralPos.scale(mHeight);
    mNeutralPos.add(mTranslation);

    mTranslation.set(mNeutralPos);
    mCocoonPos.set(mNeutralPos);
    mPointerPos.set(mNeutralPos);

    initHitSensor(3);
    MR::addHitSensor(this, "Bind", ATYPE_TRANSFERABLE_BINDER, 8, 200.0f, TVec3f(0.0f, 100.0f, 0.0f));
    MR::addHitSensorMapObj(this, "Push", 8, 50.0f, TVec3f(0.0f, -50.0f, 0.0f));
    MR::addHitSensor(this, "Attack", 12, 8, 150.0f, TVec3f(0.0f, 0.0f, 0.0f));
    initBinder(50.0f, 50.0f, 8);
    MR::offBind(this);

    initSound(6, false, nullptr, TVec3f(0.0f, 0.0f, 0.0f));

    if (mIsKinopioCameraFocused) {
        MR::needStageSwitchWriteA(this, rIter);
    }

    initNerve(&NrvSpaceCocoon::SpaceCocoonNrvStop::sInstance, 0);

    mNumPoints = mHeight / 10.0f;
    mPlantPoints = new PlantPoint*[mNumPoints];

    f32 delta = mHeight / (mNumPoints + 1);

    for (s32 idx = 0; idx < mNumPoints; idx++) {
        TVec3f spawnPos(mUp);
        s32 revIdx = mNumPoints - idx;
        spawnPos.scale(delta * revIdx);
        spawnPos.add(mBasePos);

        // something got optimized out here, or this is what was written
        f32 f = MR::getEaseInValue((f32)idx / mNumPoints, 0.0f, 1.0f, 1.0f);

        f32 thickness = 1.0f;
        s32 prevRevIdx = (mNumPoints - 1) - idx;
        if (prevRevIdx < 3) {
            thickness = MR::getEaseOutValue(prevRevIdx / 3.0f, 2.0f, 1.0f, 1.0f);
        }

        mPlantPoints[idx] = new PlantPoint(spawnPos, mUp, mSide, thickness);
    }

    PlantPoint* topPoint = mPlantPoints[0];
    mTopMtx.setXYZDir(topPoint->mFront, topPoint->mUp, topPoint->mSide);
    mTopMtx.setTrans(topPoint->mPosition);

    mCocoonModel = new PartsModel(this, "先端", "SpaceCocoon", mTopMtx, MR::DrawBufferType_NoSilhouettedMapObj, false);
    mCocoonModel->initWithoutIter();

    updateDrawPoints();
    MR::initStarPointerTarget(this, 300.0f, TVec3f(0.0f, 50.0f, 0.0f));

    MR::calcGravity(this);

    MR::initShadowVolumeSphere(this, 100.0f);
    MR::setClippingFar200m(this);

    mCameraTargetMtx = new CameraTargetMtx("カメラターゲットダミー");
    MR::initMultiActorCamera(this, rIter, &mCameraInfo, "ウェイト");  // "wait"
    MR::initMultiActorCamera(this, rIter, &mCameraInfo, "狙い中");    // "aim"
    MR::initMultiActorCamera(this, rIter, &mCameraInfo, "攻撃中");    // "attack"

    mTexture = new JUTTexture(MR::loadTexFromArc("SpaceCocoon.arc", "SpaceCocoon.bti", nullptr), 0);
    makeActorAppeared();
}

void SpaceCocoon::exeStop() {
    if (MR::isFirstStep(this)) {
        mTouchTime = 0;
    }

    if (tryTouch()) {
        return;
    }
}

void SpaceCocoon::exeFree() {
    if (MR::isFirstStep(this)) {
        MR::validateHitSensor(this, "Bind");
    }

    tryTouch();

    if (updateSpringPoint()) {
        mSpringVel.zero();
        mCocoonPos.set(mNeutralPos);
        mTranslation.set(mNeutralPos);
        setNerve(&NrvSpaceCocoon::SpaceCocoonNrvStop::sInstance);
    } else {
        mTranslation.set(mCocoonPos);
    }
}

void SpaceCocoon::exeFreeInvalid() {
    if (MR::isFirstStep(this)) {
        MR::invalidateHitSensors(this);
    }

    updateSpringPoint();
    mTranslation.set(mCocoonPos);

    if (!MR::isGreaterStep(this, 5)) {
        return;
    }

    if (PSVECDistance(mRider->mTranslation.toVecPtr(), mTranslation.toVecPtr()) > 300.0f || MR::isOnGroundPlayer()) {
        mRider = nullptr;
        MR::validateClipping(this);
        MR::validateHitSensors(this);
        setNerve(&NrvSpaceCocoon::SpaceCocoonNrvFree::sInstance);
    }
}

void SpaceCocoon::exeBindLand() {
    if (MR::isFirstStep(this)) {
        if (!isKinopioAttached()) {
            MR::startBckPlayer("CocoonLand", nullptr); // this could be wrong I think
            MR::startMultiActorCameraTargetOther(this, mCameraInfo, "ウェイト", CameraTargetArg(mCameraTargetMtx), -1);
            MR::startSound(mRider, "SE_PV_CATCH", -1, -1);
        } else {
            MR::startBckWithInterpole(mRider, "CocoonLand", 1);
            setNerve(&NrvSpaceCocoon::SpaceCocoonNrvKinopioWait::sInstance);
            return;
        }
        MR::startSound(this, "SE_OJ_SPACE_COCOON_ATTACH", -1, -1);
    }

    if (isKinopioAttached()) {
        setNerve(&NrvSpaceCocoon::SpaceCocoonNrvBindWait::sInstance);
        return;
    }

    if (!updateBindWait()) {
        tryTouch();
        if (MR::isBckStopped(mRider)) {
            setNerve(&NrvSpaceCocoon::SpaceCocoonNrvBindWait::sInstance);
        }
    }
}

void SpaceCocoon::exeBindWait() {
    if (MR::isFirstStep(this)) {
        if (!isKinopioAttached()) {
            MR::startBckPlayer("CocoonWait", static_cast< const char* >(0));
        } else {
            MR::startBck(mRider, "CocoonWait", static_cast< const char* >(0));
            MR::validateClipping(this);
            MR::validateClipping(mRider);
            MR::sendSimpleMsgToActor(ACTMES_NPC_EVENT_TALK_ENABLE, mRider);
        }
        mVelocity.zero();
    }

    if (updateBindWait()) {
        return;
    }

    if (tryTouch()) {
        if (!isKinopioAttached()) {
            MR::startBckPlayer("CocoonReaction", static_cast< const char* >(0));
        }
    } else {
        if (!isKinopioAttached() && MR::isBckOneTimeAndStopped(mRider)) {
            MR::startBckPlayer("CocoonWait", static_cast< const char* >(0));
        }
    }
}

void SpaceCocoon::exeWaitKinopioAimDemo() {
    bool result = false;
    { // MR::tryStartDemoWithoutCinemaFrameValidStarPointer(this, "キノピオ狙い中")
        if (MR::canStartDemo()) {
            DemoStartRequestUtil::startDemoSystem(this, "キノピオ狙い中", 1, DemoStartInfo::DEMOTYPE_0, DemoStartInfo::CINEMAFRAMETYPE_1, DemoStartInfo::STARPOINTERTYPE_1, DemoStartInfo::DELETEEFFECTYPE_0, nullptr);
            MR::endStarPointerMode(this); // The above function will start a star pointer mode we don't want. This will discard it.
            result = true;
        }
    }

    if (result) {
        MR::startMultiActorCameraTargetPlayer(this, mCameraInfo, "狙い中", -1);
        setNerve(&NrvSpaceCocoon::SpaceCocoonNrvKinopioAim::sInstance);
    }
}

void SpaceCocoon::exeBindAim() {
    if (MR::isFirstStep(this)) {
        if (isKinopioAttached()) {
            MR::sendSimpleMsgToActor(ACTMES_NPC_EVENT_TALK_DISABLE, mRider);
        }

        MR::startSound(this, "SE_OJ_SPACE_COCOON_DRAG_ST", -1, -1);

        if (mPadChannel == WPAD_CHAN1) {
            MR::startCSSound2P("CS_DPD_HIT", nullptr, 0, 1.0f);
        }
    }

    f32 dist = PSVECDistance(mTranslation.toVecPtr(), mNeutralPos.toVecPtr());
    if (dist >= 100.0f) {
        MR::startLevelSound(this, "SE_OJ_LV_SPACE_COCOON_DRAG", ((dist - 100.0f) / 400.0f) * 100.0f, -1, -1);
    }

    if (!tryRelease()) {
        updateHang();
    }
}

void SpaceCocoon::exeBindAttack() {
    if (MR::isFirstStep(this)) {
        if (!isKinopioAttached()) {
            MR::startBckPlayer("CocoonFly", static_cast< const char* >(0));
            MR::startSound(mRider, "SE_PV_JUMP_JOY", -1, -1);
        } else {
            MR::startBck(mRider, "CocoonFly", static_cast< const char* >(0));
            MR::invalidateClipping(this);
            MR::invalidateClipping(mRider);
            MR::startSound(mRider, "SE_SV_KINOPIO_TALK_GLAD_FLY", -1, -1);
        }
        MR::startSound(this, "SE_OJ_SPACE_COCOON_LAUNCH", -1, -1);
        MR::onBind(this);
    }

    updateBindAttack();

    if (tryAttackMap() || MR::isStep(this, mAttackTime)) {
        MR::endMultiActorCamera(this, mCameraInfo, "攻撃中", true, -1);

        if (MR::isStep(this, mAttackTime) && !isKinopioAttached()) {
            MR::startBckPlayer("AirRotation", static_cast< const char* >(0));
        }

        endBind(TVec3f(0.0f, 0.0f, 0.0f), false);
    }
}

void SpaceCocoon::exeBindAttackSuccess() {
    if (MR::isFirstStep(this)) {
    }

    updateBindAttack();

    if (MR::isStep(this, 1)) {
        MR::stopScene(8);
    }

    if (MR::isStep(this, 2)) {
        if (isKinopioAttached()) {
            setNerve(&NrvSpaceCocoon::SpaceCocoonNrvKinopioAttack::sInstance);
        } else {
            setNerve(&NrvSpaceCocoon::SpaceCocoonNrvBindAttack::sInstance);
        }
    }
}

void SpaceCocoon::calcAnim() {
    if (isKinopioAttached()) {
        updateActorMtx();
        MR::setBaseTRMtx(mRider, mBaseMtx);
        mRider->mTranslation.set(mTranslation);
    }
}

void SpaceCocoon::attackSensor(HitSensor* pSender, HitSensor* pReceiver) {
    if (!isKinopioAttached() && mRider == nullptr && MR::isSensorNpc(pReceiver) && pReceiver->receiveMessage(ACTMES_NPC_EVENT_START, pSender)) {
        mRider = pReceiver->mActor;
        MR::emitEffect(mCocoonModel, "Stick");
        TVec3f side(mSide);
        TVec3f front(mFront);
        side.scale(mSide.dot(mRider->mVelocity));
        front.scale(mFront.dot(mRider->mVelocity));
        mSpringVel.set(side);
        mSpringVel.add(front);
        mRider->mVelocity.zero();
        setNerve(&NrvSpaceCocoon::SpaceCocoonNrvKinopioWait::sInstance);
        return;
    }

    if (mRider == nullptr || isNerve(&NrvSpaceCocoon::SpaceCocoonNrvKinopioWait::sInstance) ||
        isNerve(&NrvSpaceCocoon::SpaceCocoonNrvFreeInvalid::sInstance)) {
        if (!MR::isSensorType(pSender, ATYPE_SPRING_ATTACKER) &&
            (isKinopioAttached() || isNerve(&NrvSpaceCocoon::SpaceCocoonNrvFreeInvalid::sInstance))) {
            MR::sendMsgPush(pReceiver, pSender);
        }
        return;
    }

    if (MR::tryGetItem(pSender, pReceiver)) {
        return;
    }
}

bool SpaceCocoon::receiveMsgPlayerAttack(u32 msg, HitSensor* pSender, HitSensor* pReceiver) {
    if (!MR::isMsgStarPieceReflect(msg)) {
        return false;
    }

    if (isNerve(&NrvSpaceCocoon::SpaceCocoonNrvFree::sInstance) || isNerve(&NrvSpaceCocoon::SpaceCocoonNrvFreeInvalid::sInstance)) {
        MR::addVelocitySeparateHV(this, pSender, pReceiver, 5.0f, 0.0f);
        mSpringVel.add(mVelocity);
        MR::zeroVelocity(this);
    }

    return true;
}

bool SpaceCocoon::receiveMsgEnemyAttack(u32 msg, HitSensor* pSender, HitSensor* pReceiver) {
    if (mRider != nullptr && MR::isEqualSensor(pReceiver, pReceiver->mActor, "Bind")) {
        MR::endBindAndPlayerDamageMsg(this, msg);
        endBind(TVec3f(0.0f, 0.0f, 0.0f), true);
        return true;
    }

    return false;
}

bool SpaceCocoon::receiveOtherMsg(u32 msg, HitSensor* pSender, HitSensor* pReceiver) {
    if (MR::isMsgAutoRushBegin(msg)) {
        if (isNerve(&NrvSpaceCocoon::SpaceCocoonNrvFreeInvalid::sInstance) || isKinopioAttached()) {
            return false;
        }
        mRider = pSender->mActor;
        MR::invalidateClipping(this);
        TVec3f side(mSide);
        TVec3f front(mFront);
        side.scale(mSide.dot(*MR::getPlayerVelocity()));
        front.scale(mFront.dot(*MR::getPlayerVelocity()));
        mSpringVel.set(side);
        mSpringVel.add(front);
        MR::emitEffect(mCocoonModel, "Stick");
        mTranslation.set(mNeutralPos);
        mCameraTargetMtx->mMatrix.set(mTopMtx);
        mCameraTargetMtx->mMatrix.setTrans(mTranslation);
        MR::invalidateHitSensor(this, "Bind");
        if (MR::isStarPointerInScreen(WPAD_CHAN0)) {
            MR::calcStarPointerPosOnPlane(&mPointerPos, mTranslation, mUp, WPAD_CHAN0, false);
        }
        setNerve(&NrvSpaceCocoon::SpaceCocoonNrvBindLand::sInstance);
        return true;
    }

    if (MR::isMsgUpdateBaseMtx(msg)) {
        updateActorMtx();
        MR::setBaseTRMtx(mRider, mBaseMtx);
        return true;
    }

    if (MR::isMsgIsRushTakeOver(msg)) {
        return !isNerve(&NrvSpaceCocoon::SpaceCocoonNrvFreeInvalid::sInstance);
    }

    if (MR::isMsgRushCancel(msg)) {
        endBind(TVec3f(0.0f, 0.0f, 0.0f), false);
        return true;
    }

    return false;
}

bool SpaceCocoon::updateBindWait() {
    if (PSVECDistance(MR::getPlayerPos()->toVecPtr(), mTranslation.toVecPtr()) < 3000.0f && (!isKinopioAttached() || MR::isOnGroundPlayer())) {
        if (MR::isStarPointerPointing(this, WPAD_CHAN0, true, "弱")) {
            MR::requestStarPointerModeBlueStarReady(this);
        }

        bool set = tryStartCommandStream();


        if (set) {
            mPadChannel = WPAD_CHAN0;
        }

        if (mPadChannel >= 0) {
            if (isKinopioAttached()) {
                setNerve(&NrvSpaceCocoon::SpaceCocoonNrvWaitKinopioAimDemo::sInstance);
            } else {
                MR::endMultiActorCamera(this, mCameraInfo, "ウェイト", true, -1);
                MR::startMultiActorCameraTargetOther(this, mCameraInfo, "狙い中", CameraTargetArg(mCameraTargetMtx), -1);
                setNerve(&NrvSpaceCocoon::SpaceCocoonNrvBindAim::sInstance);
            }
            return true;
        }
    }

    updateSpringPoint();
    mTranslation.set(mCocoonPos);
    return false;
}

bool SpaceCocoon::updateSpringPoint() {
    mSpringVel.scale(0.95);

    TVec3f v1(mNeutralPos);
    v1.sub(mCocoonPos);

    TVec3f v2(v1);
    v2.scale(0.02f);
    mSpringVel.add(v2);
    mCocoonPos.add(mSpringVel);

    updateDrawPoints();

    if (PSVECMag(mSpringVel.toVecPtr()) < 0.1f && PSVECDistance(mCocoonPos.toVecPtr(), mNeutralPos.toVecPtr()) < 1.0f) {
        mCocoonPos.set(mNeutralPos);
        mSpringVel.zero();
        return true;
    }

    return false;
}

void SpaceCocoon::updateHang() {
    f32 dist = PSVECDistance(mTranslation.toVecPtr(), mNeutralPos.toVecPtr());

    if (MR::isStarPointerInScreen(mPadChannel) && 100.0f > dist) {
        f32 dummy = 0.0f;
    }

    if (dist >= 500.0f) {
        MR::startLevelSound(this, "SE_OJ_LV_SPACE_COCOON_DRAG3", -1, -1, -1);
    } else if (dist >= 100.0f) {
        MR::startLevelSound(this, "SE_OJ_LV_SPACE_COCOON_DRAG2", -1, -1, -1);
    }

    if (MR::isStarPointerInScreen(mPadChannel)) {
        MR::calcStarPointerPosOnPlane(&mPointerPos, mNeutralPos, mUp, mPadChannel, false);
    }

    TVec3f pos(mPointerPos);
    if (PSVECDistance(pos.toVecPtr(), mNeutralPos.toVecPtr()) > 500.0f) {
        pos.set(mPointerPos);
        pos.sub(mNeutralPos);
        MR::normalize(&pos);
        pos.scale(500.0f);
        pos.add(mNeutralPos);
    }

    TVec3f temp(pos);
    temp.scale(0.03f);
    TVec3f temp2(mTranslation);
    temp2.scale(0.97f);
    temp.add(temp2);

    mTranslation.set(temp);
    mCocoonPos.set(mTranslation);
    updateDrawPoints();

    if (mRider == nullptr) {
        return;
    }

    if (dist < 200.0f && !MR::isBckPlaying(mRider, "CocoonWait")) {
        if (isKinopioAttached()) {
            MR::startBckWithInterpole(mRider, "CocoonWait", 10);
        } else {
            MR::startBckPlayer("CocoonWait", nullptr);
        }
    }

    if (dist > 300.0f && !MR::isBckPlaying(mRider, "CocoonStretch")) {
        if (isKinopioAttached()) {
            MR::startBckWithInterpole(mRider, "CocoonStretch", 10);
        } else {
            MR::startBckPlayer("CocoonStretch", nullptr);
        }
    }
}

void SpaceCocoon::updateBindAttack() {
    if (getNerveStep() >= mAttackTime - 90) {
        mVelocity.scale(0.98);
    }
    updateSpringPoint();
}

void SpaceCocoon::updateActorMtx() {
    if (isNerve(&NrvSpaceCocoon::SpaceCocoonNrvBindAttack::sInstance) || isNerve(&NrvSpaceCocoon::SpaceCocoonNrvBindAttackSuccess::sInstance) ||
        isNerve(&NrvSpaceCocoon::SpaceCocoonNrvKinopioAttack::sInstance) || isNerve(&NrvSpaceCocoon::SpaceCocoonNrvKinopioAttackSuccess::sInstance)) {
        mBaseMtx.setTrans(mTranslation);
    } else {
        mBaseMtx.set(mTopMtx);
    }
}

void SpaceCocoon::updateDrawPoints() {
    TVec3f prevPos(mCocoonPos);
    f32 delta = mHeight / (mNumPoints + 1);

    for (s32 idx = 0; idx < mNumPoints; idx++) {
        f32 t = MR::getEaseOutValue(static_cast< f32 >(idx + 1) / static_cast< f32 >(mNumPoints + 1), 0.0f, 1.0f, 1.0f);
        t = MR::getEaseOutValue(t, 0.0f, 1.0f, 1.0f);

        TVec3f baseUp(mUp);
        baseUp.scale((idx + 1) * delta);

        TVec3f pos;
        pos.x = mCocoonPos.x * (1.0f - t) + mNeutralPos.x * t;
        pos.y = mCocoonPos.y * (1.0f - t) + mNeutralPos.y * t;
        pos.z = mCocoonPos.z * (1.0f - t) + mNeutralPos.z * t;

        pos.sub(baseUp);

        TVec3f up(prevPos);
        up.sub(pos);
        MR::normalize(&up);

        TVec3f side(mSide);
        TVec3f front(mFront);

        if (MR::isNearZero(side.dot(up), 0.001f)) {
            PSVECCrossProduct(up.toVecPtr(), front.toVecPtr(), side.toVecPtr());
            MR::normalize(&side);
            PSVECCrossProduct(side.toVecPtr(), up.toVecPtr(), front.toVecPtr());
            MR::normalize(&front);
        } else {
            PSVECCrossProduct(side.toVecPtr(), up.toVecPtr(), front.toVecPtr());
            MR::normalize(&front);
            PSVECCrossProduct(up.toVecPtr(), front.toVecPtr(), side.toVecPtr());
            MR::normalize(&side);
        }

        mPlantPoints[idx]->setAxisAndPos(side, up, front, pos);

        prevPos.set(pos);
    }

    PlantPoint* top = mPlantPoints[0];
    mTopMtx.setXYZDir(mPlantPoints[0]->mSide, mPlantPoints[0]->mUp, mPlantPoints[0]->mFront);
    mTopMtx.setTrans(top->mPosition);
}

namespace {
    bool tryTouchPointer(LiveActor* pActor, s32 padChannel) {
        if (MR::isStarPointerPointing(pActor, padChannel, false, "弱") && MR::getStarPointerScreenSpeed(padChannel) > 5.0f) {
            return true;
        }
        return false;
    }
}  // namespace

bool SpaceCocoon::tryTouch() {
    if (mTouchTime > 0) {
        mTouchTime--;
        return false;
    }

    s32 touchChannel = -1;
    if (tryTouchPointer(this, WPAD_CHAN0)) {
        touchChannel = WPAD_CHAN0;
    } else if (tryTouchPointer(this, WPAD_CHAN1)) {
        touchChannel = WPAD_CHAN1;
    }

    if (touchChannel >= 0) {
        TVec3f pointerVel(0.0f, 0.0f, 0.0f);
        MR::calcStarPointerWorldVelocityDirectionOnPlane(&pointerVel, mNeutralPos, mUp, touchChannel);
        pointerVel.scale(5.0f);
        mSpringVel.add(pointerVel);

        MR::startSound(this, "SE_OJ_SPACE_COCOON_BOUND", -1, -1);

        if (isNerve(&NrvSpaceCocoon::SpaceCocoonNrvStop::sInstance)) {
            setNerve(&NrvSpaceCocoon::SpaceCocoonNrvFree::sInstance);
        }

        mTouchTime = 20;
        return true;
    }

    return false;
}

bool SpaceCocoon::tryRelease() {
    if (MR::testCorePadButtonA(mPadChannel)) {
        return false;
    }

    endCommandStream();
    MR::endMultiActorCamera(this, mCameraInfo, "狙い中", true, -1);

    if (PSVECDistance(mTranslation.toVecPtr(), mNeutralPos.toVecPtr()) < 100.0f) {
        if (isKinopioAttached()) {
            MR::endDemo(this, "キノピオ狙い中");
            setNerve(&NrvSpaceCocoon::SpaceCocoonNrvKinopioWait::sInstance);
        } else {
            MR::startMultiActorCameraTargetOther(this, mCameraInfo, "ウェイト", CameraTargetArg(mCameraTargetMtx), -1);
            setNerve(&NrvSpaceCocoon::SpaceCocoonNrvBindWait::sInstance);
        }
        return true;
    }

    MR::emitEffect(mCocoonModel, "Stick");

    if (mIsKinopioCameraFocused) {
        MR::onSwitchA(this);
        mIsKinopioCameraFocused = false;
    }

    mVelocity.set(mNeutralPos);
    mVelocity.sub(mTranslation);
    MR::normalize(&mVelocity);

    // turn mario so he flies feet-first
    TVec3f reaxisUp(mVelocity);
    reaxisUp.scale(-1.0f);
    TVec3f reaxisFront(mUp);
    TVec3f reaxisSide;
    PSVECCrossProduct(reaxisUp.toVecPtr(), reaxisFront.toVecPtr(), reaxisSide.toVecPtr());
    MR::normalize(&reaxisSide);
    PSVECCrossProduct(reaxisSide.toVecPtr(), reaxisUp.toVecPtr(), reaxisFront.toVecPtr());
    MR::normalize(&reaxisFront);
    mBaseMtx.setXYZDir(reaxisSide, reaxisUp, reaxisFront);

    mVelocity.scale(60.0f);
    MR::emitEffect(mRider, "SpaceCocoonBlur");

    MR::startMultiActorCameraNoTarget(this, mCameraInfo, "攻撃中", -1);

    if (isKinopioAttached()) {
        setNerve(&NrvSpaceCocoon::SpaceCocoonNrvKinopioAttack::sInstance);
    } else {
        setNerve(&NrvSpaceCocoon::SpaceCocoonNrvBindAttack::sInstance);
    }

    return true;
}

bool SpaceCocoon::tryAttackMap() {
    if (!MR::isBinded(this)) {
        return false;
    }

    if (MR::isLessStep(this, 20)) {
        return false;
    }

    bool attackSuccess = true;
    for (s32 plane = 0; plane < MR::getBindedPlaneNum(this); plane++) {
        HitSensor* planeSensor = MR::getBindedPlaneSensor(this, plane);
        if (!MR::sendMsgEnemyAttack(planeSensor, getSensor("Attack"))) {
            if (!planeSensor->receiveMessage(ACTMES_IS_BROKEN, getSensor("Attack"))) {
                attackSuccess = false;
            }
        }
    }

    if (attackSuccess) {
        if (isKinopioAttached()) {
            setNerve(&NrvSpaceCocoon::SpaceCocoonNrvKinopioAttackSuccess::sInstance);
        } else {
            setNerve(&NrvSpaceCocoon::SpaceCocoonNrvBindAttackSuccess::sInstance);
        }
        return false;
    }

    return true;
}

bool SpaceCocoon::isKinopioAttached() const {
    if (isNerve(&NrvSpaceCocoon::SpaceCocoonNrvKinopioWait::sInstance) || isNerve(&NrvSpaceCocoon::SpaceCocoonNrvWaitKinopioAimDemo::sInstance) ||
        isNerve(&NrvSpaceCocoon::SpaceCocoonNrvKinopioAim::sInstance) || isNerve(&NrvSpaceCocoon::SpaceCocoonNrvKinopioAttack::sInstance) ||
        isNerve(&NrvSpaceCocoon::SpaceCocoonNrvKinopioAttackSuccess::sInstance)) {
        return true;
    }

    return false;
}

void SpaceCocoon::endBind(const TVec3f& rJumpVec, bool b) {
    MR::endMultiActorCamera(this, mCameraInfo, "ウェイト", true, -1);
    MR::endMultiActorCamera(this, mCameraInfo, "狙い中", true, -1);
    MR::endMultiActorCamera(this, mCameraInfo, "攻撃中", true, -1);
    MR::deleteEffect(mRider, "SpaceCocoonBlur");

    if (!b) {
        if (isKinopioAttached()) {
            MR::validateClipping(mRider);
            mRider->mVelocity.set(mVelocity);
            mRider->receiveMessage(ACTMES_NPC_EVENT_END, MR::getMessageSensor(), MR::getMessageSensor());
            MR::endDemo(this, "キノピオ狙い中");
        } else {
            MR::endBindAndPlayerJump(this, rJumpVec, 0);
        }
    }

    endCommandStream();
    mVelocity.zero();
    mTranslation.set(mNeutralPos);
    setNerve(&NrvSpaceCocoon::SpaceCocoonNrvFreeInvalid::sInstance);
}

#include "Game/System/StarPointerOnOffController.h"

extern "C" {
    StarPointerOnOffController* __kAutoMap_8005BCB0(); // getStarPointerOnOffController__29@unnamed@StarPointerUtil_cpp@Fv
}

bool SpaceCocoon::tryStartCommandStream() {
#ifdef STAR_POINTER_COMMAND_STREAM
    if (MR::isStarPointerPointing(this, 0, true, "弱") && MR::testCorePadTriggerA(WPAD_CHAN0) &&
        MR::tryStartStarPointerCommandStream(this, &mTranslation, WPAD_CHAN0, false)) {
        return true;
    }
    return false;
#else
    if (MR::isStarPointerPointing(this, 0, true, "弱") && MR::testCorePadTriggerA(WPAD_CHAN0) && MR::isStarPointerModeBlueStarReady()) {
        MR::startStarPointerModeBlueStar(this);
        return true;
    }
    return false;
#endif
}

void SpaceCocoon::endCommandStream() {
#ifdef STAR_POINTER_COMMAND_STREAM
    if (mPadChannel >= 0 && MR::isStarPointerCommandStream(this, mPadChannel)) {
        MR::tryEndStarPointerCommandStream(this, mPadChannel);
        mPadChannel = -1;
    }
#else
    StarPointerOnOffController* pController = __kAutoMap_8005BCB0();
    bool isMode = pController->mMode == StarPointerMode_BlueStar;

    if (mPadChannel >= 0 && isMode) {
        MR::endStarPointerMode(this);
        mPadChannel = -1;
    }
#endif
}

void SpaceCocoon::draw() const {
    // FIXME: shenanigans afoot at the beginning here, and honestly all over...
    // https://decomp.me/scratch/6froM

    initDraw();

    f32 f1 = -0.5f;
    f32 f2 = 0.0f;

    drawPlane(25.0f, -10.0f, 0.0f, -25.0f, sColor, sColor, f1, f2);  // -0.5f, 0.0f
    f2 = f1;
    f1 += -0.5f;
    drawPlane(25.0f, 10.0f, 25.0f, -10.0f, sColor, sColor, f1, f2);  // -1.0f, -0.5f
    f2 = f1;
    f1 += -0.5f;
    drawPlane(0.0f, 25.0f, 25.0f, 10.0f, sColor, sColor, f1, f2);  // -1.5f, -1.0f
    f2 = f1;
    f1 += -0.5f;
    drawPlane(-25.0f, 10.0f, 0.0f, 25.0f, sColor, sColor, f1, f2);  // -2.0f, -1.5f
    f2 = f1;
    f1 += -0.5f;
    drawPlane(-25.0f, -10.0f, -25.0f, 10.0f, sColor, sColor, f1, f2);  // -2.5f, -2.0f
    f2 = f1;
    f1 += -0.5f;
    drawPlane(0.0f, -25.0f, -25.0f, -10.0f, sColor, sColor, f1, f2);  // -3.0f, -2.5f
}

namespace {
    void drawPoints(const TVec3f& rPos, const TVec3f& rSide, const TVec3f& rFront, f32 thickness, f32 x1, f32 y1, f32 x2, f32 y2, Color8 color1, Color8 color2, f32 texX1, f32 texX2, f32 texY) {
        TVec3f side(rSide);
        TVec3f front(rFront);
        side.scale(thickness);
        front.scale(thickness);

        GXPosition3f32(rPos.x + side.x * x1 + front.x * y1, rPos.y + side.y * x1 + front.y * y1, rPos.z + side.z * x1 + front.z * y1);
        GXColor1u32(color1.mColor);
        GXTexCoord2f32(texX1, texY);

        GXPosition3f32(rPos.x + side.x * x2 + front.x * y2, rPos.y + side.y * x2 + front.y * y2, rPos.z + side.z * x2 + front.z * y2);
        GXColor1u32(color2.mColor);
        GXTexCoord2f32(texX2, texY);
    }
}  // namespace

void SpaceCocoon::drawPlane(f32 x1, f32 y1, f32 x2, f32 y2, Color8 color1, Color8 color2, f32 texX1, f32 texX2) const {
    s32 numPoints = mNumPoints + 2;
    f32 delta = 1.0f / numPoints;

    GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, numPoints * 2);

    drawPoints(mCocoonPos, mSide, mFront, 1.0f, x1, y1, x2, y2, color1, color2, texX1, texX2, 0.0f);

    for (s32 idx = 0; idx < mNumPoints; idx++) {
        f32 thickness = mPlantPoints[idx]->mThickness;

        drawPoints(mPlantPoints[idx]->mPosition, mPlantPoints[idx]->mSide, mPlantPoints[idx]->mFront, thickness, x1, y1, x2, y2, color1, color2,
                   texX1, texX2, (idx + 1) * delta);
    }

    drawPoints(mBasePos, mSide, mFront, 2.0f, x1, y1, x2, y2, color1, color2, texX1, texX2, 1.0f);
}

void SpaceCocoon::initDraw() const {
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_POS_XYZ, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_POS_XYZ, GX_F32, 0);

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);

    GXLoadPosMtxImm(MR::getCameraViewMtx(), 0);
    GXSetCurrentMtx(0);

    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_VTX, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetChanCtrl(GX_COLOR1A1, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetChanMatColor(GX_COLOR0A0, (GXColor){0xFF, 0x00, 0x00, 0xFF});

    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 60, GX_FALSE, 125);
    mTexture->load(GX_TEXMAP0);

    GXSetNumIndStages(0);

    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_KONST, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

    GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ZERO, GX_LO_NOOP);
    GXSetAlphaCompare(GX_GREATER, 0, GX_AOP_OR, GX_GREATER, 0);

    GXSetZMode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
    GXSetZCompLoc(GX_ENABLE);
    GXSetCullMode(GX_CULL_BACK);
}
