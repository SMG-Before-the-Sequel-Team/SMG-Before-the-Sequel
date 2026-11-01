#include "SimpleTutorialObj.h"

#include "Game/Util/ActorInitUtil.h"
#include "Game/Util/JMapUtil.h"
#include "Game/Util/LayoutUtil.h"
#include "Game/Util/NerveUtil.h"
#include "Game/Util/ActorSwitchUtil.h"
#include "Game/Util/SoundUtil.h"
#include "Game/Util/CSSoundUtil.h"
#include "Game/Util/ObjUtil.h"
#include "Game/Util/DemoUtil.h"
#include "Game/Screen/GameSceneLayoutHolder.h"



SimpleTutorialObj::SimpleTutorialObj(const char* pName) : NameObj(pName), NerveExecutor(pName),
	mLayout(NULL),
	mObjArg0(-1),
	mDelay(-1),
	mSwitchCtrl(NULL)
{
}

void SimpleTutorialObj::init(const JMapInfoIter& rIter)
{
	const char* objName;
	MR::getObjectName(&objName, rIter);
	mLayout = MR::createSimpleLayout("SimpleTutorial", objName, 1);
	MR::startAnimAndSetFrameAndStop(mLayout, "Appear", 0, 0);
	MR::hideLayout(mLayout);

	MR::connectToSceneMapObjMovement(this);
	MR::registerDemoSimpleCastAll(this);
	MR::getJMapInfoArg0NoInit(rIter, &mObjArg0);

	mSwitchCtrl = MR::createStageSwitchCtrl(this, rIter);
	mSpine = new Spine(this, &NrvSimpleTutorialObj::NrvHide::sInstance, 0); // DO NOT USE NerveExecutor::initNerve!!!! If you do, it will put a local pointer to the NerveExecutor part of this object instead of the full object pointer!
}

void SimpleTutorialObj::movement()
{
	mSpine->update(); // Quirky
}


namespace NrvSimpleTutorialObj {
	void NrvHide::execute(Spine* pSpine) const {
		SimpleTutorialObj* pObj = (SimpleTutorialObj*)pSpine->mExecutor;
		
		if (pObj->mSwitchCtrl->isOnSwitchAppear())
			pObj->mSpine->setNerve(&NrvSimpleTutorialObj::NrvAppear::sInstance);
	}
	NrvHide(NrvHide::sInstance);

	void NrvAppear::execute(Spine* pSpine) const {
		SimpleTutorialObj* pObj = (SimpleTutorialObj*)pSpine->mExecutor;

		if (MR::isFirstStep(pObj))
		{
			if (!pObj->isOnSwitchA() && pObj->mSwitchCtrl->isValidSwitchDead())
				pObj->mSwitchCtrl->offSwitchDead();

			pObj->mLayout->appear();
			MR::showLayout(pObj->mLayout);
			MR::startAnim(pObj->mLayout, "Appear", 0);
			MR::startCSSound("CS_NOTICE_USE_DPD", "SE_SY_CS_NOTICE_USE_DPD", 0, 0, 1.0f);
		}

		if (MR::isAnimStopped(pObj->mLayout, 0))
			pObj->setNerve(&NrvSimpleTutorialObj::NrvWait::sInstance);
	}
	NrvAppear(NrvAppear::sInstance);

	void NrvWait::execute(Spine* pSpine) const {
		SimpleTutorialObj* pObj = (SimpleTutorialObj*)pSpine->mExecutor;

		if (MR::isFirstStep(pObj))
		{
			MR::startAnim(pObj->mLayout, "Wait", 0);
			//MR::startActionSound(pObj, "Wait", -1, -1, -1);
		}
		if (!pObj->mSwitchCtrl->isOnSwitchAppear())
			pObj->setNerve(&NrvSimpleTutorialObj::NrvDisappear::sInstance);

		if (pObj->isOnSwitchA())
			pObj->setNerve(&NrvSimpleTutorialObj::NrvComplete::sInstance);
	}
	NrvWait(NrvWait::sInstance);

	void NrvComplete::execute(Spine* pSpine) const {
		SimpleTutorialObj* pObj = (SimpleTutorialObj*)pSpine->mExecutor;

		if (MR::isFirstStep(pObj))
		{
			MR::startAnim(pObj->mLayout, "Complete", 0);
			MR::startSystemSE("SE_SY_SURF_TUTORIAL_GONEXT", -1, -1);

			pObj->mDelay = pObj->mObjArg0;
		}

		if (pObj->mSwitchCtrl->isValidSwitchB() && !pObj->mSwitchCtrl->isOnSwitchB())
			return; // if SW_B is used, we need to wait for that before ObjArg0 can tick

		if (pObj->mDelay-- <= 0)
			pObj->setNerve(&NrvSimpleTutorialObj::NrvDisappear::sInstance);
	}
	NrvComplete(NrvComplete::sInstance);

	void NrvDisappear::execute(Spine* pSpine) const {
		SimpleTutorialObj* pObj = (SimpleTutorialObj*)pSpine->mExecutor;

		if (MR::isFirstStep(pObj))
		{
			MR::startAnim(pObj->mLayout, "End", 0);
			//MR::startActionSound(pObj, "Appear", -1, -1, -1);
		}

		if (!MR::isAnimStopped(pObj->mLayout, 0))
			return;

		MR::hideLayout(pObj->mLayout);
		pObj->mLayout->kill();

		if (pObj->isOnSwitchA() && pObj->mSwitchCtrl->isValidSwitchDead())
			pObj->mSwitchCtrl->onSwitchDead();

		// Block resetting until SW_APPEAR is turned off
		if (pObj->isOnSwitchA() && pObj->mSwitchCtrl->isOnSwitchAppear())
			return;

		pObj->setNerve(&NrvSimpleTutorialObj::NrvHide::sInstance);
	}
	NrvDisappear(NrvDisappear::sInstance);
}