#pragma once

#include "Game/Screen/GameSceneLayoutHolder.h"

class SubMeterLayout;

class MarioSubMeter_Ext : public MarioSubMeter {
public:

SubMeterLayout** mExtSubMeterList;
s32 mExtSubMeterListCount;
};
