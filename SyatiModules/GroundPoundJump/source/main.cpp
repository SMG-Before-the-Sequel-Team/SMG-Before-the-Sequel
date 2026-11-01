#include "syati.h"
#include "kamek/hooks.h"
bool isGPJ = false;

// Shoutouts to Evanbowl
f32 tripleJumpValue = 32.0f;
asm float setJumpHeight() {
    lis r3, isGPJ@ha
    lbz r0, isGPJ@l(r3)
    cmpwi r0, 0
    beq doDefaultJump
    lis r3, tripleJumpValue@ha
    lfs f1, tripleJumpValue@l(r3)    
    blr
doDefaultJump:
    fmr f1, f30
    blr
}
asm void doGPJAnimation () {
    lis r3, isGPJ@ha
    lbz r0, isGPJ@l(r3)
    cmpwi r0, 0
    beq doDefaultJump
    li r0, 2
    blr
doDefaultJump:
    lwz r0, 0x42C(r27)
    blr
}

void doGPJ (Mario *pMario) {
    isGPJ = true;
    pMario->tryJump();
    isGPJ = false;
}

kmCall(0x8039E124, doGPJ);
kmCall(0x803998A4, doGPJAnimation);
kmCall(0x803997DC, setJumpHeight);