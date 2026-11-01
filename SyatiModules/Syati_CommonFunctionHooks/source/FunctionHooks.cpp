#pragma once

#include "revolution.h"
#include "kamek/hooks.h"
#include "ModuleData_FunctionHooks.h"

namespace
{
    // ----------------------------------------------------------------------------------------------------------------
    // Events to be handled after GameScene::start

    void handleGameSceneStart() {
        for (s32 i = 1; i < cModuleGameSceneStartFuncTableCount; i++)
        {
            cModuleGameSceneStartFuncTable[i]();
        }
    }

    kmBranch(0x80451888, handleGameSceneStart);


    // ----------------------------------------------------------------------------------------------------------------
    // Events to be handled after any scene gets destroyed

    void handleAnySceneDestroyed() {
        for (s32 i = 1; i < cModuleAnySceneDestroyedFuncTableCount; i++)
        {
            cModuleAnySceneDestroyedFuncTable[i]();
        }
    }

#if defined(SB4P) || defined(SB4E) || defined(SB4J)
    kmBranch(0x804BAB30, handleAnySceneDestroyed);
#elif defined(SB4W) || defined(SB4K)
    kmBranch(0x804BABA0, handleAnySceneDestroyed);
#endif


    // ----------------------------------------------------------------------------------------------------------------
    // Events to be handled after GameSystem::FrameLoop

    void handleGameSystemFrameLoop() {
        for (s32 i = 1; i < cModuleGameSystemFrameLoopTableCount; i++)
        {
            cModuleGameSystemFrameLoopTable[i]();
        }
    }

#if defined(SB4P) || defined(SB4E) || defined(SB4J)
    kmBranch(0x804B6AD4, handleGameSystemFrameLoop);
#elif defined(SB4W) || defined(SB4K)
    kmBranch(0x804B6B44, handleGameSystemFrameLoop);
#endif

    // ----------------------------------------------------------------------------------------------------------------
    // Events to be handled after GameScene::update

    void handleGameSceneUpdate() {
        for (s32 i = 1; i < cModuleGameSceneUpdateTableCount; i++)
        {
            cModuleGameSceneUpdateTable[i]();
        }
    }

    kmBranch(0x804518CC, handleGameSceneUpdate);
}