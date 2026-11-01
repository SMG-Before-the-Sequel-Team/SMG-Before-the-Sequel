#pragma once

#include "Game/Enemy/BrosBaseActionBase.h"

class HammerBrosStateAttack : public BrosBaseActionBase {
public:
	HammerBrosStateAttack(BrosBase*);

	virtual void init();
	virtual void appear();

	s32 _10; //_10
};