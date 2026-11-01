#include "Game/MapObj/SpiderCoin.h"
#include "Game/LiveActor/PartsModel.h"
#include "Game/MapObj/SpiderThread.h"
#include "Game/Scene/SceneFunction.h"
#include "Game/Util/DemoUtil.h"
#include "Game/Util/LiveActorUtil.h"
#include "Game/Util/MathUtil.h"
#include "Game/Util/MtxUtil.h"
#include "Game/Util/ObjUtil.h"
#include "Game/Util/StarPointerUtil.h"
#include "JSystem/JGeometry/TMatrix.h"
#include "JSystem/JGeometry/TVec.h"
#include "revolution/mtx.h"
#include "revolution/types.h"

#include "Game/Util.h"
#include "SpiderThreadExt.h"

namespace {
    static const f32 sCoinSpeed = 20.0f;
    static const s32 sStepToTouch2nd = 5;
    static const s32 sStepToucnAndApart = 30;
    static const f32 sRotateSpeedY = 30.0f;
    static const f32 sPointerSpeedMin = 5.0f;
    static const f32 sPointerSpeedDotToKill = -20.0f;
    static const f32 sModelOffset = 50.0f;
    static const f32 sCocoonOffsetY = 20.0f;
    static const f32 sCocoonOffsetZ = 20.0f;
}  // namespace

namespace NrvSpiderCoin {
    FULL_NERVE(SpiderCoinNrvWait, SpiderCoin, Wait);
    FULL_NERVE(SpiderCoinNrvTouch, SpiderCoin, Touch);
    FULL_NERVE(SpiderCoinNrvTouchAndApart, SpiderCoin, TouchAndApart);
}  // namespace NrvSpiderCoin

#define WPAD_CHAN0 0
#define WPAD_CHAN1 1

SpiderCoin::SpiderCoin(const char* pName)
    : LiveActor(pName), mPointPos(nullptr), mNeutralPos(nullptr), mUp(nullptr), mThreadZ(0.0f), mPad0Velocity(0.0f, 0.0f), mPad1Velocity(0.0f, 0.0f),
      mCocoonModel(nullptr) {
    mBaseMtx.identity();
}

void SpiderCoin::init(const JMapInfoIter& rIter) {
    MR::initDefaultPos(this, rIter);
    mThreadZ = mTranslation.z;
    initModelManagerWithAnm("Coin", nullptr, nullptr, false);
    MR::connectToSceneItemStrongLight(this);

    initHitSensor(1);
    MR::addHitSensorCallbackMapObj(this, "body", 8, 50.0f);

    initEffectKeeper(8, nullptr, false);
    initNerve(&NrvSpiderCoin::SpiderCoinNrvWait::sInstance, 0);
    MR::initStarPointerTarget(this, 150.0f, TVec3f(0.0f, 80.0f, 0.0f));

    MR::declareCoin(this, 1);
    mCocoonModel = new PartsModel(this, "‚Ü‚ä", "SpaceCocoon", mBaseMtx, MR::DrawBufferType_NoSilhouettedMapObj, false);
    mCocoonModel->mScale.set(0.3f);
    mCocoonModel->initWithoutIter();

    MR::registerDemoSimpleCastAll(this);
    MR::registerDemoSimpleCastAll(mCocoonModel);
    makeActorAppeared();
    mCocoonModel->makeActorAppeared();
}

void SpiderCoin::kill() {
    LiveActor::kill();

    TVec3f vel;
    if (mUp != nullptr)
        vel.set(-mUp->x, -mUp->y, 0.0f);
    else
        vel.set(0.0f, 0.0f, 0.0f);
    MR::normalizeOrZero(&vel);
    vel.scale(sCoinSpeed);
    TVec3f pos(mTranslation.x, mTranslation.y, mThreadZ);

    MR::appearCoinToVelocity(this, pos, vel, 1);
    MR::emitEffect(mCocoonModel, "Stick");
}

void SpiderCoin::exeWait() {
    if (MR::isFirstStep(this)) {
        if (mPointPos == nullptr) {
            MR::startActorBindToSpiderThread(this, &mPointPos, &mNeutralPos, &mUp, mTranslation, TVec3f(0.0f, 0.0f, 0.0f));
        }
        mRotation.y = 0.0f;
    }
    if (mPointPos != nullptr)
        mTranslation.set(*mPointPos);

    if (MR::isStarPointerPointing1Por2P(this, "Žã", true, false)) {
        setNerve(&NrvSpiderCoin::SpiderCoinNrvTouch::sInstance);
    }
}

