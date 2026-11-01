#include "PictureBookCloseButton.h"
#include "PictureBookLayout.h"
#include "OtherUtils.h"

#include "Game/AudioLib/AudWrap.h"
#include "PictureBookDataStorage.h"

#ifdef GALAXY_LEVEL_ENGINE
#include "GalaxyLevelEngine.h"
#endif // GALAXY_LEVEL_ENGINE


namespace {
    const s32 cMaxPagePerChapter = 100;
    const s32 cBookOpenFrame = 60;
    // const s32 cBookCloseFrame
    const s32 cFadeFrame = 60;
    const s32 cWaitNoTextFrame = 60;
    const s32 cFadeTextFrame = 30;
    // const s32 cReadedSpeedRate
    const s32 cPageNextEndNormalSeStep = 81;
    const s32 cPageNextEndFastSeStep = 27;

    void openWithTurn(IconAButton* pAButton) {
        pAButton->appear();
        pAButton->updateFollowPos();
        MR::showPane(pAButton, "PicPlate");
        pAButton->setNerve(&NrvIconAButton::IconAButtonNrvOpen::sInstance);
        MR::setTextBoxGameMessageRecursive(pAButton, "IconAButton", "Layout_SystemTurn");
    }

    void setTextBoxHorizontalPositionRecursive(LayoutActor* pActor, const char* pPaneName, u8 position) {
        MR::executeTextBoxRecursive(pActor, pPaneName, TextBoxRecursiveSetHorizontalPosition(position));
    }

    void setTextBoxVerticalPositionRecursive(LayoutActor* pActor, const char* pPaneName, u8 position) {
        MR::executeTextBoxRecursive(pActor, pPaneName, TextBoxRecursiveSetVerticalPosition(position));
    }
}

namespace NrvPictureBookLayout {
    FULL_NERVE(PictureBookLayoutNrvOpen, PictureBookLayout, Open);
    FULL_NERVE(PictureBookLayoutNrvContentsSelect, PictureBookLayout, ContentsSelect);
    FULL_NERVE(PictureBookLayoutNrvContentsFadeOut, PictureBookLayout, ContentsFadeOut);
    FULL_NERVE(PictureBookLayoutNrvFadeIn, PictureBookLayout, FadeIn);
    FULL_NERVE(PictureBookLayoutNrvWaitNoText, PictureBookLayout, WaitNoText);
    FULL_NERVE(PictureBookLayoutNrvFadeInText, PictureBookLayout, FadeInText);
    FULL_NERVE(PictureBookLayoutNrvWaitWithText, PictureBookLayout, WaitWithText);
    FULL_NERVE(PictureBookLayoutNrvFadeOutText, PictureBookLayout, FadeOutText);
    FULL_NERVE(PictureBookLayoutNrvPageNext, PictureBookLayout, PageNext);
    FULL_NERVE(PictureBookLayoutNrvFadeOut, PictureBookLayout, FadeOut);
    FULL_NERVE(PictureBookLayoutNrvClose, PictureBookLayout, Close);
};

namespace {
    void setTextOrDefault(LayoutActor* pBookLayout, const char* pPaneName, const char* pMessageLabel, const wchar_t* pFailsafeText) {
        const wchar_t* pMsg = MR::getGameMessageDirect(pMessageLabel);
        if (pMsg == nullptr)
            pMsg = pFailsafeText;

        MR::setTextBoxMessageRecursive(pBookLayout, pPaneName, pMsg);
    }
}

PictureBookLayout::PictureBookLayout(const char* pBookName, const JMapInfo* pBgmInfo) :
    LayoutActor("絵本レイアウト", true),
    mBgmInfo(pBgmInfo),
    mBookName(pBookName),
    mLayoutName(nullptr),
    mChapterLayoutButtonNum(0),
    mChapterUnlockFlags(nullptr),
    mChapterReadFlags(nullptr),
    mChapterMax(1),
    mChapterMin(1),
    mChapterNo(1),
    mPageNo(0),
    mTextIndex(0),
    mNotReadedChapterNo(-1),
    mNotReadedPageNo(-1),
    mNotReadedTextIndex(-1),
    mTitleTexMap(nullptr),
    mCoverFrontTexMap(nullptr),
    mCoverBackTexMap(nullptr),
    mAllPageTextureStorage(nullptr),
    mChapterPageTextureStorage(nullptr),
    mNextItemDir(1),
    mIsNextItemFast(false),
    mIsAutoPlay(false),
    mIconAButton(nullptr),
    mContentsButtonPaneController(nullptr),
    mCloseButton(nullptr)
{
}

