#pragma once

#include "Game/Util/JointController.h"

namespace MR
{
    template <class T>
    JointControlDelegator<T>* createJointDelegatorWithNullMtxFunc(T* pHost, bool (T::* calcFunc)(TPos3f*, const JointControllerInfo&), const char* pName) {
        JointControlDelegator<T>* delegator = new JointControlDelegator<T>();
        setJointControllerParam(delegator, pHost, pName);
        delegator->mObjPtr = pHost;
        delegator->mCalcJointMtxFunc = NULL;
        delegator->mCalcJointMtxAfterChildFunc = calcFunc;
        return delegator;
    }
}