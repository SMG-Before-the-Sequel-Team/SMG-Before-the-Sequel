#include "Game/MapObj/MechaKoopaPartsArm.h"
#include "Game/Util.h"

MechaKoopaPartsArm::MechaKoopaPartsArm(const char* pName) : MapObjActor(pName) {
    mIsRightArm = false;
}

void MechaKoopaPartsArm::init(const JMapInfoIter& rIter) {
    MapObjActor::init(rIter);
    MR::getJMapInfoArg0NoInit(rIter, &mIsRightArm);
    MapObjActorInitInfo info;
    info.setupNerve(&NrvMechaKoopaPartsArm::MechaKoopaPartsArmNrvWaitRear::sInstance);
    info.setupRotator();

    if (mIsRightArm) {
        info.setupRailMover();
        info.setupBaseMtxFollowTarget();
    }

    initialize(rIter, info);
    MapObjActorUtil::startRotator(this);
}

void MechaKoopaPartsArm::exeWait() {
    if (MapObjActorUtil::isRotatorMoving(this)) {
        if (isNerve(&NrvMechaKoopaPartsArm::MechaKoopaPartsArmNrvWaitFront::sInstance)) {
            setNerve(&NrvMechaKoopaPartsArm::MechaKoopaPartsArmNrvMoveStartRear::sInstance);
        }
        else {
            setNerve(&NrvMechaKoopaPartsArm::MechaKoopaPartsArmNrvMoveStartFront::sInstance);
        }
    }
}

void MechaKoopaPartsArm::exeMoveStart() {
    if (MR::isFirstStep(this)) {
        MapObjActorUtil::pauseRotator(this);

        if (isNerve(&NrvMechaKoopaPartsArm::MechaKoopaPartsArmNrvMoveStartFront::sInstance)) {
            MR::startBck(this, "Front", NULL);
        }
        else {
            MR::startBck(this, "Rear", NULL);
        }

        MR::startActionSound(this, "OjMKoopaArmStart", -1, -1, -1);
        if (MR::isOnPlayer(this)) {
            MR::shakeCameraNormal();
            MR::tryRumblePadMiddle(this, 0);
        }
    }

    if (MR::isBckStopped(this)) {
        if (isNerve(&NrvMechaKoopaPartsArm::MechaKoopaPartsArmNrvMoveStartFront::sInstance)) {
            setNerve(&NrvMechaKoopaPartsArm::MechaKoopaPartsArmNrvMoveFront::sInstance);
        }
        else {
            setNerve(&NrvMechaKoopaPartsArm::MechaKoopaPartsArmNrvMoveRear::sInstance);
        }
    }
}

void MechaKoopaPartsArm::exeMove() {
    MR::startActionSound(this, "OjLvMKoopaArmMove", -1, -1, -1);

    if (!MapObjActorUtil::isRotatorMoving(this)) {
        MR::startActionSound(this, "OjMKoopaArmEnd", -1, -1, -1);

        if (isNerve(&NrvMechaKoopaPartsArm::MechaKoopaPartsArmNrvMoveFront::sInstance)) {
            setNerve(&NrvMechaKoopaPartsArm::MechaKoopaPartsArmNrvWaitFront::sInstance);
        }
        else {
            setNerve(&NrvMechaKoopaPartsArm::MechaKoopaPartsArmNrvWaitRear::sInstance);
        }
    }
}

void MechaKoopaPartsArm::exeBreakStart() {
    if (MR::isFirstStep(this)) {
        MR::startBck(this, "BreakStart", NULL);
        MR::startActionSound(this, "OjMKoopaArmFallSt", -1, -1, -1);
        MapObjActorUtil::pauseRotator(this);
    }

    if (MR::isBckStopped(this)) {
        setNerve(&NrvMechaKoopaPartsArm::MechaKoopaPartsArmNrvBreak::sInstance);
    }
}

void MechaKoopaPartsArm::exeBreak() {
    if (MR::isFirstStep(this)) {
        MR::emitEffect(this, "Spark");
        MR::emitEffect(this, "BlackSmoke");
        MapObjActorUtil::startRailMover(this);
    }

    MR::startActionSound(this, "OjLvMKoopaArmFall", -1, -1, -1);
    if (mRailMover->isReachedEnd()) { //have to replace MapObjActorUtil::isRailMoverReachedEnd(this)
        MR::startActionSound(this, "OjMKoopaArmBreak", -1, -1, -1);
        kill();
    }
}

