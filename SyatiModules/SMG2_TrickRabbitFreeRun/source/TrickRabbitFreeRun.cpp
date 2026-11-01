#include "TrickRabbitFreeRun.h"
#include "Game/NPC/NPCActor.h"
#include "Game/Util/ActionSoundUtil.h"
#include "Game/Util/BgmUtil.h"
#include "JSystem/JGeometry/TVec.h"
#include "RabbitStateCaught.h"
#include "RabbitStateWaitStart.h"
#include "WalkerStateBlowDamage.h"
#include "WalkerStateRunaway.h"
#include "revolution/types.h"

namespace {
static MR::ActorMoveParam sStartAwayParam = {1.4f, 1.0f, 0.9f, 4.0f};
static const f32 sBodyRadius = 70.0f;
static const f32 sCatchRadius = 30.0f;
static const f32 sBindRadius = 60.0f;
static const f32 sBindYOffset = 60.0f;
static const f32 sUpVecBlendRate = 0.1f;
static const f32 sFrontVecBlendRate = 0.2f;
// static const f32 sStarPieceHitAccelH = _;
// static const f32 sStarPieceHitAccelV = _;
static const f32 sPressureRange = 600.0f;
static const f32 sNormalAccel = 0.7f;
static const f32 sPressureAccel = 1.5f;
static const f32 sBrakeAccelRate = 0.3f;
}; // namespace

namespace NrvTrickRabbitFreeRun {
FULL_NERVE(TrickRabbitFreeRunNrvWaitStart, TrickRabbitFreeRun, WaitStart);
FULL_NERVE(TrickRabbitFreeRunNrvTryDemo, TrickRabbitFreeRun, TryDemo);
FULL_NERVE(TrickRabbitFreeRunNrvRunawayStart, TrickRabbitFreeRun, RunawayStart);
FULL_NERVE(TrickRabbitFreeRunNrvRunaway, TrickRabbitFreeRun, Runaway);
FULL_NERVE(TrickRabbitFreeRunNrvBlowDamage, TrickRabbitFreeRun, BlowDamage);
FULL_NERVE(TrickRabbitFreeRunNrvCaught, TrickRabbitFreeRun, Caught);
FULL_NERVE(TrickRabbitFreeRunNrvPowerStarDemo, TrickRabbitFreeRun,
           PowerStarDemo);
FULL_NERVE(TrickRabbitFreeRunNrvGiveUp, TrickRabbitFreeRun, GiveUp);

NERVE(TrickRabbitFreeRunNrvTakeOutStar);

void TrickRabbitFreeRunNrvTakeOutStar::execute(Spine *pSpine) const {
  // Deliberately do nothing
}
TrickRabbitFreeRunNrvTakeOutStar(TrickRabbitFreeRunNrvTakeOutStar::sInstance);
}; // namespace NrvTrickRabbitFreeRun

TrickRabbitFreeRun::TrickRabbitFreeRun(const char *pName)
    : NPCActor(pName), mRotateQuat(0.0f, 0.0f, 0.0f, 1.0f),
      mFrontVec(0.0f, 0.0f, 1.0f), mStateWaitStart(), mStateRunaway(),
      mStateBlowDamage(), mStateCaught(), mMsgCtrl(), mSpotMarkLight() {}

void TrickRabbitFreeRun::init(const JMapInfoIter &rIter) {
  MR::initDefaultPos(this, rIter);
  initModelManagerWithAnm("TrickRabbit", nullptr, nullptr, false);
  MR::connectToSceneNpc(this);
  MR::initLightCtrl(this);
  MR::makeQuatAndFrontFromRotate(&mRotateQuat, &mFrontVec, this);

  mSpotMarkLight = new SpotMarkLight(this, 100.0f, 1500.0f, nullptr);
  mSpotMarkLight->initWithoutIter();

  ActorShadow::initShadowFromCSV(this, "Shadow", true);

  initEffectKeeper(0, nullptr, 0);
  MR::onEmitEffectSyncClipping(this, "Light");
  MR::onForceDeleteEffectSyncClipping(this, "Light");
  initSensor();
  initBinder(::sBindRadius, ::sBindYOffset, 0);

  MR::getJMapInfoArg0WithInit(rIter, &mStarId);
  if (mStarId > 0) {
    MR::declarePowerStar(this);
    mTakeOutStar = new TakeOutStar(
        this, "TakeOutStarTrickRabbit", "TakeOutStarTrickRabbit",
        &NrvTrickRabbitFreeRun::TrickRabbitFreeRunNrvTakeOutStar::sInstance, 0);
  }

  mMsgCtrl = MR::createTalkCtrl(this, rIter, "TrickRabbit",
                                TVec3f(0.0f, 120.0f, 0.0f), nullptr);

  initState();
  initSound(4, "TrickRabbitFreeRun", &mTranslation, TVec3f(0, 0, 0));
  MR::onCalcGravity(this);

  initNerve(&NrvTrickRabbitFreeRun::TrickRabbitFreeRunNrvWaitStart::sInstance,
            0);
  makeActorAppeared();
}

