#include "Game/Enemy/IceMerameraKing.h"
#include "Game/Camera/CameraTargetDemoActor.h"
#include "Game/Effect/MerameraParticleCallBack.h"
#include "Game/Enemy/Meramera.h"
#include "Game/Enemy/MoguStone.h"
#include "Game/LiveActor/Binder.h"
#include "Game/LiveActor/HitSensor.h"
#include "Game/LiveActor/LiveActor.h"
#include "JSystem/JGeometry/TMatrix.h"
#include "JSystem/JGeometry/TVec.h"
#include "revolution/types.h"

#include "Game/Util.h"
#include "Game/NameObj/NameObjCategories.h"
#include "JSystem/JUtility/JUTException.h"
#include "Game/Demo/DemoFunction.h"

#include "PinchBgm.h"

#define PINCH_BGM _120 ? "BGM_PINCH_02" : "BGM_PINCH_01"
#define ICE_MAN_ICE_NUM 6
#define EFFECT_NAME_BODYEMIT "BodyEmit"
#define EFFECT_NAME_BODYEMITOFF "BodyEmitOff"
#define EFFECT_NAME_ROLLING "Rolling"
#define EFFECT_NAME_LAND "Land"
#define EFFECT_NAME_DAMAGE "Damage"

namespace {
    static const char* hScaleJointName[4] = {"WideInSide", "WideOutSide", "Hirgh1"};

    static const f32 hOnAirParam[4] = {0.0f, 3.0f, 0.96f, 0.0f};

    static const f32 hOnGroundParam[4] = {0.0f, 1.0f, 0.8f, 0.0f};

    static const f32 hFlyParam[4] = {0.0f, 0.5f, 0.9f, 1.0f};

    static const f32 hAngryDemoParam[4] = {0.0f, 0.5f, 0.9f, 1.5f};

    static const f32 hEscapeOnGroundParam[4] = {-0.7f, 1.0f, 0.96f, 180.0f};

    static const f32 hEscapeOnAirParam[4] = {0.0f, 3.0f, 0.96f, 0.0f};

    static const f32 hPreRecoverJumpParam[4] = {0.0f, 1.0f, 0.96f, 0.0f};

    static const f32 hDamageJumpParam[4] = {0.0f, 1.9f, 0.98f, 0.0f};

    static const f32 hAttackParam[4] = {0.0f, 6.0f, 0.98f, 0.0f};

    static const f32 hExtinguishOnAirParam[4] = {0.0f, 0.125f, 0.96f, 0.0f};

    static const f32 hExtinguishFallOnAirParam[4] = {0.0f, 4.0f, 0.96f, 0.0f};

    template< int length >
    inline const char* makeFormatName(const char* format, ...) {
        va_list args;
        va_start(args, format);
        char shockname[length];
        vsnprintf(shockname, length, format, args);
        s32 len = strlen(shockname) + 1;
        char* finalname = new(0x10) char[len];
        MR::copyString(finalname, shockname, len);
        return finalname;
    }
};  // namespace

namespace NrvIceMerameraKing {
    FULL_NERVE(HostTypeNrvSearch, IceMerameraKing, Search);
    FULL_NERVE_ONEND(HostTypeNrvBeginDemo, IceMerameraKing, Throw, Throw);
    FULL_NERVE_ONEND(HostTypeNrvThrow, IceMerameraKing, Throw, Throw);
    FULL_NERVE_ONEND(HostTypeNrvThrow2nd, IceMerameraKing, Throw, Throw);
    FULL_NERVE(HostTypeNrvExtinguish, IceMerameraKing, Extinguish);
    FULL_NERVE_ONEND(HostTypeNrvEscape, IceMerameraKing, Escape, Escape);
    FULL_NERVE(HostTypeNrvEscapeJump, IceMerameraKing, EscapeJump);
    FULL_NERVE(HostTypeNrvDamage, IceMerameraKing, Damage);
    FULL_NERVE(HostTypeNrvPreRecover, IceMerameraKing, PreRecover);
    FULL_NERVE(HostTypeNrvRecover, IceMerameraKing, Recover);
    FULL_NERVE(HostTypeNrvPreAttack, IceMerameraKing, PreAttack);
    FULL_NERVE(HostTypeNrvAttack, IceMerameraKing, Attack);
    FULL_NERVE(HostTypeNrvAttackAfter, IceMerameraKing, AttackAfter);
    FULL_NERVE(HostTypeNrvAngryDemo1st, IceMerameraKing, AngryDemo);
    FULL_NERVE(HostTypeNrvAngryDemo2nd, IceMerameraKing, AngryDemo);
    FULL_NERVE(HostTypeNrvDeathDemoWait, IceMerameraKing, DeathDemoWait);
    FULL_NERVE(HostTypeNrvDeathDemo, IceMerameraKing, DeathDemo);

    TVec3f hBinderOffset(-100.586f, 222.65601f, -91.796898f);
    TVec3f hEscapeBinderOffset(-39.0625f, 91.796898f, -47.851601f);
};  // namespace NrvIceMerameraKing

extern "C" {
    bool __kAutoMap_8013B110(LiveActor*, const char*, const char*, const Nerve*, const Nerve*, s32, DemoStartInfo::DemoType, DemoStartInfo::CinemaFrameType, DemoStartInfo::StarPointerType, DemoStartInfo::DeleteEffectType); // requestStartTimeKeepDemo

    JMapInfoIter __kAutoMap_8004D180(s32* pDest, const JMapInfo* pCsvData, const char* pItemName, const char* pDataName, const char* pElementName);
}

