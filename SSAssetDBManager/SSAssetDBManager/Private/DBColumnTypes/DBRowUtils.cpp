#include "pch.h"

#include "DBRowUtils.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/ITextureAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMaterialAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelCombinationAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IRenderAnimAsset.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataDefaultPBR.h"


AssetDBRow_Mesh_v_0 AssetToDBRow_Mesh_v_0(const IMeshAsset* InAsset)
{
	AssetDBRow_Mesh_v_0 Row;
	Row.AssetName = InAsset->GetAssetName();
	Row.AssetPath = InAsset->GetAssetPath();
	Row.LastUpdateTime = InAsset->GetLastUpdateTime();

	return Row;
}

AssetDBRow_Tex_v_0 AssetToDBRow_Tex_v_0(const ITextureAsset* InAsset)
{
	AssetDBRow_Tex_v_0 Row;
	Row.AssetName = InAsset->GetAssetName();
	Row.AssetPath = InAsset->GetAssetPath();
	Row.LastUpdateTime = InAsset->GetLastUpdateTime();
	Row.TextureType = InAsset->GetTextureType();
	ETextureType TextureType;

	return Row;
}

AssetDBRow_Mtl_DefaultPBR_v_0 AssetToDBRow_Mtl_DefaultPBR_v_0(const IMaterialAsset* InAsset)
{
	AssetDBRow_Mtl_DefaultPBR_v_0 Row;
	Row.AssetName = InAsset->GetAssetName();
	Row.AssetPath = InAsset->GetAssetPath();
	Row.LastUpdateTime = InAsset->GetLastUpdateTime();

	const MtlDataBase* MtlDataBase = InAsset->GetMtlData();
	if (MtlDataBase->_Type != EMaterialType::DefaultPBR)
	{
		SS_INTERRUPT();
	}
	const MtlDataDefaultPBR* PBRMtlData = static_cast<const MtlDataDefaultPBR*>(MtlDataBase);


	Row._BaseColorScale = PBRMtlData->_BaseColorScale;
	Row._EmissiveScale = PBRMtlData->_EmissiveScale;
	Row.NormalTexScale = PBRMtlData->_NormalTexScale;
	Row.Metallic = PBRMtlData->_Metallic;
	Row.Roughness = PBRMtlData->_Roughness;
	Row.Textures[(int32)EDefaultPBRMatTexTypes::BaseColor] = PBRMtlData->_TextureAssetNames[(int32)EDefaultPBRMatTexTypes::BaseColor];
	Row.Textures[(int32)EDefaultPBRMatTexTypes::Normal] = PBRMtlData->_TextureAssetNames[(int32)EDefaultPBRMatTexTypes::Normal];
	Row.Textures[(int32)EDefaultPBRMatTexTypes::Metallic] = PBRMtlData->_TextureAssetNames[(int32)EDefaultPBRMatTexTypes::Metallic];
	Row.Textures[(int32)EDefaultPBRMatTexTypes::Emissive] = PBRMtlData->_TextureAssetNames[(int32)EDefaultPBRMatTexTypes::Emissive];
	Row.Textures[(int32)EDefaultPBRMatTexTypes::Occlusion] = PBRMtlData->_TextureAssetNames[(int32)EDefaultPBRMatTexTypes::Occlusion];

	return Row;
}

AssetDBRow_Mdl_v_0 AssetToDBRow_Mdl_v_0(const IModelAsset* InAsset)
{
	AssetDBRow_Mdl_v_0 Row;
	Row.AssetName = InAsset->GetAssetName();
	Row.AssetPath = InAsset->GetAssetPath();
	Row.LastUpdateTime = InAsset->GetLastUpdateTime();

	Row.MeshName = InAsset->GetMeshAssetName();


	int SubMeshCnt = InAsset->GetSubMeshCnt();


	for (int32 i = 0; i < SubMeshCnt; i++)
	{
		Row.MtlNames[i] = InAsset->GetMaterialAssetName(i);
	}

	return Row;
}

AssetDBRow_Mdlc_v_0 AssetToDBRow_Mdlc_v_0(const IModelCombinationAsset* InAsset)
{
	AssetDBRow_Mdlc_v_0 Row;
	Row.AssetName = InAsset->GetAssetName();
	Row.AssetPath = InAsset->GetAssetPath();
	Row.LastUpdateTime = InAsset->GetLastUpdateTime();

	return Row;
}