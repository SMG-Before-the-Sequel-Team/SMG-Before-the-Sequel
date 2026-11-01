#pragma once

#include "JSystem/JGeometry.h"
#include "Game/Util/IKJoint.h"

namespace IKJoint_SMG1 {
    void setRootBoneLength(IKJoint* ikjnt, f32);
    void setMiddleBoneLength(IKJoint* ikjnt, f32);
    void setFirstPose(IKJoint* ikjnt, const TVec3f &, const TVec3f &);
    void getRootJointPosition(IKJoint* ikjnt, TVec3f *);
    void getEndJointPosition(IKJoint* ikjnt, TVec3f *);
    f32 getMaxLimitDistance(IKJoint* ikjnt);
    f32 getMinLimitDistance(IKJoint* ikjnt);
    void updateByLocalRootAndWorldTarget(IKJoint* ikjnt, const TPos3f &, const TVec3f &, const TVec3f &);
    s32 updateByUpVector(IKJoint* ikjnt, const TVec3f &, const TVec3f &, const TVec3f &);
}