IceMerameraKing::IceMerameraKing(const char* pName)
    : LiveActor(pName), mFixedPos(nullptr), mThrowingIce(nullptr), mMeramera(nullptr), mIce(), mShockWave(nullptr), mBreakModel(nullptr), mFrontVec(0, 0, 1), mUpVec(0, 1, 0),
      mInitPosition(0, 0, 0), mInitRotation(0, 0, 0), mThrownInARow(0), mHealth(3), mHealthChild(1), mEscapeTime(600), mCameraTarget(nullptr), mJointController(nullptr), mRollRotation(0, 0, 0, 1),
      mBinderOffsetVec(0.0f, 0.0f, 0.0f), _11C(0.0f), _120(false), _121(false) {
}

void IceMerameraKing::init(const JMapInfoIter& rIter) {
    // initDefaultPos now handled by InitActor.bcsv
    // connectToSceneEnemy now handled by InitActor.bcsv
    // initBinder now handled by InitActor.bcsv
    // SW_DEAD is now handled by InitSwitch.bcsv w/ UseWriteAuto
    // SW_A is now handled by InitSwitch.bcsv w/ UseRead
    // Setting the scale to 1.5 is now handled by InitScale.bcsv
    // Shadow is now handled by Shadow.bcsv (which was seemingly unused in SMG1)
    // HitSensor now handled by InitSensor.bcsv
    MR::processInitFunction(this, rIter, false);

    const char* objectName;
    MR::getObjectName(&objectName, rIter);
    f32 v5 = mScale.y / 1.8f;

    mInitPosition.set(mTranslation);
    mInitRotation.set(mRotation);
    MR::calcUpVec(&mUpVec, this);
    MR::calcFrontVec(&mFrontVec, this);
    mOriginalBodyRadius = MR::getBinderRadius(this);
    MR::setBinderRadius(this, mOriginalBodyRadius * v5);
    mBinder->_1C *= v5; // Unfortunately there's no setter for this...
    MR::setBinderOffsetVec(this, &mBinderOffsetVec, true);
    MR::initLightCtrl(this); // This cannot be done with InitActor, as InitActor offers no way to *just* call initLightCtrl.
    mOriginalBinderRadius = MR::getSensorRadius(this, "Body");
    MR::setSensorRadius(this, "Body", mOriginalBinderRadius * v5);

    MR::getJMapInfoArg0WithInit(rIter, &mStarId);
    if (mStarId > 0)
        MR::declarePowerStar(this, mStarId);
    MR::getJMapInfoArg1WithInit(rIter, &mAttackMsg);
    MR::getJMapInfoArg2NoInit(rIter, &mHealth);
    MR::getJMapInfoArg3NoInit(rIter, &mHealthChild);
    MR::getJMapInfoArg4NoInit(rIter, &mEscapeTime);
    MR::declareStarPiece(this, 8 * mHealth);
    mFixedPos = new FixedPosition(this, "Top", TVec3f(0, 0, 0), TVec3f(0.0f, 0.0f, 0.0f));

    initNerve(&NrvIceMerameraKing::HostTypeNrvSearch::sInstance, 0);
    MR::calcGravity(this);
    MR::tryRegisterDemoCast(this, rIter);
    MR::registerDemoActionNerve(this, &NrvIceMerameraKing::HostTypeNrvBeginDemo::sInstance, nullptr);

    mSpinParticle = new MerameraParticleCallBack();
    mJointController = MR::createJointDelegatorWithNullChildFunc(this, &IceMerameraKing::calcJoint, "JointRoot");

#pragma region Init Rocks
    JMapInfo* InitActor = MR::createInitActorCsvParser(objectName, NULL);
    const char* icename;
    if (!MR::hasCsvDataItem(InitActor, "InitFunction", "RockModel"))
        JUTException::panic_f(__FILE__, __LINE__, "InitActor property \"RockModel\" not found!\n");
    MR::getCsvDataStrByElement(&icename, InitActor, "InitFunction", "RockModel", "Data");
    s32 icenum = 6;
    if (MR::hasCsvDataItem(InitActor, "InitFunction", "RockNum"))
        __kAutoMap_8004D180(&icenum, InitActor, "InitFunction", "RockNum", "Data");

    mIce.init(ICE_MAN_ICE_NUM);
    for (s32 i = 0; i < mIce.capacity(); i++) {
        ThrowingIce* newIce = new ThrowingIce("投擲用の氷", icename, mAttackMsg);
        newIce->initWithoutIter();
        mIce.push_back(newIce);
        MR::tryRegisterDemoCast(newIce, rIter);
    }
#pragma endregion
#pragma region Init Shockwave
    const char* shockname = makeFormatName<0x30>("%sShock", objectName);
    mShockWave = new IceMerameraKingShockWave(shockname);
    mShockWave->initWithoutIter();
    mShockWave->mRotation.set(mRotation);
    mShockWave->makeActorDead();
#pragma endregion
#pragma region Init BreakModel
    const char* breakname = makeFormatName<0x30>("%sBreak", objectName);
    mBreakModel = new ModelObj("壊れモデル", breakname, nullptr, MR::CategoryList_Auto, MR::CategoryList_Auto, MR::CategoryList_Auto, false);
    mBreakModel->initWithoutIter();
    mBreakModel->mScale.set(mScale);
    mBreakModel->makeActorDead();
    MR::tryRegisterDemoCast(mBreakModel, rIter);
#pragma endregion
#pragma region Init ChildMeramera
    mChildArray.init(MR::getChildObjNum(rIter));
    for (s32 i = 0; i < mChildArray.capacity(); i++) {
        Meramera* current = new Meramera("メラメラ");
        MR::initChildObj(current, rIter, i);
        current->makeActorDead();
        MR::tryRegisterDemoCast(current, rIter);
        mChildArray.push_back(current);
    }
#pragma endregion
#pragma region Init CameraTargetDemoActor
    mCameraTarget = new CameraTargetDemoActor(getBaseMtx(), "アクター注目");
    mCameraTarget->initWithoutIter();
    mCameraTarget->mTranslation.set(mInitPosition);
    mCameraTarget->mRotation.set(mInitRotation);
    mCameraTarget->setName("メラキン注目ターゲット");
    MR::tryRegisterDemoCast(mCameraTarget, rIter);
#pragma endregion

    makeActorAppeared();
    MR::startAction(this, "Normal"); // This fixes a bug where dying after killing him would keep the darkened body color. This apparently wasn't needed in the original game, however...
    MR::tryEmitEffect(this, EFFECT_NAME_BODYEMIT);
}