void PictureBookLayout::initBookInfo(const char* pTextureName, const char* pLayoutName, const JMapInfo* pBookInfo) {
    initLayoutManagerWithTextBoxBufferLength(pLayoutName, 512, 1);
    MR::createAndAddPaneCtrl(this, "AButtonPosition", 1);
    MR::createAndAddPaneCtrl(this, "Text", 1);
    MR::connectToSceneLayout(this);
    mIconAButton = MR::createAndSetupIconAButton(this, true, false);

    mChapterMax = MR::getCsvDataElementNum(pBookInfo);
    mChapterUnlockFlags = new bool[mChapterMax];
    mChapterReadFlags = new bool[mChapterMax];

    initTexture(pTextureName, pLayoutName, pBookInfo);


    char buttonPaneName[0x20];
    bool isPaneExist = true;
    do {
        snprintf(buttonPaneName, 0x20, "Chapter%d", ++mChapterLayoutButtonNum);
        isPaneExist = _mManager->findPaneByName(buttonPaneName);
    } while (isPaneExist);
    mChapterLayoutButtonNum--; // subtract one because this is calculated starting at 1, not 0
    //OSReport("Layout chapter button count: %d\n", mChapterLayoutButtonNum);
    mContentsButtonPaneController = new ButtonPaneController * [mChapterLayoutButtonNum];
    initContentsButton(pLayoutName);
    mCloseButton = new PictureBookCloseButton(true);
    mCloseButton->initWithoutIter();


    initNerve(&NrvPictureBookLayout::PictureBookLayoutNrvFadeIn::sInstance);
}

void PictureBookLayout::initTexture(const char* pTextureName, const char* pLayoutName, const JMapInfo* pBookInfo) {
    // Load the base book textures
    char arcNameBuffer[0x40];
    snprintf(arcNameBuffer, 0x40, "%sTexture.arc", pTextureName);
    char tempBtiNameBuffer[0x40];
    snprintf(tempBtiNameBuffer, 0x40, "%sTitle.bti", pTextureName);
    mTitleTexMap = MR::createLytTexMap(arcNameBuffer, tempBtiNameBuffer);
    snprintf(tempBtiNameBuffer, 0x40, "%sCoverFront.bti", pTextureName);
    mCoverFrontTexMap = MR::createLytTexMap(arcNameBuffer, tempBtiNameBuffer);
    snprintf(tempBtiNameBuffer, 0x40, "%sCoverBack.bti", pTextureName);
    mCoverBackTexMap = MR::createLytTexMap(arcNameBuffer, tempBtiNameBuffer);

    // Load the chapter page images
    mAllPageTextureStorage = new PictureBookChapterTexEntry[mChapterMax];
    for (s32 i = 0; i < mChapterMax; i++) {
        const char* pArchiveName;
        MR::getCsvDataStrOrNULL(&pArchiveName, pBookInfo, "Name", i);

        ResourceHolder* pResourceHolder = MR::createAndAddResourceHolder(pArchiveName);
        s32 texcount = pResourceHolder->mFileInfoTable->mResCount;
        mAllPageTextureStorage[i].mCount = texcount;
        mAllPageTextureStorage[i].mTex = new PictureBookPageTexEntry[texcount];
        for (s32 j = 0; j < texcount; j++) {
            char imagename[64];
            snprintf(imagename, 64, "Chapter%dPage%d.bti", i + 1, j + 1);

            if (!pResourceHolder->mFileInfoTable->isExistRes(imagename)){
                mAllPageTextureStorage[i].mTex[j].mName = "";
                mAllPageTextureStorage[i].mTex[j].mTexture = mTitleTexMap; // Backup image... Or maybe you just want text idk
                OSReport("Could not load %s\n", imagename);
                continue;
            }

            s32 len = strlen(imagename)+1;
            char* copyimagename = new char[len]; // have to leave this memory hanging
            MR::copyString(copyimagename, imagename, len);
            mAllPageTextureStorage[i].mTex[j].mName = copyimagename;
            mAllPageTextureStorage[i].mTex[j].mTexture = MR::createLytTexMap(pArchiveName, copyimagename);
            OSReport("Loaded %s\n", copyimagename);
        }
    }
}

void PictureBookLayout::initContentsButton(const char* pLayoutName) {
    for (s32 i = 0; i < mChapterLayoutButtonNum; i++) {
        // We never need these again after this, but they must persist in memory as the ButtonPaneController does not create a copy of them.
        // We'll float them in the current heap and pray that doesn't cause problems...
        char* contentsPaneName = new char[64];
        char* contentsPointingPaneName = new char[64];
        snprintf(contentsPaneName, 64, "Chapter%d", i + 1);
        snprintf(contentsPointingPaneName, 64, "BoxButton%d", i + 1);
        mContentsButtonPaneController[i] = new ButtonPaneController(this, contentsPaneName, contentsPointingPaneName, 0, false);
        // mContentsButtonPaneController[i]->invalidateAppearance();    Seems like nothing strange happens when this is not on, so...

        char messageId[64];
        snprintf(messageId, 64, "%sChapter%d_Title", pLayoutName, i + 1);
        wchar_t defaultmsg[64]; // The game will make a copy of this, so no need to float it like the above strings thankfully...
        swprintf(defaultmsg, 64, L"Chapter %d\n-Untitled-", i + 1);
        setTextOrDefault(this, contentsPaneName, messageId, defaultmsg);
    }
    mLayoutName = pLayoutName;
}

// ===============================================================

