#pragma once

#include "syati.h"

class WalkerStateBlowDamageParam {
public:
  int _0;
};

class WalkerStateBlowDamage : public ActorStateBase<LiveActor> {
public:
  WalkerStateBlowDamage(LiveActor *pHost, TVec3f *pDirection,
                        WalkerStateBlowDamageParam *pBlowDamageParam);

  virtual void appear();

  void exeBlow();
  void exeBlowLand();

  /* 0x10 */ TVec3f *mDirection;
  /* 0x14 */ WalkerStateBlowDamageParam *mBlowDamageParam;
  LiveActor *mHost;
};
