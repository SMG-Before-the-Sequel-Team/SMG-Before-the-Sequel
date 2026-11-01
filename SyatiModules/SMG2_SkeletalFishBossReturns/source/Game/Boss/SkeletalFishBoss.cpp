#include "Game/Boss/SkeletalFishBoss.h"
#include "Game/Boss/SkeletalFishBossInfo.h"
#include "Game/Boss/SkeletalFishBossBattleDirector.h"
#include "Game/Boss/SkeletalFishBossRail.h"
#include "Game/Boss/SkeletalFishBossRailHolder.h"
#include "Game/Boss/SkeletalFishGuardHolder.h"
#include "Game/Boss/SkeletalFishJointCalc.h"
#include "Game/Boss/SkeletalFishRailControl.h"
#include "Game/Camera/CameraTargetMtx.h"
#include "Game/LiveActor/ActorCameraInfo.h"
#include "Game/LiveActor/ModelObj.h"
#include "Game/LiveActor/SensorHitChecker.h"
#include "Game/Map/CollisionParts.h"
#include "Game/NameObj/NameObjExecuteHolder.h"
#include "Game/System/ResourceHolder.h"
#include "JSystem/JMath.h"
#include "JSystem/JMATrigonometric.h"

#include "Game/Scene/SceneObjHolder.h"
#include "Game/Util/ActorAnimUtil.h"
#include "Game/Util/ActorCameraUtil.h"
#include "Game/Util/ActorInitUtil.h"
#include "Game/Util/ActorSensorUtil.h"
#include "Game/Util/ActorShadowUtil.h"
#include "Game/Util/ActorSwitchUtil.h"
#include "Game/Util/ActionSoundUtil.h"
#include "Game/Util/BaseMatrixFollowTargetHolder.h"
#include "Game/Util/BgmUtil.h"
#include "Game/Util/CameraUtil.h"
#include "Game/Util/DemoUtil.h"
#include "Game/Util/EffectUtil.h"
#include "Game/Util/GravityUtil.h"
#include "Game/Util/JMapUtil.h"
#include "Game/Util/JointUtil.h"
#include "Game/Util/LiveActorUtil.h"
#include "Game/Util/MathUtil.h"
#include "Game/Util/ObjUtil.h"
#include "Game/Util/RailUtil.h"
#include "Game/Util/SceneUtil.h"
#include "Game/Util/ScreenUtil.h"
#include "Game/Util/SoundUtil.h"
#include "Game/Player/MarioAccess.h"

#include "Game/SkeletalFishBoss_CameraUtil.h"

namespace {
    static SkeletalFishBoss::SensorToCollider sColInfo[0xE] = {
        { "Joint01", "BackBone01" },
        { "Joint02", "BackBone02" },
        { "Joint03", "BackBone03" },
        { "Joint04", "BackBone04" },
        { "Joint05", "BackBone05" },
        { "Joint06", "BackBone06" },
        { "Joint07", "BackBone07" },
        { "Joint08", "BackBone08" },
        { "Joint09", "BackBone09" },
        { "Joint10", "BackBone10" },
        { "Joint11", "BackBone11" },
        { "Joint12", "BackFin" },
        { "LeftFinJoint", "LeftFinJoint" },
        { "RightFinJoint", "RightFinJoint" }
    };

    static SkeletalFishBoss::HitPos sHitPosData[0x10] = {
        { "Wound0", "Head", { 26.144243f, 414.56268f, 552.11462f } },
        { "Wound1", "Head", { -618.77576f, 55.655441f, 163.1091f } },
        { "Wound10", "Head", { 597.22388f, 92.839088f, 188.59679f } },
        { "Wound11", "Head", { 4.24828f, -134.68741f, 869.91559f } },
        { "Wound2", "Head", { 88.673393f, 10.598214f, -206.06902f } },
        { "Wound3", "Head", { 331.39523f, 37.908966f, 794.43939f } },
        { "Wound4", "Head", { -314.09827f, -16.389574f, 782.43799f } },
        { "Wound5", "Head", { 6.6049418f, 104.82322f, 1221.328f } },
        { "Wound8", "Head", { 365.95203f, 371.12589f, 93.809898f } },
        { "Wound9", "Head", { -262.98569f, 410.83163f, 159.02629f } },
        { "WoundJow0", "Jow", { 35.980412f, -300.88702f, 76.614449f } },
        { "WoundJow1", "Jow", { -181.04329f, -124.71387f, 593.61951f } },
        { "WoundJow2", "Jow", { 475.85388f, -146.99239f, 119.52184f } },
        { "WoundJow3", "Jow", { -473.81219f, -146.99239, 119.52184f } },
        { "WoundJow4", "Jow", { 233.05017f, -124.71387f, 575.92737f } },
        { "WoundJow5", "Jow", { -4.140523f, -318.03336f, 394.53833f } },
    };
};

namespace {
    char* test[] = {
        "Shadow00",
        "Joint02",
        "SkeletalFishBossShadow"
    };
    static SkeletalFishBoss::JointToShadow sShadowInfo = {
         "Shadow00", "Joint02", "SkeletalFishBossShadow"
    };

    inline SkeletalFishBoss::JointToShadow& testInline() {
        return sShadowInfo;
    }

    inline const char* getName(SkeletalFishBoss::JointToShadow& jointToShadow) {
        return jointToShadow.mName;
    }

    inline const char* getShadowName(SkeletalFishBoss::JointToShadow& jointToShadow) {
        return jointToShadow.mShadowName;
    }

    inline const char* getJointName(SkeletalFishBoss::JointToShadow& jointToShadow) {
        return jointToShadow.mJointName;
    }
};

SkeletalFishBoss::SkeletalFishBoss(const char* pName) : LiveActor(pName) {
    mJointIndicies = NULL;
    mPartsArray = NULL;
    mBossHead = NULL;
    mScarFlash = NULL;
    mBreakModel = NULL;
    _D4 = NULL;
    mRailControl = new SkeletalFishRailControl();
    mBossDirector = NULL;
    _110 = 0;
    _114 = 3;
    _118 = 0;
    mCameraTargetMtx = NULL;
    _180.x = 0.0f;
    _180.y = 0.0f;
    _180.z = 0.0f;
    mSceneFunc = NULL;
    mCurScene = NULL;
    mSceneNerve = NULL;
    _1A0 = 3;
    _1A4 = 4;
    mBossInfo = NULL;
    mGuardHolder = NULL;
    _1B0 = 0;
    _1B4 = -1;

    for (u32 i = 0; i < 0xD; i++) {
        mControllers[i] = NULL;
    }

    _D8.identity();
    _120.identity();
    _150.identity();

    mActorCameraInfo = NULL;
}

