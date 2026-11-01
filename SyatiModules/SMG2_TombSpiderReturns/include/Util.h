#pragma once

#include "revolution.h"

// For some reason, this is required
// Otherwise the compiler thinks I redeclare TVec2

namespace JGeometry {
	template< typename T >
	struct TVec2;
	template< typename T >
	struct TVec3;
}

typedef JGeometry::TVec2< f32 > TVec2f;
typedef JGeometry::TVec3< f32 > TVec3f;

namespace MR {
	f32 calcDistanceXY(const TVec3f&, const TVec3f&);
	void rotateVecDegree(f32* x, f32* y, f32 degree);
	void rotateVecDegree(TVec2f* pDst, f32 degree);
	f32 calcRotateZ(const TVec3f& a1, const TVec3f& a2);
}