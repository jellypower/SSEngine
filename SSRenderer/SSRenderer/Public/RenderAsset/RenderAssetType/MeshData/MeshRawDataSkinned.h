#pragma once
#include "MeshDataDefault.h"



class MeshRawDataSkinned : public MeshRawDataDefault
{
public:
	int32 _BoneCnt = 0;
	SS::SHasherW* _BoneNames = nullptr; // Bone들의 이름이 SSSkinnedVertex::BoneIdx 순서로 저장돼있음

	virtual ~MeshRawDataSkinned()
	{
		delete[] _BoneNames;
	}
};