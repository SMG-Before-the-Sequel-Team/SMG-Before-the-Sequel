# SMG2 TombSpiderReturns

## Tarantox in SMG2

This is the surprise port of Tarantox to SMG2!

Below is a list of everything that's included:
- The ObjectData archives for Tarantox, the Planet, the Sling Shooters, the breakable Rocks and some Mario animations.
- The "SpiderMapBlock" ProductMapObjDataTable classe.
- JSON files for particle effects.
- BTI files for particle effects.

Below is a list of other things that you will need:

- A level editor.
- A Effect JSON supported particle editor.
- A UseResource file with all of Tarantox's sound labels.

## Credits
- @mrl - Decompiled a majority of Tarantox
- @superhackio - Decompiled a bit of Tarantox and ported & enhanced SMG1 code to SMG2.

## Setup
It is adviced to reference the template mod for this port. Regardless here are some parameters that apply to Tarantox.

### TombSpider
| Parameter | Description |
| --- | --- |
| SW_A | Activates after being flung over. Deactivates when Tarantox recovers or the third of his Vital Spots gets destroyed. Applys to any phase. |
| SW_B | Activates when Tarantox is defeated. |
> *Note: SW_DEAD is not used by Tarantox, despite being set in SMG1.*

### SpiderMapBlock
| Parameter | Description |
| --- | --- |
| SW_DEAD | Activated after being destroyed. |
> *Note: This switch works due to ActorInfo, not due to the programming of the object.*