#pragma once
#include "SSRenderer/Public/RenderCommon/SSRendererInlineSettings.h"
#include "SSRenderer/Public/RenderCommon/SSVertexType.h"


enum class EMeshType : int32
{
	None = 0,
	Rigid = 1,
	Skinned = 2,
};


constexpr int32 EachVertexSizeOfType(EMeshType InType)
{
	switch (InType)
	{
	case EMeshType::None: return 0;
	case EMeshType::Rigid: return sizeof(SSDefaultVertex);
	case EMeshType::Skinned: return sizeof(SSSkinnedVertex);
	}
}


struct alignas(16) MeshRawDataVertexHeader
{
	EMeshType MeshType = EMeshType::None;
	int32 vertexCnt = 0;
	int32 subMeshCnt = 0;
	int32 indexDataCnt[SUBMESH_COUNT_MAX] = { 0, };
	int32 indexDataStartIndex[SUBMESH_COUNT_MAX] = { 0, };
	int32 wholeIndexDataCnt = 0;

	int64 BitMaskLSB = 0;
	int64 BitMaskMSB = 0;
	int64 Padding[4] = {0, }; // 320 byte채우기용 패딩(나중에 필요하면 공간 더 사용할 수도 있음)
};

struct alignas(16) MeshRawDataBoneHeader
{
	int32 _BoneCnt = 0;
};