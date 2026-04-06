#pragma once
#include "SSEngineDefault/Public/SSVector.h"

// N차원 공간의 "영역"을 표현하는 가능한 최소한의 도형
// 1차원: 선분(2개점)
// 2차원: 삼각형(3개점)
// 3차원: 사면체(4개점)
class alignas(16) SimplexV4
{
private:
	Vector4f _Points[4];
	int32 _PointCnt;

public:
	// 생성자에 레퍼런스로 넣으면 자기 자신 대입했을 때 값이 충분히 꼬일 수 있음
	SimplexV4(Vector4f In) :
		_Points{ In, },
		_PointCnt(1)
	{
	}

	SimplexV4(Vector4f In0, Vector4f In1) :
		_Points{ In0, In1, },
		_PointCnt(2)
	{
	}

	SimplexV4(Vector4f In0, Vector4f In1, Vector4f In2) :
		_Points{ In0, In1, In2, },
		_PointCnt(3)
	{
	}

	int32 GetCnt() const
	{
		return _PointCnt;
	}

	Vector4f& operator[](int32 Idx)
	{
		if (Idx < 0 || Idx >= _PointCnt)
		{
			SS_INTERRUPT();
			return _Points[0];
		}

		return _Points[Idx];
	}

	const Vector4f& operator[](int32 Idx) const
	{
		if (Idx < 0 || Idx >= _PointCnt)
		{
			SS_INTERRUPT();
			return _Points[0];
		}

		return _Points[Idx];
	}

	void PushBack(const Vector4f& In)
	{
		if (_PointCnt >= 4)
		{
			SS_INTERRUPT();
			return;
		}

		_Points[_PointCnt] = In;
		_PointCnt++;
	}
};
