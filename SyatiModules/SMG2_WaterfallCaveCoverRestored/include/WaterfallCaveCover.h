#pragma once

#include "syati.h"

class WaterfallCaveCover : public MapObjActor {
public:
  WaterfallCaveCover(const char *pName) : MapObjActor(pName) {}

  virtual void init(const JMapInfoIter &rIter);
  virtual bool receiveMsgPlayerAttack(u32 msg, HitSensor *pSender,
                                      HitSensor *pReceiver);

  void exeWait();
  void exeBreak();

  LiveActor *mBreakModel;

  s32 mHitCount;
  s32 mMaxHitCount;
  s32 mHitCooldown;
  s32 mEndingSound;
};
