#include "Game/Boss/SkeletalFishGuard.h"
#include "Game/Boss/SkeletalFishBoss.h"
#include "Game/Map/WaterInfo.h"
#include "JSystem/JMath.h"

#include "Game/Util/ActorAnimUtil.h"
#include "Game/Util/ActorMovementUtil.h"
#include "Game/Util/ActorSensorUtil.h"
#include "Game/Util/ActorShadowUtil.h"
#include "Game/Util/AreaObjUtil.h"
#include "Game/Util/CameraUtil.h"
#include "Game/Util/GravityUtil.h"
#include "Game/Util/JointUtil.h"
#include "Game/Util/LiveActorUtil.h"
#include "Game/Util/MathUtil.h"
#include "Game/Util/ObjUtil.h"
#include "Game/Util/PlayerUtil.h"
#include "Game/Util/RailUtil.h"
#include "Game/Util/SoundUtil.h"
#include "Game/Util/StarPointerUtil.h"
#include "Game/Util/MapUtil.h"
#include "JSystem/JMATrigonometric.h"

SkeletalFishGuard::SkeletalFishGuard(SkeletalFishBoss* pOwner, const char* pName) : LiveActor(pName)
{
    _8C = 0;
    mFishBoss = pOwner;
    _94.zero();
    _A0 = 0;
    _A4.zero();
    _B0 = 0.f;
    _B4 = 0.f;
    _B8 = 0.f;
    _BC = 0.f;
    mAttackDelay = 0;                               // _C0
    _C4 = 0.f;
    _C8 = 0.f;
    _CC = NULL;
    _D0.zero();
    _DC.zero();
    _E8.zero();
    _F4.zero();
    _100.x = 0.f;
    _100.y = 0.f;
    _100.z = 1.f;
    _10C.zero();
    mScaleController = new AnimScaleController(NULL);
}

void SkeletalFishGuard::init(const JMapInfoIter& rIter) {
    initModelManagerWithAnm("SkeletalFishGuard", NULL, NULL, false);
    MR::addToAttributeGroupSearchTurtle(this);
    MR::connectToSceneEnemy(this);
    initNerve(&::SkeletalFishGuardNrvWait::sInstance, 0);
    initSensor();
    MR::initLightCtrl(this);
    MR::initShadowVolumeFlatModel(this, "SkeletalFishGuardShadow", getBaseMtx());
    initEffectKeeper(1, NULL, false);
    MR::addEffect(this, "PointerTouchManual");
    MR::setEffectHostMtx(this, "PointerTouchManual", MR::getJointMtx(this, "Head"));
    initSound(6, "SkeletalFishGuard", NULL, TVec3f(0.f));
    initBinder(120.0f, 0.0f, 0);
    MR::invalidateClipping(this);
    MR::setBinderExceptActor(this, mFishBoss->getHeadActor());
    MR::initStarPointerTargetAtJoint(this, "Joint00", 170.0f, TVec3f(0.0f, 0.0f, 0.0f));
    MR::declareStarPiece(this, 128);
}

void SkeletalFishGuard::initAfterPlacement() {
    calcTransAndFront();
    makeActorDead();
}

void SkeletalFishGuard::appear() {
    LiveActor::appear();
    MR::validateShadow(this, NULL);
}

void SkeletalFishGuard::control() {
    mScaleController->updateNerve();

    if (mAttackDelay > 0) {
        mAttackDelay--;
    }

    if (!isNerve(&::SkeletalFishGuardNrvKill::sInstance)
        && !isNerve(&::SkeletalFishGuardNrvApart::sInstance)) {

        if (MR::isBinded(this)) {
            MR::isBindedGround(this);
            MR::isBindedWall(this);
            MR::isBindedRoof(this);
            setNerve(&::SkeletalFishGuardNrvKill::sInstance);
        }
    }
}

