#include "Game/NPC/TeresaRacer.h"

#include "Game/Player/MarioAccess.h"
#include "Game/NPC/TalkMessageFunc.h"
#include "Game/Util/ObjUtil.h"
#include "Game/Util/EventUtil.h"
#include "Game/Util/JMapUtil.h"
#include "Game/Util/TalkUtil.h"
#include "Game/Util/ActorCameraUtil.h"
#include "Game/Util/LiveActorUtil.h"
#include "Game/Util/PlayerUtil.h"
#include "Game/Util/RailUtil.h"
#include "Game/Util/MtxUtil.h"
#include "Game/Util/CameraUtil.h"
#include "Game/Util/SoundUtil.h"
#include "Game/Util/ActorMovementUtil.h"
#include "Game/Util/ActorAnimUtil.h"
#include "Game/Util/ActorShadowUtil.h"
#include "Game/Util/ActorSensorUtil.h"
#include "Game/Util/MathUtil.h"
#include "Game/Util/BgmUtil.h"
#include "Game/Util/StarPointerUtil.h"

#include "Game/MapObj/GCapture.h"
#include "Game/Scene/SceneObjHolder.h"

TeresaRacer::TeresaRacer(const char* pName) : NPCActor(pName) {
	initRacer();
	mTakeOutStar = NULL;
	mActorCameraInfo = NULL;
	mIsAtGoal = false;
	mBgmState = 0;
	mObjArg0 = -1;
	mObjArg1 = false;
	mObjArg2 = 0.f;
}

void TeresaRacer::init(const JMapInfoIter& rIter) {
	NPCActorCaps caps = NPCActorCaps("TeresaRacer");
	caps.setDefault();
	caps.mSceneType = 2;// caps.setIndirect();

	caps.mInitBinder = false; //...is this correct??
	caps.mInitBcsvShadow = true;
	caps.mBodySensorRadius = 220.f;
	caps.mBodySensorOffset.x = 0.f;
	caps.mBodySensorOffset.y = 25.f;
	caps.mBodySensorOffset.z = 40.f;
	caps.mWaitNerve = &NrvTeresaRacer::TeresaRacerNrvWait::sInstance;
	caps.mInitYoshiLockOnTarget = false;
	initialize(rIter, caps, "TeresaRacer", NULL, false);

	MR::getJMapInfoArg0WithInit(rIter, &mObjArg0);
	MR::getJMapInfoArg1WithInit(rIter, &mObjArg1);
	MR::getJMapInfoArg2WithInit(rIter, &mObjArg2);

	if ((mObjArg0 == 0 && MR::isPlayerLuigi())
		||
		(mObjArg0 == 1 && !MR::isPlayerLuigi()))
	{
		makeActorDead();
		return; //this instance will not be used
	}

	MR::declarePowerStar(this);

	MR::registerBranchFunc(this->mTalkCtrl, TalkMessageFunc(this, &branchFunc));
	MR::registerAnimeFunc(this->mTalkCtrl, TalkMessageFunc(this, &animeFunc));
	MR::registerEventFunc(this->mTalkCtrl, TalkMessageFunc(this, &eventFunc));
	MR::setDistanceToTalk(this->mTalkCtrl, 500.f);

	RaceManagerFunction::entryRacerOthers((AbstractRacer*)this);

	MR::initMultiActorCamera(this, rIter, &this->mActorCameraInfo, "会話");

	mTalkParam.mActionWait = mTalkParam.mActionWaitTurn = "WaitOpen";
	mTalkParam.mActionTalk = mTalkParam.mActionTalkTurn = "WaitOpenTalk";
	mActionWalk = mActionWalkTalk = "Chase";
	mTalkParam.mTurnSpeed = 1.5f;
	_110 = 20.f;
	_114 = 0.1f;
	_118 = 0.05f;
	mTalkParam.mTurnDist = 3000.f;
	mTakeOutStar = new TakeOutStar(this, "TakeOutStarTeresaRacer", "TakeOutStarTeresaRacer", &NrvTeresaRacer::TeresaRacerNrvTakeOutStar::sInstance, 0);

	makeActorAppeared();
}

