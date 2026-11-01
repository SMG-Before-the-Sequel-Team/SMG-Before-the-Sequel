#pragma once

#include "Game/MapObj/SpinDriverPathDrawer.h"

class JUTTexture;


class SpinDriverPathDrawInit_Ext : public SpinDriverPathDrawInit {
public:

JUTTexture** mTextures;
s32 mTextureCount;
};
