#include "pch.h"
#include "CollCalc_Private.h"

#include "CollDebug_Private.h"
#include "SSCollision/Public/CollInstance/ICollInstanceBase.h"


#include "SSEngineDefault/Public/SSStaticMath.h"


bool CollCalc_Private::GJK(const ICollInstanceBase* c1, const ICollInstanceBase* c2)
{
	constexpr int GJK_ITER_MAX = 16;
	int IterCnt = GJK_ITER_MAX;


	static const Vector4f TEMP_DIR_START = { 0.4082f, 0.5307f, 0.7431f, 0};

	Vector4f Support = GJK_Support(c1, c2, TEMP_DIR_START);


	SimplexV4 Points(Support);

	Vector4f Dir = -Support;
	while (IterCnt > 0)
	{
		IterCnt--;

		Support = GJK_Support(c1, c2, Dir);

		constexpr float TEMP_GJK_EPSILON = 0.01f;
		float CosSim = SS::Dot3D(Support, Dir);
		if (CosSim <= TEMP_GJK_EPSILON)
		{
			// DEBUG
			CollDebug_Private::DrawSimplex(
				c1->GetIncludedCollWorld(),
				Points,
				Vector4f::Zero);
			// ~DEBUG

			if (Dir.Get3DSqrLength() < 0.0001)
			{
				// Dir의 길이가 0에 근사하다는 것은 민코프스키차 내부에 있는 심플렉스의 
				// 한 Line 혹은 평면이 거의 원점에 근사한다는 것을 의미함
				// 이는 곧 거의 충돌하는 형태라는 것을 의미하기 때문에 충돌에 성공했다고 넘겨줘도 됨

				// 완전 평행한 두 오브젝트가 평행한 방향으로 교차하거나 shape랑 원점이 동일한 두 오브젝트가 겹치면
				// 해당 상황이 일어날 수 있음
				return true;
			}

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