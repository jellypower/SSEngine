#include "pch.h"
#include "CollCalc_Private.h"

#include "CollDebug_Private.h"
#include "SSCollision/Public/CollInstance/ICollInstanceBase.h"


#include "SSEngineDefault/Public/SSStaticMath.h"


bool CollCalc_Private::GJK(const ICollInstanceBase* c1, const ICollInstanceBase* c2)
{
	constexpr int GJK_ITER_MAX = 16;
	int IterCnt = GJK_ITER_MAX;

	Vector4f Support = GJK_Support(c1, c2, Vector4f::Right);


	SimplexV4 Points(Support);
	Vector4f Dir = -Support;
	while (IterCnt > 0)
	{
		IterCnt--;

		Support = GJK_Support(c1, c2, Dir);

		constexpr float TEMP_GJK_EPSILON = 0.01f;
		if (SS::Dot3D(Support, Dir) <= TEMP_GJK_EPSILON)
		{
			// DEBUG
			CollDebug_Private::DrawSimplex(
				c1->GetIncludedCollWorld(),
				Points,
				Vector4f::Zero);
			// ~DEBUG
			return false;
		}

		Points.PushBack(Support);

		
		if (CollMath::ContainsOrigin_Reconstruct(Points, Dir))
		{
			// DEBUG
			CollDebug_Private::DrawSimplex(
				c1->GetIncludedCollWorld(),
				Points,
				Vector4f::Zero);
			// ~DEBUG
			return true;
		}
	}

	return false;
}

Vector4f CollCalc_Private::GJK_Support(const ICollInstanceBase* c1, const ICollInstanceBase* c2, Vector4f Dir)
{
	return
		c1->CalcFurthest(Dir) -
		c2->CalcFurthest(-Dir);
}