/* the stack on this function is not correct
    but the layers of inlines are extremely complex
*/
void SkeletalFishGuard::exeAppear() {
    TVec3f v32;
    if (MR::isFirstStep(this)) {
        MR::showModel(this);
        MR::startBck(this, "Swim", NULL);
        MR::startBrk(this, "Base");
        MR::startSound(this, "SE_BM_SKL_GUARD_APPEAR", -1, -1);
        MR::validateHitSensors(this);
        MR::clearHitSensors(this);
        MR::offBind(this);
        _C8 = 0.0f;
        TVec3f v27;
        JGeometry::negateInternal((f32*)&mGravity, (f32*)&v27);
        _D0.set(v27);
        _10C.set(mTranslation);
        _DC.set(MR::getRandom(-1.0f, 1.0f), MR::getRandom(-1.0f, 1.0f), MR::getRandom(-1.0f, 1.0f));
    }


    MR::calcGravityVector(this, _10C, &v32, NULL, false);
    TVec3f v31 = _10C - v32 * 1000.0f;
    s32 v5 = 300 - getNerveStep();
    calcTarget(&_E8, &_F4, &_100, v5);
    TVec3f v30 = _DC;
    f32 v6 = v32.dot(v30);
    //JMAVECScaleAdd(v32.toCVec(), v30.toCVec(), v30.toVec(), -v6);
    MR::normalizeOrZero(&v30);
    v31 += v30 * 500.0f;
    v30 += TVec3f(0.0f, 1.0f, 0.0f);
    MR::normalize(&v30);
    TVec3f v29 = mTranslation;

    if (getNerveStep() < 99) {
        s32 v7 = getNerveStep();
        s32 max = 0x63;
        f32 scaled = v7 / (f32)max;
        TVec3f temp_vec = TVec3f(0.0f, 1.0f, 0.0f) * 0.0f * (f32)max;
        TVec3f temp_vec2 = v30 * 10.0f * (f32)max;
        mTranslation.cubic(_10C, temp_vec, temp_vec2, v31, scaled);
    }
    else {
        f32 v9 = (getNerveStep() - 0x63);
        s32 max = 0xC9;
        f32 scaled = v9 / (f32)max;
        f32 v11 = MR::getRailCoordSpeed(mFishBoss->getCurrentBossRail());
        TVec3f temp_vec = v30 * 10.0f * (f32)max;
        TVec3f temp_vec2 = _100 * v11 * (f32)max;
        mTranslation.cubic(v31, temp_vec, temp_vec2, _F4, scaled);
    }

    TVec3f v28 = mTranslation - v29;

    if (!MR::isNearZero(v28, 0.001f)) {
        _D0.set(v28);
        MR::normalize(&_D0);
    }

    MR::calcGravity(this);
    MR::startLevelSound(this, "SE_BM_LV_SKL_GUARD_SWIM_NORMAL", -1, -1, -1);
    MR::setNerveAtStep(this, &::SkeletalFishGuardNrvNormal::sInstance, 300);
}

void SkeletalFishGuard::exeNormal() {
    if (MR::isFirstStep(this)) {
        MR::onBind(this);
    }

    calcTransAndFront();
    MR::calcGravity(this);
    MR::startLevelSound(this, "SE_BM_LV_SKL_GUARD_SWIM_NORMAL", -1, -1, -1);
    tryShiftApart();
}

void SkeletalFishGuard::exeApart() {
    if (MR::isFirstStep(this)) {
        TVec3f* grav = &mGravity;
        f32 dot = grav->dot(_A4);
        TVec3f v15;
        JMAVECScaleAdd((const Vec*)grav, (const Vec*)_A4, (Vec*)v15, -dot);
        _B0 = grav->dot(_A4 - v15);
        _A4.set(v15);
        MR::startBck(this, "Turn", NULL);
        MR::startBrk(this, "Attack");
        mAttackDelay = 0;
    }

    f32 v3 = (f32)getNerveStep() / 80.0f;
    TVec3f v14(_A4);

    if (!MR::isNearZero(v14, 0.001f)) {
        MR::normalize(&v14);
        f32 sqr = v14.squared();
        if (sqr > 0.0000038146973f) {
            f32 inv = JGeometry::TUtil<f32>::inv_sqrt(sqr);
            v14.scale(inv * 0.30000001f);
        }

        f32 v6 = _A4.squared();
        if (v14.squared() >= v6) {
            _A4.z = 0.0f;
            _A4.y = 0.0f;
            _A4.x = 0.0f;
        }
        else {
            TVec3f v10 = v14 * 0.30000001f;
            JMathInlineVEC::PSVECSubtract((const Vec*)_A4, (const Vec*)v10, (Vec*)_A4);
        }
    }

    f32 v7 = (_B0 * v3);
    TVec3f grav(mGravity);
    MR::calcGravity(this);
    TQuat4f quat;
    quat.setRotate(grav, mGravity);
    quat.transform(_D0);
    quat.transform(_A4);
    lookToPlayer((v3 * (v3 * 18.849556f)) / 180.0f, ((v3 * (v3 * 18.849556f)) / 180.0f));
    mTranslation = (_A4 + (mGravity * v7)) + mTranslation;
    MR::calcGravity(this);
    MR::startLevelSound(this, "SE_BM_LV_SKL_GUARD_SWIM_NORMAL", -1, -1, -1);
    MR::startLevelSound(this, "SE_BM_LV_SKL_GUARD_ALARM", MR::calcDistanceToPlayer(this), -1, -1);
    MR::setNerveAtStep(this, &::SkeletalFishGuardNrvFollow::sInstance, 80);
}

