#pragma once
#include "AABBBox.h"


class CollMath_Inline
{
public:

	static AABBBox UnionAABB(const AABBBox& b1, const AABBBox& b2)
	{
		return{ XMVectorMin(b1.Min.SimdVec, b2.Min.SimdVec), XMVectorMax(b1.Max.SimdVec, b2.Max.SimdVec) };
	}

	static bool BBIntersect(const AABBBox& b1, const AABBBox& b2)
	{
		XMVECTOR Result1 = XMVectorGreaterOrEqual(b1.Min.SimdVec, b2.Max.SimdVec);
		XMVECTOR Result2 = XMVectorGreaterOrEqual(b2.Min.SimdVec, b1.Max.SimdVec);
		XMVECTOR Result = XMVectorOrInt(Result1, Result2);

		return XMVector3EqualInt(Result, XMVectorZero());
	}
};