void SpiderCoin::exeTouch() {
    if (MR::isFirstStep(this)) {
        mPad0Velocity.y = 0.0f;
        mPad0Velocity.x = 0.0f;
        mPad1Velocity.y = 0.0f;
        mPad1Velocity.x = 0.0f;
    }

    if (mPointPos != nullptr)
        mTranslation.set(*mPointPos);
    mRotation.y += sRotateSpeedY;
    MR::repeatDegree(&mRotation.y);
    if (MR::testCorePadTriggerA(WPAD_CHAN0) || MR::testCorePadTriggerA(WPAD_CHAN1)) {
        kill();
        return;
    }

    if (!tryRub(WPAD_CHAN0, &mPad0Velocity) && !tryRub(WPAD_CHAN1, &mPad1Velocity) && !MR::isStarPointerPointing(this, WPAD_CHAN0, true, "Žã") &&
        !MR::isStarPointerPointing(this, WPAD_CHAN1, true, "Žã")) {
        setNerve(&NrvSpiderCoin::SpiderCoinNrvTouchAndApart::sInstance);
    }
}

void SpiderCoin::exeTouchAndApart() {
    if (mPointPos != nullptr)
        mTranslation.set(*mPointPos);

    mRotation.y += MR::getEaseOutValue((sStepToucnAndApart - getNerveStep()) / static_cast< f32 >(sStepToucnAndApart), 0.0f, 1.0f, 1.0f) * sRotateSpeedY;
    MR::repeatDegree(&mRotation.y);

    if (MR::isGreaterStep(this, sStepToTouch2nd) && MR::isStarPointerPointing(this, WPAD_CHAN0, true, "Žã")) {
        kill();
        return;
    }

    if (MR::isStep(this, sStepToucnAndApart)) {
        setNerve(&NrvSpiderCoin::SpiderCoinNrvWait::sInstance);
    }
}

void SpiderCoin::calcAndSetBaseMtx() {

    TVec3f up;
    if (mUp != nullptr)
        up.set(*mUp);
    else
        up.set(0.0f, 1.0f, 0.0f);

    up.scale(-1.0f);
    MR::normalize(&up);

    TVec3f front(0.0f, 0.0f, 1.0f);
    TVec3f side;
    PSVECCrossProduct(up.toVecPtr(), front.toVecPtr(), side.toVecPtr());
    MR::normalize(&side);
    PSVECCrossProduct(side.toVecPtr(), up.toVecPtr(), front.toVecPtr());
    MR::normalize(&front);

    TPos3f mtx;
    mtx.identity();
    mtx.setVec(side, up, front);

    TRot3f rotMtx;
    rotMtx.identity();
    MR::makeMtxRotateY(rotMtx.toMtxPtr(), mRotation.y);
    PSMTXConcat(mtx.toMtxPtr(), rotMtx.toMtxPtr(), mtx.toMtxPtr());

    TVec3f pos(up);
    pos.scale(-sModelOffset);
    pos.add(mTranslation);
    mtx.setTrans(pos);
    mBaseMtx.setInline(mtx.toMtxPtr());

    pos.set(up);
    pos.scale(-sCocoonOffsetY);
    pos.add(mTranslation);
    TVec3f forward(front);
    forward.scale(sCocoonOffsetZ);
    pos.add(forward);
    mBaseMtx.setTrans(pos);
    MR::setBaseTRMtx(this, mtx);
}

bool SpiderCoin::tryRub(s32 padChannel, TVec2f* pVel) {
    if (!MR::isStarPointerPointing(this, padChannel, true, "Žã") || MR::getStarPointerScreenSpeed(padChannel) < sPointerSpeedMin) {
        return false;
    }

    TVec2f* vel = MR::getStarPointerScreenVelocity(padChannel);
    if (vel->x * pVel->x + vel->y * pVel->y < sPointerSpeedDotToKill) {
        kill();
        return true;
    }

    TVec2f* vel2 = MR::getStarPointerScreenVelocity(padChannel);
    pVel->x = vel2->x;
    pVel->y = vel2->y;
    return false;
}
