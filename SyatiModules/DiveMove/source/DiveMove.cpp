#include "kamek/hooks.h"
#include "syati.h"

#define DIVE_STATE_DISABLED 0
#define DIVE_STATE_DELAY 1
#define DIVE_STATE_IN_DIVE 2

u32 diveState = DIVE_STATE_DISABLED;

void checkIfDiveAllowed() {
  Mario *mMario = MarioAccess::getPlayerActor()->mMario;

  if (!MR::isOnGroundPlayer() && !mMario->isStatusActive(MARIO_WALL)) {
    if (diveState == DIVE_STATE_IN_DIVE || !MR::testCorePadTriggerA(0))
      return;

    if (MR::isStageFileSelect())
      return;

    if (MR::isCurrentRushSpinDriver())
      return;

    if ((mMario->_C & (1u << 7)) != 0)
      return;

    if (MR::isDemoActive())
      return;

    if (diveState == DIVE_STATE_DELAY) {
      MarioAccess::getPlayerActor()->mMario->trySquatJump();
      diveState = DIVE_STATE_IN_DIVE;
    } else
      diveState = DIVE_STATE_DELAY;
  } else
    diveState = DIVE_STATE_DISABLED;
}

// Change Camera Reset Button to D-Pad Down
kmWrite32(0x8011E058, 0x4BF0C319); // bl MR::testCorePadTriggerDown()
