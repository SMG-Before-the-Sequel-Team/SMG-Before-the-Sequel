#pragma once

#include "syati.h"

namespace SMG0 {
    class InfernoDeathArea : public AreaObj {
    public:
        InfernoDeathArea(const char* pName);

        virtual void init(const JMapInfoIter& rIter);
        virtual void movement();
        virtual const char* getManagerName() const;
    };
};
