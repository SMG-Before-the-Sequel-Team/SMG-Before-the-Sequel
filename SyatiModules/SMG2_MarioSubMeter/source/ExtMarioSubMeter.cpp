#include "ExtMarioSubMeter.h"
#include "ModuleData_MarioSubMeter.h"
#include "Game/NameObj/NameObj.h"
#include "revolution.h"
#include "Kamek/hooks.h"
#include "Game/Util/StringUtil.h"
#include "Game/Screen/GameSceneLayoutHolder.h"
#include "ModuleData_MarioSubMeter_Ext.h"

namespace {
	void createExtMarioSubMeter(SubMeterLayout* pAirMeter, MarioSubMeter_Ext* __restrict pMarioSubMeter) {
		pMarioSubMeter->mAirMeter = pAirMeter;
		pAirMeter->initWithoutIter(); //Vanilla code


		pMarioSubMeter->mExtSubMeterListCount = cModuleMarioSubMeterTableCount-1;
		pMarioSubMeter->mExtSubMeterList = new SubMeterLayout * [pMarioSubMeter->mExtSubMeterListCount];
		for (s32 i = 0; i < pMarioSubMeter->mExtSubMeterListCount; i++)
		{
			// The __restrict keyword for pMarioSubMeter here actually saves us two instructions!
			// __restrict promises to CodeWarrior that nothing else will modify pMarioSubMeter,
			//   thus allowing CodeWarrior to drop the two instructions that would be needed to
			//   re-load the pointer of the SubMeterLayout (that we just made anyways)
			//   tbh I don't actually know what __restrict fully requires, but it works properly here so it's fine.
			MarioSubMeterEntry msme = cModuleMarioSubMeterTable[i + 1];
			pMarioSubMeter->mExtSubMeterList[i] = new SubMeterLayout(msme.mRefName, msme.mLayoutName);
			pMarioSubMeter->mExtSubMeterList[i]->initWithoutIter();
			if (msme.mInitFunc != NULL)
				msme.mInitFunc(pMarioSubMeter->mExtSubMeterList[i]);
		}
	}
	void deactivateAllExtMeter(MarioSubMeter_Ext* pMarioSubMeter)
	{
		pMarioSubMeter->mAirMeter->requestDeactivate();
		for (s32 i = 0; i < pMarioSubMeter->mExtSubMeterListCount; i++)
		{
			pMarioSubMeter->mExtSubMeterList[i]->requestDeactivate();
		}
	}
	bool isPlayerInWaterMode_ExtForSubMeter()
	{
		register SubMeterLayout* pMeter;
		__asm {
			mr pMeter, r31
		}
		MarioSubMeter_Ext* pMarioSubMeter = getExtMarioSubMeter();
		if (pMarioSubMeter != NULL)
			for (s32 i = 0; i < pMarioSubMeter->mExtSubMeterListCount; i++)
			{
				if (pMarioSubMeter->mExtSubMeterList[i] == pMeter)
				{
					MarioSubMeterEntry msme = cModuleMarioSubMeterTable[i + 1];
					return msme.mBehaviour == 1 || msme.mBehaviour == 2;
				}
			}
		return MR::isPlayerInWaterMode(); //For the vanilla behaviour
	}
	bool isPlayerOnWaterSurface_ExtForSubMeter()
	{
		register SubMeterLayout* pMeter;
		__asm {
			mr pMeter, r31
		}
		MarioSubMeter_Ext* pMarioSubMeter = getExtMarioSubMeter();
		if (pMarioSubMeter != NULL)
			for (s32 i = 0; i < pMarioSubMeter->mExtSubMeterListCount; i++)
			{
				if (pMarioSubMeter->mExtSubMeterList[i] == pMeter)
				{
					MarioSubMeterEntry msme = cModuleMarioSubMeterTable[i + 1];
					if (msme.mBehaviour == 2)
						return false;
					break;
				}
			}
		return MR::isPlayerOnWaterSurface(); //For the vanilla behaviour
	}
	
#if defined(TWN) || defined(KOR)
	kmWrite32(0x804817C0, 0x7FC4F378); //mr r4, r30
	kmCall(0x804817C4, createExtMarioSubMeter);

	kmWrite32(0x8048194C, 0x7FE3FB78); //mr r3, r31
	kmCall(0x80481950, deactivateAllExtMeter);


	kmCall(0x804A0ECC, isPlayerInWaterMode_ExtForSubMeter);

	kmCall(0x804A0ED8, isPlayerOnWaterSurface_ExtForSubMeter);
#else
	kmWrite32(0x804817B0, 0x7FC4F378); //mr r4, r30
	kmCall(0x804817B4, createExtMarioSubMeter);

