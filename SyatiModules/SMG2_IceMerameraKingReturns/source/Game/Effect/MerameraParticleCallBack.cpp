#include "Game/Effect/MerameraParticleCallBack.h"

#include "Game/Util/PlayerUtil.h"

// This is insanely jank because I don't want 
void MerameraParticleCallBack::execute(JPABaseEmitter* emitter, JPABaseParticle* baseparticle) {
    TVec3f baseParticlePosition;
    baseParticlePosition.set(*(TVec3f*)(void*)baseparticle); // EXTREME HACK
    TVec3f pullvel;
    MR::calcPlayerSpinPullVelocity(&pullvel, baseParticlePosition);
    TVec3f* insanejank = (TVec3f*)(void*)baseparticle; // baseparticle->mOffsetPosition
    insanejank++;
    insanejank++;
    TVec3f baseParticleOffsetPosition;
    baseParticleOffsetPosition.set(*insanejank);
    f32* alsojank = (f32*)(void*)emitter;
    alsojank++;
    insanejank->set(baseParticleOffsetPosition + (pullvel * *alsojank));
}