void PictureBookLayout::prepare(bool autoplay, const JMapInfo* pBookInfo) {
    // Prepare the picture book to be opened
    // This handles unlocking chapters, which chapters have been read already, and which chapters should be shown when autoplay is used
    // Call this before calling appear

    mIsAutoPlay = autoplay; // TODO: Autoplay counts everything as being read before because... well, everything HAS been read before, according to the flags. Fix this!
    mChapterNo = 0;
    for (s32 i = 0; i < mChapterMax; i++) {
        mChapterUnlockFlags[i] = isOpenChapter(i+1);
        mChapterReadFlags[i] = PictureBookDataUtil::isReadChapter(mBookName, i+1);

        if (mChapterReadFlags[i])
            continue;

        if (mChapterNo <= 0)
            mChapterNo = i + 1; // Starting point for AutoPlay
    }
    if (mIsAutoPlay && mChapterNo <= 0)
        mChapterNo = 1; // default to the first chapter if all have been read already. Technically this is just a failsafe...
}

void PictureBookLayout::appear() {
    mPageNo = 0;
    mTextIndex = 0;
    mNotReadedChapterNo = -1;
    mNotReadedPageNo = -1;
    mNotReadedTextIndex = -1;
    mNextItemDir = 1;
    mIsNextItemFast = !mIsAutoPlay;

    if (mIsAutoPlay) {
        MR::hidePaneRecursive(this, "Contents");
        setNerve(&NrvPictureBookLayout::PictureBookLayoutNrvFadeIn::sInstance);
    }
    else {
        hideContents();
        setNerve(&NrvPictureBookLayout::PictureBookLayoutNrvOpen::sInstance);
    }

    LayoutActor::appear();

    MR::requestMovementOn(this);
    MR::requestMovementOn(mIconAButton);
    MR::pauseOnCameraDirector();
    MR::deactivateGameSceneDraw3D();
}

void PictureBookLayout::kill() {
    LayoutActor::kill();
    mIconAButton->kill();

    if (!mIsAutoPlay) {
        mCloseButton->kill();
    }

    MR::startCurrentStageBGM(false);
    MR::pauseOffCameraDirector();
    MR::activateGameSceneDraw3D();
}

void PictureBookLayout::control() {
    if (mIsAutoPlay) {
        return;
    }

    for (s32 i = 0; i < mChapterLayoutButtonNum; i++) {
        mContentsButtonPaneController[i]->update();
    }
}


bool PictureBookLayout::isOpenChapter(s32 chapterNo) {
    bool GLEResult = true;
#ifdef GALAXY_LEVEL_ENGINE
    if (mChapterUnlockInfo) {
        for (s32 i = 0; i < MR::getCsvDataElementNum(mChapterUnlockInfo); i++) {
            s32 id;
            MR::getCsvDataS32(&id, mChapterUnlockInfo, "ChapterNo", i);
            if (id != chapterNo)
                continue;

            GLEResult = GLE::isJMapEntryProgressComplete(mChapterUnlockInfo, i);
            break;
        }
    }
#endif // GALAXY_LEVEL_ENGINE
    bool SuperFlagResult = true;
#if SuperFlag
    // TODO: SuperFlag doesn't actually allow it being optional. Gotta talk to TMC about that...
#endif // SuperFlag
    return GLEResult && SuperFlagResult;
}

bool PictureBookLayout::updateText() {
    char messageId[64];

    if (mPageNo == 0) {
        
        snprintf(messageId, sizeof(messageId), "%sChapter%d_Title", mLayoutName, mChapterNo);
        setTextOrDefault(this, "Title", messageId, L"- Untitled Chapter -");

        return true;
    }
    else {
        snprintf(messageId, sizeof(messageId), "%sChapter%d_Page%d_%03d", mLayoutName, mChapterNo, mPageNo, mTextIndex);

        if (MR::isExistGameMessage(messageId)) {
            setTextOrDefault(this, "Text", messageId, L"[...]"); // just in case...

            return true;
        }

        return false;
    }
}

void PictureBookLayout::updateTexture() {
    s32 textureNum = getTextureNum(mChapterNo);
    s32 pageNo = mPageNo;
    //OSReport("updatetexture ChapterNo PageNo NextItemDir TextureNum: %d, %d, %d, %d\n", mChapterNo, mPageNo, mNextItemDir, textureNum);

    if (mNextItemDir > 0) {
        pageNo = mPageNo - 1;
    }
    else {
        pageNo = mPageNo;
    }

    if (pageNo == 0) {
        nw4r::lyt::TexMap* pTexMap = mTitleTexMap;

        MR::replacePaneTexture(this, "PicLeftPage", pTexMap, 0);
        MR::replacePaneTexture(this, "PicTurnRightPage", pTexMap, 0);
    }
    else {
        s32 texMapIndex = textureNum + pageNo - 1;
        s32 ind = texMapIndex % textureNum + 1;
        nw4r::lyt::TexMap* pTexMap = mChapterPageTextureStorage[ind-1].mTexture;

        MR::replacePaneTexture(this, "PicLeftPage", pTexMap, 0);
        MR::replacePaneTexture(this, "PicTurnRightPage", pTexMap, 0);
    }

    if (mNextItemDir > 0) {
        pageNo = mPageNo;
    }
    else {
        pageNo = mPageNo + 1;
    }

    if (pageNo == 0) {
        nw4r::lyt::TexMap* pTexMap = mTitleTexMap;

        MR::replacePaneTexture(this, "PicRightPage", pTexMap, 0);
        MR::replacePaneTexture(this, "PicTurnLeftPage", pTexMap, 0);
    }
    else {
        s32 texMapIndex = textureNum + pageNo - 1;
        s32 ind = texMapIndex % textureNum + 1;
        nw4r::lyt::TexMap* pTexMap = mChapterPageTextureStorage[ind-1].mTexture;
        
        MR::replacePaneTexture(this, "PicRightPage", pTexMap, 0);
        MR::replacePaneTexture(this, "PicTurnLeftPage", pTexMap, 0);
    }
}