void SkeletalFishBoss::init(const JMapInfoIter& rIter) {
    initModelManagerWithAnm("SkeletalFishBoss", NULL, NULL, false);
    MR::initDefaultPos(this, rIter);
    MR::getJMapInfoArg0NoInit(rIter, &_1A0);
    MR::getJMapInfoArg1NoInit(rIter, &_1A4);
    initLevelStatus();
    mBossInfo->initWithoutIter();
    JMath::gekko_ps_copy12(&_150, MR::getZonePlacementMtx(rIter));
    initSwitch(rIter);
    initJoint();
    initHead();
    initScarFlash();
    initBreakModel();
    initCollision();
    initEffectKeeper(1, "SkeketalFishBoss", false);
    initSound(4, "SkeletalFishBoss", NULL, TVec3f(0.f));
    initNerve(&::SkeletalFishBossNrvSwim::sInstance, 0);
    MR::invalidateClipping(this);
    MR::setClippingTypeSphere(this, 1000.0f);
    mBossDirector = new SkeletalFishBossBattleDirector(this);
    MR::joinToGroupArray(this, rIter, NULL, 32);
    MR::connectToSceneCollisionEnemy(this);

    if (MR::tryRegisterDemoCast(this, rIter)) {
        MR::tryRegisterDemoCast(mBossHead, rIter);
    }

    MR::initLightCtrl(this);
    initShadow();
    initCamera(rIter);
    createGuards();
    MR::createNormalBloom();
    MR::addBaseMatrixFollowTarget(this, rIter, NULL, NULL);
    MR::declareCameraRegisterVec(this, 0, &mTranslation);
    MR::declarePowerStar(this);
    MR::declareStarPiece(this, 0x19);
    MR::createCenterScreenBlur();
    MR::startBck(this, "Swim", NULL);
    MR::startBck(mBossHead, "Wait", NULL);
    MR::startBrk(this, "Base");
    MR::startBrk(mBossHead, "Base");
    MR::startBva(this, "Normal");
    MR::startBva(mBossHead, "Normal");

    // Prevent the boss from killing you before it spawns lol
    invalidateCollision();
}

void SkeletalFishBoss::initAfterPlacement() {
    NameObj::initAfterPlacement();
    _110 = -1;
    powerUp();
    makeActorDead();
    calcPlanetCenterPosition();
    mBossDirector->initiate();
}

void SkeletalFishBoss::makeActorAppeared() {
    LiveActor::makeActorAppeared();
    validateCollision();
    _110 = -1;
    powerUp();
}

void SkeletalFishBoss::appear() {
    LiveActor::appear();
    MR::hideModel(this);
    MR::hideModel(mBossHead);
    setNerve(&::SkeletalFishBossNrvAppearWait::sInstance);
}

void SkeletalFishBoss::kill() {
    LiveActor::kill();
    invalidateCollision();
    mScarFlash->kill();
}

void SkeletalFishBoss::control() {

    if (MR::isEventCameraActive(mActorCameraInfo, "デモ終了後カメラ") && MR::isGreaterEqualStep(this, 60)) {
        MR::endGlobalEventCamera("デモ終了後カメラ", -1, true);
    }

    if (!isNerve(&::SkeletalFishBossNrvDead::sInstance) && !isNerve(&::SkeletalFishBossNrvAppearWait::sInstance)) {
        bool isInDemo = false;

        if (isNerve(&::SkeletalFishBossNrvAppearDemo::sInstance)
            || isNerve(&::SkeletalFishBossNrvPowerUpDemo::sInstance)
            || isNerve(&::SkeletalFishBossNrvDeadDemo::sInstance)) {
            isInDemo = true;
        }

        if (!isInDemo) {
            mRailControl->update();
            mRailControl->getPos(&mTranslation, 0.0f);
            updateCollision();
            mGuardHolder->movement();
            updateBgm();
        }

        mBossDirector->tryColumnCollision(mBossHead->getSensor("body"));
    }
}

void SkeletalFishBoss::calcAnim() {
    LiveActor::calcAnim();
    TVec3f zDir;
    MR::getCameraInvViewMtx()->getZDir(zDir);
    zDir.negate();

    if (!MR::isNoCalcAnim(this)) {
        SkeletalFishBoss::JointToShadow* shadow = &sShadowInfo;
        f32 idx_mult;
        f32 val = MR::getZero();
        idx_mult = 2607.5945f;
        JMath::TSinCosTable<14, f32>* tbl = &JMath::sSinCosTable;

        TPos3f jointMtx;
        JMath::gekko_ps_copy12(&jointMtx, MR::getJointMtx(this, shadow->mJointName));
        TVec3f trans;
        jointMtx.getTrans(trans);
        TVec3f gravity;
        MR::calcGravityVector(this, trans, &gravity, NULL, 0);
        f32 v4 = 1.8325958f;
        v4 = (v4 < val) ? -v4 : v4;

        f32 v10 = (v4 * idx_mult);
        f64 e = tbl->get(v10);

        if (zDir.dot(gravity) < e) {
            MR::invalidateShadow(this, shadow->mName);
        }
        else {
            MR::validateShadow(this, shadow->mName);
        }

        MR::setShadowDropPosition(this, shadow->mName, trans);
        MR::setShadowDropDirection(this, shadow->mName, gravity);
    }

    mBossHead->updateCollisionMtx();
}

bool SkeletalFishBoss::calcJoint(TPos3f* pJointPos, const JointControllerInfo& rInfo) {
    if (mJointIndicies[rInfo._4->_14] == -1) {
        return false;
    }

    SkeletalFishJointCalc::calcJoint(pJointPos, &_120, mRailControl, rInfo);
    return true;
}

void SkeletalFishBoss::damage(const HitSensor* pSensor, const TVec3f& rStarPieceOffs) {
    if (isEnableToBeDamaged()) {
        _D4 = pSensor;
        MR::shakeCameraStrong();
        playDamageBrk();

        u32 numHitsLeft = _114;
        _114--;

        if (_114 == 1 && _110 + 1 == _1A0) {
            //MR::emitEffect(this, "SkeltalDamageLast");
        }

        if (_114 <= 0) {
            if (_110 + 1 == _1A0) {
                setNerve(&::SkeletalFishBossNrvDeadDamage::sInstance);
            }
            else if (MR::isDemoExist("スカルシャークデモ")) {
                setNerve(&::SkeletalFishBossNrvDown::sInstance);
            }
        }
        else {
            u32 numStarPiece = (_110 + 1 == _1A0) ? 15 : 10;
            MR::appearStarPiece(this, rStarPieceOffs, numStarPiece, 10.0f, 14.0f, false);
            MR::startSound(this, "SE_OJ_STAR_PIECE_BURST_W_F", -1, -1);
            setNerve(&::SkeletalFishBossNrvDamage::sInstance);
        }
    }
}

void SkeletalFishBoss::exeSwim() {
    if (MR::isFirstStep(this)) {
        if (!MR::isBckPlaying(this, "Swim")) {
            MR::startBckWithInterpole(this, "Swim", 60);
            MR::startBckWithInterpole(mBossHead, "Wait", 60);
        }

        mGuardHolder->validate();
    }

    MR::startLevelSound(mBossHead, "SE_BM_LV_SKL_BOSS_SWIM_FAR", -1, -1, -1);
    MR::startLevelSound(mBossHead, "SE_BM_LV_SKL_BOSS_SWIM_NEAR", -1, -1, -1);
    TVec3f mouthPos;
    getMouthSensorCenterPos(mouthPos, 5000.0f);

    bool isClose = PSVECDistance((const Vec*)mouthPos, (const Vec*)MR::getPlayerPos()) < 5000.0f;

    if (isClose) {
        setNerve(&::SkeletalFishBossNrvOpen::sInstance);
    }
}

