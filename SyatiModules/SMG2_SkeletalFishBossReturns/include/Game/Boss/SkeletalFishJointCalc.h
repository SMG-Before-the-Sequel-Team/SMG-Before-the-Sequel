#pragma once

#include "Game/Boss/SkeletalFishRailControl.h"
#include "Game/Util/JointController.h"

class SkeletalFishJointCalc {
public:
    static void calcJoint(TPos3f*, const TPos3f*, SkeletalFishRailControl*, const JointControllerInfo&);
};

namespace MR
{
    template <class T>
    JointControlDelegator<T>* createJointDelegatorWithNullChildFunc(T* pHost, bool (T::* calcFunc)(TPos3f*, const JointControllerInfo&), const char* pName) {
        JointControlDelegator<T>* delegator = new JointControlDelegator<T>();
        setJointControllerParam(delegator, pHost, pName);
        delegator->mObjPtr = pHost;
        delegator->mCalcJointMtxFunc = calcFunc;
        delegator->mCalcJointMtxAfterChildFunc = NULL;
        return delegator;
    }
};