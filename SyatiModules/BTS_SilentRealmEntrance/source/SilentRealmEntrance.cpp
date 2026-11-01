#include "SilentRealmEntrance.h"
#include "C:\SyatiManager\Modules (BTS)\GLE_API\include\GalaxyLevelEngine.h"
#include "Game/MapObj/MapObjActor.h"
#include "Game/MapObj/MapObjActorInitInfo.h"
#include "Game/Util/ActorAnimUtil.h"
#include "Game/Util/ActorSwitchUtil.h"
#include "Game/Util/DemoUtil.h"
#include "Game/Util/FileUtil.h"
#include "Game/Util/JMapInfo.h"
#include "Game/Util/JMapUtil.h"
#include "Game/Util/ObjUtil.h"

SilentRealmEntrance::SilentRealmEntrance(const char *pName)
    : MapObjActor(pName) {};

void SilentRealmEntrance::init(const JMapInfoIter &rIter) {
  MapObjActor::init(rIter);
  MapObjActorInitInfo info;
  MapObjActorUtil::setupInitInfoSimpleMapObj(&info);
  MapObjActorUtil::setupInitInfoTypical(&info, mObjName);
  MapObjActor::initialize(rIter, info);
  MR::tryRegisterDemoCast(this, rIter);

  MR::useStageSwitchReadA(this, rIter);

  mChangeSceneIndex = 0;
  MR::getJMapInfoArg0NoInit(rIter, &mChangeSceneIndex);

  mColorFrame = 0;
  MR::getJMapInfoArg1NoInit(rIter, &mColorFrame);

  MR::startBrk(this, "ColorChange");
  MR::setBrkFrameAndStop(this, mColorFrame);

  // [NOTE] Takes the main galaxy's ChangeSceneListInfo!
  mTable = GLE::getChangeSceneListInfoFromZone(0);
}

void SilentRealmEntrance::control() {
  if (MR::isValidSwitchA(this)) {
    if (MR::isOnSwitchA(this)) {
      GLE::requestMoveStageFromJMapInfo(mTable, mChangeSceneIndex);
    }
  }
}