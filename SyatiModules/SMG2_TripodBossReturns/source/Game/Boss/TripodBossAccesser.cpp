#include "Game/Boss/TripodBossAccesser.h"
#include "Game/Scene/SceneObjHolder.h"
#include "Game/Util.h"

namespace {
    static s32 cJMapBoneIDToBoneIndexTableSize = 0x29;
    static s32 cJMapBoneIDToBoneIndexTable[0x29] = {
        0, 1, 2, 7, 8, 9, 0xE, 0xF, 0x10, 0x15, 3, 4, 0, 1, 5, 6, 2, -1, -1, -1,
        0xA, 0xB, 7, 8, 0xC, 0xD, 9, -1, -1, -1, 0x11, 0x12, 0xE, 0xF, 0x13, 0x14, 0x10, -1, -1, -1, 0x15
    };

    s32 convertBoneIDToIndex(s32 id) {
        if (id < 0 || cJMapBoneIDToBoneIndexTableSize <= id) {
            return -1;
        }
        else {
            return cJMapBoneIDToBoneIndexTable[id];
        }
    }
};

TripodBossAccesser::TripodBossAccesser(const char *pName) : NameObj(pName) {
    mBoss = NULL;
    mNumParts = 0;
}

void TripodBossAccesser::setTriPodBoss(TripodBoss *pBoss) {
    mBoss = pBoss;
}

void TripodBossAccesser::addTripodBossParts(LiveActor *pActor) {
    mParts[mNumParts] = pActor;
    mNumParts++;
}

void TripodBossAccesser::showTripodBossParts() {
    for (s32 i = 0; i < mNumParts; i++) {
        if (MR::isDead(mParts[i])) {
            mParts[i]->makeActorAppeared();   
        }
    }
}

void TripodBossAccesser::hideTripodBossParts() {
    for (s32 i = 0; i < mNumParts; i++) {
        if (!MR::isDead(mParts[i])) {
            mParts[i]->makeActorDead();   
        }
    }
}

void TripodBossAccesser::activeTripodBossParts() {
    for (s32 i = 0; i < mNumParts; i++) {
        if (MR::isDead(mParts[i])) {
            mParts[i]->makeActorAppeared();
        }

        MR::sendSimpleMsgToActor(0xAA, mParts[i]);
    }
}

bool TripodBossAccesser::isTripoddBossParts(const NameObj* pObj) const {
    if (pObj == this || pObj == mBoss) {
        return true;
    }

    for (s32 i = 0; i < mNumParts; i++) {
        if (mParts[i] == pObj) {
            return true;
        }
    }

    return false;
}

TripodBossAccesser* TripodBossAccesser::createSceneObj() {
    return (TripodBossAccesser*)MR::createSceneObj(SCENE_OBJ_DELETED_71);
}

#include "Game/NameObj/MovementOnOffGroupHolder.h"

namespace MR {
    NameObjGroup* findGroupFromName_Replica(MovementOnOffGroupHolder* pHolder, const char* pName) {
        for (u32 i = 0; i < pHolder->mGroups.size(); i++)
            if (MR::isEqualString(pHolder->mGroups[i]->mName, pName))
                return pHolder->mGroups[i];
        return NULL;
    }

    NameObjGroup* createGroup_Replica(MovementOnOffGroupHolder* pHolder, const char* pName, s32 groupLimit) {
        NameObjGroup* pGroup = new NameObjGroup(pName, groupLimit);
        pGroup->initWithoutIter();
        pHolder->mGroups.push_back(pGroup);
        return pGroup;
    }

    void joinToMovementOnOffGroup_Replica(const char* pName, NameObj* pObj, s32 groupLimit)
    {
        // Thankfully, the MovementOnOffGroup SceneObj still exists, and is still created for us. it's just that all of it's functions got gutted.
        MovementOnOffGroupHolder* pHolder = (MovementOnOffGroupHolder*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_MOVEMENT_ON_OFF_GROUP_HOLDER);
        NameObjGroup* pGroup = findGroupFromName_Replica(pHolder, pName);
        if (pGroup == NULL)
            pGroup = createGroup_Replica(pHolder, pName, groupLimit);
        pGroup->registerObj(pObj);
    }

    void onMovementGroup_Replica(const char* pName) {
        MovementOnOffGroupHolder* pHolder = (MovementOnOffGroupHolder*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_MOVEMENT_ON_OFF_GROUP_HOLDER);
        NameObjGroup* pGroup = findGroupFromName_Replica(pHolder, pName);
        if (pGroup == NULL)
            return;
        for (int i = 0; i < pGroup->mNumObjs; i++)
            MR::requestMovementOn(pGroup->mObjs[i]);
    }

    void addTripodBossParts(LiveActor *pActor) {
        ((TripodBossAccesser*)MR::createSceneObj(SCENE_OBJ_DELETED_71))->addTripodBossParts(pActor);
        joinToMovementOnOffGroup_Replica("三脚ボス部品", pActor, 0x100);
    }

