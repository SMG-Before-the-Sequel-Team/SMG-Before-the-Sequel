#include "revolution/types.h"
#include "Kamek/hooks.h"
#include "Game/LiveActor/LiveActor.h"
#include "Game/Screen/LayoutActor.h"
#include "Game/Screen/StarPointerCommandStream.h"
#include "Game/Screen/StarPointerBlur.h"
#include "Game/Screen/StarPointerLayout.h"
#include "Game/System/StarPointerOnOffController.h"
#include "Game/Util/StarPointerUtil.h"
#include "Game/Util.h"
#include "CommandStream.h"
#include "ModuleData_StarPointerLayout_Ext.h"
#include "ModuleData_ExtStarPointer.h"

#if defined(SB4E) || defined(SB4P) || defined(SB4J)
#define REGION_OFFSET 0x00
#elif defined(SB4K) || defined(SB4W)
#define REGION_OFFSET 0x70
#else
#define REGION_OFFSET 0x00
#endif // 

#define WPAD_CHAN0 0
#define WPAD_CHAN1 1

static GXColor sColorA[] = {
    {0x00, 0x32, 0xFF, 0xFF},
    {0xFF, 0xFF, 0x00, 0xFF},
    {0x55, 0xFF, 0x00, 0xFF},
    {0x7F, 0x7F, 0x00, 0xFF},
};

namespace {
    inline StarPointerDirector* getStarPointerDirector() {
        return StarPointerFunction::getStarPointerDirector();
    }

    inline StarPointerLayout* getStarPointerLayout(s32 channel) {
        return getStarPointerDirector()->getStarPointerLayout(channel);
    }

}  // namespace

extern "C" {
    StarPointerOnOffController* __kAutoMap_8005BCB0(); // getStarPointerOnOffController__29@unnamed@StarPointerUtil_cpp@Fv
}

// -------------------------------------------------

kmWrite16(0x8049A4A8 + REGION_OFFSET + 0x02, (sizeof(StarPointerLayout_Ext) + 8) * 2); // ...

kmWrite16(0x8049A4F8 + REGION_OFFSET + 0x02, sizeof(StarPointerLayout_Ext)); // StarPointerDirector::createLayout
kmWrite16(0x8049A7A0 + REGION_OFFSET + 0x02, sizeof(StarPointerLayout_Ext)); // StarPointerDirector::getStarPointerLayout
kmWrite16(0x8049B048 + REGION_OFFSET + 0x02, sizeof(StarPointerLayout_Ext)); // StarPointerFunction::canShoot
kmWrite16(0x8049A468 + REGION_OFFSET + 0x02, sizeof(StarPointerLayout_Ext)); // StarPointerDirector::draw
kmWrite16(0x8049A2BC + REGION_OFFSET + 0x02, sizeof(StarPointerLayout_Ext)); // StarPointerDirector::update
kmWrite16(0x8049A3A4 + REGION_OFFSET + 0x02, sizeof(StarPointerLayout_Ext)); // StarPointerDirector::update


// init the command stream
void initCommandStream_StarPointerLayout(StarPointerLayout_Ext* pLayout, const Nerve* initnerve) {
	pLayout->initNerve(initnerve);

	pLayout->mCommandStream = new StarPointerCommandStream(&pLayout->mPosition);
	pLayout->mCommandStream->setPortAndColor(pLayout->mPadChannel, &sColorA[pLayout->mPadChannel]);
	pLayout->mCommandStream->initWithoutIter();
}
kmCall(0x8049CB0C + REGION_OFFSET, initCommandStream_StarPointerLayout);

void calcAnim_StarPointerLayout(StarPointerLayout_Ext* pLayout) {
    pLayout->LayoutActor::calcAnim();
    pLayout->mCommandStream->calcAnim();
}
kmCall(0x8049DE54 + REGION_OFFSET, calcAnim_StarPointerLayout);

void drawDecoration_StarPointerLayout(StarPointerLayout_Ext* pLayout) {
    // I guess we fully replacing this one
    if (pLayout->mPointerKind == StarPointerKind_HandPointer ||
        pLayout->mPointerKind == StarPointerKind_2 ||
        pLayout->mPointerKind == StarPointerKind_FingerPointer ||
        pLayout->mPointerKind == StarPointerKind_8 || // NEW to SMG2
        (pLayout->mPointerKind == StarPointerKind_5 && MR::testCorePadButtonA(pLayout->mPadChannel))) {
        pLayout->mBlur->hideAll();
        return;
    }

    MR::loadProjectionMtxFor2D();
    pLayout->mBlur->draw();
    pLayout->mCommandStream->draw();
}
kmBranch(0x8049DF30 + REGION_OFFSET, drawDecoration_StarPointerLayout);


