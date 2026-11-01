#pragma once

#include "revolution.h"
#include "Game/LiveActor/ActorStateBase.h"
#include "Game/Enemy/BrosBase.h"

class BrosBaseActionBase : public ActorStateBaseInterface {
public:
	BrosBaseActionBase(const char*, BrosBase*);

	BrosBase* mOwner; //_C
};