void SkeletalFishBoss::exeOpen() {
    if (MR::isFirstStep(this)) {
        MR::startBck(mBossHead, "Open", NULL);
        MR::startSound(mBossHead, "SE_BM_SKL_BOSS_MOUTH_OPEN", -1, -1);
    }

    MR::startLevelSound(mBossHead, "SE_BM_LV_SKL_BOSS_SWIM_FAR", -1, -1, -1);
    MR::startLevelSound(mBossHead, "SE_BM_LV_SKL_BOSS_SWIM_NEAR", -1, -1, -1);

    if (MR::isBckStopped(mBossHead)) {
        MR::startBck(mBossHead, "OpenWait", NULL);
        setNerve(&::SkeletalFishBossNrvOpenWait::sInstance);
    }
}

void SkeletalFishBoss::exeOpenWait() {
    TVec3f mouthPos;
    getMouthSensorCenterPos(mouthPos, 7800.0f);
    bool isClose = PSVECDistance((const Vec*)mouthPos, (const Vec*)MR::getPlayerPos()) < 7000.0f;

    if (isClose) {
        MR::startLevelSound(mBossHead, "SE_BM_LV_SKL_BOSS_SWIM_FAR", -1, -1, -1);
        MR::startLevelSound(mBossHead, "SE_BM_LV_SKL_BOSS_SWIM_NEAR", -1, -1, -1);
    }
    else {
        setNerve(&::SkeletalFishBossNrvClose::sInstance);
    }
}

void SkeletalFishBoss::exeClose() {
    if (MR::isFirstStep(this)) {
        MR::startBck(mBossHead, "Close", NULL);
        MR::startSound(mBossHead, "SE_BM_SKL_BOSS_MOUTH_CLOSE", -1, -1);
    }

    MR::startLevelSound(mBossHead, "SE_BM_LV_SKL_BOSS_SWIM_FAR", -1, -1, -1);
    MR::startLevelSound(mBossHead, "SE_BM_LV_SKL_BOSS_SWIM_NEAR", -1, -1, -1);

    if (MR::isBckStopped(mBossHead)) {
        MR::startBck(mBossHead, "Wait", NULL);
        setNerve(&::SkeletalFishBossNrvSwim::sInstance);
    }
}

void SkeletalFishBoss::exeBite() {
    if (MR::isFirstStep(this)) {
        MR::startBck(mBossHead, "Bite", NULL);
    }

    MR::startLevelSound(mBossHead, "SE_BM_LV_SKL_BOSS_SWIM_FAR", -1, -1, -1);
    MR::startLevelSound(mBossHead, "SE_BM_LV_SKL_BOSS_SWIM_NEAR", -1, -1, -1);

    if (MR::isBckStopped(mBossHead)) {
        MR::startBck(mBossHead, "Wait", NULL);
        setNerve(&::SkeletalFishBossNrvSwim::sInstance);
    }
}

void SkeletalFishBoss::exeDamage() {
    if (MR::isFirstStep(this)) {
        startDamageAnim();
        MR::startSound(mBossHead, "SE_BM_SKL_BOSS_DAMAGE_S", -1, -1);
        MR::startSystemSE("SE_SY_VS_BOSS_DAMAGE_1", -1, -1);
        mGuardHolder->invalidate();
    }

    MR::startLevelSound(mBossHead, "SE_BM_LV_SKL_BOSS_SWIM_FAR", -1, -1, -1);
    MR::startLevelSound(mBossHead, "SE_BM_LV_SKL_BOSS_SWIM_NEAR", -1, -1, -1);

    if (MR::isBckStopped(this)) {
        if (_114 == 2) {
            MR::startBrk(this, "Damage1Color");
            MR::startBrk(mBossHead, "Damage1Color");
        }
        else if (_114 == 1) {
            MR::startBrk(this, "Damage2Color");
            MR::startBrk(mBossHead, "Damage2Color");
        }

        setNerve(&::SkeletalFishBossNrvSwim::sInstance);
        MR::appearStarPiece(this, mTranslation, 6, 10.0f, 40.0f, false);
        MR::startSound(this, "SE_OJ_STAR_PIECE_BURST_W_F", -1, -1);
    }
}

void SkeletalFishBoss::exeDown() {
    if (MR::isFirstStep(this)) {
        startDamageAnim();
        MR::startSound(mBossHead, "SE_BM_SKL_BOSS_DAMAGE_L", -1, -1);
        MR::startSystemSE("SE_SY_BOSS_DAMAGE_3", -1, -1);
        mGuardHolder->invalidate();

        if (_110 == 1) {
            MR::startBva(this, "Crack");
            MR::startBva(mBossHead, "Crack");
        }
    }

    MR::startLevelSound(mBossHead, "SE_BM_LV_SKL_BOSS_SWIM_FAR", -1, -1, -1);
    MR::startLevelSound(mBossHead, "SE_BM_LV_SKL_BOSS_SWIM_NEAR", -1, -1, -1);

    if (MR::isStep(this, 120)) {
        stopScene("SkeletalFishBossPowerUp1", &::SkeletalFishBossNrvPowerUpDemo::sInstance, &SkeletalFishBoss::startPowerUpDemo);
    }
}

void SkeletalFishBoss::exeDeadDamage() {
    if (MR::isFirstStep(this)) {
        startDamageAnim();
        MR::startSound(mBossHead, "SE_BM_SKL_BOSS_DAMAGE_LAST", -1, -1);
        MR::startSystemSE("SE_SY_VS_BOSS_LAST_HIT", -1, -1);
        mGuardHolder->invalidate();
        mGuardHolder->killAll();
        mBossHead->_114 = 0;
    }

    MR::startLevelSound(mBossHead, "SE_BM_LV_SKL_BOSS_SWIM_FAR", -1, -1, -1);
    MR::startLevelSound(mBossHead, "SE_BM_LV_SKL_BOSS_SWIM_NEAR", -1, -1, -1);

    if (MR::isStep(this, 120)) {
        MR::stopStageBGM(30);
        stopScene("SkeletalFishBossDown", &::SkeletalFishBossNrvDeadDemo::sInstance, &SkeletalFishBoss::startDeadDemo);
    }
}

void SkeletalFishBoss::exeDead() {
    if (MR::isFirstStep(this)) {
        Mtx pos;
        if (MR::tryFindLinkNamePos(NULL, "マリオ再セット位置2", pos)) {
            MR::setPlayerBaseMtx(pos);
        }

        resetCamera();
    }
    else {
        bool isCameraActive = MR::isEventCameraActive(mActorCameraInfo, "デモ終了後カメラ") == false;
        if (isCameraActive) {
            kill();
        }
    }
}

