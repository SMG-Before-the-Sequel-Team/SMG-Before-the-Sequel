#include "WalkerStateBlowDamage.h"

namespace {
static const f32 sAirFric = 0.99f;
static const f32 sAirGravityAccel = 1.0f;
static const f32 sDamageTurnLimit = 15.0f;
static const s32 sDamageLandTime = 30;
}; // namespace

namespace NrvWalkerStateBlowDamage {
FULL_NERVE(WalkerStateBlowDamageNrvBlow, WalkerStateBlowDamage, Blow);
FULL_NERVE(WalkerStateBlowDamageNrvBlowLand, WalkerStateBlowDamage, BlowLand);
}; // namespace NrvWalkerStateBlowDamage

WalkerStateBlowDamage::WalkerStateBlowDamage(
    LiveActor *pHost, TVec3f *pDirection,
    WalkerStateBlowDamageParam *pBlowDamageParam)
    : ActorStateBase<LiveActor>("吹き飛びダメージ状態"), mDirection(pDirection),
      mBlowDamageParam(pBlowDamageParam), mHost(pHost) {
  initNerve(&NrvWalkerStateBlowDamage::WalkerStateBlowDamageNrvBlow::sInstance,
            0);
}

void WalkerStateBlowDamage::appear() {
  mIsDead = false;
  setNerve(&NrvWalkerStateBlowDamage::WalkerStateBlowDamageNrvBlow::sInstance);
}

void WalkerStateBlowDamage::exeBlow() {
  if (MR::isFirstStep(this)) {
    MR::startAction(mHost, "Damage");
  }
  MR::attenuateVelocity(mHost, ::sAirFric);
  MR::addVelocityToGravity(mHost, ::sAirGravityAccel);
  MR::turnDirectionDegree(mHost, mDirection, -(mHost->mVelocity),
                          ::sDamageTurnLimit);

  if (MR::isGreaterStep(this, 5)) {
    if (MR::isBindedGround(mHost)) {
      MR::startAction(mHost, "DamageLand");
      MR::zeroVelocity(mHost);
      setNerve(&NrvWalkerStateBlowDamage::WalkerStateBlowDamageNrvBlowLand::
                   sInstance);
    }
  }
}

void WalkerStateBlowDamage::exeBlowLand() {
  if (MR::isGreaterStep(this, ::sDamageLandTime)) {
    kill();
  }
}
