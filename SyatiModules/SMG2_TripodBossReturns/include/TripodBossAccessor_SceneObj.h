#pragma once

#include "Game/Boss/TripodBossAccesser.h"

namespace MR
{
	NameObj* doMakeTripodBossAccesser()
	{
		return new TripodBossAccesser("TripodBossAccesser");
	}
}