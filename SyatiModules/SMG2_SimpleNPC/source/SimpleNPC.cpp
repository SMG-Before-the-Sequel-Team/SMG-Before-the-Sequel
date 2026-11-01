#include "SimpleNPC.h"

#ifdef GALAXY_LEVEL_ENGINE
#include "GalaxyLevelEngine.h"
#endif

SimpleNPC::SimpleNPC(const char* pName) : NPCActor(pName)
{
	mTakeOutStar = nullptr;
	mJointCtrl = nullptr;
	mBehaviourData = nullptr;
	mRailMoveDelayTimer = 0;
	mIsDynamicJointActive = false;
	mIsFaceJointActive = false;
	mIsPreventReaction = false;
}

void SimpleNPC::init(const JMapInfoIter& rIter)
{
	initNPCData(rIter);
	initBehaviourData(rIter);

	if (MR::tryRegisterDemoCast(this, rIter))
	{
		s32 messageid;
		MR::getJMapInfoMessageID(rIter, &messageid);
		if (messageid != -2)
			DemoFunction::registerDemoTalkMessageCtrl(this, mTalkCtrl);
	}

#ifdef GALAXY_LEVEL_ENGINE
	GLE::registerAllGlobalFuncs(this);
#endif
}

void trySetStrFromInitFunctionData(const char** pDest, const JMapInfo* pCsvData, const char* pDataName)
{
	if (MR::hasCsvDataItem(pCsvData, "InitFunction", pDataName))
		MR::getCsvDataStrByElement(pDest, pCsvData, "InitFunction", pDataName, "Data");
}
void trySetBoolFromInitFunctionData(bool* pDest, const JMapInfo* pCsvData, const char* pDataName)
{
	if (MR::hasCsvDataItem(pCsvData, "InitFunction", pDataName))
		*pDest = MR::isInitFunctionBoolSet(pCsvData, pDataName);
}
void trySetF32FromInitFunctionData(f32* pDest, const JMapInfo* pCsvData, const char* pDataName)
{
	if (!MR::hasCsvDataItem(pCsvData, "InitFunction", pDataName))
		return;

	const char* pTemp;
	JMapInfoIter tempIter(MR::getCsvDataStrByElement(&pTemp, pCsvData, "InitFunction", pDataName, "Data"));
	if (tempIter.isValid())
		MR::getCsvDataF32(pDest, pCsvData, "Param00F32", tempIter.mIndex);
}