void SkeletalFishBoss::exeAppearWait() {
    if (!MarioAccess::isInRush() && !MR::isEventCameraActive()) {
        stopScene("SkeletalFishBossBattleStart", &::SkeletalFishBossNrvAppearDemo::sInstance, &SkeletalFishBoss::startAppearDemo);
    }
}

void SkeletalFishBoss::exeAppearDemo() {
    if (MR::isFirstStep(this)) {
        MR::showModel(this);
        MR::showModel(mBossHead);
        MR::startBck(this, "BattleStart", NULL);
        MR::startBck(mBossHead, "BattleStart", NULL);
        MR::stopStageBGM(60);
        mBossDirector->appearBirdLouse();
        mGuardHolder->invalidate();
    }

    if (MR::isStep(this, 280)) {
        MR::startStageBGM("MBGM_SMG_BOSS_09A", false);
    }

    MR::startLevelSound(mBossHead, "SE_BM_LV_SKL_BOSS_SWIM_APPEAR", -1, -1, -1);

    if (MR::isBckStopped(this)) {
        if (!MR::isPlayingStageBgm()) {
            MR::startStageBGM("MBGM_SMG_BOSS_09A", false);
        }

        _1B4 = 120;
        mBossDirector->killBirdLouse();
        endAppearDemo();
    }
}

void SkeletalFishBoss::exePowerUpDemo() {
    if (MR::isFirstStep(this)) {
        MR::showModel(this);
        powerUp();
        mGuardHolder->invalidate();
        mGuardHolder->forceKillAll();
        mRailControl->_14 = 60.0f;

        if (_110 == 1) {
            MR::startBck(this, "PowerUp1", NULL);
            MR::startBck(mBossHead, "PowerUp1", NULL);
            mBossDirector->startPowerUpDemo1();
        }
        else {
            MR::startBck(this, "PowerUp2", NULL);
            MR::startBck(mBossHead, "PowerUp2", NULL);
            mBossDirector->startPowerUpDemo2();
        }

        mBossDirector->appearBirdLouse();

        if (_110 > 1) {
            MR::stopStageBGM(30);
        }
    }

    if (_110 == 2 && MR::isStep(this, 240)) {
        MR::startAtmosphereSE("SE_BM_SKL_GUARD_COME_FRONT", -1, -1);
    }

    MR::startLevelSound(mBossHead, "SE_BM_LV_SKL_BOSS_SWIM_POW_UP", -1, -1, -1);

    if (MR::isBckStopped(this)) {
        if (_110 > 1) {
            MR::startStageBGM("MBGM_SMG_BOSS_09B", false);
        }

        _1B4 = 120;
        mBossDirector->killBirdLouse();
        endPowerUpDemo();
    }
}

void SkeletalFishBoss::exeDeadDemo() {
    if (MR::isFirstStep(this)) {
        MR::startBck(this, "Down", NULL);
        MR::startBrk(this, "Down");
        MR::startBck(mBossHead, "Down", NULL);
        MR::startBrk(mBossHead, "Down");
        MR::startSound(mBossHead, "SE_BM_SKL_BOSS_MOUTH_OPEN", -1, -1);
        MR::setImageEffectControlAuto();
        mGuardHolder->invalidate();
        mBossDirector->appearBirdLouse();
    }

    if (MR::isStep(this, 30)) {
        MR::startSound(mBossHead, "SE_BM_SKL_BOSS_BODY_SOLID", -1, -1);
    }

    if (MR::isLessStep(this, 218)) {
        MR::startLevelSound(mBossHead, "SE_BM_LV_SKL_BOSS_SWIM_DIE", -1, -1, -1);
    }

    if (MR::isBckStopped(this)) {
        mBossDirector->killBirdLouse();
        MR::overlayWithPreviousScreen(2);
        setNerve(&::SkeletalFishBossNrvBreakDemo::sInstance);
    }
}

void SkeletalFishBoss::exeBreakDemo() {
    if (MR::isFirstStep(this)) {
        MR::hideModel(this);
        MR::hideModel(mBossHead);
        mBossHead->kill();
        mBreakModel->makeActorAppeared();
        MR::requestMovementOn(mBreakModel);
        MR::startBck(mBreakModel, "Break", NULL);
        MR::startBrk(mBreakModel, "Break");
        MR::startSound(mBossHead, "SE_BM_SKL_BOSS_EXPLODE", -1, -1);
        mGuardHolder->invalidate();
        mBossDirector->appearBirdLouse();
        MR::tryRumblePadMiddle(this, 0);
    }

    if (MR::isStep(this, 204)) {
        MR::tryRumblePadStrong(this, 0);
    }

    if (MR::isStep(this, 206)) {
        MR::startSound(mBossHead, "SE_BM_SKL_BOSS_EXPLODE_2", -1, -1);
    }

    if (MR::isStep(this, 0)) {
        MR::startCenterScreenBlur(60, 5.0f, 0x50, 5, 30);
    }

    if (MR::isStep(this, 200)) {
        MR::startCenterScreenBlur(60, 80.0f, 0x50, 5, 30);
    }

    if (MR::isBckStopped(mBreakModel)) {
        //MR::startAfterBossBGM(); //Not putting this here lol
        mBossDirector->killBirdLouse();
        endBreakDemo();
        setNerve(&::SkeletalFishBossNrvDead::sInstance);
    }
}

void SkeletalFishBoss::exeDemoWait() {
    if (MR::tryStartDemo(this, mCurScene)) {
        (this->*mSceneFunc)();
        MR::requestMovementOn(this);
        MR::requestMovementOn(mBossHead);
        MR::requestMovementOn(mScarFlash);
        mBossDirector->pauseOffCast();
        setNerve(mSceneNerve);
    }
}

void SkeletalFishBoss::notifyAttack(SkeletalFishGuard* pGuard) {
    mGuardHolder->notifyAttack(pGuard);
}

LiveActor* SkeletalFishBoss::getCurrentRail() {
    return mRailControl->_8;
}

f32 SkeletalFishBoss::getBodyThickness() const {
    return 400.0f;
}

SkeletalFishBossHead* SkeletalFishBoss::getHeadActor() const {
    return mBossHead;
}

void SkeletalFishBoss::calcAndSetBaseMtx() {
    bool isInDemo = false;

    if (isNerve(&::SkeletalFishBossNrvAppearDemo::sInstance) || isNerve(&::SkeletalFishBossNrvPowerUpDemo::sInstance) || isNerve(&::SkeletalFishBossNrvDeadDemo::sInstance)) {
        isInDemo = true;
    }

    if (isInDemo) {
        MR::setBaseTRMtx(this, _150.toMtxPtr());
    }
    else {
        for (u32 i = 0; i < 0xD; i++) {
            mControllers[i]->registerCallBack();
        }

        TPos3f railMtx;
        mRailControl->getMtx(&railMtx, 0.0f);
        MR::setBaseTRMtx(this, railMtx.toMtxPtr());
        JMath::gekko_ps_copy12(&_120, &railMtx);
        _120.invert(_120);

        if (_D4) {
            JMath::gekko_ps_copy12(&_D8, &railMtx);
            _D8.mMtx[0][3] = _D4->mPosition.x;
            _D8.mMtx[1][3] = _D4->mPosition.y;
            _D8.mMtx[2][3] = _D4->mPosition.z;
        }
    }
}

