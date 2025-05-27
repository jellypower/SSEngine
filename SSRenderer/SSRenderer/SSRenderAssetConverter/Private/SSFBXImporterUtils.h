#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

#define FBXSDK_SHARED
#include <fbxsdk.h>

class MeshAsset;

class SSFBXImporterUtils
{
public:
	static int32 CalcWholeNodeCnt_Recursion(const FbxNode* node);
	static Transform ExtractTransformFromNode(FbxNode* node, FbxTime fbxTime = FBXSDK_TIME_INFINITE);
	static MeshAsset* GenerateNewMeshAssestFromFbxMesh(FbxMesh* fbxMesh, SS::SHasherW NewAssetName, const utf16* InAssetPath);
};
