#include "Game/Boss/TombSpiderEnvironment.h"
#include "Game/LiveActor/LodCtrl.h"
#include "Game/LiveActor/ModelObj.h"
#include "Game/Map/PlanetMap.h"
#include "Game/MapObj/SpiderThread.h"
#include "Game/Scene/SceneObjHolder.h"

#include "Game/Util.h"
#include "Game/NameObj/NameObjCategories.h"
#include "SpiderThreadExt.h"


TombSpiderEnvironment::TombSpiderEnvironment(LiveActor* pActor) : mActor(pActor), mPlanet(nullptr), mCocoon(nullptr) {
    MR::setEffectHostSRT(mActor, "Noctiluca", MR::getPlayerPos(), nullptr, nullptr);
    MR::createSceneObj(SCENE_OBJ_SPIDER_THREAD);
    
    MR::initSpiderThread(mActor->mTranslation);
}

void TombSpiderEnvironment::initPlanet(const JMapInfoIter& rIter) {
    mPlanet = new PlanetMap("戦場[トゥームスパイダー]", "TombSpiderPlanet");
    mPlanet->mTranslation.set(mActor->mTranslation);
    mPlanet->initWithoutIter();
    mPlanet->mLodCtrl->setDistanceToLow(25000.0f);
    MR::registerDemoSimpleCastAll(mPlanet);
    mPlanet->appear();
}

// The rIter is futureproofing
void TombSpiderEnvironment::initCocoon(const JMapInfoIter& rIter) {
    mCocoon = new ModelObj("まゆ[トゥームスパイダー]", "TombSpiderCocoon", nullptr, MR::CategoryList_Auto, MR::CategoryList_Auto, MR::CategoryList_Auto, false);
    mCocoon->mTranslation.set(mActor->mTranslation);
    mCocoon->initWithoutIter();
    MR::invalidateClipping(mCocoon);
    MR::registerDemoSimpleCastAll(mCocoon);
    mCocoon->kill();
}