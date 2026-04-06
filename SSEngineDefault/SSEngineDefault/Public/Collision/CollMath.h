#pragma once
#include "SSEngineDefault/Public/SSVector.h"


class SimplexV4;

class SSENGINEDEFAULT_MODULE CollMath
{
public:
	static Vector4f CalcFurthest(const Vector4f& Dir, const Vector4f* Points, int PointCnt);

	// 1. 들어온 Simplex가 사면체인 상태에서 원점을 포함하면 true를 리턴합니다
	// 2. 만약 사면체가 아니라면 해당 도형에서 원점으로 나아가는 새로운 NewDirection을 추가합니다.
	// 3. 원점으로 나가는 것이 
	static bool ContainsOrigin_Reconstruct(SimplexV4& Points, Vector4f& NewDirection);
	static void Reconstruct_Line(SimplexV4& Points, Vector4f& NewDirection);
	static void Reconstruct_Triangle(SimplexV4& Points, Vector4f& NewDirection);
	static bool Reconstruct_Tetrahedron(SimplexV4& Points, Vector4f& NewDirection);
};
