#pragma once

#include "Game/Boss/SkeletalFishBossRailHolder.h"

namespace MR
{
	NameObj* doMakeSkeletalFishBossRailHolder()
	{
		return new SkeletalFishBossRailHolder("スカルシャークボスレール管理");
	}
}