void MechaKoopaPartsArm::initCaseUseSwitchB(const MapObjActorInitInfo& rInfo) {
    if (mIsRightArm) {
        MR::FunctorV0M<MechaKoopaPartsArm*, void (MechaKoopaPartsArm::*)()> breakFunc = MR::Functor<MechaKoopaPartsArm>(this, &MechaKoopaPartsArm::startBreak);
        MR::listenStageSwitchOnB(this, breakFunc);
    }
}

void MechaKoopaPartsArm::initCaseNoUseSwitchB(const MapObjActorInitInfo&) {

}

void MechaKoopaPartsArm::startBreak() {
    setNerve(&NrvMechaKoopaPartsArm::MechaKoopaPartsArmNrvBreakStart::sInstance);
}

namespace NrvMechaKoopaPartsArm {
    MechaKoopaPartsArmNrvWaitFront(MechaKoopaPartsArmNrvWaitFront::sInstance);
    MechaKoopaPartsArmNrvWaitRear(MechaKoopaPartsArmNrvWaitRear::sInstance);
    MechaKoopaPartsArmNrvMoveStartFront(MechaKoopaPartsArmNrvMoveStartFront::sInstance);
    MechaKoopaPartsArmNrvMoveStartRear(MechaKoopaPartsArmNrvMoveStartRear::sInstance);
    MechaKoopaPartsArmNrvMoveFront(MechaKoopaPartsArmNrvMoveFront::sInstance);
    MechaKoopaPartsArmNrvMoveRear(MechaKoopaPartsArmNrvMoveRear::sInstance);
    MechaKoopaPartsArmNrvBreakStart(MechaKoopaPartsArmNrvBreakStart::sInstance);
    MechaKoopaPartsArmNrvBreak(MechaKoopaPartsArmNrvBreak::sInstance);

    void MechaKoopaPartsArmNrvBreak::execute(Spine* pSpine) const {
        MechaKoopaPartsArm* arm = reinterpret_cast<MechaKoopaPartsArm*>(pSpine->mExecutor);
        arm->exeBreak();
    }

    void MechaKoopaPartsArmNrvBreakStart::execute(Spine* pSpine) const {
        MechaKoopaPartsArm* arm = reinterpret_cast<MechaKoopaPartsArm*>(pSpine->mExecutor);
        arm->exeBreakStart();
    }

    void MechaKoopaPartsArmNrvMoveRear::execute(Spine* pSpine) const {
        MechaKoopaPartsArm* arm = reinterpret_cast<MechaKoopaPartsArm*>(pSpine->mExecutor);
        arm->exeMove();
    }

    void MechaKoopaPartsArmNrvMoveFront::execute(Spine* pSpine) const {
        MechaKoopaPartsArm* arm = reinterpret_cast<MechaKoopaPartsArm*>(pSpine->mExecutor);
        arm->exeMove();
    }

    void MechaKoopaPartsArmNrvMoveStartRear::executeOnEnd(Spine* pSpine) const {
        MechaKoopaPartsArm* arm = reinterpret_cast<MechaKoopaPartsArm*>(pSpine->mExecutor);
        MapObjActorUtil::resumeAllMapPartsFunctions(arm);
    }

    void MechaKoopaPartsArmNrvMoveStartRear::execute(Spine* pSpine) const {
        MechaKoopaPartsArm* arm = reinterpret_cast<MechaKoopaPartsArm*>(pSpine->mExecutor);
        arm->exeMoveStart();
    }

    void MechaKoopaPartsArmNrvMoveStartFront::executeOnEnd(Spine* pSpine) const {
        MechaKoopaPartsArm* arm = reinterpret_cast<MechaKoopaPartsArm*>(pSpine->mExecutor);
        MapObjActorUtil::resumeAllMapPartsFunctions(arm);
    }

    void MechaKoopaPartsArmNrvMoveStartFront::execute(Spine* pSpine) const {
        MechaKoopaPartsArm* arm = reinterpret_cast<MechaKoopaPartsArm*>(pSpine->mExecutor);
        arm->exeMoveStart();
    }

    void MechaKoopaPartsArmNrvWaitRear::execute(Spine* pSpine) const {
        MechaKoopaPartsArm* arm = reinterpret_cast<MechaKoopaPartsArm*>(pSpine->mExecutor);
        arm->exeWait();
    }

    void MechaKoopaPartsArmNrvWaitFront::execute(Spine* pSpine) const {
        MechaKoopaPartsArm* arm = reinterpret_cast<MechaKoopaPartsArm*>(pSpine->mExecutor);
        arm->exeWait();
    }
};

MechaKoopaPartsArm::~MechaKoopaPartsArm() {

}