#include "Util.h"

#include "JSystem/JGeometry/TVec.h"
#include "JSystem/JGeometry/TUtil.h"
#include "JSystem/JMATrigonometric.h"
#include "JSystem/JMath.h"

namespace MR {
    f32 calcDistanceXY(const TVec3f& rPos1, const TVec3f& rPos2) {
        f32 xDelta = rPos1.x - rPos2.x;
        f32 yDelta = rPos1.y - rPos2.y;

        return JGeometry::TUtil< f32 >::sqrt(xDelta * xDelta + yDelta * yDelta);
    }

    void rotateVecDegree(f32* x, f32* y, f32 degree) {
        f32 originalX = *x;
        f32 originalY = *y;
        f32 Sin = JMASinDegree(degree);
        f32 Cos = JMACosDegree(degree);

        *x = (Cos * originalX) - (Sin * originalY);
        *y = (Sin * originalX) + (Cos * originalY);
    }
    void rotateVecDegree(TVec2f* pDst, f32 degree) {
        rotateVecDegree(&pDst->x, &pDst->y, degree);
    }

    f32 calcRotateZ(const TVec3f& a1, const TVec3f& a2) {
        TVec2f vec(a2.y - a1.y, a2.x - a1.x);
        return 57.29578f * JMath::sAtanTable.atan2_(vec.x, vec.y);
    }
}