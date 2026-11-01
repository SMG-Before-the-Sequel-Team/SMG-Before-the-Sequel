#include "IKJoint_SMG1.h"

#include "Game/Util/MathUtil.h"
#include "Game/Util/MtxUtil.h"

namespace IKJoint_SMG1 {
    void setRootBoneLength(IKJoint* ikjnt, f32 boneLength) {
        ikjnt->mRootBoneLength = boneLength;
    }

    void setMiddleBoneLength(IKJoint* ikjnt, f32 boneLength) {
        ikjnt->mMiddleBoneLength = boneLength;
    }

    void setFirstPose(IKJoint* ikjnt, const TVec3f& a1, const TVec3f& a2) {
        MR::makeMtxSideUp(&ikjnt->_0, a1, a2);
    }

    void updateByLocalRootAndWorldTarget(IKJoint* ikjnt, const TPos3f& a1, const TVec3f& a2, const TVec3f& a3) {
        TPos3f mtx = a1;
        mtx.invert(a1);
        TVec3f stack_14;
        mtx.mult(a3, stack_14);
        ikjnt->updateByLocalRootAndDirection(a1, a2, stack_14 - a2);
    }

    s32 updateByUpVector(IKJoint* ikjnt, const TVec3f& a1, const TVec3f& a2, const TVec3f& a3) {
        f32 v9;
        TPos3f mtx;
        TVec3f sub = a2 - a1;
        MR::separateScalarAndDirection(&v9, &sub, sub);
        s32 v7 = ikjnt->updateByDistanceOnly(v9);
        MR::makeMtxSideUpPos(&mtx, sub, a3, a1);
        ikjnt->_30.concat(mtx, ikjnt->_30);
        ikjnt->_60.concat(mtx, ikjnt->_60);
        return v7;
    }

    void getRootJointPosition(IKJoint* ikjnt, TVec3f* pPos) {
        ikjnt->_30.getTrans(*pPos);
    }

    void getEndJointPosition(IKJoint* ikjnt, TVec3f* pPos) {
        TVec3f xDir;
        ikjnt->_60.getXDir(xDir);
        ikjnt->_60.getTrans(*pPos);
        TVec3f scale = (xDir * ikjnt->mMiddleBoneLength);
        PSVECAdd((const Vec*)pPos, (const Vec*)&scale, (Vec*)pPos);
    }

    f32 getMinLimitDistance(IKJoint* ikjnt) {
        return __fabsf(ikjnt->mRootBoneLength - ikjnt->mMiddleBoneLength);
    }

    f32 getMaxLimitDistance(IKJoint* ikjnt) {
        return ikjnt->mRootBoneLength + ikjnt->mMiddleBoneLength;
    }

}