extern "C" {
    void __kAutoMap_80085F70(u32*, AudSoundNameConverter*, const char*);
}

void PictureBookLayout::updateBgm(s32 chapter, s32 page, s32 textline) {
    if (mBgmInfo == nullptr)
        return;

    //OSReport("BGM: %d %d %d\n", chapter, page, textline);
    // Track down the BCSV row that corresponds to the chapter page and textline
    s32 idx = -1;
    s32 count = MR::getCsvDataElementNum(mBgmInfo);
    for (s32 i = 0; i < count; i++) {
        s32 C, P, T;
        MR::getCsvDataS32(&C, mBgmInfo, "Chapter", i);
        MR::getCsvDataS32(&P, mBgmInfo, "Page", i);
        MR::getCsvDataS32(&T, mBgmInfo, "Text", i);

        if (C != chapter && C != -1) // If -1, we ignore it
            continue;

        if (P != page && P != -1) // If -1, we ignore it
            continue;

        if (T != textline && T != -1) // If -1, we ignore it
            continue;

        idx = i;
        break;
    }

    if (idx < 0)
        return; // Return without doing anything...

    // We do need to attempt a music change now
    const char* pLabel;
    MR::getCsvDataStrOrNULL(&pLabel, mBgmInfo, "BgmName", idx);
    s32 state = 0;
    MR::getCsvDataS32(&state, mBgmInfo, "BgmStateNo", idx);
    s32 wipeout = 120;
    s32 statechange = 120;
    if (MR::hasCsvItem(mBgmInfo, "BgmWipeoutFrame"))
        MR::getCsvDataS32(&wipeout, mBgmInfo, "BgmWipeoutFrame", idx);
    if (MR::hasCsvItem(mBgmInfo, "BgmStateFrame"))
        MR::getCsvDataS32(&statechange, mBgmInfo, "BgmStateFrame", idx);
    if (wipeout < 0)
        wipeout = 120;
    if (statechange < 0)
        statechange = 120;

    //OSReport("%03d: %s @ %d ^ %d (%d)\n", idx, pLabel, state, statechange, wipeout);
    if (pLabel == nullptr) { // Calling for no BGM
        MR::stopStageBGM(wipeout);
    }
    else { // New BGM will start
        if (MR::isEqualCurrentStageBgmName(pLabel)) {
            MR::setStageBGMState(state, statechange);
            //OSReport("Same Song playing\n");
        }
        else if (MR::isStopOrFadeoutBgmName(pLabel)) {
            MR::startStageBGM(pLabel, false);
            MR::setStageBGMState(state, statechange);
            //OSReport("BGM: IsStopOrFadeOut = TRUE\n");
        }
        else {
            MR::clearBgmQueue();
            AudSoundNameConverter* psoundconverter = AudSingletonHolder<AudSoundNameConverter>::sInstance;
            u32 ID;
            const char* ptemp = pLabel;
            // The fact that this is REQUIRED is rediclous...
            __kAutoMap_80085F70(&ID, psoundconverter, pLabel);
            AudWrap::setNextIdStageBgm(ID);
            MR::stopStageBGM(wipeout);
            MR::setStageBGMState(state, statechange);
            //OSReport("BGM: IsStopOrFadeOut = FALSE | isPlayingStageBgmName = FALSE\n");
        }
    }
}

s32 PictureBookLayout::getTextureNum(s32 chapterNo) const {
    return mAllPageTextureStorage[chapterNo-1].mCount;
}

s32 PictureBookLayout::getPageNum(s32 chapterNo) const {
    for (s32 pageNo = 0; pageNo < cMaxPagePerChapter; pageNo++) {
        char messageId[64];
        snprintf(messageId, sizeof(messageId), "%sChapter%d_Page%d_%03d", mLayoutName, chapterNo, pageNo + 1, 0);

        if (MR::isExistGameMessage(messageId)) {
            continue;
        }

        return pageNo;
    }

    return 1; // minimum 1 because of the title, which always exists
}


bool PictureBookLayout::textNext() {
    if (mPageNo == 0) {
        return false;
    }

    mTextIndex += mNextItemDir;

    if (mTextIndex < 0) {
        return false;
    }

    return updateText();
}

bool PictureBookLayout::pageNext() {
    mPageNo += mNextItemDir;

    if (mPageNo < 0) {
        return false;
    }

    if (mPageNo != 0) {
        if (mNextItemDir > 0) {
            mTextIndex = 0;
        }
        else {
            mTextIndex = getCurrentMaxTextIndex();
        }
    }

    return updateText();
}

bool PictureBookLayout::chapterNext() {
    if (mNextItemDir > 0) // going forward? We can complete the chapter
        PictureBookDataUtil::setReadChapter(mBookName, mChapterNo);

    do
    {
        mChapterNo += mNextItemDir;
    } while (mChapterNo >= mChapterMin && mChapterNo <= mChapterMax && !isOpenChapter(mChapterNo));

    if (mChapterNo > mChapterMax || mChapterNo < mChapterMin) {
        return false;
    }

    if (mNextItemDir > 0) {
        mPageNo = 0;
        mTextIndex = 0;
    }
    else {
        mPageNo = getPageNum(mChapterNo);
        mTextIndex = getCurrentMaxTextIndex();
    }

    return true;
}


