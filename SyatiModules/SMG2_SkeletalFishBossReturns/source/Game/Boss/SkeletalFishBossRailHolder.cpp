#include "Game/Boss/SkeletalFishBossRailHolder.h"
#include "Game/Boss/SkeletalFishBossRail.h"
#include "Game/Scene/SceneObjHolder.h"

SkeletalFishBossRailHolder::SkeletalFishBossRailHolder(const char* pName) : NameObj(pName) {
    mNumRails = 0;

    for (s32 i = 0; i < 3; i++) {
        mRails[i] = NULL;
    }
}

void SkeletalFishBossRailHolder::add(SkeletalFishBossRail* pRail) {
    mRails[mNumRails++] = pRail;
}

SkeletalFishBossRail* SkeletalFishBossRailHolder::getInOrder(s32 idx) const {
    return mRails[idx];
}

SkeletalFishBossRail* SkeletalFishBossRailHolder::getByID(s32 id) const {
    for (u32 i = 0; i < mNumRails; i++) {
        SkeletalFishBossRail* rail = getInOrder(i);

        if (id == rail->_8C) {
            return rail;
        }
    }

    return mRails[0];
}

namespace MR {
    SkeletalFishBossRailHolder* getSkeletalFishBossRailHolder() {
        return (SkeletalFishBossRailHolder*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_SKELETAL_FISH_RAIL_HOLDER);
    }

    void createSkeletalFishBossRailHolder() {
        MR::createSceneObj(SCENE_OBJ_SKELETAL_FISH_RAIL_HOLDER);
    }
};

SkeletalFishBossRailHolder::~SkeletalFishBossRailHolder() {

}