#pragma once


#include "GSTRecord.h"
#include "PadRecord.h"

#include "FPS_Include.h"

#include "DiveMove.h"


typedef void (*HookFuncDef)();

const HookFuncDef cModuleGameSceneStartFuncTable[] = {
    NULL,
pad::startPadRecorderPrepared, //handleGameSceneStart

};
const s32 cModuleGameSceneStartFuncTableCount = sizeof(cModuleGameSceneStartFuncTable) / sizeof(HookFuncDef);


const HookFuncDef cModuleAnySceneDestroyedFuncTable[] = {
    NULL,
gst::waitGstRecorder, //GSTRecordGameSceneDestroy
pad::waitPadRecorderNotPrepared, //handleAnySceneDestroyed

};
const s32 cModuleAnySceneDestroyedFuncTableCount = sizeof(cModuleAnySceneDestroyedFuncTable) / sizeof(HookFuncDef);


const HookFuncDef cModuleGameSystemFrameLoopTable[] = {
    NULL,
MR::CalcFPS, //FPSCounter_CalcFPS

};
const s32 cModuleGameSystemFrameLoopTableCount = sizeof(cModuleGameSystemFrameLoopTable) / sizeof(HookFuncDef);

const HookFuncDef cModuleGameSceneUpdateTable[] = {
    NULL,
checkIfDiveAllowed, //CheckIfDiveAllowed

};
const s32 cModuleGameSceneUpdateTableCount = sizeof(cModuleGameSceneUpdateTable) / sizeof(HookFuncDef);