void IceMerameraKing::initAfterPlacement() {
    MR::trySetMoveLimitCollision(this);

    u32 binderFlag = mBinder->_8;
    for (s32 i = 0; i < mChildArray.size(); i++) {
        Binder* binder = mChildArray[i]->mBinder;
        binder->_8 = binderFlag;
        binder->_1EC._2 = binderFlag;
    }
}

void IceMerameraKing::kill() {
    LiveActor::kill();

    if (mStarId > 0)
        MR::requestAppearPowerStar(this, mStarId, mTranslation);

    for (s32 i = 0; i < mIce.mCount; i++)
        if (!MR::isDead(mIce[i]))
            mIce[i]->kill();
}

void IceMerameraKing::control() {
    TVec3f v7;
    MR::copyJointScale(this, "Center", &v7);
    f32 v3 = mOriginalBinderRadius * v7.x;
    MR::setBinderRadius(this, v3);
    mBinderOffsetVec.set(0.0f, v3, 0.0f);
    getSensor("Body")->mRadius = mOriginalBodyRadius * v7.x;
    MR::setShadowVolumeSphereRadius(this, nullptr, v3);

    if (MR::isValidSwitchA(this)) {
        if (MR::isOnSwitchA(this))
            MR::invalidateClipping(this); 
        else
            MR::validateClipping(this);
    }

    if (!isNerve(&NrvIceMerameraKing::HostTypeNrvEscape::sInstance) && !isNerve(&NrvIceMerameraKing::HostTypeNrvEscapeJump::sInstance)) {
        TQuat4f quad(0, 0, 0, 1);
        mRollRotation.slerp(quad, 0.25f);
    }

    if (!isNerve(&NrvIceMerameraKing::HostTypeNrvEscape::sInstance) && !isNerve(&NrvIceMerameraKing::HostTypeNrvEscapeJump::sInstance)) {
        MR::startActionSound(this, "BmLvWait", -1, -1, -1);
    }

    NameObj* demoexe = (NameObj*)DemoFunction::findDemoExecutor(this);
    if (demoexe != nullptr && MR::isDemoActive(demoexe->mName)) {
        MR::startSystemLevelSE("SE_DM_LV_MUTE_BGM", -1, -1);
        if (MR::isDemoPartLastStep("カメラ寄る")) {
            // MR::playLevelMarioPinchBGM(_120);
            PinchBGM::startPinchBGM(PINCH_BGM, _120);
            _120 = true;
            _121 = true;
        }
    } else if (!isNerve(&NrvIceMerameraKing::HostTypeNrvDeathDemoWait::sInstance) && !isNerve(&NrvIceMerameraKing::HostTypeNrvDeathDemo::sInstance) && MR::isNearPlayer(this, 5000.0f) && _121) {
        // MR::playLevelMarioPinchBGM(_120);
        PinchBGM::startPinchBGM(PINCH_BGM, _120);
        _120 = true;
    }
}

void IceMerameraKing::calcAndSetBaseMtx() {
    TPos3f v3;
    MR::makeMtxUpFrontPos(&v3, mUpVec, mFrontVec, mTranslation);
    MR::setBaseTRMtx(this, v3);
    mFixedPos->calc();
    mJointController->registerCallBack();
}


void IceMerameraKing::exeSearch() {
    if (MR::isFirstStep(this)) {
        MR::startAction(this, "Wait");
    }
    addVelocityToInitPos();
    MR::moveAndTurnToPlayer(this, &mFrontVec, hFlyParam[0], hFlyParam[1], hFlyParam[2], hFlyParam[3]);
    MR::addVelocityKeepHeightUseShadow(this, 400.0f, 1.5f, 300.0f, nullptr);
    f32 distToPlayer = MR::calcDistanceToPlayer(this);

    if (!(4500.0f < distToPlayer)) {
        if (distToPlayer < 1000.0f) {
            setNerve(&NrvIceMerameraKing::HostTypeNrvPreAttack::sInstance);
        } else {
            if (isEnableThrow()) {
                if (!(mHealth > 2)) {
                    if (MR::isGreaterStep(this, 100) && isDeadAllIce()) {
                        mThrownInARow = 0;
                        setNerve(&NrvIceMerameraKing::HostTypeNrvThrow2nd::sInstance);
                    }
                } else if (MR::isGreaterStep(this, 200) && isDeadAllIce()) {
                    mThrownInARow = 0;
                    setNerve(&NrvIceMerameraKing::HostTypeNrvThrow::sInstance);
                }
            }
        }
    }
}

