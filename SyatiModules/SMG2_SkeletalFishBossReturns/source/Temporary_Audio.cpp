// Hello!
// If you're reading this, you're probably browsing the source code of this module.
// Okay so what is this file? (and why does it get a special explaination?)
// In SMG1, Nintendo had a different resource management system in place that worked differently to SMG2's new UseResource system.
// Now, that wouldn't be so much of an issue in this scenario if there weren't one, simple factor in the way:
// In SMG2, there are 3 inaccessable .aw files -- One of those 3 has all of Kingfin's sounds.
// As a result of this, we had to do grueling research to figure out exactly how to add a new .aw to the game.
// THIS WAS NOT AN EASY TASK and almost got the port cancelled.
//
// Okay, dramatic explaination aside, what is this code for?
// In order for SMG2's UseResource system to work, it has a lookup table for every sound ID in the game, which is then used to track down
// what instrument bank represents the sound data it's looking for. This will be used to load the correct .aw file into memory.
// Without the .aw being loaded, there were no sounds.
// Unfortunately, Nintendo made a hardcoded table to manage this...
// So this code extends the lookup table to include our custom entry.
// The WSYS ID Represents the actual in-game ID of the WSYS data, while the File ID represents the ID of the WSYS file that's inside the SMR.szs file lookup table. (Hex editing that was a nightmare btw)
//
// If you are familiar with the already existing Primary Syati Modules (such as Syati_Init or Syati_ObjectFactories), this will look pretty familiar
// Due to the extremely niche use case, a new module has not been created for this functionality, as there isn't yet a need to do so.
// Once a public tool for sound effect and .aw creation is created, I will create a new Primary module that will allow other modules to register custom UseResource WSYS lookup entries.
// But until that happens, this file will remain (unless one of the other 2 inaccessable .aw files is needed by a module)
// Ideally this is handled by a BCSV instead of hardcoded code (since sounds can be assigned to objects via ActionSound) so the module may not actually need registering like this.
// It's also possible that GLE will get it's own standalone implementation. Depends on how easily adding custom sounds becomes once tools are available, because in my opinion, custom sounds shouldn't mandate the use of both GLE and Syati.
//
// Also, fun fact, it took me 3 hours to figure out where to put the kmBranch in the first place :^)

#include "syati.h"

struct AWMapEntry
{
public:
	s32 WSYSID;
	s32 FileID;
};

const AWMapEntry cAWRemapTableCustom[] = {
	{ 20, 61 },

};
const s32 cAWRemapTableCustomCount = sizeof(cAWRemapTableCustom) / sizeof(AWMapEntry);

s32 getAdditionalRemaps(s32 WSYSID)
{
	for (s32 i = 0; i < cAWRemapTableCustomCount; i++)
	{
		if (cAWRemapTableCustom[i].WSYSID == WSYSID)
			return cAWRemapTableCustom[i].FileID;
	}
	return -1;
}

kmBranch(0x8007C314, getAdditionalRemaps);