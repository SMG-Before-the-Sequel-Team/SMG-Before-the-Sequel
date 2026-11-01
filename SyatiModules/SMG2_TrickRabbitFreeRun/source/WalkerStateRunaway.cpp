#include "WalkerStateRunaway.h"

namespace G17 {
void turnDirectionFromTargetDegree(const LiveActor *pActor, TVec3f *pA2,
                                   const TVec3f &rA3, f32 a4) {
  TVec3f stack_8 = pActor->mTranslation - rA3;
  MR::turnVecToVecCosOnPlane(pA2, stack_8, pActor->mGravity, MR::cosDegree(a4));
}

void calcWallNormalHorizontal(TVec3f *pVec, const LiveActor *pActor) {
  const TVec3f normal = MR::getWallNormal(pActor);
  const TVec3f grav = pActor->mGravity;
  JMAVECScaleAdd(grav, normal, (Vec *)pVec, -grav.dot(normal));
}

void addVelocityClockwiseToDirection(LiveActor *pActor, const TVec3f &rA2,
                                     f32 a3) {
  TVec3f stack_14;
  if (MR::normalizeOrZero(rA2, &stack_14)) {
    return;
  }

  PSVECCrossProduct(pActor->mGravity, stack_14, stack_14);
  TVec3f stack_8;
  MR::calcVelocityMoveToDirection(&stack_8, pActor, stack_14, a3);
  (pActor->mVelocity).add(stack_8);
}
}; // namespace G17

namespace {
static WalkerStateRunawayParam sDefaultParam;
}; // namespace

namespace NrvWalkerStateRunaway {
FULL_NERVE(WalkerStateRunawayNrvWait, WalkerStateRunaway, Wait);
FULL_NERVE(WalkerStateRunawayNrvRunaway, WalkerStateRunaway, Runaway);
FULL_NERVE(WalkerStateRunawayNrvWallJump, WalkerStateRunaway, WallJump);
}; // namespace NrvWalkerStateRunaway

WalkerStateRunawayParam::WalkerStateRunawayParam() {
  mWaitAction = "FollowMe";
  mRunawayAction = "Run";
  mJumpAction = "Jump";
  mGroundAccel = 0.05f;
  mAirAccel = 1.0f;
  _14 = 0.9f;
  _18 = 0.99f;
  _1C = 5;
  mTurnMaxRateDegree = 3.0f;
  mRunawayDistance = 1100.0f;
  mWaitDistance = 1300.0f;
  mRunawayTurnRateMaxDegree = 10.0f;
  mRunawayTurnRateMinDegree = 3.0f;
  mRunawayTurnTime = 30;
  mPlayerFrontLineLength = 25.0f;
  mRunawayBckRatio = 1.0f;
  mMinRunawayBckRate = 0.9f;
  mMaxRunawayBckRate = 1.4f;
  mWallJumpTime = 6;
  mWallReboundPower = 0.3f;
  mWallJumpPowerH = 8.0f;
  mWallJumpPowerV = 15.0f;
}

WalkerStateRunaway::WalkerStateRunaway(LiveActor *pHost, TVec3f *pDirection,
                                       WalkerStateRunawayParam *pRunawayParam)
    : ActorStateBase<LiveActor>("歩行型アクター逃げ"),
      mRunawayParam(pRunawayParam), mDirection(pDirection), _18(0),
      mRunawaySpeed(1.0f), mHost(pHost) {
  if (mRunawayParam == nullptr) {
    mRunawayParam = &::sDefaultParam;
  }

  initNerve(&NrvWalkerStateRunaway::WalkerStateRunawayNrvWait::sInstance, 0);
}

void WalkerStateRunaway::appear() {
  mIsDead = false;
  setNerve(&NrvWalkerStateRunaway::WalkerStateRunawayNrvWait::sInstance);
  _18 = 0;
}

bool WalkerStateRunaway::tryRunaway() {
  if (MR::isNearPlayer(mHost, mRunawayParam->mRunawayDistance)) {
    setNerve(&NrvWalkerStateRunaway::WalkerStateRunawayNrvRunaway::sInstance);
    return true;
  }

  return false;
}

bool WalkerStateRunaway::tryWait() {
  if (isInWaitRange(mRunawayParam->mWaitDistance)) {
    setNerve(&NrvWalkerStateRunaway::WalkerStateRunawayNrvWait::sInstance);
    return true;
  }

  return false;
}

bool WalkerStateRunaway::isInWaitRange(f32 range) const {
  return !MR::isNearPlayer(mHost, range);
}

