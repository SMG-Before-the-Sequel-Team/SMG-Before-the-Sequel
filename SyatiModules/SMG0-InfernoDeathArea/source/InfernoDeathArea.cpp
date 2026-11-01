#include "SMG0/InfernoDeathArea.h"

namespace SMG0 {

    InfernoDeathArea::InfernoDeathArea(const char* pName) : AreaObj(pName) {

    }

    void InfernoDeathArea::init(const JMapInfoIter& rIter) {
        AreaObj::init(rIter);
        MR::connectToSceneAreaObj(this);
    }

    void InfernoDeathArea::movement() {

    }

    const char* InfernoDeathArea::getManagerName() const {
        return "TripodBossStepStart";
    }


    void tryKillInfernoIfInArea(register NerveExecutor* pNerveExecutor)
    {
        register LiveActor* pInferno;
        register LiveActor* pInfernoModel;
        __asm
        {
            lwz pInferno, 0x0C(pNerveExecutor)
            lwz pInfernoModel, 0x10(pNerveExecutor)
        }
        register InfernoDeathArea* CurrentArea = (InfernoDeathArea*)MR::getAreaObj("TripodBossStepStart", pInferno->mTranslation);
        if (CurrentArea == NULL)
            return;

        // Initially I thought these would have different addresses across regions, but apparently that's not the case...
        register Nerve* I_DONT_KNOW; // Death nerve
        register Nerve* I_DONT_KNOW_2; // Is Following Mario nerve
        __asm
        {
            addi      I_DONT_KNOW, r13, 0xFFFFDAF4
            addi      I_DONT_KNOW_2, r13, 0xFFFFDAEC
        }
        if (pNerveExecutor->isNerve(I_DONT_KNOW_2))
        {
            pInfernoModel->appear();
            MR::invalidateClipping(pInferno);
            MR::invalidateClipping(pInfernoModel);
            pNerveExecutor->setNerve(I_DONT_KNOW);
        }
    }

    kmBranch(0x803838A0, tryKillInfernoIfInArea);
}
