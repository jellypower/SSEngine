#include "ModelAsset.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMaterialAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshDataDefault.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataBase.h"

ModelAsset::ModelAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath)
{
	_assetName = InAssetName;
	_assetPath = InAssetPath;
}

EAssetType ModelAsset::GetAssetType() const
{
	return ThisAssetType;
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
		_MeshAsset->AddAssetReference(ThisReferencer);

		int32 SubMeshCnt = GetSubMeshCnt();
		for (int32 i = 0; i < SubMeshCnt; i++)
		{
			IMaterialAsset* MtlItem = _MaterialAssets[i]; // TODO: MtlItem이 nullptr이 되지 않고 EmptyMaterial을 넣어주도록 수정하기
			if (MtlItem != nullptr)
			{
				MtlItem->AddAssetReference(ThisReferencer);
			}
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
		AssetInstanceReferencer ThisReferencer = MakeThisAssetReferencer();
		_MeshAsset->RemoveAssetReference(ThisReferencer);


		SS::PooledList<IMaterialAsset*, SS::InlineAllocator<SUBMESH_COUNT_MAX>> ReferencingMaterialSet;

		int32 SubMeshCnt = GetSubMeshCnt();
		for (int32 i = 0; i < SubMeshCnt; i++)
		{
			IMaterialAsset* MtlItem = _MaterialAssets[i];

			bool bIsItemAlreadyInSet = false;
			for (IMaterialAsset* ItemAlreadyInSet : ReferencingMaterialSet)
			{
				if (ItemAlreadyInSet == MtlItem)
				{
					bIsItemAlreadyInSet = true;
					break;
				}
			}

			if (bIsItemAlreadyInSet == false)
			{
				ReferencingMaterialSet.PushBack(MtlItem);
			}
		}


		for (IMaterialAsset* ReferencingMtlItem : ReferencingMaterialSet)
		{
			ReferencingMtlItem->RemoveAssetReference(ThisReferencer);
		}
	}


}

int32 ModelAsset::GetSubMeshCnt() const
{
	if (_MeshAsset == nullptr)
	{
		return 0;
	}

	return _MeshAsset->GetSubMeshCnt();
}

void ModelAsset::SetMesh(IMeshAsset* InMeshAsset)
{
	if (GetAssetInstanceReferenceCnt() > 0)
	{
		AssetInstanceReferencer ThisAssetReferencer = MakeThisAssetReferencer();

		int32 PrevSubMeshCnt = _MeshAsset->GetSubMeshCnt();
		int32 NewSubMeshCnt = InMeshAsset->GetSubMeshCnt();
		for (int32 i = NewSubMeshCnt; i < PrevSubMeshCnt; i++)
		{
			if (_MaterialAssets[i] != nullptr) // TODO: MtlItem이 nullptr이 되지 않고 EmptyMaterial을 넣어주도록 수정하기
			{
				_MaterialAssets[i]->RemoveAssetReference(ThisAssetReferencer);
				_MaterialAssets[i] = nullptr; // 서브메시의 개수가 줄어들면 줄어든 만큼 메테리얼 레퍼런스를 날려줘야 함.
			}
		}

		_MeshAsset->RemoveAssetReference(ThisAssetReferencer);
		InMeshAsset->AddAssetReference(ThisAssetReferencer);
	}

	_MeshAsset = InMeshAsset;
}

void ModelAsset::SetMaterial(IMaterialAsset* InMaterialAsset, int32 InMaterialIdx)
{
	if (InMaterialIdx >= SUBMESH_COUNT_MAX)
	{
		SS_ASSERT(false);
		return;
	}

	int32 SubMeshCnt = GetSubMeshCnt();
	if (SubMeshCnt <= InMaterialIdx)
	{
		SS_ASSERT(false);
		return;
	}

	if (GetAssetInstanceReferenceCnt() > 0)
	{
		AssetInstanceReferencer ThisAssetReferencer = MakeThisAssetReferencer();

		IMaterialAsset* PrevMaterial = _MaterialAssets[InMaterialIdx];
		_MaterialAssets[InMaterialIdx] = nullptr;

		if (PrevMaterial != nullptr)
		{
			bool bShouldPrevMtlRefRelease = true;

			for (int32 i=0;i<SubMeshCnt;i++)
			{
				if (_MaterialAssets[i] == PrevMaterial)
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

		if (InMaterialAsset != nullptr)
		{
			bool bShouldNewMtlAddRef = true;

			for (int32 i=0;i<SubMeshCnt;i++)
			{
				if (_MaterialAssets[i] == InMaterialAsset)
				{
					bShouldNewMtlAddRef = false;
					break;
				}
			}

			if (bShouldNewMtlAddRef)
			{
				InMaterialAsset->AddAssetReference(ThisAssetReferencer);
			}
		}

	}

	_MaterialAssets[InMaterialIdx] = InMaterialAsset;
}