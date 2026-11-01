#pragma once

#include "ActorCreation.h"

{{IncludeList}}


const PostCreateEntry cModulePostCreateTable[] = {
	//DUMMY - This is never read
	{ "1234567890", NULL },
{{PostCreateFuncList}}
};

const s32 cModulePostCreateTableCount = sizeof(cModulePostCreateTable) / sizeof(PostCreateEntry);