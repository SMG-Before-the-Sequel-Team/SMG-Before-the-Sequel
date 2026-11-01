# LavaStrangeRock

The weird stone columns from Melty Molten Galaxy, now in SMG2!

You have to add the following into `ProductMapObjDataTable.bcsv`.

| ModelName            | ClassName       |
| -------------------- | --------------- |
| LavaStrangeRock      | LavaStrangeRock |
| LavaStrangeRockL     | LavaStrangeRock |
| LavaStrangeRockTable | LavaStrangeRock |

All three objects have entries in `ActionSound.bcsv`, so that should work as-is.

The objects require particles.

---

LavaStrangeRock can be broken if the player is in a "rush" state. These include:

- Spin Drill drilling
- Rock Mario is rolling
- flying through it with a Launch Star
- probably something else

The objects activate SW_DEAD when destroyed.

If SW_B is activated, the object gets destroyed.