void SimpleNPC::initNPCData(const JMapInfoIter& rIter)
{
	const char* mObjectName;
	MR::getObjectName(&mObjectName, rIter);

	NPCActorCaps caps = NPCActorCaps(mObjectName);
	caps.mInitAppear = true;
	caps._D = true;
	caps._28 = true;
	caps.mInitScene = true;
	caps.mInitSearchTurtle = true;
	caps.mInitLodCtrlNPC = true;
	caps.mInitSpine = true;
	caps.mInitHitSensor = true;
	caps.mInitBcsvShadow = true;
	caps.mInitRailRider = true;
	caps.mUseSwitchWriteDead = true;
	caps.mUseStageSwitchSyncAppear = true;
	caps.mInitStarPointerTarget = true;
	caps.mInitTalkCtrl = true;
	caps.mInitYoshiLockOnTarget = false;
	caps.mWaitNerve = &NrvSimpleNPC::NrvWait::sInstance;

	JMapInfo* NpcInitData = MR::createInitActorCsvParser(mObjectName, NULL);
	if (NpcInitData != NULL)
	{
		// Animations
		trySetStrFromInitFunctionData(&mTalkParam.mActionWait, NpcInitData, "NpcActionWait");
		trySetStrFromInitFunctionData(&mTalkParam.mActionWaitTurn, NpcInitData, "NpcActionWaitTurn");
		trySetStrFromInitFunctionData(&mTalkParam.mActionTalk, NpcInitData, "NpcActionTalk");
		trySetStrFromInitFunctionData(&mTalkParam.mActionTalkTurn, NpcInitData, "NpcActionTalkTurn");
		trySetStrFromInitFunctionData(&this->mActionSpin, NpcInitData, "NpcActionSpin");
		trySetStrFromInitFunctionData(&this->mActionTrampled, NpcInitData, "NpcActionTrample");
		trySetStrFromInitFunctionData(&this->mActionPointing, NpcInitData, "NpcActionPointing");
		trySetStrFromInitFunctionData(&this->mActionReaction, NpcInitData, "NpcActionReaction");
		trySetStrFromInitFunctionData(&this->mActionWalk, NpcInitData, "NpcActionWalk");
		trySetStrFromInitFunctionData(&this->mActionWalkTalk, NpcInitData, "NpcActionWalkTalk");

		// Toggles
		trySetBoolFromInitFunctionData(&mTalkParam.mEnableTurn, NpcInitData, "NpcRotate");
		trySetBoolFromInitFunctionData(&mTalkParam.mEnableTalkTurn, NpcInitData, "NpcRotateTalk");
		trySetBoolFromInitFunctionData(&caps.mInitYoshiLockOnTarget, NpcInitData, "NpcYoshiTarget");
		trySetBoolFromInitFunctionData(&caps.mInitSearchTurtle, NpcInitData, "NpcSearchTurtle");
		trySetBoolFromInitFunctionData(&caps.mReactSupportTicoSpin, NpcInitData, "NpcSupportTicoSpin");
	}

	NPCActor::initialize(rIter, caps, mObjectName, NULL, false);

	if (NpcInitData != NULL)
	{
		trySetF32FromInitFunctionData(&this->mSpinDist, NpcInitData, "NpcSpinDist");
		trySetF32FromInitFunctionData(&mTalkParam.mTurnDist, NpcInitData, "NpcRotateDist");
		trySetF32FromInitFunctionData(&mTalkParam.mTurnSpeed, NpcInitData, "NpcRotateSpeed");
		trySetF32FromInitFunctionData(&this->_110, NpcInitData, "NpcWalkSpeed"); // Default walk speed of the NPC
		trySetF32FromInitFunctionData(&this->_114, NpcInitData, "NpcWalkAccel"); // Default walk acceleration of the NPC
		trySetF32FromInitFunctionData(&this->mTalkCtrl->_40, NpcInitData, "NpcTalkDist"); // This doubles as a call to MR::setDistanceToTalk() !

		if (MR::hasCsvDataItem(NpcInitData, "InitFunction", "NpcGoods"))
		{
			s32 arg = -1;
			MR::getJMapInfoArg3NoInit(rIter, &arg);
			NPCActorItem item(mObjectName, "", "", "", "");
			MR::getNPCItemData(&item, arg);
			equipment(item, false);
		}
		if (MR::hasCsvDataItem(NpcInitData, "InitFunction", "NpcAnimScale"))
		{
			const char* pJointName;
			MR::getCsvDataStrByElement(&pJointName, NpcInitData, "InitFunction", "NpcAnimScale", "Data");

			AnimScaleParam* newParam = new AnimScaleParam();

			const JMapInfo* AnimScaleData = MR::tryCreateCsvParser(mObjectName, "AnimScaleParam.bcsv");
			if (AnimScaleData != NULL)
			{
				s32 i = 0;
				for (f32* ptr = &newParam->_0; ptr <= &newParam->_30; ptr++, i++)
					MR::getParamDataF32(ptr, AnimScaleData, i, 0); // So EVIL
				//MR::getParamDataF32(&newParam->_0, AnimScaleData, 0, 0);
				//MR::getParamDataF32(&newParam->_4, AnimScaleData, 1, 0);
				//MR::getParamDataF32(&newParam->_8, AnimScaleData, 2, 0);
				//MR::getParamDataF32(&newParam->_C, AnimScaleData, 3, 0);
				//MR::getParamDataF32(&newParam->_10, AnimScaleData, 4, 0);
				//MR::getParamDataF32(&newParam->_14, AnimScaleData, 5, 0);
				//MR::getParamDataF32(&newParam->_18, AnimScaleData, 6, 0);
				//MR::getParamDataF32(&newParam->_1C, AnimScaleData, 7, 0);
				//MR::getParamDataF32(&newParam->_20, AnimScaleData, 8, 0);
				//MR::getParamDataF32(&newParam->_24, AnimScaleData, 9, 0);
				//MR::getParamDataF32(&newParam->_28, AnimScaleData, 10, 0);
				//MR::getParamDataS32(&newParam->_2C, AnimScaleData, 11, 0);
				//MR::getParamDataF32(&newParam->_30, AnimScaleData, 12, 0);
			}
			mAnimScaleCtrl = new AnimScaleController(newParam);

			mJointCtrlParam = MR::createJointDelegatorWithNullChildFunc<SimpleNPC>(this, &NPCActor::calcJointScale, pJointName); // The addition of <SimpleNPC> is required.
		}
	}

	if (MR::isExistBrk(this, "ColorChange"))
	{
		f32 arg = 0.0f;
		MR::getJMapInfoArg0NoInit(rIter, &arg);
		MR::startBrkAndSetFrameAndStop(this, "ColorChange", arg);
	}
	if (MR::isExistBtp(this, "TexChange"))
	{
		f32 arg = 0.0f;
		MR::getJMapInfoArg1NoInit(rIter, &arg);
		MR::startBtpAndSetFrameAndStop(this, "TexChange", arg);
	}

	s32 InstanceParameters = 0;
	MR::getJMapInfoArg4NoInit(rIter, &InstanceParameters);
	if (InstanceParameters & 0b00000000000000000000000000000001)
	{
		MR::declarePowerStar(this);
		mTakeOutStar = new TakeOutStar(this, "TakeOutStar", "TakeOutStar", &NrvSimpleNPC::NrvNull::sInstance, 0);
	}

	mJointCtrl = new ActorJointCtrl(this);

	if (MR::isConnectedWithRail(rIter))
	{
		mRailSnapToGround = !(InstanceParameters & 0b00000000000000000000000000000010); // If this bit is set, do not snap to the ground

		s32 startpoint = (InstanceParameters & 0b00000000000000000000000000000100) ? MR::getRandom(0, MR::getRailPointNum(this)) : 0;
		MR::moveCoordAndTransToRailPoint(this, startpoint);
		MR::onCalcShadow(this, nullptr);
	}
}

