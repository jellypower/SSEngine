#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

#define FBXSDK_SHARED
#include <fbxsdk.h>

class MeshRawDataSkinned;
class IMeshAsset;

class SSFBXImporterUtils
{
public:
	static int32 CalcWholeNodeCnt_Recursion(const FbxNode* node);
	static Transform ExtractTransformFromNode(FbxNode* node, FbxTime fbxTime = FBXSDK_TIME_INFINITE);
	static IMeshAsset* GenerateNewMeshAssestFromFbxMesh(FbxMesh* fbxMesh, SS::SHasherW NewAssetName, const utf16* InAssetPath);
	static IMeshAsset* GenerateNewSkinnedMeshAssestFromFbxMesh(FbxMesh* fbxMesh, SS::SHasherW NewAssetName, const utf16* InAssetPath);

private:
	static void ExtractOriginalBoneFromFbxSkin(SS::SHasherW RootBoneName, MeshRawDataSkinned* RawDataToSaveBone, FbxSkin* fbxSkin);
};
