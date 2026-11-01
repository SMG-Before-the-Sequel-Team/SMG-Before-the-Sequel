# SMG2_WaterfallCaveCoverRestored

The Underwater Cave Cover from Beach Bowl Galaxy's 3rd Star, now in SMG2!

You have to add the following into `ProductMapObjDataTable.bcsv`.

| ModelName            | ClassName       |
| -------------------- | --------------- |
| WaterfallCaveCover   | WaterfallCaveCover |

You do not have to add anything to `ActionSound.bcsv` for sounds to work properly!

The objects require particles. (Included in the `ParticleData` folder)

---

WaterfallCaveCover can be broken with 2 methods.
1. By Using Koopa Shells (The Original Design)

2. if the player is in a "rush" state. These include:
- Rock Mario is rolling
- flying through it with a Launch Star
- While Using the Spin Drill

---


This object has gotten some new Obj_Args and Switches.
---
Obj_Arg0: How many hits the Cover can take. (Default: 1. Max Hits: 5)
Obj_Arg1: The Cooldown between Hits. (THIS IS VERY USEFUL FOR IN RUSH SETUPS) (Default: 30)
Obj_Arg2: What Sound to Use when the Object is Destroyed. (Default: SE_SY_READ_RIDDLE_S) (Other Sound is SE_SY_READ_RIDDLE_SS)

### Switches
SW_DEAD: Activates upon the object being destroyed.
SW_B: Will automatically destroy the object regardless of hits.