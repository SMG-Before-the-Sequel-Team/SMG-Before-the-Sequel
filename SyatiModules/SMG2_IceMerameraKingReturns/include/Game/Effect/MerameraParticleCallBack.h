#pragma once

#include "Game/Effect/SpinPullParticleCallBack.h"

class MerameraParticleCallBack : public SpinPullParticleCallBack {
public:
    virtual void execute(JPABaseEmitter*, JPABaseParticle*);
};