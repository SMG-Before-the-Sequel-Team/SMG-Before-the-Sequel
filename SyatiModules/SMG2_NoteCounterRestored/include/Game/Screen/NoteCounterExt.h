#pragma once
#include "Game/Screen/NoteCounter.h"
#include "Game/Screen/CountUpPaneRumbler.h"

class NoteCounterExt : public NoteCounter {
public:
    NoteCounterExt(const char* pName);
    virtual ~NoteCounterExt();
    virtual void control();

    CountUpPaneRumbler* mPaneRumbler;
};