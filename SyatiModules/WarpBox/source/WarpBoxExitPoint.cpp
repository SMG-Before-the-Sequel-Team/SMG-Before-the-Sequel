#include "WarpBoxExitPoint.h"

WarpBoxExitPoint::WarpBoxExitPoint (const char *pName) : NameObj(pName) {
    mPosition = TVec3f(0);
}

WarpBoxExitPoint::~WarpBoxExitPoint () {

}

void WarpBoxExitPoint::init (const JMapInfoIter &rIter) {
    OSReport("Hello\n");
    MR::connectToSceneMapObjMovement(this);
    MR::getJMapInfoTrans(rIter, &mPosition);
    WarpBoxMgr::getManager()->entry(this, rIter);
}