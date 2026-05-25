#include "CommonRenderAssetSet.h"

#include "SSRenderer/Private/RendererUtil/RawDataFactory.h"
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMeshAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IModelAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IAssetBase.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMaterialAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/ITextureAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshDataDefault.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"


#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/CRAN.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderAssetCreationUtils.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataSimpleLine.h"


void CommonRenderAssetSet::InitializeCommonAssets()
{
	static const SS::SHasherW NAME_EMPTY_PBR_MTL = CRAN::EMPTY_PBR_MTL;
	static const SS::SHasherW NAME_CUBE1M_MESH = CRAN::CUBE1M_MESH;
	static const SS::SHasherW NAME_SPHERE1M_MESH = CRAN::SPHERE1M_MESH;
	static const SS::SHasherW NAME_HEMISPHERE1M_OUTLINE_MESH = CRAN::HEMISPHERE1M_OUTLINE_MESH;

	IAssetManagerMutable* AssetManager = g_Renderer->GetMutableAssetManager();

	_TexBLACK = AssetManager->FindAssetByName<ITextureAsset>(CRAN::BLACK_TEX);
	_TexEMPTY = AssetManager->FindAssetByName<ITextureAsset>(CRAN::EMPTY_TEX);
	_TexEMPTYNORMAL = AssetManager->FindAssetByName<ITextureAsset>(CRAN::EMPTYNORMAL_TEX);
	_TexWHITE = AssetManager->FindAssetByName<ITextureAsset>(CRAN::WHITE_TEX);
	_EmptyPBRMaterial = AssetManager->FindAssetByName<IMaterialAsset>(NAME_EMPTY_PBR_MTL);
	_ArrowMesh = AssetManager->FindAssetByName<IMeshAsset>(CRAN::ARROW_MESH);

	if (_ArrowMesh == nullptr) // Fallback
	{
		_ArrowMesh = AssetManager->FindAssetByName<IMeshAsset>(L"__ANS_FBX_IMPORT__/Arrow/Arrow.mesh");
	}


	static const SS::SHasherW NS_RUNTIME_CREATION_HASHER = CRAN::NS_RUNTIME_CREATION;


	_Cube1mMesh = CreateEmptyMeshAsset(NS_RUNTIME_CREATION_HASHER, NAME_CUBE1M_MESH, NS_RUNTIME_CREATION_HASHER);
	MeshRawDataDefault* CubeRawData = CreateCube1mRawData();
	static_cast<IMeshAssetMutable*>(_Cube1mMesh)->InjectRawDataXXX(CubeRawData);
	AssetManager->AddToAssetPool(_Cube1mMesh);

	_Cube1mModel = CreateEmptyModelAsset(NS_RUNTIME_CREATION_HASHER, CRAN::CUBE1M_MDL, NS_RUNTIME_CREATION_HASHER);
	static_cast<IModelAssetMutable*>(_Cube1mModel)->SetMesh(NAME_CUBE1M_MESH);
	static_cast<IModelAssetMutable*>(_Cube1mModel)->SetMaterial(NAME_EMPTY_PBR_MTL, 0);
	AssetManager->AddToAssetPool(_Cube1mModel);

	_Sphere1mMesh = CreateEmptyMeshAsset(NS_RUNTIME_CREATION_HASHER, NAME_SPHERE1M_MESH, NS_RUNTIME_CREATION_HASHER);
	MeshRawDataDefault* SphereRawData = CreateSphere1mRawData(8);
	static_cast<IMeshAssetMutable*>(_Sphere1mMesh)->InjectRawDataXXX(SphereRawData);
	AssetManager->AddToAssetPool(_Sphere1mMesh);

	_Sphere1mModel = CreateEmptyModelAsset(NS_RUNTIME_CREATION_HASHER, CRAN::SPHERE1M_MDL, NS_RUNTIME_CREATION_HASHER);
	static_cast<IModelAssetMutable*>(_Sphere1mModel)->SetMesh(NAME_SPHERE1M_MESH);
	static_cast<IModelAssetMutable*>(_Sphere1mModel)->SetMaterial(NAME_EMPTY_PBR_MTL, 0);
	AssetManager->AddToAssetPool(_Sphere1mModel);


	_HemiSphereOutline1mMesh = CreateEmptyMeshAsset(NS_RUNTIME_CREATION_HASHER, NAME_HEMISPHERE1M_OUTLINE_MESH, NS_RUNTIME_CREATION_HASHER);
	MeshRawDataSimpleLine* HemiSphereRawData = CreateHemiSphereOutline1m(8);
	static_cast<IMeshAssetMutable*>(_HemiSphereOutline1mMesh)->InjectRawDataXXX(HemiSphereRawData);
	AssetManager->AddToAssetPool(_HemiSphereOutline1mMesh);

	AssetInstanceReferencer Referencer;
	Referencer.Type = EAssetInstanceReferenceType::AssetName;
	Referencer.AssetName = CRAN::AR_COMMON;
	_TexEMPTY->AddAssetReference(Referencer);
	_TexWHITE->AddAssetReference(Referencer);
	_TexBLACK->AddAssetReference(Referencer);
	_TexEMPTYNORMAL->AddAssetReference(Referencer);
	_EmptyPBRMaterial->AddAssetReference(Referencer);
	_ArrowMesh->AddAssetReference(Referencer);

	_Cube1mModel->AddAssetReference(Referencer);
	_Sphere1mModel->AddAssetReference(Referencer);

	_HemiSphereOutline1mMesh->AddAssetReference(Referencer);
}

void CommonRenderAssetSet::ReleaseCachedAssets()
{
	AssetInstanceReferencer Referencer;
	Referencer.Type = EAssetInstanceReferenceType::AssetName;
	Referencer.AssetName = CRAN::AR_COMMON;

	_TexEMPTY->RemoveAssetReference(Referencer);
	_TexWHITE->RemoveAssetReference(Referencer);
	_TexBLACK->RemoveAssetReference(Referencer);
	_TexEMPTYNORMAL->RemoveAssetReference(Referencer);
	_EmptyPBRMaterial->RemoveAssetReference(Referencer);

	_Cube1mModel->RemoveAssetReference(Referencer);
	_Sphere1mModel->RemoveAssetReference(Referencer);
	_ArrowMesh->RemoveAssetReference(Referencer);

	_HemiSphereOutline1mMesh->RemoveAssetReference(Referencer);
}