void changeLayoutShow_StarPointerLayout(StarPointerLayout_Ext* pLayout) {
    if (pLayout->mCommandStream->mWorldPos == nullptr) {
        pLayout->mAnimType = AnimType_NULL;
    }
    else {
        pLayout->mCommandStream->show();
    }
    pLayout->mRadius = 15.0f;
}
kmCall(0x8049E648 + REGION_OFFSET, changeLayoutShow_StarPointerLayout);
kmWrite32(0x8049E64C + REGION_OFFSET, PPC_NOP);
kmWrite32(0x8049E650 + REGION_OFFSET, PPC_NOP);
kmWrite32(0x8049E654 + REGION_OFFSET, PPC_NOP);

void changeLayoutHide_StarPointerLayout(register StarPointerLayout_Ext* pLayout) {
    __asm {
        stfs f0, 0x44(pLayout)
    }
    pLayout->mCommandStream->hide();
}
kmCall(0x8049E660 + REGION_OFFSET, changeLayoutHide_StarPointerLayout);
kmCall(0x8049E66C + REGION_OFFSET, changeLayoutHide_StarPointerLayout);
kmCall(0x8049E678 + REGION_OFFSET, changeLayoutHide_StarPointerLayout);
kmCall(0x8049E684 + REGION_OFFSET, changeLayoutHide_StarPointerLayout);
kmCall(0x8049E690 + REGION_OFFSET, changeLayoutHide_StarPointerLayout);
kmCall(0x8049E69C + REGION_OFFSET, changeLayoutHide_StarPointerLayout);

void changeLayoutHideALT_StarPointerLayout() {
    register StarPointerLayout_Ext* pLayout;
    __asm {
        mr pLayout, r30
        stfs f0, 0x44(pLayout)
    }
    pLayout->mCommandStream->hide();
}
kmCall(0x8049E6B0 + REGION_OFFSET, changeLayoutHideALT_StarPointerLayout);

void updateDecoration__StarPointerLayout(register StarPointerLayout_Ext* pLayout) {
    // Fully replacing this one too

    if (pLayout->mPointerKind == StarPointerKind_HandPointer || pLayout->mPointerKind == StarPointerKind_2) {
        return;
    }

    if (pLayout->mCommandStream->mWorldPos != nullptr) {
        pLayout->mCommandStream->movement();
    }
    else {
        pLayout->mBlur->movement();
    }
}
kmBranch(0x8049ED10 + REGION_OFFSET, updateDecoration__StarPointerLayout);




void exeGrip_StarPointerLayout(StarPointerLayout_Ext* pActor, const char* pPaneName, const char* pAnimName, u32 t) {
    if (pActor->mCommandStream->mWorldPos != nullptr) {
        pActor->mCommandStream->show();
        pActor->startAnimHandGuu();
    }
    else {
        pActor->changeToStarPointer();
        MR::startPaneAnim(pActor, pPaneName, pAnimName, t);
        pActor->mAnimType = AnimType_StarPointer;
    }
}
kmWrite32(0x8049CF74 + REGION_OFFSET, PPC_NOP);
kmCall(0x8049CF88 + REGION_OFFSET, exeGrip_StarPointerLayout);
kmWrite32(0x8049CF90 + REGION_OFFSET, PPC_NOP);

// Also works for exeHold!
kmWrite32(0x8049D1F8 + REGION_OFFSET, PPC_NOP);
kmCall(0x8049D20C + REGION_OFFSET, exeGrip_StarPointerLayout);
kmWrite32(0x8049D214 + REGION_OFFSET, PPC_NOP);


bool exeOutScreen_StarPointerLayout(StarPointerLayout_Ext* pActor, s32 step) {
    if (pActor->mCommandStream->mWorldPos != nullptr && MR::testCorePadButtonA(pActor->mPadChannel)) {
        pActor->setNerve(&NrvStarPointerLayout::HostTypeNrvOutScreen::sInstance);
        return false; // Skips the code in the caller
    }
    return MR::isGreaterStep(pActor, step);
}
kmCall(0x8049DAAC + REGION_OFFSET, exeOutScreen_StarPointerLayout);


