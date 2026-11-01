#pragma once

#include "syati.h"
#include "Game/Scene/GameScene.h"

namespace nw4r {
    namespace lyt {
        class TexMap;
        class TextBox;
        class Pane;
    };
};

class ButtonPaneController;
class IconAButton;
class PictureBookCloseButton;

struct PictureBookPageTexEntry {
public:
    const char* mName;
    nw4r::lyt::TexMap* mTexture;
};
struct PictureBookChapterTexEntry {
public:
    PictureBookPageTexEntry* mTex;
    s32 mCount;
};

class PictureBookLayout : public LayoutActor {
public:
    PictureBookLayout(const char* pBookName, const JMapInfo*);

    virtual void appear();
    virtual void kill();
    virtual void control();

    void initBookInfo(const char* pTextureName, const char* pLayoutName, const JMapInfo* pBookInfo);
    void initTexture(const char* pTextureName, const char* pLayoutName, const JMapInfo* pBookInfo);
    void initContentsButton(const char* pLayoutName);
    void prepare(bool autoplay, const JMapInfo* pBookInfo);
    bool isOpenChapter(s32 chapterNo);

    bool updateText();
    void updateTexture();
    void updateBgm(s32 chapter, s32 page, s32 textline);
    s32 getTextureNum(s32 chapterNo) const;
    s32 getPageNum(s32 chapterNo) const;

    bool textNext();
    bool pageNext();
    bool chapterNext();
    void updateTexMapChapterBase();
    bool isReadedCurrentText() const;
    s32 getReadSpeed() const;
    bool isBookEndCurrentText() const;
    void setTextAlpha(f32 alpha);
    s32 getChapterNumberMax() const;
    bool isValidCloseButton() const;
    bool isSelectedCloseButton() const;
    s32 getCurrentMaxTextIndex() const;
    void exeOpen();
    void exeContentsSelect();
    void exeContentsFadeOut();
    void exeFadeIn();
    void exeWaitNoText();
    void exeFadeInText();
    void exeWaitWithText();
    void exeFadeOutText();
    void exePageNext();
    void exeFadeOut();
    void exeClose();
    void hideContents();
    f32 getFadeInAlphaTextBG(f32 alpha) const;
    f32 getFadeOutAlphaTextBG(f32 alpha) const;

private:
    const JMapInfo* mBgmInfo;
    const char* mBookName; // Used to create unique MSBT labels
    const char* mLayoutName; // Used to create unique MSBT labels
    s32 mChapterLayoutButtonNum; // The maximum number of chapters the layout supports. Calculated at runtime.
    bool* mChapterUnlockFlags; // Determine if a chapter is unlocked or not
    bool* mChapterReadFlags; // Determine if a chapter is already read
    s32 mChapterMax; // The max number of chapters in this book
    s32 mChapterMin; // This variable will be refractored out later
    s32 mChapterNo;
    s32 mPageNo;
    s32 mTextIndex;
    s32 mNotReadedChapterNo;
    s32 mNotReadedPageNo;
    s32 mNotReadedTextIndex;
    nw4r::lyt::TexMap* mTitleTexMap;
    nw4r::lyt::TexMap* mCoverFrontTexMap;
    nw4r::lyt::TexMap* mCoverBackTexMap;
    PictureBookChapterTexEntry* mAllPageTextureStorage;
    PictureBookPageTexEntry* mChapterPageTextureStorage;
    s32 mNextItemDir;
    bool mIsNextItemFast;
    bool mIsAutoPlay;
    IconAButton* mIconAButton;
    ButtonPaneController** mContentsButtonPaneController;
    PictureBookCloseButton* mCloseButton;

#ifdef GALAXY_LEVEL_ENGINE
public:
    const JMapInfo* mChapterUnlockInfo;
#endif // GALAXY_LEVEL_ENGINE

};