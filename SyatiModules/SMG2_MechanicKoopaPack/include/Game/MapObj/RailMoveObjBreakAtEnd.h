#pragma once

#include "Game/MapObj/RailMoveObj.h"

class RailMoveObjBreakAtEnd : public RailMoveObj {
public:
    RailMoveObjBreakAtEnd(const char*);

    virtual ~RailMoveObjBreakAtEnd();
    virtual void doAtEndPoint();
};