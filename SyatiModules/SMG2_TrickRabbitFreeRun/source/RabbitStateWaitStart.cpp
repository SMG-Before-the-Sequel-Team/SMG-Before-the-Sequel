#include "RabbitStateWaitStart.h"

namespace {
static const f32 sGravityAccel = 0.1f;
static const s32 sSmallTurnTime = 30;
static const s32 sSmallTurnStopTime = 30;
static const f32 sSmallTurnDegree = 6.0f;
static const f32 sSmallTurnStartDegree = 45.0f;
static const f32 sSpinDegree = 2.0f;
static const f32 sInStartTalkRange = 300.0f;
}; // namespace

namespace NrvRabbitStateWaitStart {
FULL_NERVE(RabbitStateWaitStartNrvWait, RabbitStateWaitStart, Wait);
FULL_NERVE(RabbitStateWaitStartNrvSmallTurn, RabbitStateWaitStart, SmallTurn);
FULL_NERVE(RabbitStateWaitStartNrvHitReaction, RabbitStateWaitStart,
           HitReaction);
FULL_NERVE(RabbitStateWaitStartNrvTrample, RabbitStateWaitStart, Trample);
FULL_NERVE(RabbitStateWaitStartNrvSpin, RabbitStateWaitStart, Spin);
FULL_NERVE(RabbitStateWaitStartNrvPointing, RabbitStateWaitStart, Pointing);
FULL_NERVE(RabbitStateWaitStartNrvTalk, RabbitStateWaitStart, Talk);
}; // namespace NrvRabbitStateWaitStart

RabbitStateWaitStart::RabbitStateWaitStart(LiveActor *pHost,
                                           TVec3f *mHostFrontVec,
                                           TalkMessageCtrl *pTalkCtrl)
    : ActorStateBase("うさぎ会話待ち状態"), mTalkActionName("Wait"),
      mHostFrontVec(mHostFrontVec), mTalkCtrl(pTalkCtrl), _1C(), _1D(true),
      mHost(pHost) {}

void RabbitStateWaitStart::setTalkActionName(const char *pName) {
  mTalkActionName = pName;
}

void RabbitStateWaitStart::init() {
  initNerve(&NrvRabbitStateWaitStart::RabbitStateWaitStartNrvWait::sInstance,
            0);
}

void RabbitStateWaitStart::appear() {
  mIsDead = false;

  setNerve(&NrvRabbitStateWaitStart::RabbitStateWaitStartNrvWait::sInstance);
}

void RabbitStateWaitStart::control() { tryPointing(); }

bool RabbitStateWaitStart::receiveMsgPlayerAttack(u32 msg, HitSensor *pSender,
                                                  HitSensor *pReceiver) {
  if (MR::isMsgStarPieceAttack(msg) || MR::isMsgPlayerHitAll(msg)) {
    return requestHitReaction();
  }

  if (MR::isMsgPlayerHipDrop(msg)) {
    MR::sendMsgAwayJump(pSender, pReceiver);

    return requestTrample();
  }

  if (MR::isMsgPlayerTrample(msg)) {
    return requestTrample();
  }

  return false;
}

bool RabbitStateWaitStart::receiveOtherMsg(u32 msg, HitSensor *pSender,
                                           HitSensor *pReceiver) {
  if (MR::isMsgSpinStormRange(msg)) {
    return requestSpin();
  }

  return false;
}

bool RabbitStateWaitStart::requestTrample() {
  if (isEnableReaction()) {
    setNerve(
        &NrvRabbitStateWaitStart::RabbitStateWaitStartNrvTrample::sInstance);

    return true;
  }

  return false;
}

bool RabbitStateWaitStart::requestSpin() {
  if (isEnableReaction()) {
    setNerve(&NrvRabbitStateWaitStart::RabbitStateWaitStartNrvSpin::sInstance);

    return true;
  }

  return false;
}

bool RabbitStateWaitStart::requestHitReaction() {
  if (isEnableReaction()) {
    setNerve(&NrvRabbitStateWaitStart::RabbitStateWaitStartNrvHitReaction::
                 sInstance);

    return true;
  }

  return false;
}

bool RabbitStateWaitStart::isEnableReaction() const {
  if (isNerve(
          &NrvRabbitStateWaitStart::RabbitStateWaitStartNrvWait::sInstance) ||
      isNerve(&NrvRabbitStateWaitStart::RabbitStateWaitStartNrvSmallTurn::
                  sInstance) ||
      isNerve(&NrvRabbitStateWaitStart::RabbitStateWaitStartNrvTrample::
                  sInstance) ||
      isNerve(
          &NrvRabbitStateWaitStart::RabbitStateWaitStartNrvTalk::sInstance) &&
          MR::isShortTalk(mTalkCtrl)) {
    return true;
  }

  return false;
}

bool RabbitStateWaitStart::trySmallTurn() {
  if (MR::isGreaterStep(this, ::sSmallTurnTime) &&
      !MR::isFaceToPlayerHorizontalDegree(mHost, *mHostFrontVec,
                                          ::sSmallTurnStartDegree)) {
    setNerve(
        &NrvRabbitStateWaitStart::RabbitStateWaitStartNrvSmallTurn::sInstance);

    return true;
  }

  return false;
}