    void addTripodBossPartsMovement(NameObj *pObj) {
        joinToMovementOnOffGroup_Replica("三脚ボス部品", pObj, 0x100);
    }

    bool isTripoddBossParts(const NameObj *pObj) {
        return ((TripodBossAccesser*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_DELETED_71))->isTripoddBossParts(pObj);
    }

    void showTripodBossParts() {
        ((TripodBossAccesser*)MR::createSceneObj(SCENE_OBJ_DELETED_71))->showTripodBossParts();
    }

    void hideTripodBossParts() {
        ((TripodBossAccesser*)MR::createSceneObj(SCENE_OBJ_DELETED_71))->hideTripodBossParts();
    }

    void activeTripodBossParts() {
        ((TripodBossAccesser*)MR::createSceneObj(SCENE_OBJ_DELETED_71))->activeTripodBossParts();
    }

    void requestMovementTripodBossParts() {
        MR::onMovementGroup_Replica("三脚ボス部品");
    }

    bool isCreatedTripodBoss() {
        if (!MR::isExistSceneObj(SCENE_OBJ_DELETED_71)) {
            return false;
        }

        return ((TripodBossAccesser*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_DELETED_71))->mBoss != NULL;
    }

    bool isStartedTripodBoss() {
        return ((TripodBossAccesser*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_DELETED_71))->mBoss->isStarted();
    }

    bool isBrokenTripodBoss() {
        return ((TripodBossAccesser*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_DELETED_71))->mBoss->isBroken();
    }

    bool isDemoTripodBoss() {
        return ((TripodBossAccesser*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_DELETED_71))->mBoss->isDemo();
    }

    bool isStartDemoTripodBoss() {
        return ((TripodBossAccesser*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_DELETED_71))->mBoss->isStartDemo();
    }

    bool isDamageDemoTripodBoss() {
        return ((TripodBossAccesser*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_DELETED_71))->mBoss->isDamageDemo();
    }

    bool isEndBreakDownDemoTripodBoss() {
        return ((TripodBossAccesser*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_DELETED_71))->mBoss->isEndBreakDownDemo();
    }

    bool isLeaveMarioNowTripodBoss() {
        return ((TripodBossAccesser*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_DELETED_71))->mBoss->isLeaveMarioNow();
    }

    bool isRideMarioTripodBoss() {
        return ((TripodBossAccesser*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_DELETED_71))->mBoss->isRideMario();
    }
    
    bool tryDamageDemoTripodBoss() {
        return ((TripodBossAccesser*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_DELETED_71))->mBoss->tryDamageDemo();
    }

    void requestEndDamageDemoTripodBoss() {
        ((TripodBossAccesser*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_DELETED_71))->mBoss->requestEndDamageDemo();
    }

    u32 getTripodBossGravityHostID() {
       return (u32)(TripodBossAccesser*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_DELETED_71);
    }

    void setTripodBossJointAttachBaseMatrix(const TPos3f &rPos, s32 id) {
        TripodBoss* boss = ((TripodBossAccesser*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_DELETED_71))->mBoss;
        boss->setJointAttachBaseMatrix(rPos, convertBoneIDToIndex(id));
    }

    void requestStartTripodBossStepSequence(s32 seq) {
        ((TripodBossAccesser*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_DELETED_71))->mBoss->requestStartStepSequence(seq);
    }

    void addTripodBossStepPoint(TripodBossStepPoint *pPoint) {
        ((TripodBossAccesser*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_DELETED_71))->mBoss->addStepPoint(pPoint);
    }

    void getTripodBossJointMatrix(TPos3f *pPos, s32 id) {
        TripodBoss* boss = ((TripodBossAccesser*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_DELETED_71))->mBoss;
        boss->getJointMatrix(pPos, convertBoneIDToIndex(id));
    }

    void getTripodBossAttachJointMatrix(TPos3f *pPos, s32 id) {
        TripodBoss* boss = ((TripodBossAccesser*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_DELETED_71))->mBoss;
        boss->getJointAttachMatrix(pPos, convertBoneIDToIndex(id));
    }

    void concatTripodBossAttachJointMatrix(TPos3f *pPos, s32 id) {
        if (MR::isCreatedTripodBoss()) {
            TPos3f mtx;
            MR::getTripodBossAttachJointMatrix(&mtx, id);
            pPos->concat(mtx, *pPos);
        }
    }

    bool isSteppingTripodBossLegID(s32 id) {
        bool ret = !((TripodBossAccesser*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_DELETED_71))->mBoss->isStopLeg(id);
        return ret;
    }

    bool isSteppingTripodBossJointID(s32 id) {
        s32 partID = TripodBoss::getPartIDFromBoneID(convertBoneIDToIndex(id));
        bool ret = false;

        if (partID >= 0 && partID < 3) {
            ret = true;
        }

        if (ret) {
            return MR::isSteppingTripodBossLegID(partID);
        }
        
        return false;
    }
};

TripodBossAccesser::~TripodBossAccesser() {
    
}
