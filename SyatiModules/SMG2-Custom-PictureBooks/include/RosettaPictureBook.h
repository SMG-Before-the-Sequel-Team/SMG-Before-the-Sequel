#pragma once

#include "syati.h"

class IconAButton;
class PictureBookLayout;

class RosettaPictureBook : public LiveActor {
public:
    /// @brief Creates a new `RosettaPictureBook`.
    /// @param pName A pointer to the null-terminated name of the object.
    RosettaPictureBook(const char* pName);

    virtual void init(const JMapInfoIter& rIter);
    virtual void appear();
    virtual void kill();
    virtual void control();
    virtual void attackSensor(HitSensor* pSender, HitSensor* pReceiver);

    void exeWait();
    void exeDemoWait();
    void exeFadeOut();
    void exeReading();
    void exeFadeIn();

    PictureBookLayout* mLayout;
    IconAButton* mIconAButton;
    const JMapInfo* mBookInfo;
    bool mIsValidOpenIconAButton;
};