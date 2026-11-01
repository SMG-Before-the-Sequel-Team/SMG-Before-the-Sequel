#pragma once

namespace PinchBGM {
	// Equivalent to playLevelMarioPinchBGM, but you use the Label instead
	void startPinchBGM(const char* pBgmName, bool startSilent = false);
	// Equivalent to stopMarioPinchBGMSoon
	void stopPinchBGMSoon();
}