void PictureBookLayout::updateTexMapChapterBase() {
    if (mChapterNo == 0) {
        mChapterPageTextureStorage = nullptr;
        return;
    }

    mChapterPageTextureStorage = mAllPageTextureStorage[mChapterNo-1].mTex;
    //OSReport("Registered tex for chapter %d\n", mChapterNo-1);
}

bool PictureBookLayout::isReadedCurrentText() const {
    if (PictureBookDataUtil::isReadChapter(mBookName, mChapterNo))
        return true;

    bool r7;
    bool r5;
    bool result;

    result = true;
    r7 = true;
    if (mChapterNo >= mNotReadedChapterNo) {
        r5 = false;
        if (mChapterNo == mNotReadedChapterNo) {
            if (mPageNo < mNotReadedPageNo) {
                r5 = true;
            }
        }
        if (!r5) {
            r7 = false;
        }
    }
    if (!r7) {
        r5 = false;
        if (mChapterNo == mNotReadedChapterNo) {
            if (mPageNo == mNotReadedPageNo) {
                if (mTextIndex <= mNotReadedTextIndex) {
                    r5 = true;
                }
            }
        }
        if (!r5) {
            result = false;
        }
    }
    return result;
}

s32 PictureBookLayout::getReadSpeed() const {
    return mIsNextItemFast ? 3 : 1;
}

bool PictureBookLayout::isBookEndCurrentText() const {
    return mChapterNo == mChapterMax && mPageNo == getPageNum(mChapterNo) && mTextIndex == getCurrentMaxTextIndex();
}

void PictureBookLayout::setTextAlpha(f32 alpha) {
    MR::setPaneAlphaFloat(this, "Text", alpha);
    MR::setPaneAlphaFloat(this, "Title", alpha);
    MR::setPaneAlphaFloat(this, "Contents", alpha);
}

s32 PictureBookLayout::getChapterNumberMax() const {
    // Count how many chapters are unlocked
    s32 numchaptersunlock = 0;
    s32 numchaptersread = 0;
    for (s32 i = 0; i < mChapterMax; i++) {
        if (mChapterUnlockFlags[i])
            numchaptersunlock++;

        if (mChapterReadFlags[i])
            numchaptersread++;
    }

    if (mIsAutoPlay)
        return numchaptersunlock - numchaptersread; // "Rosalina" will read all new chapters
    return numchaptersunlock;
}

bool PictureBookLayout::isValidCloseButton() const {
    if (mIsAutoPlay) {
        return false;
    }

    return isNerve(&NrvPictureBookLayout::PictureBookLayoutNrvOpen::sInstance)
        || isNerve(&NrvPictureBookLayout::PictureBookLayoutNrvContentsSelect::sInstance)
        || isNerve(&NrvPictureBookLayout::PictureBookLayoutNrvOpen::sInstance)
        || mPageNo == 0;
}

bool PictureBookLayout::isSelectedCloseButton() const {
    return isValidCloseButton() && mCloseButton->isSelected();
}

s32 PictureBookLayout::getCurrentMaxTextIndex() const {
    s32 chapterNo;
    s32 pageNo;

    pageNo = mPageNo;
    chapterNo = mChapterNo;

    for (s32 i = 0; i < 100; i++) {
        char messageId[64];
        snprintf(messageId, sizeof(messageId), "%sChapter%d_Page%d_%03d", mLayoutName, chapterNo, pageNo, i);

        if (MR::isExistGameMessage(messageId)) {
            continue;
        }

        return i - 1;
    }

    return 0;
}

void PictureBookLayout::hideContents() {
    for (s32 i = 0; i < mChapterLayoutButtonNum; i++) {
        mContentsButtonPaneController[i]->forceToHide();
    }
}

f32 PictureBookLayout::getFadeInAlphaTextBG(f32 alpha) const {
    bool var;
    if (!mPageNo || isBookEndCurrentText()) {
        return 0.0f;
    }
    var = false;
    if (mNextItemDir > 0) {
        if (!mTextIndex) {
            var = true;
        }
    }
    else if (getCurrentMaxTextIndex() == mTextIndex) {
        if (mPageNo < getTextureNum(mChapterNo)) {
            var = true;
        }
    }
    if (var) {
        return alpha;
    }
    return 1.0f;
}

f32 PictureBookLayout::getFadeOutAlphaTextBG(f32 alpha) const {
    bool var;
    if (!mPageNo || isBookEndCurrentText()) {
        return 0.0f;
    }
    var = false;
    if (mNextItemDir > 0) {
         if (getCurrentMaxTextIndex() == mTextIndex) {
            if (mPageNo < getTextureNum(mChapterNo)) {
                var = true;
            }
        }
    }
    else if(mTextIndex == 0 && mPageNo > 0){
        var = true;
    }
    if (var) {
        return alpha;
    }
    return 1.0f;
}




