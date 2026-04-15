#include "SSContentsBase/Public/SRenderContent/RenderComponent/SMeshRenderComponentBase.h"

#include <SSRenderer/Public/RenderAsset/CommonRenderAsset/ICommonRenderAssetSet.h>


#include "SSRenderer/Public/RenderBase/IRenderer.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/IAssetManager.h"
#include "SSRenderer/Public/RenderInstance/IRIMesh.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMaterialAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"

void SMeshRenderComponentBase::SnycMeshRIWithAssetBindingIfExists()
{
	if (_RenderInstance == nullptr)
	{
		return;
	}

	IRIMesh* RIMesh = static_cast<IRIMesh*>(_RenderInstance);
	RIMesh->SetMeshAsset(_CachedMeshAsset);

	const int32 SubMeshCnt = _CachedMeshAsset->GetSubMeshCnt();
	_CachedMtlAssets.SetSizeDirectly(SubMeshCnt);
	for (int32 i = 0; i < SubMeshCnt; i++)
	{
		RIMesh->SetMaterialAsset(_CachedMtlAssets[i], i);
	}
}

SS::SHasherW SMeshRenderComponentBase::GetMeshAssetName() const
{
	if (_CachedMeshAsset == nullptr)
	{
		return SS::SHasherW();
	}

	return _CachedMeshAsset->GetAssetName();
}

SS::SHasherW SMeshRenderComponentBase::GetMtlAssetName(int32 MtlIdx) const
{
	if (MtlIdx >= _CachedMtlAssets.GetSize())
	{
		SS_ASSERT(false);
		return SS::SHasherW();
	}

	return _CachedMtlAssets[MtlIdx]->GetAssetName();
}

void SMeshRenderComponentBase::SetModelAsset(SS::SHasherW ModelAssetName)
{
	IAssetManager* AM = g_Renderer->GetAssetManager();
	_CachedOirignalModelAsset = AM->FindAssetByName<IModelAsset>(ModelAssetName);

	_CachedMeshAsset = _CachedOirignalModelAsset->GetMeshAsset();
	int32 SubMeshCnt = _CachedMeshAsset->GetSubMeshCnt();
	_CachedMtlAssets.SetSizeDirectly(SubMeshCnt);
	for (int32 i = 0; i < SubMeshCnt; i++)
	{
		_CachedMtlAssets[i] = _CachedOirignalModelAsset->GetMaterialAsset(i);
	}

	SnycMeshRIWithAssetBindingIfExists();
}

void SMeshRenderComponentBase::SetMeshAsset(SS::SHasherW MeshAssetName)
{
	const int32 PrevSubMeshCnt = _CachedMeshAsset != nullptr ? _CachedMeshAsset->GetSubMeshCnt() : 0;

	IAssetManager* AM = g_Renderer->GetAssetManager();
	ICommonRenderAssetSet* CommonAssets = g_Renderer->GetCommonRenderAssetSet();


	_CachedMeshAsset = AM->FindAssetByName<IMeshAsset>(MeshAssetName);
	const int32 NewSubMeshCnt = _CachedMeshAsset->GetSubMeshCnt();


	_CachedMtlAssets.SetSizeDirectly(NewSubMeshCnt);
	for (int32 i = PrevSubMeshCnt; i < NewSubMeshCnt; i++)
	{
		// 만약 필요로하는 메테리얼의 개수가 많아지면 폴백 에셋으로 설정해준다.
		_CachedMtlAssets[i] = CommonAssets->GetEmptyPBRMaterial();
	}

	_CachedOirignalModelAsset = nullptr;
	SnycMeshRIWithAssetBindingIfExists();
}

void SMeshRenderComponentBase::SetMaterialAsset(SS::SHasherW MtlAssetName, int32 MtlIdx)
{
	if (_CachedMeshAsset == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	const int32 SubMeshCnt = _CachedMeshAsset->GetSubMeshCnt();
	if (SubMeshCnt <= MtlIdx)
	{
		SS_ASSERT(false);
		return;
	}

	IAssetManager* AM = g_Renderer->GetAssetManager();
	IMaterialAsset* MtlAsset = AM->FindAssetByName<IMaterialAsset>(MtlAssetName);
	_CachedMtlAssets[MtlIdx] = MtlAsset;
}


void SMeshRenderComponentBase::ApplyModelAssetChange()
{
	if (_CachedOirignalModelAsset == nullptr)
	{
		return;
	}

	_CachedMeshAsset = _CachedOirignalModelAsset->GetMeshAsset();
	int32 SubMeshCnt = _CachedMeshAsset->GetSubMeshCnt();
	_CachedMtlAssets.SetSizeDirectly(SubMeshCnt);
	for (int32 i = 0; i < SubMeshCnt; i++)
	{
		_CachedMtlAssets[i] = _CachedOirignalModelAsset->GetMaterialAsset(i);
	}

	SnycMeshRIWithAssetBindingIfExists();
}
