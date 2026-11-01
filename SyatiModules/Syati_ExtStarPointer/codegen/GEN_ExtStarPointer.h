#pragma once

enum ExtStarPointerModeEnum {
    /* BUILT-IN */
    ExtStarPointerMode_HomeButton,                // Built-in
    ExtStarPointerMode_ErrorWindow,               // Built-in
    ExtStarPointerMode_SaveLoad,                  // Built-in
    ExtStarPointerMode_Logo,                      // Built-in
    ExtStarPointerMode_Title,                     // Built-in
    ExtStarPointerMode_FileSelect,                // Built-in
    ExtStarPointerMode_ScenarioSelectScene,       // Built-in
    ExtStarPointerMode_PauseMenu,                 // Built-in
    ExtStarPointerMode_ChooseYesNo,               // Built-in
    ExtStarPointerMode_DemoWithStarPointer,       // Built-in
    ExtStarPointerMode_DemoWithHandPointerFinger, // Built-in
    ExtStarPointerMode_Demo,                      // Built-in
    ExtStarPointerMode_DemoMarioDeath,            // Built-in
    ExtStarPointerMode_PowerStarGetDemo,          // Built-in
    ExtStarPointerMode_1PInvalid2PValid,          // Built-in
    ExtStarPointerMode_SphereSelectorFinger,      // Built-in
    ExtStarPointerMode_MarioLauncher,             // Built-in
    ExtStarPointerMode_BlueStar,                  // Built-in
    ExtStarPointerMode_BlueStarReady,             // Built-in
    ExtStarPointerMode_StarPieceTarget,           // Built-in
    ExtStarPointerMode_BigBubble,                 // Built-in
    ExtStarPointerMode_UNKNOWN,                   // Built-in
    ExtStarPointerMode_Yoshi,                     // Built-in
    ExtStarPointerMode_Base,                      // Built-in
    ExtStarPointerMode_Game,                      // Built-in

    /* EXTENDED */
{{EnumList}}
    /* END */
    ExtStarPointerMode_Count
};

{{IncludeList}}

class StarPointerOnOffController;

typedef void (*StarPointerSetupFunc)(StarPointerOnOffController*);

const StarPointerSetupFunc cModuleSetupPointerModeTable[] = {
{{SetupFuncList}}
    nullptr
};