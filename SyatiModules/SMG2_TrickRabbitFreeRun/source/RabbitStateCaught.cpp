#include "RabbitStateCaught.h"
#include "Game/Util/ActionSoundUtil.h"

namespace {
// static const f32 sGravityAccel = _;
// static const f32 sBodyRadius = _;
static const s32 sMarioPoseBlendTime = 5;
static const s32 sCaughtLandStartTime = 20;
// static const s32 sCaughtEventTime = _;
static const f32 sCaughtJumpPower = 20.0f;
static const f32 sCaughtGravityAccel = 1.0f;
static const f32 sCaughtAirFreq = 0.99f;
}; // namespace

namespace NrvRabbitStateCaught {
FULL_NERVE(RabbitStateCaughtNrvTryStartDemo, RabbitStateCaught, TryStartDemo);
FULL_NERVE(RabbitStateCaughtNrvCaught, RabbitStateCaught, Caught);
FULL_NERVE(RabbitStateCaughtNrvCaughtLand, RabbitStateCaught, CaughtLand);
FULL_NERVE(RabbitStateCaughtNrvCaughtEvent, RabbitStateCaught, CaughtEvent);
}; // namespace NrvRabbitStateCaught

RabbitStateCaught::RabbitStateCaught(LiveActor *pHost,
                                     TalkMessageCtrl *pTalkCtrl)
    : ActorStateBase("うさぎ捕まり状態"),
      mCaughtStartMarioQuat(0.0f, 0.0f, 0.0f, 1.0f),
      mCaughtStartMarioTrans(0.0f, 0.0f, 0.0f), mTalkCtrl(pTalkCtrl),
      mPowerStarModel(), mUsePowerStarModel(true), mHost(pHost) {
  mCaughtLandMarioBaseMtx.identity();
}

void RabbitStateCaught::unusePowerStarModel() { mUsePowerStarModel = false; }

void RabbitStateCaught::init() {
  initNerve(&NrvRabbitStateCaught::RabbitStateCaughtNrvCaught::sInstance, 0);

  if (mUsePowerStarModel) {
    mPowerStarModel = MR::createPowerStarDemoModel(
        mHost, "パワースターデモモデル", mCaughtLandMarioBaseMtx);
    mPowerStarModel->initWithoutIter();
    MR::invalidateClipping(mPowerStarModel);
    mPowerStarModel->makeActorDead();
  }
}

void RabbitStateCaught::appear() {
  mIsDead = false;

  MR::requestStartDemoMarioPuppetable(
      this, mHost, "捕まり",
      &NrvRabbitStateCaught::RabbitStateCaughtNrvCaught::sInstance,
      &NrvRabbitStateCaught::RabbitStateCaughtNrvTryStartDemo::sInstance);
}

void RabbitStateCaught::control() {}

void RabbitStateCaught::exeTryStartDemo() {}

void RabbitStateCaught::exeCaught() {
  if (MR::isFirstStep(this)) {
    MR::startAction(mHost, "Caught");
    MR::startBckPlayer("Catch", nullptr);
    MR::zeroVelocity(mHost);
    MR::startActionSound(mHost, "SmRabbitCaught", -1, -1, -1);
    setCaughtStartMarioPose();
  }

  if (!MR::isBindedGround(mHost)) {
    MR::addVelocityToGravity(mHost, ::sCaughtGravityAccel);
  }

  MR::attenuateVelocity(mHost, ::sCaughtAirFreq);
  blendBaseMatrixToMario(MR::calcNerveRate(this, ::sMarioPoseBlendTime));

  if (MR::isStep(this, ::sMarioPoseBlendTime)) {
    MR::setVelocityJump(mHost, ::sCaughtJumpPower);
  }

  if (MR::isGreaterEqualStep(this, ::sCaughtLandStartTime) &&
      MR::isBindedGround(mHost)) {
    MR::zeroVelocity(mHost);
    setNerve(&NrvRabbitStateCaught::RabbitStateCaughtNrvCaughtLand::sInstance);
  }
}

void RabbitStateCaught::exeCaughtLand() {
  TMtx34f baseMtx;

  if (MR::isFirstStep(this)) {
    baseMtx.set(mHost->getBaseMtx());
    MR::setPlayerBaseMtx(baseMtx);
    mCaughtLandMarioBaseMtx.set(baseMtx);

    if (mPowerStarModel != nullptr) {
      mPowerStarModel->appear();
      MR::requestMovementOn(mPowerStarModel);
      MR::startAction(mPowerStarModel, "TrickRabbitLand");
    }

    MR::startAction(mHost, "CaughtLand");
    MR::startBckPlayer("CatchLand", nullptr);
  }

  MR::startActionSound(mHost, "SmLvRabbitStruggle", -1, -1, -1);

  if (MR::isBckStopped(mHost)) {
    if (mTalkCtrl == nullptr) {
      kill();
    } else {
      setNerve(
          &NrvRabbitStateCaught::RabbitStateCaughtNrvCaughtEvent::sInstance);
    }
  }
}

void RabbitStateCaught::exeCaughtEvent() {
  if (MR::isFirstStep(this)) {
    MR::startAction(mHost, "CaughtWait");
    MR::startBckPlayer("CatchWait", nullptr);

    if (mPowerStarModel != nullptr) {
      MR::requestMovementOn(mPowerStarModel);
      MR::startAction(mPowerStarModel, "TrickRabbitWait");
    }
  }

  MR::startActionSound(mHost, "SmLvRabbitStruggle", -1, -1, -1);

  if (MR::tryTalkForceWithoutDemoMarioPuppetableAtEnd(mTalkCtrl)) {
    if (mPowerStarModel != nullptr) {
      mPowerStarModel->kill();
    }

    kill();
  }
}

void RabbitStateCaught::setCaughtStartMarioPose() {
  TPos3f baseMtx(MR::getPlayerDemoActor()->getBaseMtx());
  baseMtx.getQuat(mCaughtStartMarioQuat);
  baseMtx.getTrans(mCaughtStartMarioTrans);
}

void RabbitStateCaught::blendBaseMatrixToMario(f32 blendRate) const {
  TMtx34f baseMtx;
  TMtx34f finalBaseMtx;

  if (blendRate >= 1.0f) {
    finalBaseMtx.set(mHost->getBaseMtx());
    MR::setPlayerBaseMtx(finalBaseMtx);
  } else {
    baseMtx.identity();
    MR::blendMtx(MR::getPlayerBaseMtx(), mHost->getBaseMtx(), blendRate,
                 baseMtx);
    MR::setPlayerBaseMtx(baseMtx);
  }
}