void PictureBookLayout::exeOpen() {
    nw4r::lyt::TexMap* pTexMap;

    if (MR::isFirstStep(this)) {
        updateTexMapChapterBase();
        updateText();
        MR::hidePaneRecursive(this, "Text");
        MR::hidePaneRecursive(this, "Title");
        MR::hidePaneRecursive(this, "PicToneDown");
        pTexMap = mCoverFrontTexMap;
        MR::replacePaneTexture(this, "PicLeftPage", pTexMap, 0);
        MR::replacePaneTexture(this, "PicTurnRightPage", pTexMap, 0);
        pTexMap = mTitleTexMap;
        MR::replacePaneTexture(this, "PicRightPage", pTexMap, 0);
        MR::replacePaneTexture(this, "PicTurnLeftPage", pTexMap, 0);
        MR::startAnim(this, "Appear", 0);
        MR::setAnimFrameAndStop(this, 0.0f, 0);
        MR::openWipeFade(cFadeFrame);
        updateBgm(0, 0, 0); // Chapter 0 page 0 text 0 is the default song
    }
    if (MR::isStep(this, cBookOpenFrame)) {
        MR::setAnimRate(this, 1.0f, 0);
    }

    if (MR::isStep(this, 140)) {
        MR::startSystemSE("SE_SY_PICTUREBOOK_NEXT_ST", -1, -1);
    }

    // s32 animFrameMax = MR::getAnimFrameMax(this, (u32)0);    // The game does not read this right and it just returns 0
    s32 animFrameMax = 160;
    s32 stepMin = animFrameMax + cBookOpenFrame;
    s32 stepMax = stepMin + cFadeTextFrame;

    if (MR::isStep(this, stepMin)) {
        for (s32 i = 0; i < mChapterMax; i++) {
            if (isOpenChapter(i+1))
                mContentsButtonPaneController[i]->appear();
        }

        mCloseButton->appear();
    }
    
    if (MR::isGreaterEqualStep(this, stepMin)) {
        setTextAlpha(MR::calcNerveRate(this, stepMin, stepMax));

        if (isValidCloseButton()) {
            MR::requestStarPointerModePictureBook(this);
        }
    }
    MR::setNerveAtStep(this, &NrvPictureBookLayout::PictureBookLayoutNrvContentsSelect::sInstance, stepMax);
}

void PictureBookLayout::exeContentsSelect() {
    if (isValidCloseButton()) {
        MR::requestStarPointerModePictureBook(this);
    }
    if (mCloseButton->trySelect()) {
        setNerve(&NrvPictureBookLayout::PictureBookLayoutNrvContentsFadeOut::sInstance);
    }
    else {
        for (s32 i = 0; i < mChapterMax; i++) {
            if (mContentsButtonPaneController[i]->isPointingTrigger()) {
                MR::startSystemSE("SE_SY_PICBOOK_CONTENTS_CUR", -1, -1);
            }

            if (mContentsButtonPaneController[i]->trySelect()) {
                MR::startSystemSE("SE_SY_TALK_OK", -1, -1);
                mChapterNo = i + 1;
                mCloseButton->disappear();
                setNerve(&NrvPictureBookLayout::PictureBookLayoutNrvContentsFadeOut::sInstance);
                break;
            }
        }
    }
}

void PictureBookLayout::exeContentsFadeOut() {
    s32 stepMin = 20; // MR::getPaneAnimFrameMax(this, cContentsPaneName[mChapterNo - 1], 0);
    s32 stepMax = stepMin + cFadeTextFrame;

    if (MR::isGreaterEqualStep(this, stepMin)) {
        setTextAlpha(1.0f - MR::calcNerveRate(this, stepMin, stepMax));
    }

    if (MR::isLessStep(this, stepMax)) {
        if (isValidCloseButton()) {
            MR::requestStarPointerModePictureBook(this);
        }
    }

    if (isSelectedCloseButton()) {
        if (MR::isGreaterStep(this, stepMax)) {
            if (MR::isDead(mCloseButton)) {
                hideContents();
                setNerve(&NrvPictureBookLayout::PictureBookLayoutNrvClose::sInstance);
            }
        }
    } else {
        if (MR::isStep(this, stepMax)) {
            MR::closeWipeFade(cFadeFrame);
        }

        if (MR::isGreaterStep(this, stepMax) && !MR::isWipeActive()) {
            hideContents();
            setNerve(&NrvPictureBookLayout::PictureBookLayoutNrvFadeIn::sInstance);
        }
    }
}

void PictureBookLayout::exeFadeIn() {
    if (MR::isFirstStep(this)) {
        updateTexMapChapterBase();
        updateText();
        MR::hidePaneRecursive(this, "Text");
        MR::hidePaneRecursive(this, "Title");
        MR::hidePaneRecursive(this, "PicToneDown");
        MR::showPaneRecursive(this, "PicToneDown");

        f32 alpha;

        if (mNextItemDir > 0) {
            alpha = 0.0f;
        }
        else {
            alpha = 1.0f;
        }

        MR::setPaneAlphaFloat(this, "PicToneDown", alpha);

        mNextItemDir = -1; // I have no clue of why is this needed now

        updateTexture();
        
        MR::startAnim(this, "PageNext", 0);

        f32 animFrame;

        if (mNextItemDir > 0) {
            animFrame = MR::getAnimFrameMax(this, (u32)0);
            OSReport("AnimFrameMax: PageNext %ff\n", animFrame);
            animFrame = 60;
        }
        else {
            animFrame = 0.0f;
        }

        MR::setAnimFrameAndStop(this, animFrame, 0);
        MR::openWipeFade(cFadeFrame / getReadSpeed());

        if (mIsAutoPlay) {
            updateBgm(mChapterNo, mPageNo, mTextIndex);
        }
    }

    if (!MR::isWipeActive()) {
        setNerve(&NrvPictureBookLayout::PictureBookLayoutNrvWaitNoText::sInstance);
    }
}

