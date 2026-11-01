#pragma once

#include "syati.h"
#include "WarpBoxMgr.h"

class WarpBoxExitPoint : public NameObj {
public:
    WarpBoxExitPoint(const char *pName);
    ~WarpBoxExitPoint();
    virtual void init(const JMapInfoIter &rIter);
    TVec3f mPosition;
};