void IceMerameraKing::exeThrow() {
    if (MR::isFirstStep(this)) {
        MR::startAction(this, "AttackThrow");
        if (isNerve(&NrvIceMerameraKing::HostTypeNrvBeginDemo::sInstance)) {
            MR::calcFrontVec(&mFrontVec, mCameraTarget);
        }
    }
    MR::moveAndTurnToPlayer(this, &mFrontVec, hFlyParam[0], hFlyParam[1], hFlyParam[2], hFlyParam[3]);
    MR::addVelocityKeepHeightUseShadow(this, 400.0f, 1.5f, 300.0f, nullptr);
    addVelocityToInitPos();

    if (MR::isStep(this, 22)) {
        mThrowingIce = getDeadWeaponAndAppear();
        mFixedPos->mMtx.getTrans(mThrowingIce->mTranslation);
        ThrowingIce* ice = mThrowingIce;
        if (!ice) {
            setNerve(&NrvIceMerameraKing::HostTypeNrvSearch::sInstance);
            return;
        }
        
        mThrownInARow += 1;
        TVec3f v11(*MR::getPlayerVelocity());
        v11.scale(35.0f);
        TVec3f v12(*MR::getPlayerCenterPos());
        v12.add(v11);
        mThrowingIce->emitArc(ice->mTranslation, v12, -5.0f, mGravity);
        mThrowingIce = nullptr;
        MR::startActionSound(this, "BmThrow", -1, -1, -1);
    }

    if (MR::isActionEnd(this)) {
        if (MR::calcDistanceToPlayer(this) < 1000.0f) {
            setNerve(&NrvIceMerameraKing::HostTypeNrvPreAttack::sInstance);
        } else {
            u32 v10 = 3;
            if (isNerve(&NrvIceMerameraKing::HostTypeNrvThrow2nd::sInstance)) {
                v10 = 6;
            }

            if (v10 <= mThrownInARow || !isEnableThrow()) {
                setNerve(&NrvIceMerameraKing::HostTypeNrvSearch::sInstance);
            } else if (isNerve(&NrvIceMerameraKing::HostTypeNrvThrow2nd::sInstance)) {
                setNerve(&NrvIceMerameraKing::HostTypeNrvThrow2nd::sInstance);
            } else {
                setNerve(&NrvIceMerameraKing::HostTypeNrvThrow::sInstance);
            }
        }
    }
}

void IceMerameraKing::exeOnEndThrow() {
    tearDownThrow();
}

void IceMerameraKing::tearDownThrow() {
    if (mThrowingIce) {
        mThrowingIce->kill();
        mThrowingIce = nullptr;
    }

    if (mMeramera) {
        mMeramera->kill();
        mMeramera = nullptr;
    }
}

void IceMerameraKing::exeExtinguish() {
    if (MR::isFirstStep(this)) {
        MR::startAction(this, "GoOut");
        MR::startActionSound(this, "BmBlow", -1, -1, -1);
        MR::startActionSound(this, "BmSmoke", -1, -1, -1);
        MR::tryDeleteEffect(this, EFFECT_NAME_BODYEMIT);
        if (MR::isExistEffect(this, EFFECT_NAME_BODYEMITOFF))
            MR::emitEffectWithParticleCallBack(this, EFFECT_NAME_BODYEMITOFF, mSpinParticle);
        TVec3f* v3 = MR::getPlayerCenterPos();
        TVec3f v4(mTranslation);
        v4.sub(*v3);
        MR::vecKillElement(v4, mGravity, &v4);
        MR::normalizeOrZero(&v4);
        MR::setVelocitySeparateHV(this, v4, 20.0f, 0.0f);
        _11C = 200.0f + MR::getShadowNearProjectionLength(this);
    }

    if (MR::isLessStep(this, 40)) {
        MR::addVelocityKeepHeightUseShadow(this, _11C, 1.3f, 700.0f, nullptr);
        MR::moveAndTurnToPlayer(this, &mFrontVec, hExtinguishOnAirParam[0], hExtinguishOnAirParam[1], hExtinguishOnAirParam[2], hExtinguishOnAirParam[3]);
    } else if (MR::isOnGround(this) && MR::isActionEnd(this)) {
        MR::moveAndTurnToPlayer(this, &mFrontVec, hOnGroundParam[0], hOnGroundParam[1], hOnGroundParam[2], hOnGroundParam[3]);
        MR::startActionSound(this, "BmLand", -1, -1, -1);
        mTimeInEscape = 0;
        setNerve(&NrvIceMerameraKing::HostTypeNrvEscape::sInstance);
    } else {
        MR::moveAndTurnToPlayer(this, &mFrontVec, hExtinguishFallOnAirParam[0], hExtinguishFallOnAirParam[1], hExtinguishFallOnAirParam[2], hExtinguishFallOnAirParam[3]);
    }
}

void IceMerameraKing::exeEscape() {
    if (MR::isFirstStep(this)) {
    }
    mTimeInEscape++;

    if (mTimeInEscape > mEscapeTime && MR::isOnGround(this)) {
        mTimeInEscape = 0;
        setNerve(&NrvIceMerameraKing::HostTypeNrvPreRecover::sInstance);
    } else {
        MR::tryEmitEffect(this, EFFECT_NAME_ROLLING);
        if (MR::isOnGround(this)) {
            f32 v11 = MR::getLinerValueFromMinMax(PSVECMag(mVelocity.toVecPtr()), 2.0f, 6.0f, 0.0f, 100.0f);
            MR::startActionSound(this, "BmLvRoll", v11, -1, 15);
        }

        if (MR::isOnGround(this)) {
            f32 radius = getSensor("Body")->mRadius;
            MR::rotateQuatRollBall(&mRollRotation, mVelocity, MR::getGroundNormal(this), (0.5f * radius));
            MR::moveAndTurnToPlayer(this, &mFrontVec, hEscapeOnGroundParam[0], hEscapeOnGroundParam[1], hEscapeOnGroundParam[2], hEscapeOnGroundParam[3]);

            bool isBindedWallFront = false;
            { // MR::isBindedWallFront(this, -_B0, 0.25f)
                if (MR::isBindedWall(this)) {
                    const TVec3f nrm = MR::getWallNormal(this);
                    if ((-mFrontVec).dot(nrm) < 0.25f)
                        isBindedWallFront = true;
                }
            }

            if (isBindedWallFront) {
                setNerve(&NrvIceMerameraKing::HostTypeNrvEscapeJump::sInstance);
            }
        } else {
            f32 sensor = getSensor("Body")->mRadius;
            MR::rotateQuatRollBall(&mRollRotation, mVelocity, -mGravity, 0.5f * sensor);
            MR::moveAndTurnToPlayer(this, &mFrontVec, hEscapeOnAirParam[0], hEscapeOnAirParam[1], hEscapeOnAirParam[2], hEscapeOnAirParam[3]);
        }
    }
}

