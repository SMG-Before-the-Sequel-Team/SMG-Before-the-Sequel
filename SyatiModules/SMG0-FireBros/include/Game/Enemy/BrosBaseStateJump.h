#pragma once

#include "Game/Enemy/BrosBaseActionBase.h"

class BrosBaseStateJump : public BrosBaseActionBase {
public:
	BrosBaseStateJump(BrosBase*);

	virtual void init();
	virtual void appear();

	s32 _10; //_10
	s32 _14; //_14
	TVec3f _18; //_18
	TVec3f _24; //_24
	TVec3f _30; //_30
	TVec3f _3C; //_3C
	TVec3f _48; //_48
	s32 _58; //_58
};