bool RabbitStateWaitStart::tryTalk() {
  if (MR::isNearPlayer(mHost, ::sInStartTalkRange) &&
      MR::tryTalkNearPlayer(mTalkCtrl)) {
    setNerve(&NrvRabbitStateWaitStart::RabbitStateWaitStartNrvTalk::sInstance);

    return true;
  }

  return false;
}

bool RabbitStateWaitStart::tryPointing() {
  if (!MR::isExistStarPointerTarget(mHost)) {
    return false;
  }

  if (!isEnableReaction()) {
    return false;
  }

  if (MR::isStarPointerPointing2POnPressButton(mHost, "弱", true, false)) {
    setNerve(
        &NrvRabbitStateWaitStart::RabbitStateWaitStartNrvPointing::sInstance);

    return true;
  }

  return false;
}

void RabbitStateWaitStart::exeWait() {
  if (MR::isFirstStep(this)) {
    MR::startAction(mHost, "Wait");
    MR::validateClipping(mHost);
    MR::zeroVelocity(mHost);
  }

  if (_1D) {
    if (MR::isExistBinder(mHost) && !MR::isBindedGround(mHost)) {
      MR::addVelocityToGravity(mHost, ::sGravityAccel);
    } else {
      MR::zeroVelocity(mHost);
    }
  }

  if (tryTalk()) {
    return;
  }

  if (trySmallTurn()) {
    return;
  }
}

void RabbitStateWaitStart::exeSmallTurn() {
  if (MR::isFirstStep(this)) {
    MR::startAction(mHost, "TurnSmall");
  }

  MR::turnDirectionToPlayerDegree(mHost, mHostFrontVec, ::sSmallTurnDegree);
  MR::zeroVelocity(mHost);

  if (!tryTalk() && MR::isGreaterStep(this, ::sSmallTurnStopTime)) {
    setNerve(&NrvRabbitStateWaitStart::RabbitStateWaitStartNrvWait::sInstance);
  }
}

void RabbitStateWaitStart::exeHitReaction() {
  if (MR::isFirstStep(this)) {
    MR::startAction(mHost, "Reaction");
    MR::limitedStarPieceHitSound();
    MR::startActionSound(mHost, "SmRabbitStarPieceHit", -1, -1, -1);
    MR::startActionSound(mHost, "SvRabbitStarPieceHit", -1, -1, -1);
  }

  MR::zeroVelocity(mHost);

  if (MR::isActionEnd(mHost)) {
    setNerve(&NrvRabbitStateWaitStart::RabbitStateWaitStartNrvWait::sInstance);
  }
}

void RabbitStateWaitStart::exeTrample() {
  if (MR::isFirstStep(this)) {
    MR::startAction(mHost, "Press");
    MR::startActionSound(mHost, "SmNpcTrampled", -1, -1, -1);
    MR::startActionSound(mHost, "SvRabbitTrampled", -1, -1, -1);
  }

  MR::zeroVelocity(mHost);

  if (MR::isActionEnd(mHost)) {
    setNerve(&NrvRabbitStateWaitStart::RabbitStateWaitStartNrvWait::sInstance);
  }
}

void RabbitStateWaitStart::exeSpin() {
  if (MR::isFirstStep(this)) {
    MR::startAction(mHost, "Spin");
    MR::startActionSound(mHost, "SmRabbitSpin", -1, -1, -1);
    MR::startActionSound(mHost, "SvRabbitSpin", -1, -1, -1);
  }

  MR::turnDirectionToPlayerDegree(mHost, mHostFrontVec, ::sSpinDegree);
  MR::zeroVelocity(mHost);

  if (MR::isActionEnd(mHost)) {
    setNerve(&NrvRabbitStateWaitStart::RabbitStateWaitStartNrvWait::sInstance);
  }
}

void RabbitStateWaitStart::exePointing() {
  if (MR::isFirstStep(this)) {
    MR::startAction(mHost, "Pointing");
    MR::startDPDHitSound();
  }

  MR::startActionSound(mHost, "SmLvRabbitPoint", -1, -1, -1);
  MR::zeroVelocity(mHost);

  if ((MR::isShortTalk(mTalkCtrl) || !tryTalk()) && MR::isActionEnd(mHost)) {
    setNerve(&NrvRabbitStateWaitStart::RabbitStateWaitStartNrvWait::sInstance);
  }
}

void RabbitStateWaitStart::exeTalk() {
  if (MR::isFirstStep(this)) {
    MR::invalidateClipping(mHost);
    MR::startAction(mHost, mTalkActionName);
    MR::zeroVelocity(mHost);
  }

  MR::turnDirectionToPlayerDegreeHorizon(mHost, mHostFrontVec, 30.0f);

  if (MR::tryTalkNearPlayerAtEnd(mTalkCtrl)) {
    if (_1C) {
      kill();
    } else {
      setNerve(
          &NrvRabbitStateWaitStart::RabbitStateWaitStartNrvWait::sInstance);
    }
  }
}