void SkeletalFishGuard::exeFollow() {
    if (MR::isFirstStep(this)) {
        _C4 = 0.0f;
        MR::startBck(this, "Attack", NULL);
        MR::startBrk(this, "Attack");
        MR::startSound(this, "SE_BM_SKL_GUARD_ATTACK_START", -1, -1);
    }

    MR::startLevelSound(this, "SE_BM_LV_SKL_GUARD_SWIM_ATTACK", -1, -1, -1);
    MR::startLevelSound(this, "SE_BM_LV_SKL_GUARD_SWIM_NORMAL", -1, -1, -1);
    MR::startLevelSound(this, "SE_BM_LV_SKL_GUARD_ALARM", MR::calcDistanceToPlayer(this), -1, -1);
    mTranslation += _D0 * _C4;
    _C4 += 0.80000001f;
    if (_C4 >= 22.0f) {
        _C4 = 22.0f;
    }

    f32 dist = PSVECDistance((const Vec*)MR::getPlayerCenterPos(), (const Vec*)mTranslation);
    f32 v6 = 0.0f;
    f32 v7 = (dist - 1000.0f) / 500.0f;
    if (v7 >= 0.0f) {
        v6 = 1.0f;

        if (v7 <= 1.0f) {
            v6 = ((dist - 1000.0f) / 500.0f);
        }
    }

    lookToPlayer((((1.5f * (1.0f - ((1.0f - v6) * (1.0f - v6)))) * 3.1415927f) / 180.0f), (((1.0f - ((1.0f - v6) * (1.0f - v6))) * 3.1415927f) / 180.0f));
    MR::calcGravity(this);

    if (!tryShiftNumb(&::SkeletalFishGuardNrvFollow::sInstance)) {
        tryShiftStraight();
    }
}

void SkeletalFishGuard::exeStraight() {
    if (MR::isFirstStep(this)) {
        MR::startBrk(this, "AttackNear");
    }

    MR::startLevelSound(this, "SE_BM_LV_SKL_GUARD_SWIM_ATTACK", -1, -1, -1);
    MR::startLevelSound(this, "SE_BM_LV_SKL_GUARD_SWIM_NORMAL", -1, -1, -1);
    MR::startLevelSound(this, "SE_BM_LV_SKL_GUARD_ALARM", MR::calcDistanceToPlayer(this), -1, -1);
    mTranslation += _D0 * _C4;

    _C4 += 0.80000001f;
    if (_C4 >= 22.0f) {
        _C4 = 22.0f;
    }

    MR::calcGravity(this);
    if (!tryShiftNumb(&::SkeletalFishGuardNrvStraight::sInstance)) {
        tryShiftKill();
    }
}

void SkeletalFishGuard::exeKill() {
    if (MR::isFirstStep(this)) {
        MR::invalidateHitSensors(this);
        MR::invalidateShadow(this, NULL);
        MR::startBck(this, "Break", NULL);
        MR::startSound(this, "SE_BM_SKL_GUARD_EXPLODE", -1, -1);
    }

    if (MR::isBckOneTimeAndStopped(this)) {
        MR::hideModel(this);
        kill();
    }
}

void SkeletalFishGuard::exeNumb() {
    if (MR::isFirstStep(this)) {
        MR::startDPDHitSound();
        MR::stopBck(this);
        MR::forceDeleteEffectAll(this);
        MR::emitEffectWithScale(this, "PointerTouchManual", 1.5f, -1);
        mScaleController->startDpdHitVibration();
    }

    MR::startDPDFreezeLevelSound(this);

    if (!MR::isStarPointerPointing2POnPressButton(this, "Žã", true, false)) {
        setNerve(_CC);
    }
}