bool WalkerStateRunaway::tryWallJump() {
  if (MR::isBindedWall(mHost) && MR::calcHitPowerToWall(mHost) > 0.0f) {
    TVec3f horiz;
    MR::calcVecFromPlayerH(&horiz, mHost);
    TVec3f wallNorml;
    G17::calcWallNormalHorizontal(&wallNorml, mHost);

    if (MR::isOppositeDirection(horiz, wallNorml, 0.01f)) {
      G17::addVelocityClockwiseToDirection(
          mHost, wallNorml,
          MR::isHalfProbability() ? mRunawayParam->mWallJumpPowerH
                                  : -mRunawayParam->mWallJumpPowerH);
    } else {
      TVec3f dir;
      dir.add(wallNorml, horiz);
      MR::addVelocityMoveToDirection(mHost, dir,
                                     mRunawayParam->mWallJumpPowerH);
    }

    MR::addVelocityJump(mHost, mRunawayParam->mWallJumpPowerV);
    setNerve(&NrvWalkerStateRunaway::WalkerStateRunawayNrvWallJump::sInstance);
    return true;
  }

  return false;
}

void WalkerStateRunaway::exeWait() {
  if (MR::isFirstStep(this)) {
    MR::startAction(mHost, mRunawayParam->mWaitAction);
  }

  MR::turnDirectionToPlayerDegree(mHost, mDirection,
                                  mRunawayParam->mTurnMaxRateDegree);
  MR::addVelocityToGravity(mHost, MR::isBindedGround(mHost)
                                      ? mRunawayParam->mGroundAccel
                                      : mRunawayParam->mAirAccel);
  MR::attenuateVelocity(mHost,
                        check18() ? mRunawayParam->_14 : mRunawayParam->_18);

  if (tryRunaway()) {
    return;
  }
}

void WalkerStateRunaway::exeRunaway() {
  if (MR::isFirstStep(this)) {
    MR::startAction(mHost, mRunawayParam->mRunawayAction);
  }

  TVec3f v13;
  v13 = *MR::getPlayerVelocity() * mRunawayParam->mPlayerFrontLineLength +
        *MR::getPlayerPos();

  TVec3f *actorPos = &mHost->mTranslation;
  MR::calcPerpendicFootToLineInside(&v13, *actorPos, *MR::getPlayerPos(), v13);
  G17::turnDirectionFromTargetDegree(
      mHost, mDirection, v13,
      MR::calcNerveValue(mHost, mRunawayParam->mRunawayTurnTime,
                         mRunawayParam->mRunawayTurnRateMaxDegree,
                         mRunawayParam->mRunawayTurnRateMinDegree));

  MR::setBckRate(mHost,
                 MR::clamp(mRunawaySpeed / mRunawayParam->mRunawayBckRatio,
                           mRunawayParam->mMinRunawayBckRate,
                           mRunawayParam->mMaxRunawayBckRate));
  MR::addVelocityMoveToDirection(mHost, *mDirection,
                                 check18() ? mRunawaySpeed : 0.0f);
  MR::addVelocityToGravity(mHost, MR::isBindedGround(mHost)
                                      ? mRunawayParam->mGroundAccel
                                      : mRunawayParam->mAirAccel);
  MR::attenuateVelocity(mHost,
                        check18() ? mRunawayParam->_14 : mRunawayParam->_18);

  if (!tryWait()) {
    if (tryWallJump()) {
      return;
    }
  }
}

void WalkerStateRunaway::exeWallJump() {
  if (MR::isFirstStep(this)) {
    MR::startAction(mHost, mRunawayParam->mJumpAction);
  }

  MR::turnDirectionDegree(mHost, mDirection, mHost->mVelocity, 45.0f);
  MR::addVelocityToGravity(mHost, mRunawayParam->mAirAccel);
  MR::attenuateVelocity(mHost, mRunawayParam->_18);
  MR::reboundVelocityFromEachCollision(
      mHost, 0.0f, mRunawayParam->mWallReboundPower, 0.0f, 0.0f);

  if (MR::isGreaterStep(this, mRunawayParam->mWallJumpTime)) {
    if (MR::isBindedGround(mHost)) {
      setNerve(&NrvWalkerStateRunaway::WalkerStateRunawayNrvRunaway::sInstance);
    }
  }
}

bool WalkerStateRunaway::isRunning() const {
  return isNerve(
      &NrvWalkerStateRunaway::WalkerStateRunawayNrvRunaway::sInstance);
}