void TrickRabbitFreeRun::initAfterPlacement() {
  MR::trySetMoveLimitCollision(this);
}

void TrickRabbitFreeRun::initSensor() {
  initHitSensor(2);
  MR::addHitSensorAtJointEnemy(this, "Body", "Spine", 8, ::sBodyRadius,
                               TVec3f(0.0f, 0.0f, 0.0f));
  MR::addHitSensorAtJointEnemy(this, "Catch", "Spine", 8, ::sCatchRadius,
                               TVec3f(0.0f, 0.0f, 0.0f));
  MR::initStarPointerTargetAtJoint(this, "Spine", ::sBodyRadius, TVec3f(0.0f));
}

void TrickRabbitFreeRun::initState() {
  mStateCaught = new RabbitStateCaught(this, mMsgCtrl);
  mStateCaught->init();
  mStateCaught->unusePowerStarModel();

  mStateWaitStart = new RabbitStateWaitStart(this, &mFrontVec, mMsgCtrl);
  mStateWaitStart->init();

  mStateRunaway = new WalkerStateRunaway(this, &mFrontVec, nullptr);
  mStateRunaway->init();

  mStateBlowDamage = new WalkerStateBlowDamage(this, &mFrontVec, nullptr);
  mStateBlowDamage->init();

}

void TrickRabbitFreeRun::control() {
  MR::blendQuatUpFront(&mRotateQuat, -mGravity, mFrontVec, ::sUpVecBlendRate,
                       ::sFrontVecBlendRate);
}

void TrickRabbitFreeRun::calcAndSetBaseMtx() {
  MR::setBaseTRMtx(this, mRotateQuat);
}

void TrickRabbitFreeRun::attackSensor(HitSensor *pSender,
                                      HitSensor *pReceiver) {
  if (!MR::isSensorPlayer(pReceiver)) {
    return;
  }

  if (pSender == getSensor("Catch") && requestCaught()) {
    return;
  }

  if (isEnableCaught()) {
    return;
  }

  MR::sendMsgPush(pReceiver, pSender);
}

bool TrickRabbitFreeRun::receiveMsgPush(HitSensor *pSender,
                                        HitSensor *pReceiver) {
  if (MR::isSensorPlayer(pSender)) {
    return !isEnableCaught();
  }

  return false;
}

bool TrickRabbitFreeRun::receiveMsgEnemyAttack(u32 msg, HitSensor *pSender,
                                               HitSensor *pReceiver) {
  if (MR::isMsgToEnemyAttackBlow(msg)) {
    return receiveMsgBlowDamage(pSender, pReceiver);
  }

  return false;
}

bool TrickRabbitFreeRun::receiveMsgPlayerAttack(u32 msg, HitSensor *pSender,
                                                HitSensor *pReceiver) {
  if (isNerve(
          &NrvTrickRabbitFreeRun::TrickRabbitFreeRunNrvWaitStart::sInstance) ||
      isNerve(&NrvTrickRabbitFreeRun::TrickRabbitFreeRunNrvGiveUp::sInstance)) {
    return mStateWaitStart->receiveMsgPlayerAttack(msg, pSender, pReceiver);
  }

  if (MR::isMsgStarPieceAttack(msg)) {
    if (receiveMsgBlowDamage(pSender, pReceiver)) {
      return true;
    }
  }

  if (MR::isMsgPlayerSpinAttack(msg)) {
    return requestCaught();
  }

  return false;
}

bool TrickRabbitFreeRun::receiveOtherMsg(u32 msg, HitSensor *pSender,
                                         HitSensor *pReceiver) {
  if (isNerve(
          &NrvTrickRabbitFreeRun::TrickRabbitFreeRunNrvWaitStart::sInstance) ||
      isNerve(&NrvTrickRabbitFreeRun::TrickRabbitFreeRunNrvGiveUp::sInstance)) {
    return mStateWaitStart->receiveOtherMsg(msg, pSender, pReceiver);
  }

  return false;
}

bool TrickRabbitFreeRun::receiveMsgBlowDamage(HitSensor *pSender,
                                              HitSensor *pReceiver) {
  if (isEnableBlowDamage()) {
    MR::setVelocitySeparateHV(this, pSender, pReceiver, 20.0f, 20.0f);
    setNerve(
        &NrvTrickRabbitFreeRun::TrickRabbitFreeRunNrvBlowDamage::sInstance);

    return true;
  }

  return false;
}

bool TrickRabbitFreeRun::requestCaught() {
  if (isEnableCaught()) {
    MR::invalidateClipping(this);
    MR::forwardNode(mMsgCtrl);
    setNerve(&NrvTrickRabbitFreeRun::TrickRabbitFreeRunNrvCaught::sInstance);

    return true;
  }

  return false;
}