	kmWrite32(0x8048193C, 0x7FE3FB78); //mr r3, r31
	kmCall(0x80481940, deactivateAllExtMeter);


	kmCall(0x804A0E5C, isPlayerInWaterMode_ExtForSubMeter);

	kmCall(0x804A0E68, isPlayerOnWaterSurface_ExtForSubMeter);
#endif
}

// === API Stuff ===

// Returns the existing MarioSubMeter_Ext instance that is used by the game.
// Note that the definition of MarioSubMeter_Ext requires including the Class Extensions API in the "RequiredAPIs"
MarioSubMeter_Ext* getExtMarioSubMeter()
{
	GameSceneLayoutHolder* pGameSceneLayoutHolder = MR::getGameSceneLayoutHolder();
	if (pGameSceneLayoutHolder == NULL)
		return NULL;
	return (MarioSubMeter_Ext*)(pGameSceneLayoutHolder->mMarioSubMeter);
}

// Returns the first SubMeterLayout that has the specified name
// You can get the Vanilla SubMeters (AIR and FLY) with this as well
SubMeterLayout* getMarioSubMeter(const char* pRefName)
{
	if (pRefName == NULL)
		return NULL;
	MarioSubMeter_Ext* pMarioSubMeter = getExtMarioSubMeter();
	if (pMarioSubMeter == NULL)
		return NULL;
	if (MR::isEqualString(pRefName, pMarioSubMeter->mBeeMeter->mName))
		return pMarioSubMeter->mBeeMeter;
	if (MR::isEqualString(pRefName, pMarioSubMeter->mAirMeter->mName))
		return pMarioSubMeter->mAirMeter;
	for (s32 i = 0; i < pMarioSubMeter->mExtSubMeterListCount; i++)
	{
		if (MR::isEqualString(pRefName, pMarioSubMeter->mExtSubMeterList[i]->mName))
			return pMarioSubMeter->mExtSubMeterList[i];
	}
	return NULL;
}

// Returns the currently active SubMeterLayout
// Note that using this does not require including the Class Extensions API in the "RequiredAPIs"
SubMeterLayout* getCurrentMarioSubMeter()
{
	MarioSubMeter_Ext* pmarioSubMeter = getExtMarioSubMeter();
	return pmarioSubMeter->mCurrentMeter;
}

// Attempts to activate a given SubMeterLayout
// Returns TRUE if it succeeds
// Returns FALSE if a SubMeter with the given name cannot be found, or if the SubMeter that is requested is already active
bool activeSubMeterExt(const char* pRefName) {
	SubMeterLayout* pMeter = getMarioSubMeter(pRefName);
	if (pMeter == NULL)
		return false;
	MarioSubMeter_Ext* pMarioSubMeter = getExtMarioSubMeter();
	if (pMarioSubMeter->mCurrentMeter == pMeter)
		return false;
	if (pMarioSubMeter->mCurrentMeter != NULL) {
		pMarioSubMeter->mCurrentMeter->requestDeactivate();
		pMarioSubMeter->mCurrentMeter = NULL;
	}
	pMeter->requestAppear();
	pMarioSubMeter->mCurrentMeter = pMeter;
	return true;
}

// Attempts to deactivate a given SubMeterLayout
// Returns TRUE if it succeeds
// Returns FALSE if a SubMeter with the given name cannot be found, or if the SubMeter that is requested is not the currently active SubMeter
bool deactivateSubMeterExt(const char* pRefName) {
	SubMeterLayout* pMeter = getMarioSubMeter(pRefName);
	if (pMeter == NULL)
		return false;
	MarioSubMeter_Ext* pMarioSubMeter = getExtMarioSubMeter();
	if (pMarioSubMeter->mCurrentMeter != pMeter)
		return false;
	pMarioSubMeter->mCurrentMeter->requestDeactivate();
	pMarioSubMeter->mCurrentMeter = NULL;
	return true;
}

bool requestFrameInSubMeterExt(const char* pRefName)
{
	SubMeterLayout* pMeter = getMarioSubMeter(pRefName);
	if (pMeter == NULL)
		return false;
	MarioSubMeter_Ext* pMarioSubMeter = getExtMarioSubMeter();
	if (pMarioSubMeter->mCurrentMeter != pMeter)
		return false;
	pMarioSubMeter->mCurrentMeter->requestFrameIn();
	return true;
}

bool isActiveSubMeterExt(const char* pRefName)
{
	SubMeterLayout* pMeter = getMarioSubMeter(pRefName);
	if (pMeter == NULL)
		return false;
	MarioSubMeter_Ext* pMarioSubMeter = getExtMarioSubMeter();
	return pMarioSubMeter->mCurrentMeter == pMeter && (pMeter->_20 == 1 || pMeter->_20 == 2 || pMeter->_20 == 3);
}