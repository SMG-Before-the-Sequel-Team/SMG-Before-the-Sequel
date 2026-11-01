#include "Game/Boss/SkeletalFishBossBattleDirector.h"
#include "Game/Boss/SkeletalFishBoss.h"
#include "Game/LiveActor/LiveActorGroup.h"
#include "Game/MapObj/SubmarineVolcanoBigColumn.h"
#include "Game/Map/CollisionParts.h"
#include "Game/Map/HitInfo.h"
#include "Game/Util/ActorAnimUtil.h"
#include "Game/Util/ActorSensorUtil.h"
#include "Game/Util/EffectUtil.h"
#include "Game/Util/LiveActorUtil.h"
#include "Game/Util/ObjUtil.h"
#include "Game/Util/SoundUtil.h"

SkeletalFishBossBattleDirector::SkeletalFishBossBattleDirector(SkeletalFishBoss* pBoss) : NameObj("スカルシャークボス戦指揮") {
    mFishBoss = pBoss;
    _90 = 0;
    _D4 = 0;
    MR::connectToSceneEnemyMovement(this);

    for (s32 i = 0; i < 0x20; i++) {
        mColumns[i] = NULL;
    }

    for (s32 i = 0; i < 4; i++) {
        ModelObj* guardMdl = new ModelObj("デモ用スカルシャークガード", "SkeletalFishGuard", NULL, 18, -2, -2, false);
        mGuardModels[i] = guardMdl;
        mGuardModels[i]->initWithoutIter();
        MR::initLightCtrl(mGuardModels[i]);
        MR::invalidateClipping(mGuardModels[i]);
        mGuardModels[i]->makeActorDead();
    }
}

void SkeletalFishBossBattleDirector::initiate() {
    LiveActorGroup* grp = MR::getGroupFromArray(mFishBoss);

    for (s32 i = 0; i < grp->mNumObjs; i++) {
        if (!strcmp(grp->getActor(i)->mName, "海底火山巨大石柱") || (!strcmp(grp->getActor(i)->mName, "海底火山石柱（小）"))) {
            mColumns[_90] = static_cast<SubmarineVolcanoBigColumn*>(grp->getActor(i));
            _90++;
        }

        if (strcmp(grp->getActor(i)->mName, "羽虫Ｓ")) {
            if (strcmp(grp->getActor(i)->mName, "羽虫Ｌ")) {
                continue;
            }
        }

        _94[_D4] = grp->getActor(i);
        MR::invalidateClipping(_94[_D4]);
        _D4++;
    }

    killBirdLouse();
}

void SkeletalFishBossBattleDirector::startPowerUpDemo1() {
    playGuardAnim("PowerUp1", 2);
}

void SkeletalFishBossBattleDirector::startPowerUpDemo2() {
    playGuardAnim("PowerUp2", 4);
}

void SkeletalFishBossBattleDirector::playGuardAnim(const char* pAnimName, s32 numGuards) {
    for (s32 i = 0; i < numGuards; i++) {
        char buf[0x80];
        snprintf(buf, sizeof(buf), "%s%c", pAnimName, i + 0x41);
        mGuardModels[i]->makeActorAppeared();
        MR::requestMovementOn(mGuardModels[i]);
        MR::startBck(mGuardModels[i], buf, NULL);
        MR::startSound(mGuardModels[i], "SE_BM_SKL_GUARD_APPEAR", -1, -1);
    }
}

void SkeletalFishBossBattleDirector::movement() {

}

void SkeletalFishBossBattleDirector::tryColumnCollision(HitSensor* pSensor) {
    for (s32 i = 0; i < _90; i++) {
        CollisionParts* parts = MR::getCollisionParts(mColumns[i]);

        if (parts != NULL) {
            HitInfo hitInfo;
            if (parts->checkStrikeBall(&hitInfo, 1, pSensor->mPosition, pSensor->mRadius, false, NULL)) {
                if (hitInfo.mParentTriangle.mSensor != NULL) {
                    MR::sendMsgEnemyAttack(hitInfo.mParentTriangle.mSensor, pSensor);
                    return;
                }
            }
        }
    }
}

void SkeletalFishBossBattleDirector::pauseOffCast() {
    LiveActorGroup* group = MR::getGroupFromArray(mFishBoss);

    for (s32 i = 0; i < group->mNumObjs; i++) {
        MR::requestMovementOn(group->getActor(i));
        if (MR::isExistEffectKeeper(group->getActor(i))) {
            MR::pauseOffEffectAll(group->getActor(i));
        }
    }

    for (s32 i = 0; i < _90; i++) {
        mColumns[i]->pauseOff();
    }
}

void SkeletalFishBossBattleDirector::endPowerUpDemo1() {
    killGuard();
}

void SkeletalFishBossBattleDirector::endPowerUpDemo2() {
    killGuard();
}

void SkeletalFishBossBattleDirector::killGuard() {
    for (s32 i = 0; i < 4; i++) {
        mGuardModels[i]->makeActorDead();
    }
}

void SkeletalFishBossBattleDirector::appearBirdLouse() {
    for (s32 i = 0; i < _D4; i++) {
        _94[i]->endClipped();
    }
}

void SkeletalFishBossBattleDirector::killBirdLouse() {
    for (s32 i = 0; i < _D4; i++) {
        _94[i]->startClipped();
    }
}

SkeletalFishBossBattleDirector::~SkeletalFishBossBattleDirector() {

}