void hideAll_StarPointerLayout(StarPointerLayout_Ext* pActor) {
    pActor->hideBlur();
    pActor->mCommandStream->hide();
}
kmWrite32(0x8049DB00 + REGION_OFFSET, PPC_NOP);
kmCall(0x8049DB04 + REGION_OFFSET, hideAll_StarPointerLayout);

// Also works for exeHide, so long as we move the register
kmWrite32(0x8049DD2C + REGION_OFFSET, PPC_MR(3, 29)); // mr r3, r29
kmCall(0x8049DD30 + REGION_OFFSET, hideAll_StarPointerLayout);

// -------------------------------------------------

bool startCommandStream_StarPointerLayout(StarPointerLayout_Ext* pLayout, const LiveActor* pActor, const TVec3f* pPos, bool b) {
    if (pLayout->mCommandStream->mWorldPos != nullptr) {
        return false;
    }

    pLayout->mCommandStream->start(pPos, b);
    pLayout->hideBlur();
    pLayout->mActor = pActor;
    //MR::startStarPointerModeCommandStream(this);
    {
        StarPointerOnOffController* onoff = __kAutoMap_8005BCB0();
        onoff->incModeCounter((void*)pLayout, (s32)ExtStarPointerMode_CommandStream);
    }
    return true;
}

void forceEndCommandStream_StarPointerLayout(StarPointerLayout_Ext* pLayout) {
    pLayout->mCommandStream->hide();
    pLayout->mCommandStream->clear();
    pLayout->mBlur->show();
    pLayout->mActor = nullptr;
}
void endCommandStream_StarPointerLayout(StarPointerLayout_Ext* pLayout, const LiveActor* pActor) { // LiveActor for no reason??
    forceEndCommandStream_StarPointerLayout(pLayout);
    MR::endStarPointerMode(pLayout);
}
bool isCommandStream_StarPointerLayout(StarPointerLayout_Ext* pLayout, const LiveActor* pActor) {
    return pLayout->mCommandStream->mWorldPos != nullptr && pLayout->mActor == pActor;
}

// -------------------------------------------------

namespace MR {
    bool tryStartStarPointerCommandStream(const LiveActor* pActor, const TVec3f* pVec, s32 channel, bool b) {
        return startCommandStream_StarPointerLayout((StarPointerLayout_Ext*)getStarPointerLayout(channel), pActor, pVec, b);
    }
    
    bool tryEndStarPointerCommandStream(const LiveActor* pActor, s32 channel) {
        StarPointerLayout_Ext* layout = (StarPointerLayout_Ext*)getStarPointerLayout(channel);
        if (isCommandStream_StarPointerLayout(layout, pActor)) {
            endCommandStream_StarPointerLayout(layout, pActor);
            return true;
        }
        return false;
    }

    bool isStarPointerCommandStream(const LiveActor* pActor, s32 channel) {
        return isCommandStream_StarPointerLayout((StarPointerLayout_Ext*)getStarPointerLayout(channel), pActor);
    }
}

void setupCommandStream_StarPointerOnOffController(StarPointerOnOffController* pCtrl) {
    getStarPointerLayout(WPAD_CHAN0)->show();
    getStarPointerLayout(WPAD_CHAN1)->show();
    getStarPointerDirector()->startStarPointer();
    getStarPointerLayout(WPAD_CHAN0)->mIsPointerValid = true;
    getStarPointerLayout(WPAD_CHAN1)->mIsPointerValid = true;
    MR::activeStarPointerGuidance();
    MR::disableStarPointerShootStarPiece();
    getStarPointerDirector()->mIsAllowP2StarPieceShot = true;
}

void forceEndCommandStreamOnSceneOut() {
    forceEndCommandStream_StarPointerLayout((StarPointerLayout_Ext*)getStarPointerLayout(WPAD_CHAN0));
    forceEndCommandStream_StarPointerLayout((StarPointerLayout_Ext*)getStarPointerLayout(WPAD_CHAN1));
}
kmBranch(0x803FBDB0, forceEndCommandStreamOnSceneOut); // This one is region free