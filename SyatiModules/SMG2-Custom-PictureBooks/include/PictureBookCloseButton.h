#pragma once

#include "syati.h"

class ButtonPaneController;

class PictureBookCloseButton : public LayoutActor {
public:
    /// @brief Creates a new `PictureBookCloseButton`.
    PictureBookCloseButton(bool canCloseWithPad);

    virtual void init(const JMapInfoIter& rIter);
    virtual void appear();
    virtual void control();

    void disappear();
    bool trySelect();
    bool isSelected() const;
    void exeSelect();
    void exeSelected();
    void exeNotSelected();

private:
    /* 0x20 */ ButtonPaneController* mPaneCtrl;
    /* 0x24 */ bool mCanCloseWithPad;
};