void SkeletalFishBoss::initLevelStatus() {
    mBossInfo = new SkeletalFishBossInfo(this, _1A0, _1A4, "ボス戦パラメータ");
}

void SkeletalFishBoss::initJoint() {
    mJointIndicies = new s32[MR::getJointNum(this)];

    for (u32 i = 0; i < MR::getJointNum(this); i++) {
        mJointIndicies[i] = -1;
    }

    for (s32 i = 0; i < 0xD; i++) {
        char buf[16];
        snprintf(buf, sizeof(buf), "Joint%02d", i);
        mControllers[i] = MR::createJointDelegatorWithNullChildFunc(this, &SkeletalFishBoss::calcJoint, buf);
        mJointIndicies[MR::getJointIndex(this, buf)] = i;
    }
}

void SkeletalFishBoss::initCollision() {
    mPartsArray = new CollisionParts * [0xE];

    for (u32 i = 0; i < 0xE; i++) {
        mPartsArray[i] = MR::createCollisionPartsFromLiveActor(this, sColInfo[i].mColliderName, mBossHead->getSensor("body"), MR::getJointMtx(this, sColInfo[i].mJointName), MR::CollisionScaleType_Default);
        MR::validateCollisionParts(mPartsArray[i]);
    }
}

void SkeletalFishBoss::initHead() {
    mBossHead = new SkeletalFishBossHead(this);
    mBossHead->initWithoutIter();
    _D4 = NULL;
}

void SkeletalFishBoss::initScarFlash() {
    mScarFlash = new SkeletalFishBossScarFlash(this);
    mScarFlash->initWithoutIter();
}

void SkeletalFishBoss::initBreakModel() {
    mBreakModel = new ModelObj("壊れモデル", "SkeletalFishBossBreak", _150.mMtx, MR::DrawBufferType_Enemy, MR::CategoryList_Auto, MR::CategoryList_Auto, false);
    mBreakModel->initWithoutIter();
    MR::initLightCtrl(mBreakModel);
    mBreakModel->makeActorDead();
    MR::invalidateClipping(mBreakModel);
}

void SkeletalFishBoss::initSwitch(const JMapInfoIter& rIter) {
    if (MR::useStageSwitchReadAppear(this, rIter)) {
        MR::syncStageSwitchAppear(this);
    }
    else if (MR::useStageSwitchReadA(this, rIter)) {
        MR::listenStageSwitchOnA(this, MR::FunctorV0M<SkeletalFishBoss*, void (SkeletalFishBoss::*)(void)>(this, &SkeletalFishBoss::appear));
    }
}

void SkeletalFishBoss::createGuards() {
    mGuardHolder = new SkeletalFishGuardHolder(this, _1A4, "スカルシャークガード管理");
}

void SkeletalFishBoss::initShadow() {
    initShadowControllerList(1);
    SkeletalFishBoss::JointToShadow& shadow = testInline();
    const char** bruh = (const char**)test;
    const char* name = bruh[0];
    MtxPtr mtx = MR::getJointMtx(this, bruh[1]);
    //SkeletalFishBoss::JointToShadow* shadow = &sShadowInfo;
    MR::addShadowVolumeFlatModel(this, name, test[2], mtx);
    MR::setShadowDropLength(this, name, 4000.0f);
    MR::excludeCalcShadowToSensorAll(this, mBossHead->getSensor("body"));
}

void SkeletalFishBoss::initCamera(const JMapInfoIter& rIter) {
    mActorCameraInfo = new ActorCameraInfo(rIter);
    const char* fileName = "SkeletalFishBossBattleStart";
    MR::initAnimCamera(this, mActorCameraInfo, fileName); //MR::declareEventCameraAnim(&cameraInfo, "スカルシャーク出現", MR::getResourceHolder(this)->mResourceTable->getRes(fileName));
    fileName = "SkeletalFishBossPowerUp1";
    MR::initAnimCamera(this, mActorCameraInfo, fileName); //MR::declareEventCameraAnim(&cameraInfo, "スカルシャークパワーアップ", MR::getResourceHolder(this)->mResourceTable->getRes(fileName));
    fileName = "SkeletalFishBossPowerUp2";
    MR::initAnimCamera(this, mActorCameraInfo, fileName); //MR::declareEventCameraAnim(&cameraInfo, "スカルシャークパワーアップ２", MR::getResourceHolder(this)->mResourceTable->getRes(fileName));
    fileName = "SkeletalFishBossDown";
    MR::initAnimCamera(this, mActorCameraInfo, fileName); //MR::declareEventCameraAnim(&cameraInfo, "スカルシャーク死亡", MR::getResourceHolder(this)->mResourceTable->getRes(fileName));
    MR::declareEventCameraProgrammable("デモ終了後カメラ");
    mCameraTargetMtx = new CameraTargetMtx("カメラターゲットダミー");
    mCameraTargetMtx->setMtx((MtxPtr)&_150);
    //JMath::gekko_ps_copy12(&mCameraTargetMtx->mMatrix, &_150);
}

void SkeletalFishBoss::validateCollision() {
    for (u32 i = 0; i < 0xE; i++) {
        MR::validateCollisionParts(mPartsArray[i]);
    }

    SkeletalFishBossHead* head = getHeadActor();
    MR::validateCollisionParts(head->_CC);
    MR::validateCollisionParts(head->_100);
}

void SkeletalFishBoss::invalidateCollision() {
    for (u32 i = 0; i < 0xE; i++) {
        MR::invalidateCollisionParts(mPartsArray[i]);
    }

    SkeletalFishBossHead* head = getHeadActor();
    MR::invalidateCollisionParts(head->_CC);
    MR::invalidateCollisionParts(head->_100);
}

void SkeletalFishBoss::getMouthSensorCenterPos(TVec3f& rPos, f32 a2) const {
    TMtx34f mtx;
    JMath::gekko_ps_copy12(&mtx, MR::getJointMtx(this, "Head"));
    rPos.z = a2;
    rPos.x = 0.0f;
    rPos.y = 0.0f;
    mtx.mult(rPos, rPos);
}

void SkeletalFishBoss::updateCollision() {
    for (u32 i = 0; i < 0xE; i++) {
        mPartsArray[i]->setMtx();
    }

    SkeletalFishBossHead* head = getHeadActor();
    head->_CC->setMtx();
    head->_100->setMtx();
}

void SkeletalFishBoss::powerUp() {
    _110++;
    _114 = mBossInfo->getLevelStatus(_110)->mEnergyLevel;
    resetRail();
}

void SkeletalFishBoss::startDamageAnim() {
    MR::startBck(this, "Damage", NULL);
    MR::startBck(mBossHead, "Damage", NULL);
    mScarFlash->appear();
}

