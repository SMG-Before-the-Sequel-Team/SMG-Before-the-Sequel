#include "Game/Boss/TripodBossBaseJointPosition.h"
#include "Game/Boss/TripodBossAccesser.h"
#include "Game/Util.h"

#include "Game/NameObj/NameObjCategories.h"
#include "Game/NameObj/NameObjExecuteHolder.h"

TripodBossBaseJointPosition::TripodBossBaseJointPosition(const char *pName) : LiveActor(pName) {
    _8C = -1;
    _90.identity();
}

void TripodBossBaseJointPosition::init(const JMapInfoIter &rIter) {
    MR::initDefaultPos(this, rIter);
    //MR::connectToSceneCollisionEnemyMovement(this); // TODO: Doesn't exist in SMG2.....
    {
        MR::connectToScene(this, MR::MovementType_CollisionEnemy, MR::CategoryList_Null, MR::CategoryList_Null, MR::CategoryList_Null);
        MR::connectToSceneTemporarily(this);
        MR::connectToDrawTemporarily(this);
    }
    MR::getJMapInfoArg0NoInit(rIter, &_8C);
    MR::getJMapInfoMatrixFromRT(rIter, &_90);
    MR::addBaseMatrixFollowTarget(this, rIter, NULL, NULL);
    MR::addTripodBossParts(this);
    MR::invalidateClipping(this);
    makeActorDead();
}

void TripodBossBaseJointPosition::initAfterPlacement() {
    MR::makeMtxTR((MtxPtr)&_90, this);
    MR::setTripodBossJointAttachBaseMatrix(_90, _8C);
    MR::concatTripodBossAttachJointMatrix(&_90, _8C);
}

void TripodBossBaseJointPosition::control() {
    MR::makeMtxTR((MtxPtr)&_90, this);
    MR::concatTripodBossAttachJointMatrix(&_90, _8C);
    if (MR::isEndBreakDownDemoTripodBoss()) {
        kill();
    }
}

TripodBossBaseJointPosition::~TripodBossBaseJointPosition() {

}

MtxPtr TripodBossBaseJointPosition::getBaseMtx() const {
    return (MtxPtr)&_90;
}
