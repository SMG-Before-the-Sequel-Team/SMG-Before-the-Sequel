#include "ExtBlackHole.h"
#include "Game/Camera/CameraDirector.h"
#include "Game/Camera/CameraTargetArg.h"
#include "Game/AudioLib/AudSceneMgr.h"
#include "Game/AudioLib/AudWrap.h"

/* #ifdef GALAXY_LEVEL_ENGINE
#include "GalaxyLevelEngine.h"
#endif */

void extInitMapToolInfo(ExtBlackHole *pBlackHole, const JMapInfoIter &rIter)
{
    pBlackHole->initMapToolInfo(rIter);
    MR::getJMapInfoArg1WithInit(rIter, &pBlackHole->mIsTeleport);
    MR::getJMapInfoArg2WithInit(rIter, &pBlackHole->mScreenType);
    MR::getJMapInfoArg3WithInit(rIter, &pBlackHole->mUseScream);
    /* #ifdef GALAXY_LEVEL_ENGINE
    MR::getJMapInfoArg7WithInit(rIter, &pBlackHole->mGLEEntry);
    #endif */
}

void extStartPlayerEvent(const char *pDeathEvent)
{
    /* register MarioActor *mMarioH->mMarioActor = 0;
    __asm {
        mr mMarioH->mMarioActor, r31
    } */
    MarioHolder* mMarioH = MR::getMarioHolder();
    ExtBlackHole *mBlackHole = (ExtBlackHole *)mMarioH->mMarioActor->_524;
    if (!mBlackHole->mIsTeleport)
    {
        MR::startPlayerEvent(pDeathEvent);
    }
}

void resumeGameplay()
{
    OSReport("Executing resume\n");
    /* // Don't talk about this, required for hooking...
    __asm {
        lfs f0, -0x7FAC(r2)
        stfs f0, 8(r3)
        stfs f0, 4(r3)
        stfs f0, 0(r3)
    } */
    /* register MarioActor *mMarioH->mMarioActor = 0;
    __asm {
        mr mMarioH->mMarioActor, r31
    } */
    MarioHolder* mMarioH = MR::getMarioHolder();
    ExtBlackHole *mBlackHole = (ExtBlackHole *)mMarioH->mMarioActor->_524;
    mBlackHole->mVelocity.zero();   // I think this was the function that I hooked. In SMG1 is inlined and in SMG2 no? Well, until now this works.
    if (mBlackHole->mIsTeleport)
    {
        if (MR::isStep(mMarioH->mMarioActor, 50) && mBlackHole->mUseScream)
        {
            if (MR::isPlayerElementModeYoshi())
                MR::startSoundPlayer("SE_SY_SV_YOSHI_DEATH_FALL", -1, 1);
            MR::startSoundPlayer("SE_PV_FALL_DIE", -1, -1);
        }
        if (MR::isStep(mMarioH->mMarioActor, 200))
        {
            closeType(mBlackHole->mScreenType);
        }
        if (MR::isStep(mMarioH->mMarioActor, 280))
        {
            /* #ifdef GALAXY_LEVEL_ENGINE  // BUG: If you don't use this feature in a GLE mod the black hole becomes useless after one use.
            if (mBlackHole->mGLEEntry >= 0) {
                JMapInfo* ChangeSceneListInfo = GLE::getChangeSceneListInfoFromZone(0);
                GLE::requestMoveStageFromJMapInfo(ChangeSceneListInfo, mBlackHole->mGLEEntry);
            }
            #endif */
            
            // Mario position
            char pString[15];
            snprintf(pString, sizeof(pString), "BlackHoleTP%03d", mBlackHole->mLinkedInfo.mLinkId); // Today I learnt what is this
            MR::setPlayerPosAndWait(pString);

            // Camera suffering, the best way to fix it was starting a talk camera and then removing it, just that.
            CameraDirector *pCameraDirector = MR::getCameraDirector();
            /* pCameraDirector->endEvent(0, "ブラックホール", false, -1);
            pCameraDirector->resetCameraMan();
            pCameraDirector->push((CameraMan *)pCameraDirector->mCameraManGame); */
            pCameraDirector->setTargetPlayer(mMarioH->mMarioActor);
            pCameraDirector->startTalkCamera(TVec3f(0), TVec3f(0), 0, 0, 0);
            pCameraDirector->endTalkCamera(true, 0);


            // Nerves to as they were before
            MR::trySetNerve(mMarioH->mMarioActor, &NrvMarioActor::MarioActorNrvWait::sInstance); // This is for restoring Mario back to normal gameplay. MarioActor's nerves are not documented yet so I have to do this.
            MR::trySetNerve(mBlackHole, &NrvBlackHole::BlackHoleNrvWait::sInstance);  // Returns the black hole to its original state


            // Re-enable star pointer shooting
            MR::endStarPointerMode(mMarioH->mMarioActor);
            // MR::startStarPointerModeDemoWithStarPointer(mMarioH->mMarioActor);
            MR::startStarPointerModeGame(mMarioH->mMarioActor);
            // MR::enableStarPointerShootStarPiece();
            MR::activateDefaultGameLayout();

            // Mario becomes smaller after using a black hole, this fixes it
            mMarioH->mMarioActor->mScale = TVec3f(1, 1, 1);

            // BGMChangeAreas were disabled, now they won't. Thanks to whoever added this to headers, it saved my code.
            AudSceneMgr* scene = AudWrap::getSceneMgr();
            scene->_C = 0;


            // Resume gameplay. Wow that was a lot of work. Nintendo was not ready to use black holes as portals.
            MR::showPlayer();
            openType(mBlackHole->mScreenType);
            // MR::setNerveAtStep(mBlackHole, &NrvBlackHole::BlackHoleNrvWait::sInstance, 0);
            
        }
    }
    if (!MR::isPlayerElementModeYoshi()) {
        MR::startStarPointerModeGame(0);
    }
}

void closeType(s32 mScreenType) {
    switch (mScreenType)
    {
    case 0:
        MR::closeWipeWhiteFade(60);
        break;
    case 1:
        MR::closeWipeCircle(60);
        break;
    case 2:
        MR::closeSystemWipeMario(60);
        break;
    default:
        MR::closeWipeFade(60);
        break;
    }
}

void openType(s32 mScreenType) {
    switch (mScreenType)
    {
    case 0:
        MR::openWipeWhiteFade(60);
        break;
    case 1:
        MR::openWipeCircle(60);
        break;
    case 2:
        MR::openSystemWipeMario(60);
        break;
    default:
        MR::openWipeFade(60);
        break;
    }
}

kmWrite16(0x8033EC06, 0x124);      // More size (sorry Syati_ClassExtensions) 
kmCall(0x8027A21C, extInitMapToolInfo);     
kmCall(0x803BE378, extStartPlayerEvent);
kmCall(0x803BE384, extStartPlayerEvent);
kmCall(0x803BE598, resumeGameplay);