void SkeletalFishBoss::calcPlanetCenterPosition() {
    TVec3f firstPnt;
    MR::calcRailPointPos(&firstPnt, MR::getSkeletalFishBossRailHolder()->getInOrder(0), 0);
    TVec3f secondPnt;
    MR::calcRailPointPos(&secondPnt, MR::getSkeletalFishBossRailHolder()->getInOrder(0), 1);
    calcGravityCenter(&_180, firstPnt, secondPnt);
}

bool SkeletalFishBoss::calcGravityCenter(TVec3f* pOut, const TVec3f& rFirstPoint, const TVec3f& rSecondPoint) {
    TVec3f firstGravityCenter(rFirstPoint);
    TVec3f firstGravityVector;
    MR::calcGravityVector(this, firstGravityCenter, &firstGravityVector, NULL, 0);
    firstGravityVector.negate();

    TVec3f secondGravityCenter(rSecondPoint);
    TVec3f secondGravityVector;
    MR::calcGravityVector(this, secondGravityCenter, &secondGravityVector, NULL, 0);
    secondGravityVector.negate();

    f32 firstGravitySquared = firstGravityVector.squared();
    f32 secondGravitySquared = secondGravityVector.squared();
    f32 dotProduct = firstGravityVector.dot(secondGravityVector);
    f32 dotProductSquared = dotProduct * dotProduct;

    TVec3f distanceVector(firstGravityCenter - secondGravityCenter);
    TVec3f reversedDistanceVector(secondGravityCenter - firstGravityCenter);
    f32 dotFirst = distanceVector.dot(secondGravityVector);
    f32 dotSecond = reversedDistanceVector.dot(firstGravityVector);

    if (((firstGravitySquared * secondGravitySquared) - dotProductSquared) < 0.001f) {
        return false;
    }

    f32 scaleFactor = ((1.0f / ((firstGravitySquared * secondGravitySquared) - dotProductSquared)) * ((secondGravitySquared * dotSecond) + (dotProduct * dotFirst)));
    pOut->set(firstGravityCenter + MR::createVecAndScale(firstGravityVector, scaleFactor));
    return true;
}

void SkeletalFishBoss::resetRail() {
    SkeletalFishBossInfo::LevelStatus* lvl = mBossInfo->getLevelStatus(_110);
    SkeletalFishBossRail* rail = MR::getSkeletalFishBossRailHolder()->getByID(lvl->mRailIDLevel);
    mRailControl->setRailActor(rail, NULL, true);
    mRailControl->_14 = mBossInfo->getLevelStatus(_110)->mSpeedLevel;
}

void SkeletalFishBoss::updateBgm() {
    s32 curType = _1B4;
    if (curType < 0 || (_1B4 = curType - 1, curType - 1 < 0)) {
        _1B4 = -1;
        s32 bgType, v4;

        if (MR::calcDistanceToPlayer(this->mTranslation) <= 4200.0f) {

            bgType = 2;
            v4 = 60;
        }
        else {
            bgType = 1;
            v4 = 120;
        }

        if (bgType != _1B0) {
            MR::setStageBGMState(bgType, v4);
        }

        _1B0 = bgType;
    }
}

LiveActor* SkeletalFishBoss::getCurrentBossRail() {
    return mRailControl->_8;
}

void SkeletalFishBoss::stopScene(const char* pName, const Nerve* pNerve, SceneFunc func) {
    setNerve(&::SkeletalFishBossNrvDemoWait::sInstance);
    mSceneFunc = func;
    mCurScene = pName;
    mSceneNerve = pNerve;
}

void SkeletalFishBoss::startCamera(const char* pCameraName) {
    CameraTargetArg target(mCameraTargetMtx);
    MR::startAnimCameraTargetOther(this, mActorCameraInfo, pCameraName, target, 0, false, 1.0f);
}

void SkeletalFishBoss::resetCamera() {
    /*MR::startGlobalEventCameraTargetPlayer("デモ終了後カメラ", 0);
    TPos3f mtxPos;
    JMath::gekko_ps_copy12(&mtxPos, MR::getPlayerBaseMtx());
    TVec3f pos;
    mtxPos.getTrans(pos);
    TVec3f stack_2C;
    f32 z = mtxPos.mMtx[2][1];
    f32 y = mtxPos.mMtx[1][1];
    f32 x = mtxPos.mMtx[0][1];
    stack_2C.set(x, y, z);
    TVec3f stack_20;
    TVec3f stack_14 = pos + MR::createVecAndScale(stack_2C, 1200.0f);
    stack_2C.set(stack_14);
    mtxPos.getZDir(stack_20);
    MR::setProgrammableCameraParam("デモ終了後カメラ", pos, stack_20, stack_2C, true);*/
}

void SkeletalFishBoss::playDamageBrk() {
    char buf[0x80];
    snprintf(buf, sizeof(buf), "Damage%dColor", _118);
    MR::startBrk(this, buf);
    MR::startBrk(mBossHead, buf);
    _118 += 1;
}

void SkeletalFishBoss::startAppearDemo() {
    MR::requestMovementOn(MR::getSceneObjHolder()->getObj(SCENE_OBJ_SENSOR_HIT_CHECKER));
    Mtx namePosMtx;
    if (MR::tryFindLinkNamePos(NULL, "マリオ再セット位置1", namePosMtx)) {
        MR::setPlayerBaseMtx(namePosMtx);
    }

    MR::hidePlayer();
    mBossHead->_114 = 1;
    startCamera("SkeletalFishBossBattleStart");

    if (!MR::isNormalBloomOn()) {
        MR::turnOnNormalBloom();
        MR::setNormalBloomIntensity(0xD5);
        MR::setNormalBloomThreshold(0);
        MR::setNormalBloomBlurIntensity1(0x2A);
        MR::setNormalBloomBlurIntensity2(0x15);
    }
}

void SkeletalFishBoss::endAppearDemo() {
    Mtx namePosMtx;
    if (MR::tryFindLinkNamePos(NULL, "マリオ再セット位置1", namePosMtx)) {
        MR::setPlayerBaseMtx(namePosMtx);
    }

    MR::showPlayer();
    const char* cameraName = "SkeletalFishBossBattleStart";
    
    MR::endAnimCamera(this, mActorCameraInfo, cameraName, 0, false);
    resetCamera();

    if (cameraName != NULL) {
        MR::endDemo(this, cameraName);
    }

    MR::startBck(this, "Swim", NULL);
    MR::startBck(mBossHead, "Wait", NULL);
    resetRail();
    setNerve(&::SkeletalFishBossNrvSwim::sInstance);
}

void SkeletalFishBoss::startPowerUpDemo() {
    MR::requestMovementOn(MR::getSceneObjHolder()->getObj(SCENE_OBJ_SENSOR_HIT_CHECKER));
    MR::overlayWithPreviousScreen(2);
    MR::hidePlayer();

    if (!_110) {
        startCamera("SkeletalFishBossPowerUp1");
    }
    else {
        startCamera("SkeletalFishBossPowerUp2");
    }
}