void TeresaRacer::control() {
	if (!mReactTrampleAttack) { //???is this even the right value?
		//MR::startSound(this, "SE_SM_TERERACER_TRAMPLE", -1, -1);
	}
	NPCActor::control();
}

bool TeresaRacer::branchFunc(u32 vParam)
{
	RaceManagerFunction::updateRank();
	return RaceManagerFunction::getRaceRank() == 1;
}
bool TeresaRacer::animeFunc(u32 vParam)
{
	if (!isNerve(&NrvTeresaRacer::TeresaRacerNrvReady::sInstance))
		pushNerve(&NrvTeresaRacer::TeresaRacerNrvReady::sInstance);

	if (MR::isBckOneTimeAndStopped(this))
	{
		popNerve();
		return true;
	}

	if (vParam == 0)
	{
		MR::tryStartActionNotPlaying(this, "Close");
		mTalkParam.mActionTalk =
			mTalkParam.mActionTalkTurn =
			mTalkParam.mActionWait =
			mTalkParam.mActionWaitTurn = "CloseTalk";
	}
	else if (vParam == 1)
	{
		mTalkParam.mActionWait =
			mTalkParam.mActionWaitTurn =
			mTalkParam.mActionTalk =
			mTalkParam.mActionTalkTurn = "Defeat";
		popNerve();
		return true;
	}
	else if (vParam == 2)
	{
		MR::tryStartActionNotPlaying(this, "Open");
		mTalkParam.mActionTalk =
			mTalkParam.mActionTalkTurn = "WaitOpenTalk";
		mTalkParam.mActionWait =
			mTalkParam.mActionWaitTurn = "WaitOpen";
	}
	else if (vParam == 3)
	{
		MR::tryStartActionNotPlaying(this, "Open");
		mTalkParam.mActionTalk =
			mTalkParam.mActionTalkTurn = "WaitOpenTalk";
		mTalkParam.mActionWait =
			mTalkParam.mActionWaitTurn = "WaitOpen";
	}

	return false;
}
bool TeresaRacer::eventFunc(u32 vParam)
{
	if (vParam == 0)
		return this->mTakeOutStar->takeOut();
	return true;
}