void SkeletalFishGuard::appearNaturally() {
    appear();
    setNerve(&::SkeletalFishGuardNrvWait::sInstance);
    MR::hideModel(this);
}

void SkeletalFishGuard::appearForce() {
    makeActorAppeared();
    MR::startBck(this, "Swim", NULL);
    MR::startBrk(this, "Base");
    MR::startSound(this, "SE_BM_SKL_GUARD_APPEAR", -1, -1);
    MR::validateHitSensors(this);
    MR::clearHitSensors(this);
    
    setNerve(&::SkeletalFishGuardNrvNormal::sInstance);
    MR::showModel(this);
}

void SkeletalFishGuard::killNaturally() {
    if (!MR::isDead(this)) {
        if (isNerve(&::SkeletalFishGuardNrvWait::sInstance)) {
            makeActorDead();
        }
        else if (!isNerve(&::SkeletalFishGuardNrvKill::sInstance)) {
            setNerve(&::SkeletalFishGuardNrvKill::sInstance);
        }
    }
}

void SkeletalFishGuard::waitAttack(s32 time) {
    mAttackDelay = time;
}

void SkeletalFishGuard::calcAndSetBaseMtx() {
    TVec3f stack_64;
    stack_64.mul(mScale, mScaleController->mScale);
    MR::setBaseScale(this, stack_64);

    if (isNerve(&::SkeletalFishGuardNrvAppear::sInstance)) {
        TVec3f gravityVec;
        MR::calcGravityVector(this, _F4, &gravityVec, NULL, 0);
        JGeometry::negateInternal((f32*)&gravityVec, (f32*)&gravityVec);
        TVec3f stack_4C;
        PSVECCrossProduct((const Vec*)&gravityVec, (const Vec*)&_100, (Vec*)&stack_4C);
        MR::normalize(&stack_4C);
        PSVECCrossProduct((const Vec*)&_100, (const Vec*)&stack_4C, (Vec*)&gravityVec);
        TQuat4f quat;
        quat.setRotate(_100, _D0);
        quat.transform(gravityVec);
        quat.transform(stack_4C);
        TPos3f stack_D0;
        stack_D0.setXDir(stack_4C);
        stack_D0.setYDir(gravityVec);
        stack_D0.setZDir(_D0);
        stack_D0.setTrans(mTranslation);
        MR::setBaseTRMtx(this, stack_D0);
    }
    else {
        TVec3f stack_30;
        TVec3f stack_24;
        JGeometry::negateInternal((f32*)&mGravity, (f32*)&stack_30);
        PSVECCrossProduct((const Vec*)&stack_30, (const Vec*)&_D0, (Vec*)&stack_24);

        if (MR::isNearZero(stack_24, 0.001f)) {
            MtxPtr mtx = getBaseMtx();
            TPos3f stack_A0;
            f32 x, y, z;
            stack_A0.set(mtx);
            x = stack_A0.mMtx[0][0];
            y = stack_A0.mMtx[1][0];
            z = stack_A0.mMtx[2][0];

            stack_24.set(x, y, z);
            z = stack_A0.mMtx[2][1];
            y = stack_A0.mMtx[1][1];
            x = stack_A0.mMtx[0][1];
            stack_30.set(x, y, z);
            TVec3f stack_18;
            z = stack_A0.mMtx[2][2];
            y = stack_A0.mMtx[1][2];
            x = stack_A0.mMtx[0][2];
            stack_18.set(x, y, z);
            TQuat4f stack_8;
            stack_8.setRotate(stack_18, _D0);
            stack_8.transform(stack_24);
            stack_8.transform(stack_30);
        }
        else {
            MR::normalize(&stack_24);
            PSVECCrossProduct((const Vec*)&_D0, (const Vec*)&stack_24, (Vec*)&stack_30);
        }

        TPos3f stack_70;
        stack_70.setXDir(stack_24);
        stack_70.setYDir(stack_30);
        stack_70.setZDir(_D0);
        stack_70.setTrans(mTranslation);
        MR::setBaseTRMtx(this, stack_70);
    }
}

void SkeletalFishGuard::exeWait() {
    MR::setNerveAtStep(this, &::SkeletalFishGuardNrvAppear::sInstance, _A0);
}

