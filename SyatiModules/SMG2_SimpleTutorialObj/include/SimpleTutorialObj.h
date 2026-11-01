#pragma once

#include "Game/LiveActor/LiveActor.h"
#include "Game/Screen/SimpleLayout.h"
#include "Game/Map/StageSwitch.h"
#include "Game/System/NerveExecutor.h"

class SimpleTutorialObj : public NameObj, public NerveExecutor {
public:
	SimpleTutorialObj(const char*);

	virtual void init(const JMapInfoIter& rIter);
	virtual void movement();

	SimpleLayout* mLayout;
	s32 mObjArg0;
	s32 mDelay;
	StageSwitchCtrl* mSwitchCtrl;

	inline bool isOnSwitchA()
	{
		return mSwitchCtrl->isValidSwitchA() && mSwitchCtrl->isOnSwitchA();
	}
};

namespace NrvSimpleTutorialObj {
	NERVE(NrvHide);		 // Layout not visible
	NERVE(NrvAppear);	 // Layout is appearing
	NERVE(NrvWait);		 // Layout is appeared and not complete
	NERVE(NrvComplete);  // Layout is appeared and complete
	NERVE(NrvDisappear); // Layout is disappearing
}