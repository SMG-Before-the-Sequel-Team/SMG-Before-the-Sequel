#include "kamek/hooks.h"
#include "Game/Screen/NoteCounterExt.h"

// Currently this doesn't use ClassExtensions. Once SMG1 has Class Extensions, I'll switch to using that.

NoteCounterExt::NoteCounterExt(const char* pName) : NoteCounter(pName) {
	mPaneRumbler = 0;
}

NoteCounterExt* createNoteCounterExt() {
	return new NoteCounterExt("音符カウンタ");
}

NoteCounterExt::~NoteCounterExt() {}
void NoteCounterExt::control() {
	mPaneRumbler->update();
}

void NoteCounter_init(NoteCounterExt* pLayout, const Nerve* pInitNerve)
{
	MR::connectToSceneLayout(pLayout);
	pLayout->initLayoutManager("NoteCounter", 2);
	pLayout->initNerve(pInitNerve);
	MR::createAndAddPaneCtrl(pLayout, "ShaDigitL", 2);
	MR::createAndAddPaneCtrl(pLayout, "ShaDigitR", 2);
	MR::createAndAddPaneCtrl(pLayout, "Counter", 1);
	MR::hideLayout(pLayout);
	pLayout->mPaneRumbler = new CountUpPaneRumbler(pLayout, "Counter");
	pLayout->mPaneRumbler->mRumbleCalculator->mRumbleStrength = 8.0f;
}

void NoteCounter_exeHideToShow(NoteCounterExt* pLayout)
{
	MR::startAnimAtFirstStep(pLayout, "Appear", 0);
	MR::setTextBoxFormatRecursive(pLayout, "Counter", L"%02d", 0);
}

void NoteCounter_exeShowToHide(NoteCounterExt* pLayout)
{
	pLayout->mPaneRumbler->reset();
	MR::startAnimAtFirstStep(pLayout, "End", 0);
}

bool NoteCounter_declareNoteNumMaxAndMelody(NoteCounterExt* pLayout, const Nerve* pNerve)
{
	MR::startPaneAnim(pLayout, "ShaDigitL", "DigitL", 0);
	MR::startPaneAnim(pLayout, "ShaDigitR", "DigitR", 0);

	int ones = (pLayout->mNoteNumMax % 10);
	int tens = (pLayout->mNoteNumMax / 10) % 10;

	MR::setPaneAnimFrameAndStop(pLayout, "ShaDigitL", static_cast<f32>(tens), 0);
	MR::setPaneAnimFrameAndStop(pLayout, "ShaDigitR", static_cast<f32>(ones), 0);

	return pLayout->isNerve(pNerve);
}

void NoteCounter_add(NoteCounterExt* pLayout)
{
	MR::setTextBoxFormatRecursive(pLayout, "Counter", L"%02d", pLayout->mNoteNum);
	MR::startAnim(pLayout, "Flash", 0);
	pLayout->mPaneRumbler->start();

	pLayout->tryEndDisp();
}

// Hooks and Symbols
extern const u8 init__11NoteCounterFRC12JMapInfoIter;
extern const u8 exeHideToShow__11NoteCounterFv;
extern const u8 exeShowToHide__11NoteCounterFv;
extern const u8 declareNoteNumMaxAndMelody__11NoteCounterFP9LiveActorllf;
extern const u8 add__11NoteCounterFv;
extern const u8 __ct__21GameSceneLayoutHolderFv;

kmCall(&init__11NoteCounterFRC12JMapInfoIter + 0x18, NoteCounter_init);
kmCall(&exeHideToShow__11NoteCounterFv + 0x34, NoteCounter_exeHideToShow);
kmWrite32(&exeHideToShow__11NoteCounterFv + 0x38, 0x48000064);
kmCall(&exeShowToHide__11NoteCounterFv + 0x1C, NoteCounter_exeShowToHide);
kmWrite32(&exeShowToHide__11NoteCounterFv + 0x20, 0x48000064);
kmCall(&declareNoteNumMaxAndMelody__11NoteCounterFP9LiveActorllf + 0x34, NoteCounter_declareNoteNumMaxAndMelody);

#if defined(SB4)
kmCall(&add__11NoteCounterFv + 0x120, NoteCounter_add);
kmCall(&__ct__21GameSceneLayoutHolderFv + 0x180, createNoteCounterExt);
kmWrite32(&__ct__21GameSceneLayoutHolderFv + 0x184, 0x48000014);
#elif defined (RMG)
kmCall(&add__11NoteCounterFv + 0x110, NoteCounter_add);
kmCall(&__ct__21GameSceneLayoutHolderFv + 0x15C, createNoteCounterExt);
kmWrite32(&__ct__21GameSceneLayoutHolderFv + 0x160, 0x48000014);
#endif