void TeresaRacer::exeWait()
{
	if (!MR::tryTalkNearPlayerAndStartTalkAction(this))
		return;

	if (mObjArg1)
		MR::startMultiActorCameraTargetSelf(this, this->mActorCameraInfo, "会話", -1);

	setNerve(&NrvTeresaRacer::TeresaRacerNrvPre::sInstance);
}
void TeresaRacer::exePre()
{
	if (MR::isFirstStep(this))
	{
		//MR::resetGCapture
		{
			if (MR::isExistSceneObj(SCENE_OBJ_G_CAPTURE))
			{
				GCapture* p = (GCapture*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_G_CAPTURE);
				//GCapture::reset
				{
					if (p->_10C != 0)
					{
						MR::endBindAndPlayerWeakGravityLimitJump(p, p->mVelocity);
						p->_10C = 0;
						MR::endStarPointerMode(p);
					}
					p->releaseTractTarget();
					p->setNerve(&NrvGCapture::GCaptureNrvWait::sInstance);
				}
			}
		}
	}

	if (MR::isActionContinuous(this))
		return;

	if (!MR::tryTalkNearPlayerAtEndAndStartTalkAction(this))
		return;

	if (MR::tryTalkSelectLeft(this->mTalkCtrl))
	{
		RaceManagerFunction::startRaceWithWipe();
		setNerve(&NrvTeresaRacer::TeresaRacerNrvReady::sInstance);
		return;
	}

	if (mObjArg1)
		MR::endMultiActorCamera(this, this->mActorCameraInfo, "会話", false, -1);

	setNerve(&NrvTeresaRacer::TeresaRacerNrvWait::sInstance);
}
void TeresaRacer::exePost()
{
	if (MR::isActionContinuous(this))
		return;

	if (!MR::tryTalkNearPlayerAtEndAndStartTalkAction(this))
		return;

	MR::endMultiActorCamera(this, this->mActorCameraInfo, "会話", true, -1);
	RaceManagerFunction::updateRank();
	if (RaceManagerFunction::getRaceRank() == 1)
	{
		setNerve(&NrvTeresaRacer::TeresaRacerNrvTalk::sInstance);
		return;
	}

	MR::forceKillPlayerByGroundRace();
	setNerve(&NrvTeresaRacer::TeresaRacerNrvReady::sInstance);
}
void TeresaRacer::exeMove()
{
	if (MR::isRailReachedNearGoal(this, 500.f) || !MR::isRailGoingToEnd(this))
	{
		setNerve(&NrvTeresaRacer::TeresaRacerNrvGoal::sInstance);
	}
	else
	{
		s32 mPoseMode = 0;
		MR::getCurrentRailPointArg0NoInit(this, &this->_110);
		MR::getCurrentRailPointArg1NoInit(this, &mPoseMode);

		TVec3f tVec;
		MR::extractMtxYDir((MtxPtr)getBaseMtx(), &tVec);

		MR::adjustmentRailCoordSpeed(this, this->_110, this->_114);
		MR::moveRailRider(this);

		if (mPoseMode == 0)
		{
			MR::decidePose(this, tVec, MR::getRailDirection(this), MR::getRailPos(this), 1.f, 0.05f, 0.1f);
		}
		else
		{
			TVec3f CamZDir = MR::getCamZdir();
			TVec3f InvCamZDir = -CamZDir;
			InvCamZDir.x *= 0.4f;
			InvCamZDir.y *= 0.4f;
			InvCamZDir.z *= 0.4f;

			TVec3f v2C = TVec3f(MR::getRailDirection(this));
			register TVec3f* asmReg31 = &v2C;
			register TVec3f* asmReg1 = &InvCamZDir;
			__asm
			{
				psq_l     f1, 0x0(asmReg1), 0, 0
				psq_l     f0, 0(asmReg31), 0, 0
				psq_l     f2, 8(asmReg31), 1, 0
				ps_add    f0, f0, f1
				psq_l     f3, 0x8(asmReg1), 1, 0
				ps_add    f1, f2, f3
				psq_st    f0, 0(asmReg31), 0, 0
				psq_st    f1, 8(asmReg31), 1, 0
			}
			if (!MR::normalizeOrZero(asmReg31))
			{
				TVec3f v14 = MR::getRailPos(this);
				MR::decidePose(this, tVec, v2C, v14, 1.0f, 0.05f, 0.1f);
			}
		}
	}

	if (MR::calcDistanceToPlayer(this) < mObjArg2) //not gonna bother checking to see if the arg is 0, 'cause I don't think that "distance to player" can ever result in Zero
	{
		MR::setRasterScroll(0x10, 0x12C, 0x3E8);
		MR::startAtmosphereLevelSE("SE_AT_LV_DEATH_PROM_RASTER", -1, -1);
	}

	changeBgmState();
}
void TeresaRacer::exeGoal()
{
	if (MR::isFirstStep(this))
	{
		mTalkParam.mEnableTurn = true;
		this->_110 = 0.f;
		mTalkParam.mEnableTalkTurn = true;
	}

	if (MR::getRailCoordSpeed(this) < 1.f)
	{
		MR::tryStartMoveTurnAction(this);
	}
	else
	{
		MR::tryStartTurnAction(this);
	}

	if (mObjArg2 > 0.f)
	{
		MR::setRasterScroll(0x10, 0x12C, 0x3E8);
		MR::startAtmosphereLevelSE("SE_AT_LV_DEATH_PROM_RASTER", -1, -1);
	}

	changeBgmState();
}
void TeresaRacer::changeBgmState()
{
	if (MR::isLessStep(this, 480) || mIsAtGoal)
		return;

	if (MR::calcDistanceToPlayer(this) >= 3000.0f)
	{
		if (mBgmState != 1)
			MR::setStageBGMState(1, 150);
		mBgmState = 1;
	}
	else
	{
		if (mBgmState != 2)
			MR::setStageBGMState(2, 150);
		mBgmState = 2;
	}
}



