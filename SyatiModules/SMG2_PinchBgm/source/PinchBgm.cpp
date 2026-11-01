#include "PinchBgm.h"

#include "Game/GameAudio/AudBgmConductor.h"
#include "Game/Scene/SceneObjHolder.h"
#include "Game/Util.h"

namespace {
	inline AudBgmConductor* getConductor() {
		return static_cast<AudBgmConductor*>(MR::getSceneObjHolder()->getObj(SCENE_OBJ_AUD_BGM_CONDUCTOR));
	}
}

void PinchBGM::startPinchBGM(const char* pBgmName, bool startSilent) {
	if (MR::isPlayerDead())
		return;

	if (AudBgmConductor* pConductor = getConductor()) {
		if (MR::isPlayingSubBgmID(0x0100000B) || MR::isPlayingSubBgmID(0x0100000C) ||
			MR::isPlayingSubBgmID(0x0100000F) || MR::isPlayingSubBgmID(0x01000010) ||
			MR::isPlayingSubBgmID(0x0100000D) || MR::isPlayingSubBgmID(0x0100000E)) {
			pConductor->_20 = -1;
			pConductor->_24 = -1;
			return;
		}
		if (!MR::isPowerStarGetDemoActive()) {
			if (pConductor->_20 < 0) {
				MR::startSubBGM(pBgmName, false);

				if (startSilent) {
					MR::moveVolumeSubBGM(0.0f, 0);
					pConductor->_24 = 2;
				}

				pConductor->_28 = false;
			}
			pConductor->_20 = 3;
		}
	}
}

void PinchBGM::stopPinchBGMSoon() {
	if (AudBgmConductor* pC = getConductor()) {
		pC->_20 = 0;
		pC->_28 = true;
	}
}