#include "ExtStarPointerOnOffController.h"
#include "Kamek/hooks.h"

// This works really well. Maybe we should include it in Syati?
#define offsetof(type, member) ((s32)(&((type*)nullptr)->member))

#if defined(SB4E) || defined(SB4P) || defined(SB4J)
#define REGION_OFFSET 0x00
#elif defined(SB4K) || defined(SB4W)
#define REGION_OFFSET 0x90
#else
#define REGION_OFFSET 0x00
#endif // 


// We cannot use Class Extensions for this, as the arrays change size based on the enum length
kmWrite16(0x804D5870 + REGION_OFFSET + 2, sizeof(ExtStarPointerOnOffController)); // GameSequenceDirector::GameSequenceDirector

/* StarPointerOnOffController::StarPointerOnOffController */
kmWrite16(0x804CC960 + REGION_OFFSET + 2, offsetof(ExtStarPointerOnOffController, mMode));
kmWrite16(0x804CC9C4 + REGION_OFFSET + 2, ExtStarPointerMode_Count);
kmWrite16(0x804CC9EC + REGION_OFFSET + 2, offsetof(ExtStarPointerOnOffController, mRequested));

/* StarPointerOnOffController::update */
kmWrite16(0x804CCA28 + REGION_OFFSET + 2, ExtStarPointerMode_Count);
kmWrite16(0x804CCA40 + REGION_OFFSET + 2, offsetof(ExtStarPointerOnOffController, mRequested));

/* StarPointerOnOffController::requestMode */
kmWrite16(0x804CCB50 + REGION_OFFSET + 2, offsetof(ExtStarPointerOnOffController, mRequested));

/* StarPointerOnOffController::updateMode */
kmWrite16(0x804CD7D0 + REGION_OFFSET + 2, ExtStarPointerMode_Count);
kmWrite16(0x804CD7F4 + REGION_OFFSET + 2, offsetof(ExtStarPointerOnOffController, mRequested));
kmWrite16(0x804CD800 + REGION_OFFSET + 2, offsetof(ExtStarPointerOnOffController, mMode));

void ExtUpdateModeInner_StarPointerOnOffController(ExtStarPointerOnOffController* pCtrl, s32 mode) {
    if (mode != pCtrl->mMode) {
		pCtrl->mMode = mode;

		s32 idx = mode - StarPointerMode_Count;
		cModuleSetupPointerModeTable[idx]((StarPointerOnOffController*)pCtrl);
    }
}
// I don't think there's any way around this being required
asm void ExtUpdateMode_StarPointerOnOffController() {
	stwu r1, -0x10(r1);
	mflr r0;
	stw r0, 0x14(r1);
	stw r31, 0x0C(r1);

	cmpwi r5, StarPointerMode_Count;
	lwz r0, 0xCC(r31)
	blt FunctionEnd; // take the normal game code route
	mr r3, r31;
	mr r31, r5;
	mr r4, r5
	bl ExtUpdateModeInner_StarPointerOnOffController;

	li r0, 0;
	li r5, 0; // Making these equal will force return the caller
	FunctionEnd:
	lwz       r31, 0x0C(r1);
	lwz       r4, 0x14(r1);
	mtlr      r4; // Unusual use of r4 but the caller needs r0 soooo
	addi      r1, r1, 0x10;
	blr
}
kmCall(0x804CD800 + REGION_OFFSET, ExtUpdateMode_StarPointerOnOffController);

kmWrite16(0x804CD810 + REGION_OFFSET + 2, offsetof(ExtStarPointerOnOffController, mMode));
kmWrite16(0x804CD96C + REGION_OFFSET + 2, offsetof(ExtStarPointerOnOffController, mMode));

/* StarPointerOnOffController::clearInfo */
kmWrite16(0x804CD990 + REGION_OFFSET + 2, ExtStarPointerMode_Count);