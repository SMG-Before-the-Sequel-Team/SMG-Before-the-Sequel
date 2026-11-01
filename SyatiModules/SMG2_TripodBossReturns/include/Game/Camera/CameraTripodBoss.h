#pragma once

#include "Game/Camera/CameraTower.h"

class CameraTripodBoss : public CameraTower {
public:
    CameraTripodBoss(const char* pName = "三脚ボスカメラ");
    virtual ~CameraTripodBoss();

    virtual CamTranslatorBase* createTranslator();
    virtual void arrangeRound();

    f32 upZ;  // 0x94
};