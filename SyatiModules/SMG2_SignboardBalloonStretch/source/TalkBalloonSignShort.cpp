#include "TalkBalloonSignShort.h"

#include "ModuleData_TalkBalloonHolder_Ext.h"

TalkBalloonSignShort::TalkBalloonSignShort(const char* pName) : TalkBalloonShort(pName)
{

}


void TalkBalloonSignShort::init(const JMapInfoIter& rIter)
{
	create("SignboardBalloonStretch", false, false);
}






// ==================================


void initStretchSignBoard(IconAButton* pButton, TalkBalloonHolder_Ext* pHolder)
{
	pHolder->mIconAButton = pButton;
	pButton->initWithoutIter();

	pHolder->mTalkBalloonSignShort = new TalkBalloonSignShort("TalkBalloonSignShort");
	pHolder->mTalkBalloonSignShort->initWithoutIter();
	pHolder->mTalkBalloonSignShort->kill();
}

kmWrite32(0x80378784, 0x7F84E378);
kmCall(0x80378788, initStretchSignBoard);


TalkBalloon* getSignBalloon_inner(TalkMessageInfo* pInfo, TalkBalloonHolder_Ext* pHolder)
{
	if (pInfo->isShortTalk())
	{
		return pHolder->mTalkBalloonSignShort;
	}
	else
	{
		return pHolder->mTalkBalloonSign;
	}
}
asm void getSignBalloon()
{
	mr r4, r29
	mr r3, r30
	b getSignBalloon_inner
}
kmCall(0x80378848, getSignBalloon);
kmWrite32(0x8037884C, 0x7C7F1B78);



TalkBalloonSign* pauseOffSignBalloons()
{
	register TalkBalloonHolder_Ext* pHolder;

	__asm {
		mr pHolder, r29
	}

	pHolder->mTalkBalloonSignShort->pauseOff();

	return pHolder->mTalkBalloonSign;
}
kmCall(0x803788FC, pauseOffSignBalloons);