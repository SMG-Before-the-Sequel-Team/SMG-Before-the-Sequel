#include "RosettaPictureBook.h"
#include "PictureBookLayout.h"

namespace MR {
    s32 getPictureBookChapterCanRead() {
        return 9;   // Possible BCSV setup later
    }
}

namespace {
    const s32 hFadeOutFrame = 60;
    const s32 hFadeInFrame = 60;
};

namespace NrvRosettaPictureBook {
    FULL_NERVE(HostTypeNrvWait, RosettaPictureBook, Wait);
    FULL_NERVE(HostTypeNrvDemoWait, RosettaPictureBook, DemoWait);
    FULL_NERVE(HostTypeNrvFadeOut, RosettaPictureBook, FadeOut);
    FULL_NERVE(HostTypeNrvReading, RosettaPictureBook, Reading);
    FULL_NERVE(HostTypeNrvFadeIn, RosettaPictureBook, FadeIn);
};

RosettaPictureBook::RosettaPictureBook(const char* pName) : LiveActor(pName), mLayout(nullptr), mIconAButton(nullptr), mIsValidOpenIconAButton(false) {}

void RosettaPictureBook::init(const JMapInfoIter& rIter) {
    MR::processInitFunction(this, rIter, false);

    const char* mObjectName;
    MR::getObjectName(&mObjectName, rIter);
    JMapInfo* InitActor = MR::createInitActorCsvParser(mObjectName, NULL);
    if (InitActor == NULL) { // This should not happen
        OSReport("%s.arc - InitActor.bcsv not found.\n", mObjectName);
        makeActorDead();
        return;
    }
    if (!MR::hasCsvDataItem(InitActor, "InitFunction", "Texture")) { // This setting is mandatory
        OSReport("%s.arc - Texture not assigned.\n", mObjectName);
        makeActorDead();
        return;
    }
    const char* pBookTextureName;
    MR::getCsvDataStrByElement(&pBookTextureName, InitActor, "InitFunction", "Texture", "Data");
    const char* pLayoutOverride = "PictureBook"; // Default value
    if (MR::hasCsvDataItem(InitActor, "InitFunction", "Layout"))
        MR::getCsvDataStrByElement(&pLayoutOverride, InitActor, "InitFunction", "Layout", "Data");


    mBookInfo = MR::tryCreateCsvParser(mObjectName, "InitBook.bcsv");
    if (mBookInfo == nullptr) {
        OSReport("%s.arc - InitBook.bcsv not found.\n", mObjectName);
        makeActorDead();
        return;
    }

    mLayout = new PictureBookLayout(mObjectName, MR::tryCreateCsvParser(mObjectName, "BookBgm.bcsv")); // Set the bool to TRUE to auto read, false for Chapter Select. Make this depend on new unlocks?
#ifdef GALAXY_LEVEL_ENGINE
    mLayout->mChapterUnlockInfo = MR::tryCreateCsvParser(mObjectName, "ChapterInfo.bcsv");
#endif // GALAXY_LEVEL_ENGINE
    mLayout->initBookInfo(pBookTextureName, pLayoutOverride, mBookInfo);

    mIconAButton = new IconAButton(true, false);
    mIconAButton->initWithoutIter();

    initNerve(&NrvRosettaPictureBook::HostTypeNrvWait::sInstance, 0);
    MR::tryRegisterDemoCast(this, rIter);

    makeActorAppeared();
}

void RosettaPictureBook::appear() {
    mIsValidOpenIconAButton = false;
    setNerve(&NrvRosettaPictureBook::HostTypeNrvWait::sInstance);
    LiveActor::appear();
}

void RosettaPictureBook::kill() {
    LiveActor::kill();
    mIconAButton->kill();
}

void RosettaPictureBook::control() {
    mIsValidOpenIconAButton = false;
}

void RosettaPictureBook::attackSensor(HitSensor* pSender, HitSensor* pReceiver) {
    if (MR::isOnGroundPlayer()) {
        mIsValidOpenIconAButton = true;
    }
}


void RosettaPictureBook::exeWait() {
    if (mIsValidOpenIconAButton) {
        if (!mIconAButton->isOpen()) {
            MR::startSystemSE("SE_SY_TALK_BUTTON_APPEAR", -1, -1);
            mIconAButton->openWithRead();
        }
    } else if (mIconAButton->isOpen()) {
        mIconAButton->term();
    }

    if (!mIsValidOpenIconAButton) {
        return;
    }

    if (!mIconAButton->isOpen()) {
        return;
    }

    if (!MR::testCorePadTriggerA(0)) {
        return;
    }
    
    DemoStartRequestUtil::requestStartDemo(this, "ロゼッタ絵本デモ", &NrvRosettaPictureBook::HostTypeNrvFadeOut::sInstance, &NrvRosettaPictureBook::HostTypeNrvDemoWait::sInstance, 2, DemoStartInfo::DEMOTYPE_0, DemoStartInfo::CINEMAFRAMETYPE_1, DemoStartInfo::STARPOINTERTYPE_0, DemoStartInfo::DELETEEFFECTYPE_0);
}

void RosettaPictureBook::exeDemoWait() {}

void RosettaPictureBook::exeFadeOut() {
    if (MR::isFirstStep(this)) {
        MR::startSystemSE("SE_SY_TALK_START", -1, -1);
        MR::requestMovementOn(mIconAButton);
        mIconAButton->term();
        MR::closeWipeCircle(hFadeOutFrame);
        MR::stopStageBGM(hFadeOutFrame);
        MR::startBckPlayer("Wait", (const char*)nullptr);
    }

    if (MR::isGreaterStep(this, hFadeOutFrame)) {
        mLayout->appear();
        setNerve(&NrvRosettaPictureBook::HostTypeNrvReading::sInstance);
    }
}

void RosettaPictureBook::exeReading() {
    if (MR::isFirstStep(this)) {
        mLayout->prepare(false, mBookInfo); // RosettaReading would use TRUE instead of FALSE here...
        mLayout->appear();
    }

    if (MR::isDead(mLayout)) {
        MR::endDemo(this, "ロゼッタ絵本デモ");
        setNerve(&NrvRosettaPictureBook::HostTypeNrvFadeIn::sInstance);
    }
}

void RosettaPictureBook::exeFadeIn() {
    if (MR::isFirstStep(this)) {
        MR::openWipeCircle(hFadeInFrame);
    }

    if (MR::isGreaterStep(this, hFadeInFrame)) {
        setNerve(&NrvRosettaPictureBook::HostTypeNrvWait::sInstance);
    }
}

