# Mushroom Bounce Pads
### From Super Mario 3DL/3DW

This module allows you to use the Bounce Pads from Super Mario 3D Land / World in SMG2!

You can also create your own bouncy platforms with this module. Use the provided archives in the `Disc/ObjectData` folder as an example.

Below is a list of everything included:
- The "KinokoTrampoline" ProductMapObjDataTable class.
- The ObjectData archives for 3 Height Variants of bounce pads.
  - Green: Low Height
  - Orange: Medium Height
  - Red: High Height
> *Note: Bounce Heights are determined by the floor collision codes on the bounce pad's collision models. There are 3 KCL collision codes for bouncy floor, and each can be customized by editing [MarioConst](https://github.com/SuperHackio/SMG2MarioConstEditor ). Be aware that this will affect all objects that use those KCL floor codes.*

Below is a list of other things that you will need:
- A level editor
- Edits to the ProductMapObjDataTable (as you will need to add an entry for each KinokoTrampoline object you want)

## Credits
- @superhackio - Created the Original source code (and then proceeded to accidentally delete it)
- @bavario-lginc - Decompiled a CustomCode.bin to "retrieve" the source code. (The decompiled code is the code in this repository)
- @kairossmg - Helped with the ObjectData models (extracting animations, etc.)

## Features
- Supports everything that a [RailMoveObj](https://smgcommunity.github.io/galaxydatabase/config_RailMoveObj.html ) can do.
- Can be placed in both General objects and MapParts objects
- SW_A can be used to Disable / Enable the bouncing functionality