void IceMerameraKing::exeOnEndEscape() {
    MR::tryDeleteEffect(this, EFFECT_NAME_ROLLING);
}

void IceMerameraKing::exeEscapeJump() {
    if (MR::isStep(this, 20)) {
        MR::addVelocitySeparateHV(this, mFrontVec, 20.0f, 90.0f);
        MR::startActionSound(this, "BmJump", -1, -1, -1);
    }
    mTimeInEscape++;

    if (MR::isOnGround(this)) {
        f32 radius = getSensor("Body")->mRadius;
        MR::rotateQuatRollBall(&mRollRotation, mVelocity, MR::getGroundNormal(this), (0.5f * radius));
        MR::moveAndTurnToPlayer(this, &mFrontVec, hEscapeOnGroundParam[0], hEscapeOnGroundParam[1], hEscapeOnGroundParam[2], hEscapeOnGroundParam[3]);

        if (MR::isGreaterStep(this, 20)) {
            MR::tryEmitEffect(this, EFFECT_NAME_LAND);
            MR::startActionSound(this, "BmLand", -1, -1, -1);
            setNerve(&NrvIceMerameraKing::HostTypeNrvEscape::sInstance);
        }
    } else {
        f32 radius = getSensor("Body")->mRadius;
        MR::rotateQuatRollBall(&mRollRotation, mVelocity, -mGravity, (0.5f * radius));
        MR::moveAndTurnToPlayer(this, &mFrontVec, hEscapeOnAirParam[0], hEscapeOnAirParam[1], hEscapeOnAirParam[2], hEscapeOnAirParam[3]);
    }
}

void IceMerameraKing::exeDamage() {
    if (MR::isFirstStep(this)) {
        MR::startAction(this, "Damage");
        MR::tryEmitEffect(this, EFFECT_NAME_DAMAGE);
        MR::startBlowHitSound(this);
        MR::startActionSound(this, "BmDamage", -1, -1, -1);
        MR::setVelocityJump(this, 80.0f);
        MR::stopSceneForDefaultHit(4);
    }

    if (MR::isGreaterStep(this, 20)) {
        MR::addVelocityKeepHeightUseShadow(this, 900.0f, 3.0f, 300.0f, nullptr);
    }

    if (MR::isOnGround(this)) {
        MR::moveAndTurnToPlayer(this, &mFrontVec, hOnGroundParam[0], hOnGroundParam[1], hOnGroundParam[2], hOnGroundParam[3]);
    } else {
        MR::moveAndTurnToPlayer(this, &mFrontVec, hDamageJumpParam[0], hDamageJumpParam[1], hDamageJumpParam[2], hDamageJumpParam[3]);
    }

    if (MR::isGreaterStep(this, 60)) {
        if (mHealth <= mHealthChild) {
            if (MR::tryStartDemoRegisteredMarioPuppetable(this, "怒りデモ")) {
                setNerve(&NrvIceMerameraKing::HostTypeNrvAngryDemo2nd::sInstance);
            }
        } else if (MR::tryStartDemoRegisteredMarioPuppetable(this, "怒りデモ")) {
            setNerve(&NrvIceMerameraKing::HostTypeNrvAngryDemo1st::sInstance);
        }
    }
}

void IceMerameraKing::exePreRecover() {
    if (MR::isFirstStep(this)) {
        MR::setVelocityJump(this, 70.0f);
        MR::startActionSound(this, "BmJump", -1, -1, -1);
    }
    addVelocityToInitPos();

    if (MR::isLessStep(this, 50)) {
        MR::moveAndTurnToPlayer(this, &mFrontVec, hPreRecoverJumpParam[0], hPreRecoverJumpParam[1], hPreRecoverJumpParam[2], hPreRecoverJumpParam[3]);
    } else if (MR::isStep(this, 50)) {
        MR::startAction(this, "PreRecover");
        MR::moveAndTurnToPlayer(this, &mFrontVec, hFlyParam[0], hFlyParam[1], hFlyParam[2], hFlyParam[3]);
    } else if (MR::isLessStep(this, 120)) {
        MR::addVelocityKeepHeightUseShadow(this, 750.0f, 1.5f, 300.0f, nullptr);
        MR::moveAndTurnToPlayer(this, &mFrontVec, hFlyParam[0], hFlyParam[1], hFlyParam[2], hFlyParam[3]);
    } else {
        setNerve(&NrvIceMerameraKing::HostTypeNrvRecover::sInstance);
    }
}