void SimpleNPC::initBehaviourData(const JMapInfoIter& rIter) {
	const char* mObjectName;
	MR::getObjectName(&mObjectName, rIter);

	mBehaviourData = MR::tryCreateCsvParser(mObjectName, "NpcBehavior.bcsv");
	if (mBehaviourData == nullptr)
		return;

	s32 arg = -1;
	MR::getJMapInfoArg2NoInit(rIter, &arg);
	setBehaviour(arg);
}

void SimpleNPC::setBehaviour(s32 idx) {
	if (mBehaviourData == nullptr)
		return;

	if (idx < 0 || idx >= MR::getCsvDataElementNum(mBehaviourData))
		return;

	const char* behaviourType;
	MR::getCsvDataStrOrNULL(&behaviourType, mBehaviourData, "Type", idx);
	if (behaviourType == NULL)
		return;

	const char* Param00Str;
	const char* Param01Str;
	const char* Param02Str;
	MR::getParamDataStrOrNULL(&Param00Str, mBehaviourData, 0, idx);
	MR::getParamDataStrOrNULL(&Param01Str, mBehaviourData, 1, idx);
	MR::getParamDataStrOrNULL(&Param02Str, mBehaviourData, 2, idx);

	
	if (MR::isEqualString(behaviourType, "NoTurn"))
	{
		if (Param00Str != NULL)
			mTalkParam.setNoTurnAction(Param00Str);
		if (Param01Str != NULL)
			mActionPointing = mActionReaction = mActionSpin = mActionTrampled = Param01Str;
	}
	else if (MR::isEqualString(behaviourType, "Single"))
	{
		if (Param00Str != NULL)
			mTalkParam.setSingleAction(Param00Str);
	}

	if (MR::isEqualSubString(behaviourType, "Wait"))
	{
		if (Param00Str != NULL)
		{
			mTalkParam.mActionWait = Param00Str;
			mTalkParam.mActionTalk = Param00Str;
		}
	}
	if (MR::isEqualSubString(behaviourType, "Turn"))
	{
		if (Param01Str != NULL)
		{
			mTalkParam.mActionWaitTurn = Param01Str;
			mTalkParam.mActionTalkTurn = Param01Str;
		}
	}
	if (MR::isEqualSubString(behaviourType, "Walk"))
	{
		if (Param02Str != NULL)
		{
			mActionWalk = Param02Str;
			mActionWalkTalk = Param02Str;
		}
	}
}

void SimpleNPC::control() {
	if (mJointCtrl != nullptr)
	{
		// The radius at which the joint controls activate is the same range that is active for the NPC turning to face you
		// that makes the most logical sense performance wise. This works regardless of if turning is enabled or not.
		bool isInRange = MR::calcDistanceToPlayer(mTranslation) < mTalkParam.mTurnDist;
		if (mJointCtrl->mDynamicJointCtrl != nullptr)
		{
			if (mIsDynamicJointActive && !isInRange)
			{
				mJointCtrl->endDynamicCtrl(0);
				mIsDynamicJointActive = false;
			}
			else if (!mIsDynamicJointActive && isInRange)
			{
				mJointCtrl->startDynamicCtrl(-1);
				mIsDynamicJointActive = true;
			}
		}
		if (mJointCtrl->mFaceJointCtrl != nullptr)
		{
			if (mIsFaceJointActive && !isInRange)
			{
				mJointCtrl->endFaceCtrl(0);
				mIsFaceJointActive = false;
			}
			else if (!mIsFaceJointActive && isInRange)
			{
				mJointCtrl->startFaceCtrl(-1);
				mIsFaceJointActive = true;
			}
		}
		mJointCtrl->update();
	}

	NPCActor::control();
}

