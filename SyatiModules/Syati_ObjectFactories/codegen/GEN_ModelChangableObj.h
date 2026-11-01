#pragma once

#include "ActorCreation.h"

{{IncludeList}}


const ChangableObjEntry cNewModelChangableObjTable[] = {
	//DUMMY - This is never read
	{ "1234567890", NULL, NULL },
{{ModelChangableObjList}}
};

const s32 cNewModelChangableObjTableCount = sizeof(cNewModelChangableObjTable) / sizeof(ChangableObjEntry);