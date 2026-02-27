#include "ModelAsset.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/IAssetManager.h"
#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/CRAN.h"
#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/ICommonRenderAssetSet.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMaterialAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshDataDefault.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataBase.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

ModelAsset::ModelAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath)
{
	_DBNameSpace = InDBNameSpace;
	_assetName = InAssetName;
	_assetPath = InAssetPath;
}

EAssetType ModelAsset::GetAssetType() const
{
	return ThisAssetType;
}

IMeshAsset* ModelAsset::GetMeshAsset() const
{
	return _MeshAssetCache;
}

IMaterialAsset* ModelAsset::GetMaterialAsset(int32 materialIdx) const
{
	if (_SubMeshCntCache <= materialIdx || materialIdx < 0)
	{
		SS_ASSERT(false);
		return nullptr;
	}

	return _MaterialAssetCache[materialIdx];
}

SS::SHasherW ModelAsset::GetMeshAssetName() const
{
	return _MeshAssetName;
}

SS::SHasherW ModelAsset::GetMaterialAssetName(int32 materialIdx) const
{
	if (_SubMeshCntCache <= materialIdx || materialIdx < 0)
	{
		SS_ASSERT(false);
		return SS::SHasherW::GetEmpty();
	}

	return _MaterialAssetNames[materialIdx];
}

void ModelAsset::AddAssetReference(const AssetInstanceReferencer& Referencer)
{
	for (int32 i = 0; i < _AssetInstanceReferencers.GetSize(); i++)
	{
		if (_AssetInstanceReferencers[i] == Referencer)
		{
			SS_ASSERT_MSG(false, L"Reference already exists.");
			return;
		}
	}

	int32 PrevReferencerCnt = _AssetInstanceReferencers.GetSize();
	_AssetInstanceReferencers.PushBack(Referencer);

	if (PrevReferencerCnt == 0)
	{
		AssetInstanceReferencer ThisReferencer = MakeThisAssetReferencer();

		_MeshAssetCache->AddAssetReference(ThisReferencer);

		for (int32 i = 0; i < _SubMeshCntCache; i++)
		{
			_MaterialAssetCache[i]->AddAssetReference(ThisReferencer);
		}
	}

}

void ModelAsset::RemoveAssetReference(const AssetInstanceReferencer& ReferencerName)
{
	bool bReferencerEverRemoved = false;

	for (int32 i = 0; i < _AssetInstanceReferencers.GetSize(); i++)
	{
		if (_AssetInstanceReferencers[i] == ReferencerName)
		{
			_AssetInstanceReferencers.RemoveAtAndFillLast(i);
			bReferencerEverRemoved = true;
			break;
		}
	}

	if (bReferencerEverRemoved == false)
	{
		SS_ASSERT_MSG(false, L"Reference does not exist.");
		return;
	}

	int32 ReferencerCnt = _AssetInstanceReferencers.GetSize();
	if (ReferencerCnt == 0)
	{
		const AssetInstanceReferencer ThisReferencer = MakeThisAssetReferencer();
		_MeshAssetCache->RemoveAssetReference(ThisReferencer);


		SS::PooledList<IMaterialAsset*, SS::InlineAllocator<SUBMESH_COUNT_MAX>> ReferencingMaterialSet;


		for (int32 i = 0; i < _SubMeshCntCache; i++)
		{
			IMaterialAsset* MtlNameItem = _MaterialAssetCache[i];

			bool bIsItemAlreadyInSet = false;
			for (IMaterialAsset* ItemInSet : ReferencingMaterialSet)
			{
				if (ItemInSet == MtlNameItem)
				{
					bIsItemAlreadyInSet = true;
					break;
				}
			}

			if (bIsItemAlreadyInSet == false)
			{
				ReferencingMaterialSet.PushBack(MtlNameItem);
			}
		}


		for (IMaterialAsset* MtlItem : ReferencingMaterialSet)
		{
			MtlItem->RemoveAssetReference(ThisReferencer);
		}
	}


}

void ModelAsset::BindAssetManager(IAssetManager* InAssetManager)
{
	_BoundAssetManager = InAssetManager;

	IMeshAsset* FoundMeshAsset = _BoundAssetManager->FindAssetByName<IMeshAsset>(_MeshAssetName);
	if (FoundMeshAsset == nullptr)
	{
		SS_ASSERT(false);
		_SubMeshCntCache = 0;
		_MeshAssetName = SS::SHasherW();
		_MeshAssetCache = nullptr;
		return;
	}

	_MeshAssetCache = FoundMeshAsset;
	_SubMeshCntCache = FoundMeshAsset->GetSubMeshCnt();

	static SS::SHasherW EmptyMtlName = CRAN::EMPTY_PBR_MTL;
	ICommonRenderAssetSet* CommRenderAssetSet = g_Renderer->GetCommonRenderAssetSet();

	for (int32 i = 0; i < _SubMeshCntCache; i++)
	{
		IMaterialAsset* FoudnMtl = _BoundAssetManager->FindAssetByName<IMaterialAsset>(_MaterialAssetNames[i]);

		if (FoudnMtl == nullptr)
		{
			_MaterialAssetNames[i] = EmptyMtlName;
			FoudnMtl = CommRenderAssetSet->GetEmptyPBRMaterial();
			SS_ASSERT(FoudnMtl != nullptr);
		}

		_MaterialAssetCache[i] = FoudnMtl;
	}
}