void TeresaRacer::initRacer() {
	AbstractRacer::initRacer();
}

void TeresaRacer::prepRacer(const RaceManager* pRaceManager) {
	MR::invalidateClipping(this);
	MR::moveCoordToStartPos(this);
	MR::setRailCoordSpeed(this, 0.0f);
	MR::followRailPose(this, 1.f, 1.f);
	MR::requestMovementOn(this);
	MR::startAction(this, "Chase");

	mTalkParam.mEnableTurn = false;
	mTalkParam.mEnableTalkTurn = false;
}

void TeresaRacer::startRacer() {
	mIsAtGoal = false;
	mBgmState = 0;
	setNerve(&NrvTeresaRacer::TeresaRacerNrvMove::sInstance);
}

bool TeresaRacer::updateRacer(const RaceManager* pRaceManager) {
	
	f32 vNearest = MR::calcNearestRailCoord(pRaceManager->_100, this->mTranslation);
	f32 vTotal = MR::getRailTotalLength(pRaceManager->_100);
	_18.set(_C);
	_C.set(this->mTranslation);

	register f32 _4 = this->_4;
	_4 -= vNearest;
	__asm {
		fabs _4, _4
		frsp _4, _4
	}
	if (_4 < 2000.f)
	{
		this->_4 = vNearest;
	}

	//_4 = this->_4;
	//_4 -= vTotal;
	//__asm {
	//	fabs _4, _4
	//	frsp _4, _4
	//}
	//if (_4 >= 100.f)
	//	return false;

	register f32 x = vNearest - vTotal;
	__asm {
		fabs x, x
		frsp x, x
	}
	if (x > 1000.f)
		return false; //Too far away :(

	//mIsAtGoal = pRaceManager->isGoal((AbstractRacer*)this);
	mIsAtGoal = MR::isRaceManagerGoalFromPathArg6(pRaceManager, (AbstractRacer*)this);
	return mIsAtGoal;
}

bool TeresaRacer::goalRacer() {
	return false;
}

void TeresaRacer::loseRacer() {
	//do nothing lol
}

void TeresaRacer::resetRacer(const RaceManager* pRaceManager) {
	if (pRaceManager->_90 == 1)
	{
		MR::setDefaultPose(this);
	}
	else
	{
		//adding MR::setNPCActorPos manually
		{
			TPos3f vPos;
			vPos.identity();
			MR::findNamePos("レース終了後位置テレサ", (MtxPtr)(&vPos));
			setBaseMtx(vPos);
			vPos.getTrans(this->mTranslation);
			MR::resetPosition(this);
			MR::onCalcShadowOneTimeAll(this);
		}

		setInitPose();
	}

	turnToPlayer();
	setNerve(&NrvTeresaRacer::TeresaRacerNrvPost::sInstance);
	MR::startMultiActorCameraTargetSelf(this, this->mActorCameraInfo, "会話", -1);
	mTalkParam.mEnableTurn = true;
	mTalkParam.mEnableTalkTurn = true;
}

void TeresaRacer::exitRacer() {
	MR::validateClipping(this);
}

//Is this even correct??
void TeresaRacer::requestMovementOn() {
	MR::requestMovementOn(this);
}

namespace NrvTeresaRacer {
	void TeresaRacerNrvWait::execute(Spine* pSpine) const {
		((TeresaRacer*)pSpine->mExecutor)->exeWait();
	}
	TeresaRacerNrvWait(TeresaRacerNrvWait::sInstance);

	void TeresaRacerNrvPre::execute(Spine* pSpine) const {
		((TeresaRacer*)pSpine->mExecutor)->exePre();
	}
	TeresaRacerNrvPre(TeresaRacerNrvPre::sInstance);

	void TeresaRacerNrvReady::execute(Spine* pSpine) const {
		//Deliberately do nothing
	}
	TeresaRacerNrvReady(TeresaRacerNrvReady::sInstance);