void PictureBookLayout::exeWaitNoText() {
    MR::setNerveAtStep(this, &NrvPictureBookLayout::PictureBookLayoutNrvFadeInText::sInstance, mIsNextItemFast ? 0 : cWaitNoTextFrame);
}

void PictureBookLayout::exeFadeInText() {
    if (MR::isFirstStep(this)) {
        if (mPageNo == 0) {
            MR::showPaneRecursive(this, "Title");

            if (!mIsAutoPlay) {
                mCloseButton->appear();
            }
        }
        else {
            MR::showPaneRecursive(this, "Text");
            MR::showPaneRecursive(this, "PicToneDown");
            MR::startPaneAnim(this, "Text", "TextColor", 0);

            if (isReadedCurrentText()) {
                MR::setPaneAnimFrameAndStop(this, "Text", 1.0f, 0);
            }
            else {
                MR::setPaneAnimFrameAndStop(this, "Text", 0.0f, 0);
            }
        }

        updateBgm(mChapterNo, mPageNo, mTextIndex);
       
        if (isBookEndCurrentText()) {
            setTextBoxHorizontalPositionRecursive(this, "Text", 1);
            MR::setTextBoxVerticalPositionCenterRecursive(this, "Text");
        }
        else {
            setTextBoxHorizontalPositionRecursive(this, "Text", 0);
            setTextBoxVerticalPositionRecursive(this, "Text", 0);   // Crazy that TopRecursive isn't in SMG2 when it would be just a copy of SMG1
        }

        if (mIsNextItemFast && !isReadedCurrentText()) {
            mIsNextItemFast = false;
        }
    }

    if (isValidCloseButton()) {
        MR::requestStarPointerModePictureBook(this);
    }

    s32 step = 30 / getReadSpeed();
    f32 alpha = MR::calcNerveRate(this, step);

    MR::setPaneAlphaFloat(this, "Text", alpha);
    MR::setPaneAlphaFloat(this, "Title", alpha);
    MR::setPaneAlphaFloat(this, "Contents", alpha);
    MR::setPaneAlphaFloat(this, "PicToneDown", getFadeInAlphaTextBG(alpha));
    MR::setNerveAtStep(this, &NrvPictureBookLayout::PictureBookLayoutNrvWaitWithText::sInstance, step);
}

void PictureBookLayout::exeWaitWithText() {
    if (MR::isFirstStep(this)) {
        openWithTurn(mIconAButton);
    }

    if (isValidCloseButton()) {
        MR::requestStarPointerModePictureBook(this);
    }

    if (isValidCloseButton() && mCloseButton->trySelect()) {
        setNerve(&NrvPictureBookLayout::PictureBookLayoutNrvFadeOutText::sInstance);
    }
    else {
        // TODO: rewrite so that chapters that are locked are skipped
        bool isTriggerNextPage = MR::testCorePadTriggerA(0) || MR::testCorePadTriggerRight(0) || MR::testSubPadStickTriggerRight(0);

        if (isTriggerNextPage) {
            MR::startSystemSE("SE_SY_TALK_FOCUS_ITEM", -1, -1);

            if (mNextItemDir > 0 && !isReadedCurrentText()) {
                mNotReadedChapterNo = mChapterNo;
                mNotReadedPageNo = mPageNo;
                mNotReadedTextIndex = mTextIndex;
            }

            if (isValidCloseButton()) {
                mCloseButton->disappear();
            }

            mNextItemDir = 1;

            setNerve(&NrvPictureBookLayout::PictureBookLayoutNrvFadeOutText::sInstance);
        }
        else {
            bool isTriggerPrevPage = MR::testCorePadTriggerLeft(0) || MR::testSubPadStickTriggerLeft(0);

            if (isTriggerPrevPage) {
                s32 curChapter = mChapterNo;
                do
                {
                    curChapter += -1;
                } while (curChapter >= mChapterMin && !isOpenChapter(curChapter));

                if (curChapter >= mChapterMin || (mPageNo > 0 || mTextIndex > 0))
                {
                    MR::startSystemSE("SE_SY_TALK_FOCUS_ITEM", -1, -1);

                    if (isValidCloseButton()) {
                        mCloseButton->disappear();
                    }

                    mIsNextItemFast = true;
                    mNextItemDir = -1;

                    setNerve(&NrvPictureBookLayout::PictureBookLayoutNrvFadeOutText::sInstance);
                }
            }
        }
    }
}