void SimpleNPC::calcAndSetBaseMtx() {
	if (mJointCtrl != nullptr) // sanity
		mJointCtrl->setCallBackFunction();
	NPCActor::calcAndSetBaseMtx();
}

namespace NrvSimpleNPC
{
	void NrvNull::execute(Spine* pSpine) const {
		//do nothing for now
	}
	NrvNull(NrvNull::sInstance);

	void NrvTalk::execute(Spine* pSpine) const {
		((SimpleNPC*)pSpine->mExecutor)->exeTalk(); // I cannot tell if the game is even using this...
	}
	NrvTalk(NrvTalk::sInstance);

	void NrvWait::execute(Spine* pSpine) const {
		SimpleNPC* self = (SimpleNPC*)pSpine->mExecutor;

		bool hasRail = MR::isExistRail(self);
		if (MR::isFirstStep(self) && !hasRail)
			MR::onCalcShadowOneTime(self, nullptr);
		s32 pointno = hasRail ? MR::getNextRailPointNo(self) : 0;
		if (hasRail)
		{
			s32 v = -1;
			MR::getCurrentRailPointArg1NoInit(self, &v);
			if (v != -1)
				self->_110 = static_cast<f32>(v) * 0.01f;

			v = -1;
			{ // The s32 version of getCurrentRailPointArg2NoInit is not implemented in SMG2
				self->mRailRider->getCurrentPointArgS32WithInit("point_arg2", &v);
			}
			if (v != -1) {
				self->mRailSnapToGround = v & 0x1;
				self->mTalkParam.mEnableTurn = v & 0x2;
				self->mIsPreventReaction = v & 0x4;
			}
		}

		if (self->mRailMoveDelayTimer > 0)
		{
			self->mRailMoveDelayTimer--;
			if (self->mIsPreventReaction || !MR::tryStartReactionAndPushNerve(self, self->mReactionNerve))
				MR::tryTalkNearPlayerAndStartTalkAction(self);
		}
		else
		{
			if (self->mIsPreventReaction || !MR::tryStartReactionAndPushNerve(self, self->mReactionNerve))
				MR::tryTalkNearPlayerAndStartMoveTalkAction(self);
		}


		s32 pointno2 = hasRail ? MR::getNextRailPointNo(self) : 0;
		if (pointno != pointno2)
		{
			s32 v;
			MR::getCurrentRailPointArg0WithInit(self, &v);
			if (v >= 0)
				self->setBehaviour(v);

			MR::getCurrentRailPointArg3WithInit(self, &v);
			if (v > 0)
				self->mRailMoveDelayTimer = v;
		}
	}
	NrvWait(NrvWait::sInstance);

	void NrvReaction::execute(Spine* pSpine) const {
		MR::tryStartReactionAndPopNerve(((NPCActor*)pSpine->mExecutor));
	}
	NrvReaction(NrvReaction::sInstance);
}

namespace MR {
	void getParamDataS32(s32* pDest, const JMapInfo* pCsvData, s32 ParamID, s32 Row) {
		char Buffer[0xC];
		snprintf(Buffer, 0xC, "Param%02dInt", ParamID);
		MR::getCsvDataS32(pDest, pCsvData, Buffer, Row);
	}
	void getParamDataF32(f32* pDest, const JMapInfo* pCsvData, s32 ParamID, s32 Row) {
		char Buffer[0xC];
		snprintf(Buffer, 0xC, "Param%02dF32", ParamID);
		MR::getCsvDataF32(pDest, pCsvData, Buffer, Row);
	}
	void getParamDataStrOrNULL(const char** pDest, const JMapInfo* pCsvData, s32 ParamID, s32 Row) {
		char Buffer[0xC];
		snprintf(Buffer, 0xC, "Param%02dStr", ParamID);
		MR::getCsvDataStrOrNULL(pDest, pCsvData, Buffer, Row);
	}
}