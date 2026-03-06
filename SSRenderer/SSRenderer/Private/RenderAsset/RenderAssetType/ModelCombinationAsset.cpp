#include "ModelCombinationAsset.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

ModelCombinationAsset::ModelCombinationAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath, int32 ReservedChildCnt, time_t LastUpdateTime)
{
	_childs.Reserve(ReservedChildCnt);
	_DBNameSpace = InDBNameSpace;
	_assetName = InAssetName;
	_assetPath = InAssetPath;
	_LastUpdateTime = LastUpdateTime;
}

EAssetType ModelCombinationAsset::GetAssetType() const
{
	return ThisAssetType;
}

void ModelCombinationAsset::AddAssetReference(const AssetInstanceReferencer& Referencer)
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
		AssetInstanceReferencer ThisAssetReferencer = MakeThisAssetReferencer();
		for (const AssetPlacementReference& ChildItem: _childs)
		{
			IModelAsset* ModelAssetItem = _BoundAssetManager->FindAssetByName<IModelAsset>(ChildItem.AssetName);
			ModelAssetItem->AddAssetReference(ThisAssetReferencer);
		}
	}

	_AssetInstanceReferencers.PushBack(Referencer);
}

void ModelCombinationAsset::RemoveAssetReference(const AssetInstanceReferencer& ReferencerName)
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
		SS_ASSERT_MSG(false, L"Reference do not exist.");
		return;
	}

	int32 ReferencerCnt = _AssetInstanceReferencers.GetSize();
	if (ReferencerCnt == 0)
	{
		AssetInstanceReferencer ThisAssetReferencer = MakeThisAssetReferencer();

		for (const AssetPlacementReference& ChildItem : _childs)
		{
			IModelAsset* ModelAssetItem = _BoundAssetManager->FindAssetByName<IModelAsset>(ChildItem.AssetName);
			ModelAssetItem->RemoveAssetReference(ThisAssetReferencer);
		}
	}
}

void ModelCombinationAsset::BindAssetManager(IAssetManager* InAssetManager)
{
	_BoundAssetManager = InAssetManager;
}

void ModelCombinationAsset::ReserveChilds(int32 Capacity)
{
	_childs.Reserve(Capacity);
}

void ModelCombinationAsset::ClearChilds()
{
	_childs.Clear();
	time(&_LastUpdateTime);
}

void ModelCombinationAsset::AddNewChild(const AssetPlacementReference& newReference)
{
	_childs.PushBack(newReference);
	time(&_LastUpdateTime);
}