int32 ModelAsset::GetSubMeshCnt() const
{
	return _SubMeshCntCache;
}

void ModelAsset::SetMesh(SS::SHasherW InMeshAssetName)
{
	if (_BoundAssetManager == nullptr)
	{
		_MeshAssetName = InMeshAssetName;
		return;
	}

	if (_MeshAssetName == InMeshAssetName)
	{
		return;
	}

	IMeshAsset* NewMeshAsset = _BoundAssetManager->FindAssetByName<IMeshAsset>(InMeshAssetName);
	if (NewMeshAsset == nullptr)
	{
		SS_ASSERT_MSG(false, L"No such asset");
		return;
	}

	IMeshAsset* PrevMeshAssetCache = _MeshAssetCache;
	const int32 PrevSubMeshCnt = _SubMeshCntCache;

	_MeshAssetCache = NewMeshAsset;
	_SubMeshCntCache = NewMeshAsset->GetSubMeshCnt();
	_MeshAssetName = InMeshAssetName;
	time(&_LastUpdateTime);

	if (GetAssetInstanceReferenceCnt() > 0)
	{
		AssetInstanceReferencer ThisAssetReferencer = MakeThisAssetReferencer();
		
		for (int32 i = _SubMeshCntCache; i < PrevSubMeshCnt; i++)
		{
			if (_MaterialAssetCache[i] != nullptr)
			{
				_MaterialAssetCache[i]->RemoveAssetReference(ThisAssetReferencer);
				_MaterialAssetCache[i] = nullptr;
			}
		}

		if (PrevMeshAssetCache != nullptr)
		{
			PrevMeshAssetCache->RemoveAssetReference(ThisAssetReferencer);
		}
		_MeshAssetCache->AddAssetReference(ThisAssetReferencer);
	}
}

void ModelAsset::SetMaterial(SS::SHasherW InMaterialAssetName, int32 InMaterialIdx)
{
	if (InMaterialIdx >= SUBMESH_COUNT_MAX)
	{
		SS_INTERRUPT(); // Out of Bounds
		return;
	}

	if (_BoundAssetManager == nullptr)
	{
		_MaterialAssetNames[InMaterialIdx] = InMaterialAssetName;
		return;
	}

	if (InMaterialIdx >= _SubMeshCntCache)
	{
		SS_INTERRUPT(); // Out of Bounds
		return;
	}

	if (_MaterialAssetNames[InMaterialIdx] == InMaterialAssetName)
	{
		return;
	}



	IMaterialAsset* NewMaterial = _BoundAssetManager->FindAssetByName<IMaterialAsset>(InMaterialAssetName);
	if (NewMaterial == nullptr)
	{
		SS_ASSERT_MSG(false, L"No Such Mtl");
		return;
	}

	IMaterialAsset* PrevMaterial = _MaterialAssetCache[InMaterialIdx];
	_MaterialAssetCache[InMaterialIdx] = nullptr;

	if (GetAssetInstanceReferenceCnt() > 0)
	{
		AssetInstanceReferencer ThisAssetReferencer = MakeThisAssetReferencer();

		if (PrevMaterial != nullptr)
		{
			bool bShouldPrevMtlRefRelease = true;

			for (int32 i = 0; i < _SubMeshCntCache; i++)
			{
				if (_MaterialAssetCache[i] == PrevMaterial)
				{
					bShouldPrevMtlRefRelease = false;
					break;
				}
			}

			if (bShouldPrevMtlRefRelease)
			{
				PrevMaterial->RemoveAssetReference(ThisAssetReferencer);
			}
		}

		if (NewMaterial != nullptr)
		{
			bool bShouldNewMtlAddRef = true;

			for (int32 i = 0; i < _SubMeshCntCache; i++)
			{
				if (_MaterialAssetCache[i] == NewMaterial)
				{
					bShouldNewMtlAddRef = false;
					break;
				}
			}

			if (bShouldNewMtlAddRef)
			{
				NewMaterial->AddAssetReference(ThisAssetReferencer);
			}
		}
	}

	_MaterialAssetCache[InMaterialIdx] = NewMaterial;
	_MaterialAssetNames[InMaterialIdx] = InMaterialAssetName;
	time(&_LastUpdateTime);
}