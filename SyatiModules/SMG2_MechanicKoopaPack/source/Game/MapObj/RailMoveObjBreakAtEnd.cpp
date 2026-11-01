#include "Game/MapObj/RailMoveObjBreakAtEnd.h"

#include "Game/Util/EffectUtil.h"

RailMoveObjBreakAtEnd::RailMoveObjBreakAtEnd(const char* pName) : RailMoveObj(pName) {

}

void RailMoveObjBreakAtEnd::doAtEndPoint() {
    RailMoveObj::doAtEndPoint();

    if (MR::isRegisteredEffect(this, "Break")) {
        MR::emitEffect(this, "Break");
    }

    kill();
}

RailMoveObjBreakAtEnd::~RailMoveObjBreakAtEnd() {

}