void PictureBookLayout::exeFadeOutText() {
    if (MR::isFirstStep(this)) {
        mIconAButton->term();
    }

    if (isValidCloseButton()) {
        MR::requestStarPointerModePictureBook(this);
    }

    s32 step = 30 / getReadSpeed();
    f32 alpha = 1.0f - MR::calcNerveRate(this, step);

    MR::setPaneAlphaFloat(this, "Text", alpha);
    MR::setPaneAlphaFloat(this, "Title", alpha);
    MR::setPaneAlphaFloat(this, "Contents", alpha);
    MR::setPaneAlphaFloat(this, "PicToneDown", getFadeOutAlphaTextBG(alpha));

    if (isSelectedCloseButton()) {
        if (MR::isGreaterStep(this, step) && MR::isDead(mCloseButton)) {
            setNerve(&NrvPictureBookLayout::PictureBookLayoutNrvClose::sInstance);
        }
    }
    else if (MR::isStep(this, step)) {
        if (textNext()) {
            setNerve(&NrvPictureBookLayout::PictureBookLayoutNrvFadeInText::sInstance);
        }
        else if (pageNext()) {
            setNerve(&NrvPictureBookLayout::PictureBookLayoutNrvPageNext::sInstance);
        }
        else if (chapterNext()) {
            setNerve(&NrvPictureBookLayout::PictureBookLayoutNrvFadeOut::sInstance);
        }
        else if (mIsAutoPlay) {
            setNerve(&NrvPictureBookLayout::PictureBookLayoutNrvFadeOut::sInstance);
        }
        else {
            setNerve(&NrvPictureBookLayout::PictureBookLayoutNrvClose::sInstance);
        }
    }
}

void PictureBookLayout::exePageNext() {
    if (MR::isFirstStep(this)) {
        MR::hidePaneRecursive(this, "Text");
        MR::hidePaneRecursive(this, "Title");
        MR::hidePaneRecursive(this, "PicToneDown");
        updateTexture();

        if (mNextItemDir > 0) {
            MR::startAnim(this, "PageNext", 0);
        }
        else {
            MR::startAnimReverseOneTime(this, "PageNext", 0);
        }

        if (mIsNextItemFast) {
            MR::startSystemSE("SE_SY_PICTUREBOOK_NEXT_F_ST", -1, -1);
        }
        else {
            MR::startSystemSE("SE_SY_PICTUREBOOK_NEXT_ST", -1, -1);
        }

        MR::setAnimRate(this, mNextItemDir * getReadSpeed(), 0);
    }

    if (mIsNextItemFast) {
        if (MR::isStep(this, cPageNextEndFastSeStep)) {
            MR::startSystemSE("SE_SY_PICTUREBOOK_NEXT_F_ED", -1, -1);
        }
    }
    else if (MR::isStep(this, cPageNextEndNormalSeStep)) {
        MR::startSystemSE("SE_SY_PICTUREBOOK_NEXT_ED", -1, -1);
    }

    MR::setNerveAtAnimStopped(this, &NrvPictureBookLayout::PictureBookLayoutNrvWaitNoText::sInstance, 0);
}

void PictureBookLayout::exeFadeOut() {
    if (MR::isFirstStep(this)) {
        MR::closeWipeFade(cFadeFrame / getReadSpeed());

        if (mIsAutoPlay) {
            MR::stopStageBGM(cFadeFrame / getReadSpeed());
        }
    }

    if (MR::isWipeActive()) {
        return;
    }

    if (mChapterMax >= mChapterNo) {
        setNerve(&NrvPictureBookLayout::PictureBookLayoutNrvFadeIn::sInstance);
    }
    else {
        kill();
    }
}

void PictureBookLayout::exeClose() {
    nw4r::lyt::TexMap* pTexMap;

    if (MR::isFirstStep(this)) {
        MR::hidePaneRecursive(this, "Text");
        MR::hidePaneRecursive(this, "Title");
        MR::hidePaneRecursive(this, "PicToneDown");
        MR::startAnim(this, "End", 0);

        if (isSelectedCloseButton()) {
            pTexMap = mTitleTexMap;

            MR::replacePaneTexture(this, "PicLeftPage", pTexMap, 0);
            MR::replacePaneTexture(this, "PicTurnRightPage", pTexMap, 0);
        } else {
            if ((mPageNo - 2) < 0)
                pTexMap = mTitleTexMap;
            else
                pTexMap = mChapterPageTextureStorage[mPageNo - 2].mTexture;

            MR::replacePaneTexture(this, "PicLeftPage", pTexMap, 0);
            MR::replacePaneTexture(this, "PicTurnRightPage", pTexMap, 0);
        }

        pTexMap = mCoverBackTexMap;

        MR::replacePaneTexture(this, "PicRightPage", pTexMap, 0);
        MR::replacePaneTexture(this, "PicTurnLeftPage", pTexMap, 0);
    }

    // s32 step = MR::getAnimFrameMax(this, (u32)0);    // Okay this thing doesn't work. Maybe because it's a SMG1 layout?
    s32 step = 160;

    if (MR::isStep(this, step)) {
        MR::closeWipeFade(cFadeFrame);
        MR::stopStageBGM(cFadeFrame);
    }

    if (MR::isStep(this, 150)) {
        MR::startSystemSE("SE_SY_PICTUREBOOK_END", -1, -1);
    }

    if (MR::isGreaterStep(this, step)) {
        if (!MR::isWipeActive()) {
            kill();
        }
    }
}