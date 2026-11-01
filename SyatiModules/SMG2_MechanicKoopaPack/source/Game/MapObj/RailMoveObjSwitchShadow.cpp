#include "Game/MapObj/RailMoveObjSwitchShadow.h"

#include "Game/Util/ActorShadowUtil.h"
#include "Game/Util/ActorSwitchUtil.h"

void RailMoveObjSwitchShadow::init(const JMapInfoIter& rIter) {
    RailMoveObj::init(rIter);
    MR::invalidateShadow(this, 0);
}

void RailMoveObjSwitchShadow::initCaseUseSwitchA(const MapObjActorInitInfo& rInfo) {
    void (RailMoveObjSwitchShadow:: * startFunc)(void) = &RailMoveObjSwitchShadow::startOnShadow;
    MR::listenStageSwitchOnA(this, MR::Functor(this, startFunc));
}

void RailMoveObjSwitchShadow::startOnShadow() {
    MR::validateShadow(this, 0);
}