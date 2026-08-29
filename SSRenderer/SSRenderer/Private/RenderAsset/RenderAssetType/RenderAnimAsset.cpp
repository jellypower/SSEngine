#include "RenderAnimAsset.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderKeyFrameAnimData/RenderAnimData.h"

RenderAnimAsset::RenderAnimAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath, time_t LastUpdateTime)
{
	_DBNameSpace = InDBNameSpace;
	_assetName = InAssetName;
	_assetPath = InAssetPath;
	_LastUpdateTime = LastUpdateTime;
}

void RenderAnimAsset::Release()
{
	if (_AnimRawData != nullptr)
	{
		delete _AnimRawData;
		_AnimRawData = nullptr;
	}

	delete this;
}

EAssetType RenderAnimAsset::GetAssetType() const
{
	return ThisAssetType;
}

const RenderAnimRawData* RenderAnimAsset::GetKeyFrameAnimData() const
{
	return _AnimRawData;
}

const IModelCombinationAsset* RenderAnimAsset::GetOriginalMdlcAsset() const
{
	return _OriginalMdlcAsset;
}

void RenderAnimAsset::AddAssetReference(const AssetInstanceReferencer& Referencer)
{
	// TODO: 나중에 라이프사이클 추가하기

	for (int32 i = 0; i < _AssetInstanceReferencers.GetSize(); i++)
	{
		if (_AssetInstanceReferencers[i] == Referencer)
		{
			SS_ASSERT_MSG(false, L"Reference already exists.");
			return;
		}
	}

	_AssetInstanceReferencers.PushBack(Referencer);
}

void RenderAnimAsset::RemoveAssetReference(const AssetInstanceReferencer& ReferencerName)
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

	// TODO: 나중에 라이프사이클 추가하기
}

void RenderAnimAsset::BindAssetManager(IAssetManager* InAssetManager)
{
	_BoundAssetManager = InAssetManager;
}

RenderAnimRawData* RenderAnimAsset::GetMutableRawData()
{
	return _AnimRawData;
}

void RenderAnimAsset::SetOriginMdlcAsset(IModelCombinationAsset* InMdlcAsset)
{
	_OriginalMdlcAsset = InMdlcAsset;
}

void RenderAnimAsset::InjectRawDataXXX(RenderAnimRawData* InRenderAnimData)
{
	_AnimRawData = InRenderAnimData;
}