void IceMerameraKing::exeRecover() {
    if (MR::isFirstStep(this)) {
        MR::startAction(this, "Recover");
        MR::tryEmitEffect(this, EFFECT_NAME_BODYEMIT);
        MR::startActionSound(this, "BmPreRecover", -1, -1, -1);
    }

    if (MR::isStep(this, 80)) {
        MR::startActionSound(this, "BmRecover", -1, -1, -1);
    }
    addVelocityToInitPos();
    MR::moveAndTurnToPlayer(this, &mFrontVec, hFlyParam[0], hFlyParam[1], hFlyParam[2], hFlyParam[3]);
    MR::addVelocityKeepHeightUseShadow(this, 750.0f, 1.5f, 300.0f, nullptr);

    if (MR::isGreaterStep(this, 160)) {
        setNerve(&NrvIceMerameraKing::HostTypeNrvSearch::sInstance);
    }
}

void IceMerameraKing::exePreAttack() {
    if (MR::isFirstStep(this)) {
    }

    if (MR::isLessStep(this, 120)) {
        MR::addVelocityKeepHeightUseShadow(this, 800.0f, 1.5f, 300.0f, nullptr);
    } else if (MR::isDead(mShockWave)) {
        setNerve(&NrvIceMerameraKing::HostTypeNrvAttack::sInstance);
        return;
    }
    addVelocityToInitPos();
    MR::moveAndTurnToPlayer(this, &mFrontVec, hFlyParam[0], hFlyParam[1], hFlyParam[2], hFlyParam[3]);
}

void IceMerameraKing::exeAttack() {
    if (MR::isFirstStep(this)) {
        MR::startAction(this, "Attack");
        MR::startActionSound(this, "BmPreAttack", -1, -1, -1);
    }

    if (!MR::isActionEnd(this)) {
        MR::addVelocityKeepHeightUseShadow(this, 800.0f, 1.5f, 300.0f, nullptr);
        MR::moveAndTurnToPlayer(this, &mFrontVec, hFlyParam[0], hFlyParam[1], hFlyParam[2], hFlyParam[3]);
    } else {
        MR::moveAndTurnToPlayer(this, &mFrontVec, hAttackParam[0], hAttackParam[1], hAttackParam[2], hAttackParam[3]);
    }

    if (MR::isOnGround(this)) { 
        setNerve(&NrvIceMerameraKing::HostTypeNrvAttackAfter::sInstance);
    }
}

void IceMerameraKing::exeAttackAfter() {
    if (MR::isFirstStep(this)) {
        mShockWave->appear();
        mShockWave->mTranslation.set(mTranslation);
        mShockWave->mRotation.set(mInitRotation);
        MR::tryEmitEffect(this, EFFECT_NAME_LAND);
        MR::startAction(this, "AttackEnd");
        MR::startActionSound(this, "BmHipDrop", -1, -1, -1);
        MR::tryRumblePadAndCameraDistanceStrong(this, 800.0f, 1200.0f, 2000.0f);
    }
    MR::moveAndTurnToPlayer(this, &mFrontVec, hOnAirParam[0], hOnAirParam[1], hOnAirParam[2], hOnAirParam[3]);

    if (MR::isGreaterStep(this, 90)) {
        setNerve(&NrvIceMerameraKing::HostTypeNrvSearch::sInstance);
    }
}

void IceMerameraKing::exeAngryDemo() {
    if (MR::isFirstStep(this)) {
        MR::tryEmitEffect(this, EFFECT_NAME_BODYEMIT);
        if (isNerve(&NrvIceMerameraKing::HostTypeNrvAngryDemo2nd::sInstance)) {
            MR::startAction(this, "Summon");
            MR::startActionSound(this, "BmPreAngry2", -1, -1, -1);
        } else {
            MR::startAction(this, "Recover");
            MR::startActionSound(this, "BmPreAngry1", -1, -1, -1);
        }
    }

    if (MR::isStep(this, 1)) {
        MR::resetPosition(this, mInitPosition);
    }
    MR::moveAndTurnToPlayer(this, &mFrontVec, hAngryDemoParam[0], hAngryDemoParam[1], hAngryDemoParam[2], hAngryDemoParam[3]);
    MR::addVelocityKeepHeightUseShadow(this, 400.0f, 1.5f, 300.0f, nullptr);

    if (isNerve(&NrvIceMerameraKing::HostTypeNrvAngryDemo2nd::sInstance) && MR::isStep(this, 90)) {
        for (int i = 0; i < mChildArray.size(); i++) {
            if (MR::isDead(mChildArray[i])) {
                mChildArray[i]->resetAppear();
                mChildArray[i]->makeActorAppeared();
            }
        }
    }

    if (isNerve(&NrvIceMerameraKing::HostTypeNrvAngryDemo2nd::sInstance) && MR::isStep(this, 85)) {
        MR::startActionSound(this, "BmAngry2", -1, -1, -1);
    } else if (MR::isStep(this, 85)) {
        MR::startActionSound(this, "BmAngry1", -1, -1, -1);
    }
    // MR::playLevelMarioPinchBGM(true);
    PinchBGM::startPinchBGM(PINCH_BGM, _120);
    
    if (MR::isDemoPartLastStep("怒りデモ")) {
        if (!(mHealth > 2)) {
            TVec3f v7(mGravity);
            v7.scale(200.0f);
            TVec3f v8(mTranslation);
            v8.sub(v7);
            MR::appearStarPiece(this, v8, 8, 15.0f, 70.0f, false);
        } else {
            TVec3f v5(mGravity);
            v5.scale(200.0f);
            TVec3f v6(mTranslation);
            v6.sub(v5);
            MR::appearStarPiece(this, v6, 16, 15.0f, 70.0f, false);
        }
        setNerve(&NrvIceMerameraKing::HostTypeNrvSearch::sInstance);
    }
}

void IceMerameraKing::exeDeathDemoWait() {
    if (MR::isFirstStep(this) && mStarId > 0) {
        MR::stopStageBGM(30);
    }

    mVelocity.zero();
}