void SkeletalFishGuard::attackSensor(HitSensor* a2, HitSensor* a3) {
    if (!isNerve(&::SkeletalFishGuardNrvKill::sInstance) && (!isNerve(&::SkeletalFishGuardNrvNormal::sInstance) || getNerveStep() >= 2)) {
        if (MR::isSensorPlayer(a3) && MR::sendMsgEnemyAttackStrong(a3, a2)) {
            setNerve(&::SkeletalFishGuardNrvKill::sInstance);
        }
        else {
            MR::sendMsgPush(a3, a2);
        }
    }
}

bool SkeletalFishGuard::receiveMsgPlayerAttack(u32 msg, HitSensor* a3, HitSensor* a4) {
    if (isNerve(&::SkeletalFishGuardNrvKill::sInstance)) {
        return false;
    }

    if (MR::isMsgStarPieceReflect(msg)) {
        return true;
    }

    if (isNerve(&::SkeletalFishGuardNrvNormal::sInstance) && getNerveStep() < 2) {
        return false;
    }

    if (MR::isMsgJetTurtleAttack(msg)) {
        MR::appearStarPiece(this, mTranslation, 3, 10.0f, 40.0f, false);
        MR::startSound(this, "SE_OJ_STAR_PIECE_BURST_W_F", -1, -1);
        setNerve(&::SkeletalFishGuardNrvKill::sInstance);
        return true;
    }

    return false;
}

void SkeletalFishGuard::initSensor() {
    initHitSensor(2);
    MR::addHitSensorAtJointEnemy(this, "head", "Joint00", 16, 120.0f, TVec3f(0.0f, 0.0f, 0.0f));
    MR::addHitSensorAtJointEnemy(this, "body", "Joint02", 16, 120.0f, TVec3f(0.0f, 0.0f, 0.0f));
}

void SkeletalFishGuard::calcTransAndFront() {
    calcTarget(&_E8, &_F4, &_100, 0);
    _D0.set(_100);
    JMathInlineVEC::PSVECSubtract((const Vec*)&_F4, (const Vec*)&mTranslation, (Vec*)&_A4);
    mTranslation.set(_F4);
}

void SkeletalFishGuard::rotateHorizontal(const TVec3f& a2, f32 scalar) {
    TVec3f* grav = &mGravity;
    f32 dot = grav->dot(a2);
    TVec3f v10;
    JMAVECScaleAdd((const Vec*)grav, (const Vec*)&a2, (Vec*)&v10, -dot);
    if (!MR::isNearZero(v10, 0.001f)) {
        MR::normalize(&v10);
        TVec3f* v11 = &_D0;
        TVec3f* g = &mGravity;
        f32 v8 = g->dot(*v11);
        TVec3f v9;
        JMAVECScaleAdd((const Vec*)g, (const Vec*)&_D0, (Vec*)&v9, -v8);
        if (!MR::isNearZero(v9, 0.001f)) {
            MR::normalize(&v9);
            turn(v11, v9, v10, scalar);
        }
    }
}

void SkeletalFishGuard::rotateVertical(const TVec3f& a2, f32 a3) {
    TVec3f v12;
    PSVECCrossProduct((const Vec*)&mGravity, (const Vec*)&_D0, (Vec*)&v12);

    if (!MR::isNearZero(v12, 0.001f)) {
        MR::normalize(&v12);
        f32 angle = mGravity.angle(a2);
        f32 v7 = (angle - mGravity.angle(_D0));

        if (v7 > 0.0f) {
            if (v7 > a3) {
                v7 = a3;
            }
        }
        else {
            if (v7 < -a3) {
                v7 = -a3;
            }
        }

        f32 v8 = (0.5f * v7);
        f32 v9 = sin(v8);
        TQuat4f v10;
        v10.scale(v9, v12);
        v10.w = cos(v8);
        v10.transform(_D0);
    }
}

bool SkeletalFishGuard::tryShiftApart() {
    if (mAttackDelay > 0) {
        return false;
    }

    if (!isInScreen()) {
        return false;
    }

    if (!isPlayerInAttackRange()) {
        return false;
    }

    if (!isLineOfSightClear()) {
        return false;
    }

    mFishBoss->notifyAttack(this);
    setNerve(&::SkeletalFishGuardNrvApart::sInstance);
    return true;
}

