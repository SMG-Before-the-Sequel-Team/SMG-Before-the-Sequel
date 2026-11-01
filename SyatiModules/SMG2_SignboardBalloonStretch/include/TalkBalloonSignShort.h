#pragma once

#include "syati.h"

class TalkBalloonSignShort : public TalkBalloonShort {
public:
	TalkBalloonSignShort(const char*);

	virtual void init(const JMapInfoIter& rIter);
};