void SkeletalFishBoss::endPowerUpDemo() {
    Mtx namePosMtx;
    if (MR::tryFindLinkNamePos(NULL, "マリオ再セット位置2", namePosMtx)) {
        MR::setPlayerBaseMtx(namePosMtx);
    }

    MR::showPlayer();

    const char* eventCameraName;

    if (_110 == 1) {
        eventCameraName = "SkeletalFishBossPowerUp1";
        MR::endAnimCamera(this, mActorCameraInfo, eventCameraName, 0, false);
        resetCamera();
        mBossDirector->endPowerUpDemo1();
    }
    else {
        eventCameraName = "SkeletalFishBossPowerUp2";
        MR::endAnimCamera(this, mActorCameraInfo, eventCameraName, 0, false);
        resetCamera();
        mBossDirector->endPowerUpDemo2();
    }

    // alright
    if ("SkeletalFishBossPowerUp1" != NULL) {
        MR::endDemo(this, "SkeletalFishBossPowerUp1");
    }

    resetRail();
    MR::startBck(this, "Swim", NULL);
    MR::startBck(mBossHead, "Wait", 0);
    mGuardHolder->forceAppearAll();
    setNerve(&::SkeletalFishBossNrvSwim::sInstance);
}

void SkeletalFishBoss::startDeadDemo() {
    MR::overlayWithPreviousScreen(2);
    MR::hidePlayer();
    startCamera("SkeletalFishBossDown");
}

void SkeletalFishBoss::endBreakDemo() {
    const char* cameraName = "SkeletalFishBossDown";
    MR::endAnimCamera(this, mActorCameraInfo, cameraName, 0, false);
    resetCamera();

    if (cameraName != NULL) {
        MR::endDemo(this, cameraName);
    }

    MR::showPlayer();
    mBreakModel->makeActorDead();
    MR::hideModel(this);
    MR::hideModel(mBossHead);
    TVec3f offset(0.0f, 1573.0f, 0.0f);
    _150.mult(offset, offset);
    MR::requestAppearPowerStar(this, offset);
}

bool SkeletalFishBoss::isEnableToBeDamaged() const {
    bool isDmg = false;

    if (!isNerve(&::SkeletalFishBossNrvDamage::sInstance)
        && !isNerve(&::SkeletalFishBossNrvDown::sInstance)
        && !isNerve(&::SkeletalFishBossNrvDeadDamage::sInstance)
        && !isNerve(&::SkeletalFishBossNrvDead::sInstance)
        && !isNerve(&::SkeletalFishBossNrvDemoWait::sInstance)
        && !isNerve(&::SkeletalFishBossNrvAppearDemo::sInstance)
        && !isNerve(&::SkeletalFishBossNrvPowerUpDemo::sInstance)
        && !isNerve(&::SkeletalFishBossNrvDeadDemo::sInstance)) {
        isDmg = true;
    }

    return isDmg;
}

SkeletalFishBossHead::SkeletalFishBossHead(LiveActor* pActor) : PartsModel(pActor, "スカルシャーク頭", "SkeletalFishBossHeadA", NULL, MR::DrawBufferType_Enemy, false) {
    initFixedPosition("Head");
    initHitSensor(17);
    MR::addHitSensorAtJointEnemy(this, "body", "Head", 8, 400.0f, TVec3f(0.0f, -120.0f, 320.0f));

    for (u32 i = 0; i < 0xD; i++) {
        //SkeletalFishBoss::HitPos* data = &sHitPosData[i];
        TVec3f offset(sHitPosData[i].mOffset);
        //offset.set((TVec3f)data->mOffset);
        offset.scale(9.0f / 10.0f);
        MR::addHitSensorAtJointEnemyAttack(this, sHitPosData[i].mName, sHitPosData[i].mSensorName, 8, 270.0f, offset);
    }

    HitSensor* senBody = getSensor("body");
    MR::initLightCtrl(this);
    _9C.identity();
    _CC = MR::createCollisionPartsFromLiveActor(this, "Head", senBody, MR::getJointMtx(this, "Head"), MR::CollisionScaleType_Default);
    MR::validateCollisionParts(_CC);
    _D0.identity();
    _100 = MR::createCollisionPartsFromLiveActor(this, "Jow", senBody, MR::getJointMtx(this, "Jow"), MR::CollisionScaleType_Default);
    MR::validateCollisionParts(_100);
    MR::addToAttributeGroupSearchTurtle(this);
    createSubModel();

    // Fix the Kingfin headlight glitch
    _114 = 0;
    for (s32 i = 0; i < 2; i++) {
        MR::disconnectToDrawTemporarily(mLightModels[i]);
        MR::disconnectToDrawTemporarily(mBloomModels[i]);
    }
}

void SkeletalFishBossHead::attackSensor(HitSensor* a1, HitSensor* a2) {
    if (MR::isEqualSensor(a1, this, "body")) {
        LiveActor* host = mHost;
        bool curFlag = false;

        if (host->isNerve(&::SkeletalFishBossNrvDeadDamage::sInstance)
            || host->isNerve(&::SkeletalFishBossNrvDead::sInstance)) {
            curFlag = true;
        }

        if (!curFlag) {
            if (MR::isSensorPlayer(a2)) {
                curFlag = false;

                if (host->isNerve(&::SkeletalFishBossNrvAppearDemo::sInstance)
                    || host->isNerve(&::SkeletalFishBossNrvPowerUpDemo::sInstance)
                    || host->isNerve(&::SkeletalFishBossNrvDeadDemo::sInstance)) {
                    curFlag = true;
                }

                if (!curFlag) {
                    if (host->isNerve(&::SkeletalFishBossNrvDamage::sInstance)) {
                        MR::sendMsgPush(a2, a1);
                    }
                    else {
                        MR::sendMsgPush(a2, a1);

                        if (host->isNerve(&::SkeletalFishBossNrvOpenWait::sInstance)) {
                            host->setNerve(&::SkeletalFishBossNrvBite::sInstance);
                        }
                    }
                }
            }
            else if (!MR::isSensorEnemyAttack(a1) && MR::isSensorEnemy(a2)) {
                MR::sendMsgEnemyAttack(a2, a1);
            }
        }
    }
}

void SkeletalFishBossHead::movement() {
    PartsModel::movement();

    if (_114) {
        if (MR::isConnectToDrawTemporarily(this) && MR::isHiddenModel(this)) {
            if (MR::isConnectToDrawTemporarily(mLightModels[0])) {
                for (s32 i = 0; i < 2; i++) {
                    MR::disconnectToDrawTemporarily(mLightModels[i]);
                    MR::disconnectToDrawTemporarily(mBloomModels[i]);
                }
            }
        }
        else if (!MR::isConnectToDrawTemporarily(mLightModels[0])) {
            for (s32 i = 0; i < 2; i++) {
                MR::connectToDrawTemporarily(mLightModels[i]);
                MR::connectToDrawTemporarily(mBloomModels[i]);
            }
        }
    }
    else if (MR::isConnectToDrawTemporarily(mLightModels[0])) {
        for (s32 i = 0; i < 2; i++) {
            MR::disconnectToDrawTemporarily(mLightModels[i]);
            MR::disconnectToDrawTemporarily(mBloomModels[i]);
        }
    }
}