bool SkeletalFishGuard::tryShiftStraight() {
    TVec3f pos(*MR::getPlayerCenterPos());
    JMathInlineVEC::PSVECSubtract((const Vec*)pos, (const Vec*)mTranslation, (Vec*)pos);
    if (_D0.dot(pos) < 0.0f) {
        setNerve(&::SkeletalFishGuardNrvStraight::sInstance);
        return true;
    }

    return false;
}

bool SkeletalFishGuard::tryShiftKill() {
    WaterInfo info;
    if (MR::getWaterAreaObj(&info, mTranslation)) {
        return false;
    }

    setNerve(&::SkeletalFishGuardNrvKill::sInstance);
    return true;
}

void SkeletalFishGuard::turn(TVec3f* a1, const TVec3f& a2, const TVec3f& a3, f32 a4) {
    TQuat4f quat;

    f32 angle = a2.angle(a3);
    f32 v10 = 1.0f;

    if (angle > a4) {
        v10 = (a4 / angle);
    }

    TVec3f v15;
    PSVECCrossProduct((const Vec*)&a2, (const Vec*)&a3, (Vec*)&v15);
    f32 v11 = PSVECMag((const Vec*)&v15);
    f32 factor = 1.0f / 262144.0f;

    if (v11 <= factor) {
        quat.set(0.0f, 0.0f, 0.0f, 1.0f);
    }
    else {
        f32 v12 = a2.dot(a3);
        f32 v13 = (v10 * (0.5f * JMath::sAtanTable.atan2_(v11, v12)));
        f32 v14 = sin(v13);
        quat.scale((v14 / v11), v15);
        quat.w = cos(v13);
    }

    quat.transform(*a1);
}

void SkeletalFishGuard::lookToPlayer(f32 a2, f32 a3) {
    TVec3f pos(*MR::getPlayerCenterPos());
    pos -= mTranslation;

    if (!MR::isNearZero(pos, 0.001f)) {
        MR::normalize(&pos);
        rotateHorizontal(pos, a2);
        rotateVertical(pos, a3);
    }
}

void SkeletalFishGuard::calcTarget(TVec3f* a1, TVec3f* a2, TVec3f* a3, s32 a4) {
    f32 v11 = MR::getRailCoord(mFishBoss->getCurrentRail());
    f32 v13 = (_94.z + (v11 + (MR::getRailCoordSpeed(mFishBoss->getCurrentRail()) * a4)));
    MR::calcRailPosAtCoord(a1, mFishBoss->getCurrentRail(), v13);
    MR::calcRailDirectionAtCoord(a3, mFishBoss->getCurrentRail(), v13);
    TVec3f v22;
    MR::calcGravityVector(this, *a1, &v22, NULL, 0);
    TVec3f v21;
    PSVECCrossProduct((const Vec*)a3, (const Vec*)&v22, (Vec*)&v21);
    MR::normalizeOrZero(&v21);
    f32 v16 = _94.y;
    TVec3f v18(v22);
    v18.scale(v16);
    f32 v17 = _94.x;
    TVec3f v19(v21);
    v19.scale(v17);
    TVec3f v20(v19);
    JMathInlineVEC::PSVECAdd((const Vec*)&v20, (const Vec*)&v18, (Vec*)&v20);
    JMathInlineVEC::PSVECAdd((const Vec*)a1, (const Vec*)&v20, (Vec*)a2);
}

bool SkeletalFishGuard::isInScreen() const {
    TVec2f screen;
    return MR::calcScreenPosition(&screen, mTranslation);
}

bool SkeletalFishGuard::isPlayerInAttackRange() const {
    const Vec* posPtr = (const Vec*)&mTranslation;
    TVec3f v7(*MR::getPlayerCenterPos());
    JMathInlineVEC::PSVECSubtract((const Vec*)&v7, posPtr, (Vec*)&v7);

    if (PSVECMag((const Vec*)&v7) > 5000.0f) {
        return false;
    }

    if (MR::isNearZero(v7, 0.001f)) {
        return false;
    }

    MR::normalize(&v7);
    f32 lap = JMASCos(180.0f);
    f32 dot = v7.dot(_D0);
    if (dot < lap) {
        return false;
    }

    return true;
}