void TrickRabbitFreeRun::exeWaitStart() {
  if (MR::isFirstStep(this)) {
    mStateWaitStart->setTalkActionName("StartEvent");
    mStateWaitStart->_1C = true;
  }

  if (MR::updateActorState(this, mStateWaitStart)) {
    MR::requestStartDemoMarioPuppetable(
        this, "逃走",
        &NrvTrickRabbitFreeRun::TrickRabbitFreeRunNrvRunawayStart::sInstance,
        &NrvTrickRabbitFreeRun::TrickRabbitFreeRunNrvTryDemo::sInstance);
  }
}

void TrickRabbitFreeRun::exeTryDemo() {}

void TrickRabbitFreeRun::exeRunawayStart() {
  if (MR::isFirstStep(this)) {
    MR::startBckPlayer("BattleWait", nullptr);
    MR::startAction(this, "Run");
    MR::startMinigameBGM();
  }

  MR::escapeFromPlayer(this, &mFrontVec, ::sStartAwayParam._0,
                       ::sStartAwayParam._4, ::sStartAwayParam._8,
                       ::sStartAwayParam._C);

  if (MR::checkPassBckFrame(this, 3.0f)) {
    startJumpSound();
  }

  if (MR::isGreaterStep(this, 120)) {
    MR::endDemo(this, "逃走");
    setNerve(&NrvTrickRabbitFreeRun::TrickRabbitFreeRunNrvRunaway::sInstance);
  }
}

void TrickRabbitFreeRun::exeRunaway() {
  MR::updateActorState(this, mStateRunaway);

  if (mStateRunaway->isRunning() && MR::checkPassBckFrame(this, 3.0f)) {
    startJumpSound();
  }

  f32 speed = ::sNormalAccel;

  if (MR::isNearPlayerHorizontal(this, ::sPressureRange)) {
    speed = ::sPressureAccel;
  }

  if (MR::isBindedGroundWater(this) || MR::isBindedGroundBrake(this)) {
    speed *= ::sBrakeAccelRate;
  }

  mStateRunaway->mRunawaySpeed = speed;
}

void TrickRabbitFreeRun::exeBlowDamage() {
  MR::updateActorStateAndNextNerve(
      this, mStateBlowDamage,
      &NrvTrickRabbitFreeRun::TrickRabbitFreeRunNrvRunaway::sInstance);
}

void TrickRabbitFreeRun::exeCaught() {
  if (MR::isFirstStep(this)) {
    MR::stopStageBGM(60);
  }

  if (MR::updateActorState(this, mStateCaught)) {
    if (mStarId > 0) {
      setNerve(&NrvTrickRabbitFreeRun::TrickRabbitFreeRunNrvPowerStarDemo::
                   sInstance);
    } else {
      // [TODO] there should be two node forwards in this file
      MR::forwardNode(mMsgCtrl);
      setNerve(&NrvTrickRabbitFreeRun::TrickRabbitFreeRunNrvGiveUp::sInstance);
    }
  }
}

void TrickRabbitFreeRun::exePowerStarDemo() {
  if (MR::isFirstStep(this)) {
    MR::endDemo(this, "捕まり");
    MR::startStarChanceBGM(true);
    if (mStarId > 0)
      MR::requestAppearPowerStar(this, mTranslation - mGravity * 200.0f);
    MR::startAction(this, "Wait");
    mSpotMarkLight->kill();
  }

  MR::zeroVelocity(this);

  if (MR::isEndPowerStarAppearDemo(this)) {
    setNerve(&NrvTrickRabbitFreeRun::TrickRabbitFreeRunNrvGiveUp::sInstance);
  }
}

void TrickRabbitFreeRun::exeGiveUp() {
  if (MR::isFirstStep(this)) {
    mStateWaitStart->setTalkActionName("GiveUpTalk");
    mStateWaitStart->_1C = false;
  }

  MR::updateActorState(this, mStateWaitStart);
}

bool TrickRabbitFreeRun::isEnableCaught() const {
  if (isNerve(
          &NrvTrickRabbitFreeRun::TrickRabbitFreeRunNrvRunaway::sInstance) ||
      isNerve(
          &NrvTrickRabbitFreeRun::TrickRabbitFreeRunNrvBlowDamage::sInstance)) {
    return true;
  }

  return false;
}

bool TrickRabbitFreeRun::isEnableBlowDamage() const {
  return isNerve(
      &NrvTrickRabbitFreeRun::TrickRabbitFreeRunNrvRunaway::sInstance);
}

void TrickRabbitFreeRun::startJumpSound() {
  if (MR::isBindedGroundWater(this)) {
    MR::startActionSound(this, "SmRabbitJumpWater", -1, -1, -1);
  } else if (MR::isBindedGroundBrake(this)) {
    MR::startActionSound(this, "SmRabbitJumpWater", -1, -1, -1);
  } else {
    MR::startActionSound(this, "SmRabbitJump", -1, -1, -1);
  }
}
