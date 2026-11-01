#include "MagicBell.h"
#include "Game/Screen/StarPointerController.h"
#include "Game/Screen/StarPointerDirector.h"
#include "Game/Util/ActionSoundUtil.h"
#include "Game/Util/ActorInitUtil.h"
#include "Game/Util/ActorMovementUtil.h"
#include "Game/Util/ActorSensorUtil.h"
#include "Game/Util/ActorSwitchUtil.h"
#include "Game/Util/EffectUtil.h"
#include "Game/Util/JointUtil.h"
#include "Game/Util/LiveActorUtil.h"
#include "Game/Util/MathUtil.h"
#include "Game/Util/MtxUtil.h"
#include "Game/Util/PlayerUtil.h"
#include "Game/Util/SequenceUtil.h"
#include "Game/Util/StarPointerUtil.h"

MagicBell::MagicBell(const char* pName) : LiveActor(pName) {
    mBellSwinger = NULL;
    mSurface1Mtx = NULL;
    mSurface2Mtx = NULL;
    mBellRodSwinger = NULL;
    mSurface1Mtx = NULL;
    mHitMarkTranslation.zero();
}

void MagicBell::init(const JMapInfoIter& rIter) {
    MR::processInitFunction(this, rIter, false);

    mSurface2Mtx = MR::getJointMtx(this, "polySurface2");
    mSurface1Mtx = MR::getJointMtx(this, "polySurface1");

    MR::addEffect(this, "StarWandHitMark");
    MR::setEffectHostSRT(this, "StarWandHitMark", &mHitMarkTranslation, NULL, NULL);

    initNerve(&NrvMagicBell::MagicBellNrvWait::sInstance, 0);
    
    TMtx34f mtx;
    PSMTXIdentity((MtxPtr)&mtx);
    MR::makeMtxTR(mtx.toMtxPtr(), this);
    MR::calcGravity(this);
    mBellSwinger = new Swinger(&mTranslation, mtx.toMtxPtr(), 100.0f, 0.60000002f, 0.99000001f, &mGravity);
    mBellRodSwinger = new Swinger(&mTranslation, mtx.toMtxPtr(), 50.0f, 0.30000001f, 0.94999999f, &mGravity);
    makeActorAppeared();
}

void MagicBell::exeWait() {
    MR::isFirstStep(this);

    if (!tryRing())
        updateMtx();
}

void MagicBell::exeRing()
{
    if (MR::isFirstStep(this)) {
        if (MR::isValidSwitchA(this) && !MR::isOnSwitchA(this)) {
            MR::onSwitchA(this);
            MR::startActionSound(this, "OjHappyBellSwitchOn", -1, -1, -1);
        }

        MR::startActionSound(this, "OjHappyBellRing", -1, -1, -1);
    }

    mBellSwinger->update();
    mBellRodSwinger->update();

    if (mBellSwinger->_20.dot(mBellRodSwinger->_20) < 0.95f) {
        TVec3f v17(mBellSwinger->_20);
        //v17.sub(mBellRodSwinger->_20);
        PSVECSubtract((const Vec*)&v17, (const Vec*)&mBellRodSwinger->_20, (Vec*)&v17);
        f32 v3 = mBellRodSwinger->_20.dot(v17);
        v17.x -= v3 * mBellRodSwinger->_20.x;
        v17.y -= v3 * mBellRodSwinger->_20.y;
        v17.z -= v3 * mBellRodSwinger->_20.z;
        MR::normalizeOrZero(&v17);
        f32 v4 = mBellSwinger->mAcceleration.dot(v17);
        f32 v5 = mBellRodSwinger->mAcceleration.dot(v17);

        f32 v6 = v5 >= 0.0f ? 0.0f : v5;

        if (v4 - v6 > 0.0f) {
            f32 v7 = v5 >= 0.0f ? 0.0f : v5;

            //v17.scale(v4 - v7);
            PSVECScale((const Vec*)&v17, (Vec*)&v17, v4 - v7);
            //v17.sub(mBellRodSwinger->mAcceleration);
            PSVECSubtract((const Vec*)&v17, (const Vec*)&mBellRodSwinger->mAcceleration, (Vec*)&v17);
            mBellRodSwinger->accel(v17);
            mHitMarkTranslation.set(mBellRodSwinger->_8);
        }
    }
    updateMtx();

    if (MR::isGreaterStep(this, 10) && tryRing())
        return;

    if (mBellSwinger->_20.y > -0.99f)
        return;

    if (mBellSwinger->mAcceleration.squaredInline() < 0.01f) {
        MR::tryDeleteEffect(this, "Ring");
        setNerve(&NrvMagicBell::MagicBellNrvWait::sInstance);
    }
}

void MagicBell::attackSensor(HitSensor* a1, HitSensor* a2) {
    if (MR::isSensorPlayer(a2))
        MR::sendMsgPush(a2, a1);
}