void SkeletalFishBossHead::calcAnim() {
    PartsModel::calcAnim();

    if (MR::isConnectToDrawTemporarily(this)) {
        for (s32 i = 0; i < 2; i++) {
            mLightModels[i]->calcAnim();
            mBloomModels[i]->calcAnim();
        }
    }
}

bool SkeletalFishBossHead::receiveMsgPlayerAttack(u32 msg, HitSensor* a2, HitSensor* a3) {
    if (MR::isMsgJetTurtleAttack(msg)) {
        SkeletalFishBoss* boss = (SkeletalFishBoss*)mHost;
        boss->damage(a3, a2->mPosition);
        return true;
    }

    return MR::isMsgStarPieceReflect(msg);
}

void SkeletalFishBossHead::updateCollisionMtx() {
    JMath::gekko_ps_copy12(&_9C, MR::getJointMtx(this, "Head"));
    JMath::gekko_ps_copy12(&_D0, MR::getJointMtx(this, "Jow"));
}

void SkeletalFishBossHead::createSubModel() {
    const char* coneMdls[2] = { "LightConeLeft", "LightConeRight" };

    for (s32 i = 0; i < 2; i++) {
        mLightModels[i] = new ModelObj("眼光", "SkeletalFishBossLight", MR::getJointMtx(this, coneMdls[i]), MR::DrawBufferType_Enemy, MR::CategoryList_Null, MR::CategoryList_Null, false);
        mLightModels[i]->initWithoutIter();
        MR::invalidateClipping(mLightModels[i]);

        mBloomModels[i] = new ModelObj("眼光ブルーム", "SkeletalFishBossLightBloom", MR::getJointMtx(this, coneMdls[i]), MR::DrawBufferType_Bloom, MR::CategoryList_Null, MR::CategoryList_Null, false);
        mBloomModels[i]->initWithoutIter();
        MR::invalidateClipping(mBloomModels[i]);
    }
}

SkeletalFishBossScarFlash::SkeletalFishBossScarFlash(LiveActor* pActor) : PartsModel(pActor, "スカルシャーク傷跡エフェクトモデル", "SkeletalFishBossScarFlash", NULL, MR::DrawBufferType_Enemy, false) {
    initFixedPosition("Head");
}

void SkeletalFishBossScarFlash::init(const JMapInfoIter& rIter) {
    PartsModel::init(rIter);
    makeActorDead();
}

void SkeletalFishBossScarFlash::appear() {
    LiveActor::appear();
    MR::startBck(this, "Damage", NULL);
    MR::startBrk(this, "Damage");
}

void SkeletalFishBossScarFlash::control() {
    if (MR::isBckStopped(this)) {
        kill();
    }
}

namespace {
    void SkeletalFishBossNrvSwim::execute(Spine* pSpine) const {
        ((SkeletalFishBoss*)pSpine->mExecutor)->exeSwim();
    }
    void SkeletalFishBossNrvOpen::execute(Spine* pSpine) const {
        ((SkeletalFishBoss*)pSpine->mExecutor)->exeOpen();
    }
    void SkeletalFishBossNrvOpenWait::execute(Spine* pSpine) const {
        ((SkeletalFishBoss*)pSpine->mExecutor)->exeOpenWait();
    }
    void SkeletalFishBossNrvClose::execute(Spine* pSpine) const {
        ((SkeletalFishBoss*)pSpine->mExecutor)->exeClose();
    }
    void SkeletalFishBossNrvBite::execute(Spine* pSpine) const {
        ((SkeletalFishBoss*)pSpine->mExecutor)->exeBite();
    }
    void SkeletalFishBossNrvDamage::execute(Spine* pSpine) const {
        ((SkeletalFishBoss*)pSpine->mExecutor)->exeDamage();
    }
    void SkeletalFishBossNrvDown::execute(Spine* pSpine) const {
        ((SkeletalFishBoss*)pSpine->mExecutor)->exeDown();
    }
    void SkeletalFishBossNrvDeadDamage::execute(Spine* pSpine) const {
        ((SkeletalFishBoss*)pSpine->mExecutor)->exeDeadDamage();
    }
    void SkeletalFishBossNrvDead::execute(Spine* pSpine) const {
        ((SkeletalFishBoss*)pSpine->mExecutor)->exeDead();
    }
    void SkeletalFishBossNrvAppearWait::execute(Spine* pSpine) const {
        ((SkeletalFishBoss*)pSpine->mExecutor)->exeAppearWait();
    }
    void SkeletalFishBossNrvAppearDemo::execute(Spine* pSpine) const {
        ((SkeletalFishBoss*)pSpine->mExecutor)->exeAppearDemo();
    }
    void SkeletalFishBossNrvPowerUpDemo::execute(Spine* pSpine) const {
        ((SkeletalFishBoss*)pSpine->mExecutor)->exePowerUpDemo();
    }
    void SkeletalFishBossNrvDeadDemo::execute(Spine* pSpine) const {
        ((SkeletalFishBoss*)pSpine->mExecutor)->exeDeadDemo();
    }
    void SkeletalFishBossNrvBreakDemo::execute(Spine* pSpine) const {
        ((SkeletalFishBoss*)pSpine->mExecutor)->exeBreakDemo();
    }
    void SkeletalFishBossNrvDemoWait::execute(Spine* pSpine) const {
        ((SkeletalFishBoss*)pSpine->mExecutor)->exeDemoWait();
    }

    SkeletalFishBossNrvSwim(SkeletalFishBossNrvSwim::sInstance);
    SkeletalFishBossNrvOpen(SkeletalFishBossNrvOpen::sInstance);
    SkeletalFishBossNrvOpenWait(SkeletalFishBossNrvOpenWait::sInstance);
    SkeletalFishBossNrvClose(SkeletalFishBossNrvClose::sInstance);
    SkeletalFishBossNrvBite(SkeletalFishBossNrvBite::sInstance);
    SkeletalFishBossNrvDamage(SkeletalFishBossNrvDamage::sInstance);
    SkeletalFishBossNrvDown(SkeletalFishBossNrvDown::sInstance);
    SkeletalFishBossNrvDeadDamage(SkeletalFishBossNrvDeadDamage::sInstance);
    SkeletalFishBossNrvDead(SkeletalFishBossNrvDead::sInstance);
    SkeletalFishBossNrvAppearWait(SkeletalFishBossNrvAppearWait::sInstance);
    SkeletalFishBossNrvAppearDemo(SkeletalFishBossNrvAppearDemo::sInstance);
    SkeletalFishBossNrvPowerUpDemo(SkeletalFishBossNrvPowerUpDemo::sInstance);
    SkeletalFishBossNrvDeadDemo(SkeletalFishBossNrvDeadDemo::sInstance);
    SkeletalFishBossNrvBreakDemo(SkeletalFishBossNrvBreakDemo::sInstance);
    SkeletalFishBossNrvDemoWait(SkeletalFishBossNrvDemoWait::sInstance);
};

SkeletalFishBoss::~SkeletalFishBoss() {

}

SkeletalFishBossHead::~SkeletalFishBossHead() {

}

SkeletalFishBossScarFlash::~SkeletalFishBossScarFlash() {

}