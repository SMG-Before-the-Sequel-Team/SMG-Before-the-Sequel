#pragma once

#include "Game/Enemy/BrosBaseActionBase.h"

class BrosBaseStateSwoon : public BrosBaseActionBase {
public:
	BrosBaseStateSwoon(BrosBase*);

	virtual void init();
	virtual void appear();

	virtual void control();

	s32 _10; //_10
	bool _14; //_14
};