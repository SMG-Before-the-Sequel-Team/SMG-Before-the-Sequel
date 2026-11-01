#include "Game/MapObj/SpiderMapBlock.h"

#include "Game/Util.h"
#include "Game/Util/ActionSoundUtil.h"

namespace {
    static const f32 sCoinSpeed = 35.0f; // TODO: Move to ObjArg?
}

SpiderMapBlock::SpiderMapBlock(const char* pName) : LiveActor(pName), mPosZ(0.0f) {
}

bool SpiderMapBlock::receiveMsgPlayerAttack(u32 msg, HitSensor* pSender, HitSensor* pReceiver) {
    if (MR::isMsgPlayerHipDropFloor(msg)) {
        kill();
        return true;
    }
    return false;
}

bool SpiderMapBlock::receiveMsgEnemyAttack(u32 msg, HitSensor* pSender, HitSensor* pReceiver) {
    kill();
    return true;
}

SpiderMapBlock::~SpiderMapBlock() {
}

void SpiderMapBlock::init(const JMapInfoIter& rIter) {
    MR::processInitFunction(this, rIter, false);


    mPosZ = mTranslation.z; // is this variable even needed?

    MR::declareCoin(this, 1);
    makeActorAppeared();
}

void SpiderMapBlock::kill() {
    MR::startActionSound(this, "Break", -1, -1, -1); // SE_OJ_ROCK_BREAK in SMG1
    MR::tryEmitEffect(this, "Break");

    MtxPtr baseMtx = getBaseMtx();
    TVec3f coinVel(baseMtx[0][1], baseMtx[1][1], 0.0f);
    MR::normalizeOrZero(&coinVel);
    coinVel.scale(sCoinSpeed);
    TVec3f coinPos(mTranslation.x, mTranslation.y, mPosZ);
    MR::appearCoinToVelocity(this, coinPos, coinVel, 1);
    LiveActor::kill();
}