	void TeresaRacerNrvMove::execute(Spine* pSpine) const {
		((TeresaRacer*)pSpine->mExecutor)->exeMove();
	}
	TeresaRacerNrvMove(TeresaRacerNrvMove::sInstance);

	void TeresaRacerNrvGoal::execute(Spine* pSpine) const {
		((TeresaRacer*)pSpine->mExecutor)->exeGoal();
	}
	TeresaRacerNrvGoal(TeresaRacerNrvGoal::sInstance);

	void TeresaRacerNrvPost::execute(Spine* pSpine) const {
		((TeresaRacer*)pSpine->mExecutor)->exePost();
	}
	TeresaRacerNrvPost(TeresaRacerNrvPost::sInstance);

	void TeresaRacerNrvTalk::execute(Spine* pSpine) const {
		MR::tryTalkNearPlayerAndStartTalkAction((TeresaRacer*)pSpine->mExecutor);
	}
	TeresaRacerNrvTalk(TeresaRacerNrvTalk::sInstance);

	void TeresaRacerNrvTakeOutStar::execute(Spine* pSpine) const {
		//Deliberately do nothing
	}
	TeresaRacerNrvTakeOutStar(TeresaRacerNrvTakeOutStar::sInstance);
}

#include "Kamek/hooks.h"

namespace MR {
	inline void setRasterScroll(s32 a, s32 b, s32 c)
	{
		MarioAccess::getPlayerActor()->setRasterScroll(a,b,c);
	}
	bool isRaceManagerGoalFromPathArg6(const RaceManager* pManager, const AbstractRacer* pRacer)
	{
		//0 = -Z Axis (goal if Z falls below final path point) [VANILLA]
		//1 = -Y Axis (goal if Y falls below final path point)
		//2 = -X Axis (goal if X falls below final path point)
		//3 = Z Axis (goal if Z goes above final path point)
		//4 = Y Axis (goal if Y goes above final path point)
		//5 = X Axis (goal if X goes above final path point)
		//6 = Sphere Range (goal if actor enters range)
		s32 GoalType = 0;
		//Manually implementing MR::getRailArg6NoInit
		{
			pManager->_100->mRailRider->getRailArgNoInit("path_arg6", &GoalType);
		}

		if (GoalType == 1)
			return isRaceManagerGoalAxis(pManager, pRacer, 1, false);
		if (GoalType == 2)
			return isRaceManagerGoalAxis(pManager, pRacer, 2, false);
		if (GoalType == 3)
			return isRaceManagerGoalAxis(pManager, pRacer, 0, true);
		if (GoalType == 4)
			return isRaceManagerGoalAxis(pManager, pRacer, 1, true);
		if (GoalType == 5)
			return isRaceManagerGoalAxis(pManager, pRacer, 2, true);
		if (GoalType == 6)
			return isRaceManagerGoalFull(pManager, pRacer);

		return pManager->isGoal(pRacer); //vanilla behaviour
	}
	kmCall(0x80265C6C, isRaceManagerGoalFromPathArg6); //Update PlayerRacer
	kmCall(0x8035A12C, isRaceManagerGoalFromPathArg6); //Update PichanRacer

