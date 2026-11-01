# SMG2 SeesawMoveNut (Bolt Lift)

This module ports the Bolt Lifts from SMG1 over to SMG2 with some slight modifications. It is now a ProductMapObjDataTable class, so you can use your own model, or the included SeesawMoveNutB for the one from Dreadnought Galaxy.

## Behavior

This is a MapParts object which requires a path to move on. If the player stands on one side, it starts to rotate and move down the path, and stops at the last path point. If the player moves to the other side, it starts rotating and moving in the reverse direction.

New to this module is support for multiple axis movement. In SMG1, the object could only rotate around the axis of its path direction. But now, the object rotates around the axis of its front vector, so depending on how it is placed in the editor, it can rotate in any desired manner while still moving down the same path. For instance, applying a Y rotation of 90 degrees allows you to obtain behavior like the rolling log from Super Mario 64.

## Setup

- Add a model.arc to your ObjectData. Create your own or use the provided SeesawMoveNutB.arc for the Dreadnought Galaxy bolt lift.
- If you use a new model.arc, you must include ActorInfo with InitActor.bcsv and InitSensor.bcsv. It's easiest if you copy the files from the provided SeesawMoveNutB.arc, and simply change all instances of `SeesawMoveNutB` to your object's name.
- Add an entry to ProductMapObjDataTable for your object and give it the class `SeesawMoveNut`.
- If you want sound effects, you must add five entries to ActionSound.bcsv. A file is provided for you here on this repo which contains the necessary entries. If you use a different object name, change the corresponding name. The sounds are set to the default used by SeesawMoveNutB, but you can change them if you want.
- Place your object in the level, and give it a path. No path or point arguments are required.
- Rotate the object to your desired orientation, which will determine how it will rotate along the path.

### Parameters
- `obj_arg0` is the movement acceleration, which determines how much the object's rotation affects its movement speed down the path. Default value is 300.
- `RotateSpeed` is the rotation speed. This affects how fast it actually rotates, which indirectly affects how fast it moves down the path as well, and how difficult it is for the player to stay on the object. SMG1 uses a value of 70.
- `RotateAccel` is the rotation acceleration. This affects how quickly it rotates to top speed as the player is on it. SMG1 uses a value of 50.
- `RotateAxis` MUST be set to 0! Use the X,Y,Z rotation to choose your axis instead!
