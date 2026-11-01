#include "Game/Boss/TombSpiderThreadAttacher.h"
#include "Game/MapObj/SpiderThread.h"
#include "Game/Util/JMapInfo.h"
#include "Game/Util/JointUtil.h"
#include "Game/Util/LiveActorUtil.h"
#include "Game/Util/ObjUtil.h"

#include "SpiderThreadExt.h"

namespace NrvTombSpiderThreadAttacher {
    FULL_NERVE(TombSpiderThreadAttacherNrvFree, TombSpiderThreadAttacher, Free);
    FULL_NERVE(TombSpiderThreadAttacherNrvAttach, TombSpiderThreadAttacher, Attach);
}  // namespace NrvTombSpiderThreadAttacher

TombSpiderThreadAttacher::TombSpiderThreadAttacher(LiveActor* pActor, const char* pJointName, f32 radius, f32 offset)
    : LiveActor("巣の付着点"), mJointMtx(nullptr), mRadius(radius), mZOffset(offset) {
    mJointMtx = MR::getJointMtx(pActor, pJointName);
}

void TombSpiderThreadAttacher::init(const JMapInfoIter& rIter) {
    MR::connectToSceneEnemyMovement(this);
    initNerve(&NrvTombSpiderThreadAttacher::TombSpiderThreadAttacherNrvFree::sInstance, 0);
    MR::invalidateClipping(this);
    makeActorDead();
}

void TombSpiderThreadAttacher::exeFree() {
    TVec3f pos(mJointMtx[0][3], mJointMtx[1][3], mJointMtx[2][3] - mZOffset);
    
    MR::tryPushSpiderThread(pos, mRadius);
}

void TombSpiderThreadAttacher::exeAttach() {}
