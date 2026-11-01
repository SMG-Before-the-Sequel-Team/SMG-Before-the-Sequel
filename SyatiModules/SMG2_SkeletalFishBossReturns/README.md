# SMG2 SkeletalFishBossReturns
### Kingfin in SMG2

This is a port of Kingfin from Super Mario Galaxy. This was done using [Petari](https://github.com/SMGCommunity/Petari) and the skilled contributions from @shibbo of the SMGCommunity Decompilation Team.

Below is a list of everything included:
- The ObjectData archives Kingfin, his Bonefin Guards, the breakable stone pillars used in Bonefin Galaxy, and the unused breakable shell cones (which are the same class as the used pillars)
- The "SkeletalFishBoss", "SkeletalFishBossRail", "SubmarineVolcanoBigColumn", and "SubmarineVolcanoSmallColumn" objects in the Editor
- An edited SMR.szs file that allows Kingfin's sounds to be played (as they are dummied out in vanilla SMG2)
- An edited ActionSound.arc file that includes edits that were made to ActionSound (mainly regarding the stone Column objects)
- An edited SoundIdToInstList.arc file that tells the game to load the unused .aw file that contains Kingfin's sounds.
- An edited Effect.arc file that includes particle effects for all included objects

Below is a list of other things that you will need:
- [SkeletalFishBoss object documentation](https://smgcommunity.github.io/galaxydatabase/config_SkeletalFishBoss.html)
- [SkeletalFishBossRail object documentation](https://smgcommunity.github.io/galaxydatabase/config_SkeletalFishBossRail.html)
- A level editor

Setting up Kingfin works the same way that it does in SMG1.

## Credits
- @shibbo - SMG1 code decompilation
- @headpenguin - Implemented TVec::cubic
- @sy-24 - Sound Effect research & reinstating
- @superhackio - Ported SMG1 code to SMG2

## Bugfixes
Kingfin in SMG1 wasn't perfectly programmed. For this port, the code has been modified to fix some bugs.
- Kingfin's Headlights work properly 100% of the time (the "Confused Kingfin" glitch is fixed)
- The Bonefin Guards animations when respawning from a cutscene have been fixed.
