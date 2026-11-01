#include "WaterfallCaveCover.h"

namespace {
static const f32 sSensorRadius = 500.0f;
static const s32 sRiddleSeStep = 90;
}; // namespace

namespace NrvWaterfallCaveCover {
FULL_NERVE(HostTypeWait, WaterfallCaveCover, Wait);
FULL_NERVE(HostTypeBreak, WaterfallCaveCover, Break);
}; // namespace NrvWaterfallCaveCover

void WaterfallCaveCover::init(const JMapInfoIter &rIter) {
  MapObjActor::init(rIter);
  MapObjActorInitInfo rInitInfo;
  rInitInfo.setupHioNode("n形オブジェ");
  rInitInfo.setupDefaultPos();
  rInitInfo.setupConnectToScene();
  rInitInfo.setupSound(4, "WaterfallCaveCover");
  rInitInfo.setupGroupClipping(16);
  rInitInfo.setupNerve(&NrvWaterfallCaveCover::HostTypeWait::sInstance);
  rInitInfo.setupHitSensor();
  rInitInfo.setupHitSensorParam(8, ::sSensorRadius, TVec3f(0.0f, 0.0f, 0.0f));
  rInitInfo.setupEffect("WaterfallCaveCover", 0);
  initialize(rIter, rInitInfo);

  mHitCount = 1;
  MR::getJMapInfoArg0NoInit(rIter, &mHitCount);

  mHitCooldown = 30;
  MR::getJMapInfoArg1NoInit(rIter, &mHitCooldown);
  
  mEndingSound = 0;
  MR::getJMapInfoArg2NoInit(rIter, &mEndingSound);

  mMaxHitCount = mHitCount;

  mHitCooldown = 0;

  mBreakModel = new ModelObj("壊れモデル", "WaterfallCaveCoverBreak", nullptr,
                             MR::CategoryList_Auto, MR::CategoryList_Auto,
                             MR::CategoryList_Auto, false);
  mBreakModel->initWithoutIter();
  mBreakModel->mScale.set(mScale);
  mBreakModel->makeActorDead();
  MR::tryRegisterDemoCast(mBreakModel, rIter);
}

bool WaterfallCaveCover::receiveMsgPlayerAttack(u32 msg, HitSensor *pSender,
                                                HitSensor *pReceiver) {
  if ((MR::isMsgJetTurtleAttack(msg) || MR::isPlayerInRush()) &&
      isNerve(&NrvWaterfallCaveCover::HostTypeWait::sInstance) &&
      mHitCooldown == 0) {
    mHitCooldown = 30;

    mHitCount--;
    MR::shakeCameraNormal();

    if (mHitCount <= 0) {
      setNerve(&NrvWaterfallCaveCover::HostTypeBreak::sInstance);
    } else {
      switch (mMaxHitCount - mHitCount) {
      case 1:
        MR::startSound(this, "SE_SY_CANDLE_COMBO_1");
        break;
      case 2:
        MR::startSound(this, "SE_SY_CANDLE_COMBO_2");
        break;
      case 3:
        MR::startSound(this, "SE_SY_CANDLE_COMBO_3");
        break;
      case 4:
        MR::startSound(this, "SE_SY_CANDLE_COMBO_4");
        break;
      default:
        MR::startSound(this, "SE_SY_CANDLE_COMBO_5");
        break;
      }
    }

    return true;
  }

  return false;
}

void WaterfallCaveCover::exeWait() {
  if (mHitCooldown > 0)
    mHitCooldown--;
  if (MR::isValidSwitchB(this)) {
    if (MR::isOnSwitchB(this)) {
      if (isNerve(&NrvWaterfallCaveCover::HostTypeWait::sInstance)) {
        setNerve(&NrvWaterfallCaveCover::HostTypeBreak::sInstance);
      }
    }
  }
}

void WaterfallCaveCover::exeBreak() {
  if (MR::isFirstStep(this)) {
    MR::invalidateHitSensor(this, "body");
    MR::invalidateCollisionParts(this);
    MR::hideModel(this);

    mBreakModel->appear();
    mBreakModel->mRotation.set(mRotation);
    MR::resetPosition(mBreakModel, mTranslation);
    MR::startAction(mBreakModel, "Break");

    MR::requestMovementOn(mBreakModel);

    MR::shakeCameraStrong();
    MR::tryRumblePadVeryStrong(this, 0);
    MR::startSound(this, "SE_OJ_WATERFALL_COVER_BREAK");

    if (MR::isEffectValid(this, "Break")) {
      MR::emitEffect(this, "Break");
    }
  }

  if (MR::isStep(this, ::sRiddleSeStep)) {
    if (mEndingSound == 0) {
      MR::startSound(this, "SE_SY_READ_RIDDLE_S");
    } else if (mEndingSound == 1) {
      MR::startSound(this, "SE_SY_READ_RIDDLE_SS");
    } else {
    }

    kill();
  }

  if (!MR::isHiddenModel(mBreakModel) && MR::isActionEnd(mBreakModel)) {
    MR::hideModel(mBreakModel);
  }
}