#include "Game/Boss/TripodBossCoin.h"
#include "Game/Boss/TripodBossAccesser.h"

#include "Game/MapObj/Coin.h"
#include "Game/MapObj/PurpleCoinHolder.h"
#include "Game/Util.h"

namespace NrvTripodBossCoin {
    void TripodBossCoinNrvNonActive::execute(Spine* pSpine) const {
        ((TripodBossCoin*)pSpine->mExecutor)->exeNonActive();
    }
    TripodBossCoinNrvNonActive(TripodBossCoinNrvNonActive::sInstance);

    void TripodBossCoinNrvActive::execute(Spine* pSpine) const {
        ((TripodBossCoin*)pSpine->mExecutor)->exeActive();
    }
    TripodBossCoinNrvActive(TripodBossCoinNrvActive::sInstance);

    void TripodBossCoinNrvEnd::execute(Spine* pSpine) const {
        ((TripodBossCoin*)pSpine->mExecutor)->exeEnd();
    }
    TripodBossCoinNrvEnd(TripodBossCoinNrvEnd::sInstance);
};

TripodBossCoin::TripodBossCoin(const char *pName) : NameObj(pName) {
    mCoin = NULL;
    mSpine = NULL;
    _44 = -1;
    _14.identity();
}

void TripodBossCoin::init(const JMapInfoIter &rIter) {
    MR::getJMapInfoMatrixFromRT(rIter, &_14);
    MR::connectToSceneMapObjDecorationMovement(this);
    MR::getJMapInfoArg0NoInit(rIter, &_44);
    if (MR::isEqualObjectName(rIter, "TripodBossPurpleCoin"))
        mCoin = static_cast<Coin*>(MR::createPurpleCoin(this, "パープルコイン(三脚ボス用)"));
    else
        mCoin = static_cast<Coin*>(MR::createCoin(this, "コイン(三脚ボス用)"));
    mCoin->initWithoutIter();
    mSpine = new Spine(this, &NrvTripodBossCoin::TripodBossCoinNrvNonActive::sInstance, 0);
    MR::addTripodBossPartsMovement(this);
}

void TripodBossCoin::movement() {
    mSpine->update();
}

void TripodBossCoin::exeNonActive() {
    if (MR::isStartedTripodBoss()&& !MR::isStartDemoTripodBoss()) {
        mSpine->setNerve(&NrvTripodBossCoin::TripodBossCoinNrvActive::sInstance);
    }
}

void TripodBossCoin::exeActive() {
    TPos3f pos;
    pos.set(_14);
    MR::concatTripodBossAttachJointMatrix(&pos, _44);
    TVec3f coinPos;
    f32 z = pos.mMtx[2][3];
    f32 y = pos.mMtx[1][3];
    f32 x = pos.mMtx[0][3];
    coinPos.set(x, y, z);
    TVec3f* newPos = &mCoin->mTranslation;
    newPos->x = coinPos.x;
    newPos->y = coinPos.y;
    newPos->z = coinPos.z;

    if (mSpine->mStep == 0) {
        //mCoin->appearControlPose();
        mCoin->appear();
        MR::validateClipping(mCoin);
        MR::validateHitSensors(mCoin);
        MR::offBind(mCoin);
        MR::calcGravityOrZero(mCoin);
        mCoin->mCalcShadowPrivateGravity = false;
        mCoin->setNerve(&NrvCoin::CoinNrvControled::sInstance);
    }

    if (MR::isBrokenTripodBoss() || MR::isDead(mCoin)) {
        mCoin->kill();
        mSpine->setNerve(&NrvTripodBossCoin::TripodBossCoinNrvEnd::sInstance);
    }
}

TripodBossCoin::~TripodBossCoin() {
    
}
