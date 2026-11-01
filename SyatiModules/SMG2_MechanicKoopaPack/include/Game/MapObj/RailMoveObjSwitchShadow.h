#pragma once

#include "Game/MapObj/RailMoveObj.h"

class RailMoveObjSwitchShadow : public RailMoveObj {
public:
    inline RailMoveObjSwitchShadow(const char* pName) : RailMoveObj(pName) {

    }

    virtual ~RailMoveObjSwitchShadow();
    virtual void init(const JMapInfoIter&);
    virtual void initCaseUseSwitchA(const MapObjActorInitInfo&);

    void startOnShadow();
};