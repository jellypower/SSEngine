#include "CommonRenderAssetSet.h"

#include "SSRenderer/Private/RendererUtil/RawDataFactory.h"
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMaterialAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMeshAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IModelAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/ITextureAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IAssetBase.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMaterialAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/ITextureAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshDataDefault.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataDefaultPBR.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"


void CommonRenderAssetSet::InitializeCommonAssets()
{
	IAssetManagerMutable* AssetManager = g_Renderer->GetMutableAssetManager();


	_TexBLACK = AssetManager->CreateEmptyTextureAsset(L"BLACK.tex", L"Resource/Texture/BLACK.dds");
	AssetManager->AddToAssetPool(_TexBLACK);
	_TexEMPTY = AssetManager->CreateEmptyTextureAsset(L"EMPTY.tex", L"Resource/Texture/EMPTY.dds");
	AssetManager->AddToAssetPool(_TexEMPTY);
	_TexEMPTYNORMAL = AssetManager->CreateEmptyTextureAsset(L"EMPTYNORMAL.tex", L"Resource/Texture/EMPTYNORMAL.dds");
	AssetManager->AddToAssetPool(_TexEMPTYNORMAL);
	_TexWHITE = AssetManager->CreateEmptyTextureAsset(L"WHITE.tex", L"Resource/Texture/WHITE.dds");
	AssetManager->AddToAssetPool(_TexWHITE);

	_EmptyPBRMaterial = AssetManager->CreateEmptyMaterialAsset(L"EMPTY.mtl", "__INTERNAL_ASSET__");
	MtlDataDefaultPBR* EmptyDefaultPBR = DBG_NEW MtlDataDefaultPBR();
	EmptyDefaultPBR->_Type = EMaterialType::DefaultPBR;
	EmptyDefaultPBR->_BaseColorScale = Vector4f::One;
	EmptyDefaultPBR->_EmissiveScale = Vector4f::One;
	EmptyDefaultPBR->_NormalTexScale = 1;
	EmptyDefaultPBR->_Metallic = 0.5;
	EmptyDefaultPBR->_Roughness = 0.5;
	EmptyDefaultPBR->_Textures[(int32)EDefaultPBRMatTexTypes::BaseColor] = _TexWHITE;
	EmptyDefaultPBR->_Textures[(int32)EDefaultPBRMatTexTypes::Normal] = _TexEMPTYNORMAL;
	EmptyDefaultPBR->_Textures[(int32)EDefaultPBRMatTexTypes::Metallic] = _TexWHITE;
	EmptyDefaultPBR->_Textures[(int32)EDefaultPBRMatTexTypes::Emissive] = _TexBLACK;
	EmptyDefaultPBR->_Textures[(int32)EDefaultPBRMatTexTypes::Occlusion] = _TexBLACK;
	static_cast<IMaterialAssetMutable*>(_EmptyPBRMaterial)->InjectRawDataXXX(EmptyDefaultPBR);
	AssetManager->AddToAssetPool(_EmptyPBRMaterial);

	_Cube1mMesh = AssetManager->CreateEmptyMeshAsset("Cube1m.mesh", "__INTERNAL_ASSET__");
	MeshRawDataDefault* CubeRawData = CreateCube1mRawData();
	static_cast<IMeshAssetMutable*>(_Cube1mMesh)->InjectRawDataXXX(CubeRawData);
	AssetManager->AddToAssetPool(_Cube1mMesh);


	_Cube1mModel = AssetManager->CreateEmptyModelAsset("Cube1m.mdl", "__INTERNAL_ASSET__");
	static_cast<IModelAssetMutable*>(_Cube1mModel)->SetMesh(_Cube1mMesh);
	static_cast<IModelAssetMutable*>(_Cube1mModel)->SetMaterial(_EmptyPBRMaterial, 0);
	AssetManager->AddToAssetPool(_Cube1mModel);

	// IMeshAssetMutable* Sphere1mMesh = AssetManager->CreateEmptyMeshAsset("Sphere1m.mesh", "__INTERNAL_ASSET__");



	AssetInstanceReferencer Referencer;
	Referencer.Type = EAssetInstanceReferenceType::AssetName;
	Referencer.AssetName = L"__COMMON_ASSET_REFERENCER__";
	_TexEMPTY->AddAssetReference(Referencer);
	_TexWHITE->AddAssetReference(Referencer);
	_TexBLACK->AddAssetReference(Referencer);
	_TexEMPTYNORMAL->AddAssetReference(Referencer);
	_EmptyPBRMaterial->AddAssetReference(Referencer);

	_Cube1mModel->AddAssetReference(Referencer);
}

void CommonRenderAssetSet::ReleaseCachedAssets()
{
	AssetInstanceReferencer Referencer;
	Referencer.Type = EAssetInstanceReferenceType::AssetName;
	Referencer.AssetName = L"__COMMON_ASSET_REFERENCER__";

	_TexEMPTY->RemoveAssetReference(Referencer);
	_TexWHITE->RemoveAssetReference(Referencer);
	_TexBLACK->RemoveAssetReference(Referencer);
	_TexEMPTYNORMAL->RemoveAssetReference(Referencer);
	_EmptyPBRMaterial->RemoveAssetReference(Referencer);

	_Cube1mModel->RemoveAssetReference(Referencer);
}