void IceMerameraKing::exeDeathDemo() {
    if (MR::isFirstStep(this)) {
        MR::startAction(this, "Death");
        MR::startActionSound(this, "BmDead", -1, -1, -1);
        mCameraTarget->mTranslation.set(mTranslation);
        mBreakModel->appear();
        mBreakModel->mRotation.set(mInitRotation);
        MR::resetPosition(mBreakModel, mTranslation);
        MR::startAction(mBreakModel, "Break");
        MR::requestMovementOn(mBreakModel);

        for (s32 i = 0; i < mChildArray.size(); i++) {
            mChildArray[i]->kill();
        }

        MR::invalidateShadow(this, nullptr);
    }

    s32 frame = MR::getBckFrameMax(this);

    if (MR::isOnGround(this)) {
        mVelocity.zero();
    } else {
        MR::moveAndTurnToPlayer(this, &mFrontVec, hOnAirParam[0], hOnAirParam[1], hOnAirParam[2], hOnAirParam[3]);
    }

    if (!MR::isHiddenModel(this) && MR::isGreaterStep(this, frame - 37)) {
        MR::hideModelAndOnCalcAnim(this);
    }

    if (!MR::isHiddenModel(mBreakModel) && MR::isActionEnd(mBreakModel)) {
        MR::hideModel(mBreakModel);
    }

    if (MR::isActionEnd(this) && MR::isHiddenModel(this) && MR::isHiddenModel(mBreakModel)) {
        if (register NameObj* demoexe = (NameObj*)DemoFunction::findDemoExecutor(this)) {
            if (mStarId > 0)
                MR::endDemo(this, demoexe->mName);
        }
    }

    if (!MR::isDemoActive()) {
        mBreakModel->kill();
        kill();
    }
}


void IceMerameraKing::attackSensor(HitSensor* pSender, HitSensor* pReceiver) {
    if (pSender != getSensor("Body")) {
        return;
    }

    if (!MR::isSensorPlayer(pReceiver)) {
        if (!MR::isSensorEnemy(pReceiver))
            return;
        MR::sendMsgPush(pReceiver, pSender);
        return;
    }

    if (isNerve(&NrvIceMerameraKing::HostTypeNrvDamage::sInstance) || isNerve(&NrvIceMerameraKing::HostTypeNrvExtinguish::sInstance) ||
        isNerve(&NrvIceMerameraKing::HostTypeNrvEscape::sInstance) || isNerve(&NrvIceMerameraKing::HostTypeNrvEscapeJump::sInstance) ||
        isNerve(&NrvIceMerameraKing::HostTypeNrvPreRecover::sInstance) || isNerve(&NrvIceMerameraKing::HostTypeNrvRecover::sInstance) ||
        isNerve(&NrvIceMerameraKing::HostTypeNrvDeathDemoWait::sInstance) || isNerve(&NrvIceMerameraKing::HostTypeNrvAngryDemo1st::sInstance) ||
        isNerve(&NrvIceMerameraKing::HostTypeNrvAngryDemo2nd::sInstance) || isNerve(&NrvIceMerameraKing::HostTypeNrvDeathDemo::sInstance) ||
        (mAttackMsg == ACTMES_ENEMY_ATTACK_FREEZE && MR::isPlayerElementModeIce())) {
        MR::sendMsgPush(pReceiver, pSender);
    } else if (mAttackMsg >= 0) {
        pReceiver->receiveMessage(mAttackMsg, pSender);
    }
}

bool IceMerameraKing::receiveMsgPlayerAttack(u32 msg, HitSensor* pSender, HitSensor* pReceiver) {
    if (MR::isMsgStarPieceReflect(msg)) {
        return true;
    }

    if (!isNerve(&NrvIceMerameraKing::HostTypeNrvEscape::sInstance) && !isNerve(&NrvIceMerameraKing::HostTypeNrvEscapeJump::sInstance)) {
        return false;
    }

    if (MR::isMsgFreezeAttack(msg) || MR::isMsgPlayerSpinAttack(msg)) {
        mHealth--;

        if (mHealth == 0) {
            // MR::stopMarioPinchBGMSoon();
            if (mStarId > 0)
                PinchBGM::stopPinchBGMSoon();
            if (NameObj* demoexe = (NameObj*)DemoFunction::findDemoExecutor(this))
                __kAutoMap_8013B110(this, demoexe->mName, "死亡デモ", &NrvIceMerameraKing::HostTypeNrvDeathDemo::sInstance, &NrvIceMerameraKing::HostTypeNrvDeathDemoWait::sInstance, 1, DemoStartInfo::DEMOTYPE_1, DemoStartInfo::CINEMAFRAMETYPE_0, DemoStartInfo::STARPOINTERTYPE_0, DemoStartInfo::DELETEEFFECTYPE_0);
        } else {
            setNerve(&NrvIceMerameraKing::HostTypeNrvDamage::sInstance);
        }

        return true;
    }

    return false;
}