	bool isRaceManagerGoalAxis(const RaceManager* pManager, const AbstractRacer* pRacer, s32 fIndex, bool isPositive)
	{
		s32 GoalSize = 650;
		pManager->_100->mRailRider->getRailArgNoInit("path_arg7", &GoalSize);
		f32 GoalSizeFloat = static_cast<f32>(GoalSize);
		TVec3f vEndPos;
		MR::calcRailEndPointPos(&vEndPos, pManager->_100);
		f32 distanceToGoalNode = PSVECDistance((Vec*)&vEndPos, (Vec*)&pRacer->_C);
		if (distanceToGoalNode > GoalSizeFloat)
			return false;

		f32 EndAxis = *(((f32*)&vEndPos.x)+fIndex);
		f32 CurAxis = *(((f32*)&pRacer->_C.x)+fIndex);
		if (isPositive)
			return CurAxis > EndAxis;
		return CurAxis < EndAxis;
	}
	//a 3D version of RaceManager::isGoal
	bool isRaceManagerGoalFull(const RaceManager* pManager, const AbstractRacer* pRacer)
	{
		s32 GoalSize = 650;
		pManager->_100->mRailRider->getRailArgNoInit("path_arg7", &GoalSize);
		f32 GoalSizeFloat = static_cast<f32>(GoalSize);
		//Use pRacer->_C
		TVec3f vEndPos;
		MR::calcRailEndPointPos(&vEndPos, pManager->_100);
		f32 distanceToGoalNode = PSVECDistance((Vec*)&vEndPos, (Vec*)&pRacer->_C);
		f32 distanceToGoalNode2 = PSVECDistance((Vec*)&vEndPos, (Vec*)&pRacer->_18);
		if (distanceToGoalNode > GoalSizeFloat)
			return false;
		return distanceToGoalNode2 < GoalSizeFloat;
	}

	// Allows the player to not die when going beyond the finish
	bool tryKillPlayerBeyondGoal(const RaceManager* pManager, register PlayerRacer* pPlayerRacer)
	{
		register bool ImNotWritingThisHeader;
		register f32 ImStillNotWritingThisHeader;
		__asm {
			lwz ImNotWritingThisHeader, 0xC4(pPlayerRacer)
			lfs ImStillNotWritingThisHeader, 0xA4(pPlayerRacer);
		}
		if (ImNotWritingThisHeader)
			return false;

		s32 IsUseDeath = -1;
		pManager->_100->mRailRider->getRailArgNoInit("path_arg5", &IsUseDeath);
		if (IsUseDeath >= 0)
			return false;

		// I'm not doing inline ASM just for this...
		TVec3f vEndPos;
		MR::calcRailEndPointPos(&vEndPos, pManager->_100);

		s32 GoalType = 0;
		//Manually implementing MR::getRailArg6NoInit
		{
			pManager->_100->mRailRider->getRailArgNoInit("path_arg6", &GoalType);
		}

		if (GoalType == 6)
			return false; // how do you "pass" a sphere goal???

		f32 KillDist = 1000.0f;
		if ((GoalType == 0 && ImStillNotWritingThisHeader <= vEndPos.z - KillDist) ||
			(GoalType == 1 && ImStillNotWritingThisHeader <= vEndPos.y - KillDist) ||
			(GoalType == 2 && ImStillNotWritingThisHeader <= vEndPos.x - KillDist) ||
			(GoalType == 3 && ImStillNotWritingThisHeader <= vEndPos.z + KillDist) ||
			(GoalType == 4 && ImStillNotWritingThisHeader <= vEndPos.y + KillDist) ||
			(GoalType == 5 && ImStillNotWritingThisHeader <= vEndPos.x + KillDist))
			MR::forceKillPlayerByAbyss();

		return true;
	}
	kmWrite32(0x80265C94, 0x7FC3F378); //mr r3, r30
	kmWrite32(0x80265C98, 0x7FA4EB78); //mr r4, r29
	kmCall(0x80265C9C, tryKillPlayerBeyondGoal);
	kmWrite32(0x80265CA0, 0x48000024); //b* 0x80265CC4


#ifndef GALAXY_LEVEL_ENGINE
    // These should probably be moved to a common "Extended Race Functionality" module in the future.
    // However, GLE comes packaged with an equivalent function so this would break if you tried to use GLE at the same time.

	void ValidateIfNotNull(LiveActor* pActor)
	{
		if (pActor == NULL)
			return;
		MR::validateHitSensors(pActor);
	}
	kmCall(0x80265118, ValidateIfNotNull);

	bool GetRaceStartType(const RaceManager* pRaceManager)
	{
		s32 v = 0;
		MR::getRailArg1NoInit(pRaceManager->_100, &v);
		if (v <= 0)
			return pRaceManager->isNerve(&NrvRaceManager::RaceManagerNrvWait::sInstance);
		return true;
	}
	kmCall(0x80263A28, GetRaceStartType);
#endif
}