bool SkeletalFishGuard::isLineOfSightClear() const {
    TVec3f v12(*MR::getPlayerCenterPos());
    JMathInlineVEC::PSVECSubtract((const Vec*)&v12, (const Vec*)&mTranslation, (Vec*)&v12);

    if (Collision::checkStrikeLineToMap(mTranslation, v12, 0, NULL, NULL)) {
        return false;
    }

    TVec3f v10 = MR::getCamPos();
    JMathInlineVEC::PSVECSubtract((const Vec*)&v10, (const Vec*)&mTranslation, (Vec*)&v10);

    if (Collision::checkStrikeLineToMap(mTranslation, v10, 0, NULL, NULL)) {
        return false;
    }

    TVec3f v11(_E8);
    JMathInlineVEC::PSVECSubtract((const Vec*)&v11, (const Vec*)&mTranslation, (Vec*)&v11);
    f32 v4 = _100.dot(v11);
    JMAVECScaleAdd((const Vec*)&_100, (const Vec*)&v11, (Vec*)&v11, -v4);
    f32 v5 = _100.dot(v12);
    JMAVECScaleAdd((const Vec*)&_100, (const Vec*)&v12, (Vec*)&v12, -v5);
    bool ret = true;

    if (!MR::isNearZero(v11, 0.001f) && !MR::isNearZero(v12, 0.001f)) {
        f32 v6 = PSVECMag((const Vec*)&v11);
        f32 thick = mFishBoss->getBodyThickness();
        f64 angle = JMAAsinRadian(thick / v6);
        if (v11.angle(v12) < angle) {
            return false;
        }
    }

    return true;
}

bool SkeletalFishGuard::tryShiftNumb(const Nerve* pNerve) {
    if (MR::isStarPointerPointing2POnPressButton(this, "Žã", true, false)) {
        _CC = pNerve;
        setNerve(&::SkeletalFishGuardNrvNumb::sInstance);
        return true;
    }

    return false;
}

SkeletalFishGuard::~SkeletalFishGuard() {

}

namespace {
    void SkeletalFishGuardNrvWait::execute(Spine* pSpine) const {
        ((SkeletalFishGuard*)pSpine->mExecutor)->exeWait();
    }

    void SkeletalFishGuardNrvAppear::execute(Spine* pSpine) const {
        ((SkeletalFishGuard*)pSpine->mExecutor)->exeAppear();
    }

    void SkeletalFishGuardNrvNormal::execute(Spine* pSpine) const {
        ((SkeletalFishGuard*)pSpine->mExecutor)->exeNormal();
    }

    void SkeletalFishGuardNrvApart::execute(Spine* pSpine) const {
        ((SkeletalFishGuard*)pSpine->mExecutor)->exeApart();
    }


    void SkeletalFishGuardNrvFollow::execute(Spine* pSpine) const {
        ((SkeletalFishGuard*)pSpine->mExecutor)->exeFollow();
    }

    void SkeletalFishGuardNrvStraight::execute(Spine* pSpine) const {
        ((SkeletalFishGuard*)pSpine->mExecutor)->exeStraight();
    }

    void SkeletalFishGuardNrvDefence::execute(Spine* pSpine) const {
        // Nothing here :(
    }

    void SkeletalFishGuardNrvKill::execute(Spine* pSpine) const {
        ((SkeletalFishGuard*)pSpine->mExecutor)->exeKill();
    }

    void SkeletalFishGuardNrvNumb::execute(Spine* pSpine) const {
        ((SkeletalFishGuard*)pSpine->mExecutor)->exeNumb();
    }
    void SkeletalFishGuardNrvNumb::executeOnEnd(Spine* pSpine) const {
        ((SkeletalFishGuard*)pSpine->mExecutor)->exeOnEndNumb();
    }

    SkeletalFishGuardNrvWait(SkeletalFishGuardNrvWait::sInstance);
    SkeletalFishGuardNrvAppear(SkeletalFishGuardNrvAppear::sInstance);
    SkeletalFishGuardNrvNormal(SkeletalFishGuardNrvNormal::sInstance);
    SkeletalFishGuardNrvApart(SkeletalFishGuardNrvApart::sInstance);
    SkeletalFishGuardNrvFollow(SkeletalFishGuardNrvFollow::sInstance);
    SkeletalFishGuardNrvStraight(SkeletalFishGuardNrvStraight::sInstance);
    SkeletalFishGuardNrvDefence(SkeletalFishGuardNrvDefence::sInstance);
    SkeletalFishGuardNrvKill(SkeletalFishGuardNrvKill::sInstance);
    SkeletalFishGuardNrvNumb(SkeletalFishGuardNrvNumb::sInstance);
};