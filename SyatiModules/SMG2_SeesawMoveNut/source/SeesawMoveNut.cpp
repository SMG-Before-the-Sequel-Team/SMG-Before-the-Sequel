#include "SeesawMoveNut.h"

SeesawMoveNut::SeesawMoveNut(const char* pName) : MapObjActor(pName) {
	mSpeedModifier = 300.0f;
	mPrevSpeed = 0.0f;
	mReturnTimer = 0;
}

void SeesawMoveNut::init(const JMapInfoIter& rIter) {
	MapObjActor::init(rIter);

	MapObjActorInitInfo initInfo;
	MapObjActorUtil::setupInitInfoSeesaw(&initInfo, rIter, 0, 0.0f); // RotateAxis should be 0 so mUseSeesaw1AxisRotator = true
	MapObjActor::initialize(rIter, initInfo);

	MR::getJMapInfoArg0NoInit(rIter, &mSpeedModifier);
	MR::calcNearestRailPos(&mTranslation, this, mTranslation);

	bool isAtRailEdge = false;
	f32 railCoord = MR::getRailCoord(this);
	f32 railLength = MR::getRailTotalLength(this);

	if (railCoord == 0.0f || railCoord == railLength) {
		isAtRailEdge = true;
	}
		
	if (isAtRailEdge) {
		initNerve(&NrvSeesawMoveNut::SeesawMoveNutNrvEdge::sInstance, 0);
		mRotator->end();
	}
	else {
		initNerve(&NrvSeesawMoveNut::SeesawMoveNutNrvHalfway::sInstance, 0);
		MapObjActorUtil::startRotator(this);
	}
}

void SeesawMoveNut::exeHalfway() {
	f32 seesawSpeed = MapObjActorUtil::getSeesaw1AxisAngularSpeed(this); // v3

	if (seesawSpeed <= 0.0f) {
		if (seesawSpeed < 0.0f) {
			MR::setRailDirectionToStart(this);
		}
	}
	else {
		MR::setRailDirectionToEnd(this);
	}

	f32 moveSpeed = __fabs(0.01 * (seesawSpeed * mSpeedModifier));
	MR::moveCoordAndFollowTrans(this, moveSpeed);
	autoBackHome();

	if (mPrevSpeed == 0.0f && seesawSpeed != 0.0f) {
		MR::startActionSound(this, "SeesawNutStart", -1, -1, -1);
	}
	mPrevSpeed = seesawSpeed;
	if (seesawSpeed != 0.0f) {
		if (__fabs(seesawSpeed) >= 0.001) {
			MR::startActionSound(this, "SeesawNutRotL", -1, -1, -1);
		}
		if (__fabs(seesawSpeed) >= 0.1) {
			MR::startActionSound(this, "SeesawNutRotM", -1, -1, -1);
		}
		if (__fabs(seesawSpeed) >= 0.2) {
			MR::startActionSound(this, "SeesawNutRotH", -1, -1, -1);
		}

		bool isAtRailEdge = false; // v5
		f32 railCoord = MR::getRailCoord(this);
		f32 railLength = MR::getRailTotalLength(this);

		if (railCoord == 0.0f || railCoord == railLength) {
			isAtRailEdge = true;
		}

		if (isAtRailEdge) {
			mRotator->end();
			MR::reverseRailDirection(this);
			MR::tryRumblePadStrong(this, 0);
			MR::shakeCameraNormal();
			MR::startActionSound(this, "SeesawNutStop", -1, -1, -1);
			setNerve(&NrvSeesawMoveNut::SeesawMoveNutNrvEdge::sInstance);
		}
	}
}

void SeesawMoveNut::exeEdge() {
	if (isEnableStart()) {
		MapObjActorUtil::startRotator(this);
		mReturnTimer = 0;
		setNerve(&NrvSeesawMoveNut::SeesawMoveNutNrvHalfway::sInstance);
	}
	else {
		autoBackHome();
	}
}

void SeesawMoveNut::autoBackHome() {
	if (mReturnTimer >= 180) {
			
		if (isNerve(&NrvSeesawMoveNut::SeesawMoveNutNrvEdge::sInstance)) {
			f32 railLength = MR::getRailTotalLength(this);
			if (MR::getRailCoord(this) == railLength) {
				MapObjActorUtil::startRotator(this);
				setNerve(&NrvSeesawMoveNut::SeesawMoveNutNrvHalfway::sInstance);
			}
		}
		if (__fabs(MapObjActorUtil::getSeesaw1AxisAngularSpeed(this) < 0.5f)) {
			mRotator->_54.mMtx[2][2] = -0.02f; // zz rotation
		}
	}
	if (MR::isOnPlayer(this)) {
		mReturnTimer = 0;
	}
	else {
		mReturnTimer++;
	}
}

bool SeesawMoveNut::isEnableStart() {
	if (!MR::isOnPlayer(this)) {
		return false;
	}

	TVec3f frontVec;
	MR::calcFrontVec(&frontVec, this);
	TVec3f playerPos = *MR::getPlayerPos();
	playerPos.sub(mTranslation);
	f32 dot = (frontVec.dot(playerPos)) * -1;
	JMAVECScaleAdd(frontVec, playerPos, playerPos, dot);
	MR::normalize(&playerPos);

	TVec3f crossVec;
	PSVECCrossProduct(playerPos, mGravity, crossVec);

	TVec3f railDir = MR::getRailDirection(this);
	f32 railCoord = MR::getRailCoord(this);
	f32 railLength = MR::getRailTotalLength(this);

	if (railCoord == railLength) {
		return crossVec.dot(-frontVec) > 0.0f;
	}
	return crossVec.dot(frontVec) > 0.0f;
}

namespace NrvSeesawMoveNut {
	void SeesawMoveNutNrvHalfway::execute(Spine* pSpine) const {
		SeesawMoveNut* pActor = (SeesawMoveNut*)pSpine->mExecutor;
		pActor->exeHalfway();
	}

	void SeesawMoveNutNrvEdge::execute(Spine* pSpine) const {
		SeesawMoveNut* pActor = (SeesawMoveNut*)pSpine->mExecutor;
		pActor->exeEdge();
	}

	SeesawMoveNutNrvHalfway(SeesawMoveNutNrvHalfway::sInstance);
	SeesawMoveNutNrvEdge(SeesawMoveNutNrvEdge::sInstance);
}
