# WarpBox

Adds the WarpBox from Super Mario 3D World.

*This object is work in progress. It is fully functional right now, but new features will be added in the future.*

## Effects
Before using the Warp Box, make sure to import this effect using pygapa:
```json
[
    {
        "GroupName": "WarpBox",
        "UniqueName": "PlayerInSmoke",
        "EffectName": [
            "DeathSmokeS"
        ],
        "JointName": "root",
        "OffsetX": 0.0,
        "OffsetY": 0.0,
        "OffsetZ": 0.0,
        "StartFrame": -1,
        "ScaleValue": 1.0,
        "RateValue": 1.0,
        "LightAffectValue": 0.0,
        "DrawOrder": "3D",
        "Affect": [
            "T",
            "R"
        ]
    }
]
```