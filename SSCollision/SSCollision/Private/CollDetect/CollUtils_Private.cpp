#include "pch.h"
#include "CollUtils_Private.h"

#include "SSCollision/Public/CollisionBase/SimplexV4.h"


Vector4f CollUtils::CalcFurthest(const Vector4f& Dir, const Vector4f* Points, int PointCnt)
{
	Vector4f MaxVertex;
	float MaxDist = -FLT_MAX;

	for (int32 i = 0; i < PointCnt; i++)
	{
		const Vector4f& VertexItem = Points[i];

		float Dist = SS::Dot3D(Dir, VertexItem);

		if (Dist > MaxDist)
		{
			MaxVertex = VertexItem;
			MaxDist = Dist;
		}
	}

	return MaxVertex;
}

bool CollUtils::ContainsOrigin_Reconstruct(SimplexV4& Points, Vector4f& NewDirection)
{
	const int32 PointCnt = Points.GetCnt();

	if (PointCnt == 2)
	{
		Reconstruct_Line(Points, NewDirection);
		return false;
	}

	if (PointCnt == 3)
	{
		Reconstruct_Triangle(Points, NewDirection);
		return false;
	}

	if (PointCnt == 4)
	{
		return Reconstruct_Tetrahedron(Points, NewDirection);
	}

	SS_ASSERT(false);
	return false;
}

void CollUtils::Reconstruct_Line(SimplexV4& Points, Vector4f& NewDirection)
{
	Vector4f ab = Points[0] - Points[1];
	Vector4f ao = -Points[1];

	if (SS::Dot3D(ab, ao) > SS_EPSILON)
	{
		// SS::Cross(ab, ao) -> abo 세 점의 삼각형의 법선
		// SS::Cross(  SS::Cross(ab, ao)  , ab) -> ab에서 o점으로 내리는 수선의 발쪽으로 향하는 직선
		NewDirection = SS::Cross(SS::Cross(ab, ao), ab);

	}
	else
	{
		SS_ASSERT(false); // 일반적으로 도달할 수 없는 경로로 보임
		Points = SimplexV4(Points[1]);
		NewDirection = ao;
	}
}

void CollUtils::Reconstruct_Triangle(SimplexV4& Points, Vector4f& NewDirection)
{
	// a 점은 제일 마지막에 추가한 점이다
	// 이는 곧, a는 이전에 Line기준으로 원점으로 향하게 둔 Support Point이다.

	Vector4f ab = Points[1] - Points[2];
	Vector4f ac = Points[0] - Points[2];
	Vector4f ao = -Points[2];

	Vector4f abc = SS::Cross(ab, ac); // 삼각형의 법선벡터 -> a 기준으로 bc를 오른손으로 감았을 떄 엄지방향

	// SS::Cross(abc, ac); // ac위에서 삼각형 "밖"으로 수직하게 그린 벡터
	if (SS::Dot3D(
		SS::Cross(abc, ac),
		ao) > 0) // 만약 ac위에서 삼각형 밖으로 향하게 그린 벡터랑 ao 랑 방향이 같으면 삼각형 abc는 원점을 포함 못함
	{
		if (SS::Dot3D(ac, ao) > 0)
		{
			// "1번"
			// ac랑 ao가 방향이 같다는 건 ac 라인 사이에 o로 향하는 수선의 발을 내릴 수 있다는 것이다.
			// 그러면 다음번엔 ac 포인트를 기준으로 원점쪽으로 가는 수선의 발을 내리는 계산을 하게 함

			Points = { Points[0], Points[2] }; // Points = { c, a };

			NewDirection = SS::Cross(
				SS::Cross(ac, ao), // co 를 오른손으로 감았을 때 엄지방향
				ac // ac랑 내적하면 원점으로 향하는 수선의 발 벡터
			);
		}
		else
		{
			// "2번"
			// 여기서 ac랑 ao가 방향이 다르다는건 ac에서 원점으로 내리는 수선이 존재 할 수 없다는 것
			// 그러면 ab바깥 방향으로 수선을 그릴 수 있다 가정하고 진행
			// "3번"이랑 똑같은 동작하게 만든다 보면 됨

			// 근데 아마 여기로 도착하면 ab 기준으로도 수선의 발이 안생겨서 충돌 안할 것으로 보임
			Points = { Points[1], Points[2] }; // Points = { b, a };
			Reconstruct_Line(Points, NewDirection);
		}
	}
	else
	{
		if (SS::Dot3D(
			SS::Cross(ab, abc), // ab벡터 위에서 삼각형 밖으로 뻗는 수직벡터
			ao
		) > 0)
		{
			// "3번"
			// ab선분 위에서 삼각형 밖으로 뻗는 수직 벡터가 ao랑 방향이 같다 => 삼각형에서 ab 바깥 방향으로 원점이 있다
			// 그러면 ab를 기준으로 다시 새로운 Support 포인트를 찾아냄
			Points = { Points[1], Points[2] }; // Points = { b, a };
			Reconstruct_Line(Points, NewDirection);
		}
		else
		{
			// 여기로 들어오면 삼각형 내부에 원점이 들어올 수 있다고 보면 됨

			// bc순서로 감싼 방향의 abc가 원점으로 향했으면 좋겠음
			if (SS::Dot3D(abc, ao) > 0) // 삼각형의 법선방향이 다음 Direction
			{
				NewDirection = abc;
			}
			else 
			{
				// bc순서로 감싼 방향의 abc가 원점 반대로 향하는 경우
				// -> 삼각형 실제 도형도 b랑 c 위치를 뒤집어줘서 bc순서로 감쌌을 때 원점을 향하게 구성
				Points = { Points[1], Points[2], Points[0] };
				NewDirection = -abc;
			}
		}
	}
}

bool CollUtils::Reconstruct_Tetrahedron(SimplexV4& Points, Vector4f& NewDirection)
{
	const Vector4f& d = Points[0]; // Triangle땐 c
	const Vector4f& c = Points[1]; // Triangle땐 b
	const Vector4f& b = Points[2]; // Triangle땐 a
	const Vector4f& a = Points[3];

	Vector4f ab = b - a;
	Vector4f ac = c - a;
	Vector4f ad = d - a;
	Vector4f ao = -a;


	Vector4f abc = SS::Cross(ab, ac);
	Vector4f acd = SS::Cross(ac, ad);
	Vector4f adb = SS::Cross(ad, ab);

	if (SS::Dot3D(abc, ao) > 0) // abc 바깥쪽에 원점이 있다
	{
		Points = { c, b, a };
		Reconstruct_Triangle(Points, NewDirection);
		return false;
	}

	if (SS::Dot3D(acd, ao) > 0) // acd 바깥쪽에 원점이 있다
	{
		Points = { d, c, a };
		Reconstruct_Triangle(Points, NewDirection);
		return false;
	}
	if (SS::Dot3D(adb, ao) > 0) // adb 바깥쪽에 원점이 있다
	{
		Points = { d, b, a };
		Reconstruct_Triangle(Points, NewDirection);
		return false;
	}


	return true;
}
