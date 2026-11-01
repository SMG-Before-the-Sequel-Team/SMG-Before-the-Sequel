#include "Game/Boss/TripodBossStepStartArea.h"
#include "Game/Boss/TripodBossAccesser.h"
#include "Game/Util.h"

TripodBossStepStartArea::TripodBossStepStartArea(const char *pName) : AreaObj(pName) {
    _3C = -1;
}

void TripodBossStepStartArea::init(const JMapInfoIter &rIter) {
    AreaObj::init(rIter);
    MR::connectToSceneAreaObj(this);
    _3C = mObjArg0;
}

void TripodBossStepStartArea::movement() {
    if (isInVolume(*MR::getPlayerPos())) {
        MR::requestStartTripodBossStepSequence(_3C);
    }
}

TripodBossStepStartArea::~TripodBossStepStartArea() {
    
}

const char* TripodBossStepStartArea::getManagerName() const {
    return "TripodBossStepStart";
}
