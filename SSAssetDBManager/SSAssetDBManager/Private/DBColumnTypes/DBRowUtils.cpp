#include "pch.h"

#include "DBRowUtils.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/ITextureAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMaterialAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelCombinationAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IRenderAnimAsset.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataDefaultPBR.h"


#include "SSRenderer/Public/RenderAssetSerializer/RenderAssetStrUtil.h"


AssetDBRow_Mesh_v_0 AssetToDBRow_Mesh_v_0(const IMeshAsset* InAsset, SS::SHasherW NSConvertFrom, SS::SHasherW NSConvertTo)
{
	AssetDBRow_Mesh_v_0 Row;

	SS::SHasherW ConvertedName = ReplaceAssestNameNameSpace(InAsset->GetAssetName(), NSConvertFrom, NSConvertTo);

	Row.AssetName = ConvertedName;
	Row.AssetPath = InAsset->GetAssetPath();
	Row.LastUpdateTime = InAsset->GetLastUpdateTime();

	return Row;
}

AssetDBRow_Tex_v_0 AssetToDBRow_Tex_v_0(const ITextureAsset* InAsset, SS::SHasherW NSConvertFrom, SS::SHasherW NSConvertTo)
{
	AssetDBRow_Tex_v_0 Row;

	SS::SHasherW ConvertedName = ReplaceAssestNameNameSpace(InAsset->GetAssetName(), NSConvertFrom, NSConvertTo);

	Row.AssetName = ConvertedName;
	Row.AssetPath = InAsset->GetAssetPath();
	Row.LastUpdateTime = InAsset->GetLastUpdateTime();
	Row.TextureType = InAsset->GetTextureType();
	ETextureType TextureType;

	return Row;
}

AssetDBRow_Mtl_DefaultPBR_v_0 AssetToDBRow_Mtl_DefaultPBR_v_0(const IMaterialAsset* InAsset, SS::SHasherW NSConvertFrom, SS::SHasherW NSConvertTo)
{
	AssetDBRow_Mtl_DefaultPBR_v_0 Row;

	SS::SHasherW ConvertedName = ReplaceAssestNameNameSpace(InAsset->GetAssetName(), NSConvertFrom, NSConvertTo);

	Row.AssetName = ConvertedName;
	Row.AssetPath = InAsset->GetAssetPath();
	Row.LastUpdateTime = InAsset->GetLastUpdateTime();

	const MtlDataBase* MtlDataBase = InAsset->GetMtlData();
	if (MtlDataBase->_Type != EMaterialType::DefaultPBR)
	{
		SS_INTERRUPT();
		return AssetDBRow_Mtl_DefaultPBR_v_0();
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

AssetDBRow_Mdl_v_0 AssetToDBRow_Mdl_v_0(const IModelAsset* InAsset, SS::SHasherW NSConvertFrom, SS::SHasherW NSConvertTo)
{
	AssetDBRow_Mdl_v_0 Row;

	SS::SHasherW ConvertedName = ReplaceAssestNameNameSpace(InAsset->GetAssetName(), NSConvertFrom, NSConvertTo);

	Row.AssetName = ConvertedName;
	Row.AssetPath = InAsset->GetAssetPath();
	Row.LastUpdateTime = InAsset->GetLastUpdateTime();


	ConvertedName = ReplaceAssestNameNameSpace(InAsset->GetMeshAssetName(), NSConvertFrom, NSConvertTo);
	Row.MeshName = ConvertedName;


	int SubMeshCnt = InAsset->GetSubMeshCnt();


	for (int32 i = 0; i < SubMeshCnt; i++)
	{
		ConvertedName = ReplaceAssestNameNameSpace(InAsset->GetMaterialAssetName(i), NSConvertFrom, NSConvertTo);
		Row.MtlNames[i] = ConvertedName;
	}

	return Row;
}

AssetDBRow_Mdlc_v_0 AssetToDBRow_Mdlc_v_0(const IModelCombinationAsset* InAsset, SS::SHasherW NSConvertFrom, SS::SHasherW NSConvertTo)
{
	AssetDBRow_Mdlc_v_0 Row;

	SS::SHasherW ConvertedName = ReplaceAssestNameNameSpace(InAsset->GetAssetName(), NSConvertFrom, NSConvertTo);

	Row.AssetName = ConvertedName;
	Row.AssetPath = InAsset->GetAssetPath();
	Row.LastUpdateTime = InAsset->GetLastUpdateTime();

	return Row;
}

AssetDBRow_RAnim_v_0 AssetToDBRow_RAnim_v_0(const IRenderAnimAsset* InAsset, SS::SHasherW NSConvertFrom, SS::SHasherW NSConvertTo)
{
	AssetDBRow_RAnim_v_0 Row;

	SS::SHasherW ConvertedName = ReplaceAssestNameNameSpace(InAsset->GetAssetName(), NSConvertFrom, NSConvertTo);

	Row.AssetName = ConvertedName;
	Row.AssetPath = InAsset->GetAssetPath();
	Row.LastUpdateTime = InAsset->GetLastUpdateTime();

	return Row;
}

const utf16* CutOffNameFromFront(SS::SHasherW InPath, SS::SHasherW InNameSpacePath)
{
	const utf16* PathRaw = InPath.C_Str();
	const utf16* NamespacePathRaw = InNameSpacePath.C_Str();
	const int32 NamespacePathLen = InNameSpacePath.GetStrLen();
	if (wcsncmp(PathRaw, NamespacePathRaw, NamespacePathLen) != 0)
	{
		return nullptr;
	}

	return PathRaw + NamespacePathLen;
}