bool MagicBell::receiveMsgPlayerAttack(u32 msg, HitSensor* pSender, HitSensor* pReceiver)
{
    if (MR::isMsgLockOnStarPieceShoot(msg)) {
        return false;
    }

    if (!MR::isMsgPlayerHitAll(msg) && !MR::isMsgPlayerTrample(msg) && !MR::isMsgPlayerHipDrop(msg))
        return false;

    if (isNerve(&NrvMagicBell::MagicBellNrvWait::sInstance) || (isNerve(&NrvMagicBell::MagicBellNrvRing::sInstance) && MR::isGreaterStep(this, 10))) {

        TVec3f v15(mTranslation);
        //v15.sub(*MR::getPlayerPos());
        PSVECSubtract((const Vec*)&v15, (const Vec*)MR::getPlayerPos(), (Vec*)&v15);
        v15.y += 100.0f;
        MR::normalizeOrZero(&v15);
        TVec3f v14(v15);
        //v14.scale(-200.0f);
        PSVECScale((const Vec*)&v14, (Vec*)&v14, -200.0f);
        //v14.add(mTranslation);
        PSVECAdd((const Vec*)&v14, (const Vec*)&mTranslation, (Vec*)v14);
        startRing(v15, v14);
        return true;
    }
    return false;
}

bool MagicBell::tryRing()
{
    if (!MR::isExecScenarioStarter() && MR::isStarPointerPointing(this, 0, 0, "Žã")) {
        TVec2f* v4(MR::getStarPointerScreenVelocity(0));
        if (((v4->x * v4->x) + (v4->y * v4->y)) > 64.0f) {
            TVec3f v12(StarPointerFunction::getStarPointerDirector()->getStarPointerController(0)->_60); //MR::getStarPointerWorldVelocityDirection(&v12, 0);

            if (MR::isNearZero(v12, 0.001f)) {
                v12.set(MR::getRandom(-1.0f, 1.0f), MR::getRandom(-1.0f, 1.0f), MR::getRandom(-1.0f, 1.0f));
                if (MR::isNearZero(v12, 0.001f)) {
                    v12.set(1.0f, 0.0f, 0.0f);
                }
                else {
                    MR::normalize(&v12);
                }
            }
            TVec3f v11;
            MR::calcStarPointerWorldPointingPos(&v11, mTranslation, 0);
            startRing(v12, v11);
            return true;
        }
    }
    return false;
}

void MagicBell::startRing(const TVec3f& a1, const TVec3f& a2)
{
    f32 v10 = PSVECMag((const Vec*)&mBellSwinger->mAcceleration);
    TVec3f v13(mBellSwinger->mAcceleration);
    v13.scale(-1.0f);
    mBellSwinger->accel(v13);
    TVec3f v12(a1);
    v12.scale(5.0f + v10);
    mBellSwinger->accel(v12);
    mHitMarkTranslation.set(a2);
    MR::emitEffect(this, "StarWandHitMark");
    MR::tryEmitEffect(this, "Ring");
    setNerve(&NrvMagicBell::MagicBellNrvRing::sInstance);
}

void MagicBell::updateMtx() {
    PSMTXCopy(mBellSwinger->_60.toMtxPtr(), mSurface2Mtx);
    PSMTXCopy(mBellRodSwinger->_60.toMtxPtr(), mSurface1Mtx);
    TVec3f v3(0.0f, 0.0f, 0.0f);
    MR::setMtxTrans(mSurface2Mtx, v3.x, v3.y, v3.z);
    TVec3f v2(0.0f, 0.0f, 0.0f);
    MR::setMtxTrans(mSurface1Mtx, v2.x, v2.y, v2.z);
    PSMTXScaleApply(mSurface2Mtx, mSurface2Mtx, mScale.x, mScale.y, mScale.z);
    PSMTXScaleApply(mSurface1Mtx, mSurface1Mtx, mScale.x, mScale.y, mScale.z);
    MR::setMtxTrans(mSurface2Mtx, mTranslation.x, mTranslation.y, mTranslation.z);
    MR::setMtxTrans(mSurface1Mtx, mTranslation.x, mTranslation.y, mTranslation.z);
}

MagicBell::~MagicBell() {

}

MtxPtr MagicBell::getBaseMtx() const {
    return mSurface2Mtx;
}

namespace NrvMagicBell {
    void MagicBellNrvWait::execute(Spine* pSpine) const {
        ((MagicBell*)pSpine->mExecutor)->exeWait();
    }
    MagicBellNrvWait(MagicBellNrvWait::sInstance);

    void MagicBellNrvRing::execute(Spine* pSpine) const {
        ((MagicBell*)pSpine->mExecutor)->exeRing();
    }
    MagicBellNrvRing(MagicBellNrvRing::sInstance);
}