bool IceMerameraKing::receiveOtherMsg(u32 msg, HitSensor* pSender, HitSensor* pReceiver) {
    if (MR::isInSpinStormRange(msg, pSender, pReceiver, 600.0f)) {
        if (isNerve(&NrvIceMerameraKing::HostTypeNrvDeathDemo::sInstance) || isNerve(&NrvIceMerameraKing::HostTypeNrvExtinguish::sInstance) ||
            isNerve(&NrvIceMerameraKing::HostTypeNrvDamage::sInstance) || isNerve(&NrvIceMerameraKing::HostTypeNrvAngryDemo1st::sInstance) ||
            isNerve(&NrvIceMerameraKing::HostTypeNrvAngryDemo2nd::sInstance) || isNerve(&NrvIceMerameraKing::HostTypeNrvEscape::sInstance) ||
            isNerve(&NrvIceMerameraKing::HostTypeNrvEscapeJump::sInstance) || isNerve(&NrvIceMerameraKing::HostTypeNrvPreRecover::sInstance) ||
            isNerve(&NrvIceMerameraKing::HostTypeNrvRecover::sInstance)) {
            return false;
        } else {
            TVec3f v10(*MR::getPlayerCenterPos());
            v10.sub(mTranslation);
            MR::vecKillElement(v10, mGravity, &v10);

            if (PSVECMag(v10.toVecPtr()) < getSensor("Body")->mRadius) {
                return false;
            } else {
                setNerve(&NrvIceMerameraKing::HostTypeNrvExtinguish::sInstance);
                return true;
            }
        }
    }

    return false;
}

void IceMerameraKing::addVelocityToInitPos() {
    TVec3f v12(mInitPosition);
    v12.sub(mTranslation);

    if (0.0f < mGravity.dot(v12)) {
        MR::vecKillElement(v12, mGravity, &v12);
    }
    f32 squared = v12.squared();
    f32 half = 0.5f;

    if (squared <= 0.0000038146973f) {
        squared = squared;
    } else {
        f32 inv = JGeometry::TUtil< f32 >::inv_sqrt(squared);
        f32 v9 = inv * half;
        v12.scale(v9);
    }
    mVelocity.add(v12);
}

bool IceMerameraKing::isDeadAllIce() {
    for (s32 i = 0; i < mIce.size(); i++) {
        if (!MR::isDead(mIce[i]))
            return false;
    }
    return true;
}

bool IceMerameraKing::isEnableThrow() {
    if (MR::isValidSwitchA(this) && !MR::isOnSwitchA(this)) {
        return false;
    }

    if (MR::isPlayerInWaterMode() || MR::isPlayerOnWaterSurface()) {
        return false;
    }
    TVec3f v10(*MR::getPlayerCenterPos());
    v10.sub(mTranslation);
    MR::normalizeOrZero(&v10);
    TVec3f v9;
    MR::calcUpVec(&v9, this);
    bool flag = false;
    flag = -0.60f < v9.dot(v10);
    return flag;
}

ThrowingIce* IceMerameraKing::getDeadWeaponAndAppear() {
    for (s32 i = 0; i < mIce.mCount; i++) {
        ThrowingIce* pIce = mIce[i];
        if (pIce != nullptr && MR::isDead(pIce)) {
            MR::resetPosition(pIce, mTranslation);
            pIce->appear();
            return pIce;
        }
    }
    return nullptr;
}

bool IceMerameraKing::calcJoint(TPos3f* a2, const JointControllerInfo& info) {
    TPos3f mtx;
    TVec3f v3;
    mtx.makeQuat(mRollRotation);
    a2->getTrans(v3);
    a2->zeroTrans();
    a2->concat(mtx, *a2);
    a2->setTrans(v3);  
    return true;
}





IceMerameraKingShockWave::IceMerameraKingShockWave(const char* pName) : ModelObj("衝撃", pName, nullptr, MR::CategoryList_Auto, MR::CategoryList_Auto, MR::CategoryList_Auto, false) {
    initHitSensor(2);
    MR::addHitSensorEnemyAttack(this, "circle", 16, 15.0f, TVec3f(0.0f, -100.0f, 0.0f));
    MR::addHitSensorEnemyAttack(this, "circle_end", 16, 24.0f, TVec3f(0.0f, -100.0f, 0.0f));
}

void IceMerameraKingShockWave::appear() {
    LiveActor::appear();
    MR::startAction(this, "Shock");
    HitSensor* sensor = getSensor("circle");
    sensor->mRadius = 0.0f;
}

void IceMerameraKingShockWave::control() {
    TVec3f v7;
    MR::copyJointScale(this, hScaleJointName[2], &v7);
    f32 v3 = 15.0f * v7.y;
    getSensor("circle")->mRadius = v3;
    MR::copyJointScale(this, hScaleJointName[0], &v7);
    f32 v4 = 25.0f * v7.y;
    getSensor("circle_end")->mRadius = v4;
    if (MR::isActionEnd(this)) {
        kill();
    }
}

void IceMerameraKingShockWave::attackSensor(HitSensor* pSender, HitSensor* pReceiver) {
    if (pSender != getSensor("circle_end") && MR::isSensorPlayer(pReceiver)) {
        HitSensor* sensor = getSensor("circle_end");
        TVec3f v17(sensor->mPosition);
        v17.sub(pReceiver->mPosition);
        f32 radius2 = pReceiver->mRadius;
        f32 radius1 = sensor->mRadius;

        if (PSVECMag(v17.toVecPtr()) < (radius1 + radius2)) {
            return;
        }

        TVec3f v15(pReceiver->mPosition);
        TVec3f v16;
        v15.sub(pSender->mPosition);
        MR::calcUpVec(&v16, this);

        if (__fabsf(v15.dot(v16)) < 200.0f) {
            MR::vecKillElement(v15, v16, &v15);
            MR::sendMsgEnemyAttackFlipMaximumToDir(pReceiver, pSender, v15);
        }
    }
}



#include "Game/Player/MarioHolder.h"

namespace MR {
    void initMarioResourceIce(const JMapInfoIter& rIter) {
        s32 arg;
        MR::getJMapInfoArg0WithInit(rIter, &arg);
        if (arg == ACTMES_ENEMY_ATTACK_FREEZE)
            MR::enablePlayerResourceFreezeIce();
    }
}