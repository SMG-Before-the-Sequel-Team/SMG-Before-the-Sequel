#include "Game/MapObj/SubmarineVolcanoBigColumn.h"
#include "Game/MapObj/AirBubbleHolder.h"
#include "Game/LiveActor/ModelObj.h"
#include "Game/Util/ActorAnimUtil.h"
#include "Game/Util/ActorCameraUtil.h"
#include "Game/Util/ActorInitUtil.h"
#include "Game/Util/ActorMovementUtil.h"
#include "Game/Util/ActorSensorUtil.h"
#include "Game/Util/DemoUtil.h"
#include "Game/Util/JMapUtil.h"
#include "Game/Util/LiveActorUtil.h"
#include "Game/Util/ObjUtil.h"
#include "Game/Util/StringUtil.h"
#include "Game/Util/SoundUtil.h"
#include "Game/Util/ActionSoundUtil.h"

namespace {
    static f32 sShakeDistance = 800.0f;
};

SubmarineVolcanoBigColumn::SubmarineVolcanoBigColumn(const char* pName) : LiveActor(pName) {
    mBreakModel = NULL;
    mIsSmallColumn = false;
}

void SubmarineVolcanoBigColumn::init(const JMapInfoIter& rIter) {
    MR::processInitFunction(this, rIter, false);

    MR::setClippingTypeSphereContainsModelBoundingBox(this, 100.0f);
    MR::joinToGroupArray(this, rIter, NULL, 32);

    const char* objName;
    MR::getObjectName(&objName, rIter);
    mIsSmallColumn = MR::isEqualString("SubmarineVolcanoSmallColumn", objName);
    initBreakModel(objName);

    if (MR::tryRegisterDemoCast(this, rIter)) {
        MR::tryRegisterDemoCast(mBreakModel, rIter);
    }

    if (mIsSmallColumn) {
        MR::createAirBubbleHolder();
    }

    initNerve(&NrvSubmarineVolcanoBigColumn::SubmarineVolcanoBigColumnNrvWait::sInstance, 0);
    makeActorAppeared();
}

void SubmarineVolcanoBigColumn::kill() {
    mBreakModel->kill();
    LiveActor::kill();
}

void SubmarineVolcanoBigColumn::exeWait() {
    if (mIsSmallColumn) {
        if (MR::isStep(this, 360)) {
            TVec3f upVec;
            MR::calcUpVec(&upVec, this);
            TVec3f v11;
            v11.scale(125.0f, upVec);
            TVec3f pos;
            pos.set(v11);
            pos.add(mTranslation);
            MR::appearAirBubble(pos, 360);
            setNerve(&NrvSubmarineVolcanoBigColumn::SubmarineVolcanoBigColumnNrvWait::sInstance);
        }
    }
}

void SubmarineVolcanoBigColumn::exeBreak() {
    if (MR::isFirstStep(this)) {
        MR::startRumbleWithShakeCameraWeak(this, "‹­", "Žã", sShakeDistance, 3.4028235e38f);
        MR::hideModel(this);
        MR::invalidateHitSensors(this);
        MR::invalidateCollisionParts(this);
        mBreakModel->appear();
        MR::startBck(mBreakModel, "Break", NULL);

        MR::startActionSound(this, mIsSmallColumn ? "OjSmnVolcColSBreak" : "OjSmnVolcColLBreak", -1, -1, -1);
    }

    if (MR::isBckStopped(mBreakModel)) {
        kill();
    }
}

void SubmarineVolcanoBigColumn::pauseOff() {
    MR::requestMovementOn(this);
    MR::requestMovementOn(mBreakModel);
}

bool SubmarineVolcanoBigColumn::receiveMsgEnemyAttack(u32 msg, HitSensor*, HitSensor*) {
    if (isNerve(&NrvSubmarineVolcanoBigColumn::SubmarineVolcanoBigColumnNrvWait::sInstance)) {
        setNerve(&NrvSubmarineVolcanoBigColumn::SubmarineVolcanoBigColumnNrvBreak::sInstance);
        return true;
    }

    return false;
}

void SubmarineVolcanoBigColumn::initBreakModel(const char* pModelName) {
    char buf[256];
    snprintf(buf, sizeof(buf), "%sBreak", pModelName);
    mBreakModel = MR::createModelObjMapObj("ŠC’ê‰ÎŽRÎ’Œ‰ó‚êƒ‚ƒfƒ‹", buf, getBaseMtx());
    MR::invalidateClipping(mBreakModel);
    mBreakModel->makeActorDead();
}

SubmarineVolcanoBigColumn::~SubmarineVolcanoBigColumn() {

}

namespace NrvSubmarineVolcanoBigColumn {
    void SubmarineVolcanoBigColumnNrvWait::execute(Spine* pSpine) const {
        ((SubmarineVolcanoBigColumn*)pSpine->mExecutor)->exeWait();
    }

    void SubmarineVolcanoBigColumnNrvBreak::execute(Spine* pSpine) const {
        ((SubmarineVolcanoBigColumn*)pSpine->mExecutor)->exeBreak();
    }

    SubmarineVolcanoBigColumnNrvWait(SubmarineVolcanoBigColumnNrvWait::sInstance);
    SubmarineVolcanoBigColumnNrvBreak(SubmarineVolcanoBigColumnNrvBreak::sInstance);
};