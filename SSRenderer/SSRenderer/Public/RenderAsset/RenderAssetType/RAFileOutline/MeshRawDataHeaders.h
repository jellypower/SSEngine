#pragma once
#include "SSRenderer/Public/RenderCommon/SSRendererInlineSettings.h"


struct alignas(16) MeshRawDataVertexHeader
{
	int32 eachVertexSize = 0;
	int32 vertexCnt = 0;

	int32 subMeshCnt = 0;
	int32 indexDataCnt[SUBMESH_COUNT_MAX] = { 0, };
	int32 indexDataStartIndex[SUBMESH_COUNT_MAX] = { 0, };
	int32 wholeIndexDataCnt = 0;
};

struct alignas(16) MeshRawDataBoneHeader
{
	int32 _BoneCnt = 0;
	int32 _EachBoneNameCapacity = 0;
};