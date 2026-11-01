#include "Kamek/hooks.h"
#include "ExtCometScreenFilter.h"
#include "ModuleData_ExtCometScreenFilter.h"
#include "Game/Screen/LayoutActor.h"
#include "Game/Util/StringUtil.h"
#include "Game/Util/LayoutUtil.h"

namespace
{
	const char* cAnimNameColor = "Color";

	void getCometNameIdFromString(LayoutActor* pCometScreenFilter, char const* pName)
	{
		s32 FrameId = 0;
		for (s32 i = 0; i < cModuleCometScreenFilterTableCount; i++)
		{
			const CometScreenFilterEntry entry = cModuleCometScreenFilterTable[i];

			if (MR::isEqualString(entry.mCometName, pName))
			{
				FrameId = entry.mFrameId;
				break;
			}
		}

		MR::startAnim(pCometScreenFilter, cAnimNameColor, 0);
		MR::setAnimFrameAndStop(pCometScreenFilter, static_cast<f32>(FrameId), 0);
	}

	kmWrite32(0x80372958, 0x809D000C); //lwz r4, 0x0C(r29)
	kmCall(0x8037295C, getCometNameIdFromString);
}