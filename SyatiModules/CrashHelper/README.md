# Advanced Crash Helper

*Note: This patch is only a temporary release. It will later be included in TicoKitDebugger with more features.*

This patch allows you to identify the cause of a game crash with more context and information than the standard crash debugger provides.
It is available both as a Syati Module and a Riivolution Patch.
**If you already have Syati setup, using the Module is strongly recommended.**

Features include:
- SRR0 descriptions: Identifies the SRR0 address and tells you what the cause of the crash might be. If it does not know the exact address, it will try to give an educated guess instead.
- Object Initialization Reporting: Information is printed out when objects are getting initialized, including `NameENG`, `NameJPN` `ClassName`, `Zone` and its position. On top of this, the debugger will report if the object accesses a parameter from the StageData. This can perhaps help you to figure out if a parameter is invalid.
- Object Tracking: When an object updates (to be precise: when its `movement` function is called), the debugger keeps track of that. When a crash occurs, it will print out the information of the object that last updated and is therefore likely related to the crash.

When a crash happens, the helper will show the information it was able to gather inside the regular crash debugger on-screen, as well as OSReport it in Dolphin.

I have more ideas for the future, but they will require a separate companion app connected to the game. If you have any suggestions, do let me know!

Code: https://github.com/Lord-G-INC/CrashHelper/

Riivolution Patch (injection address 0x80001800): https://pastebin.com/kh3LZgBq

*This patch is available via SyatiManager.*
