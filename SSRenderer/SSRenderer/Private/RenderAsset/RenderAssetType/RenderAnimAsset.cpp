#include "RenderAnimAsset.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderKeyFrameAnimData/RenderAnimData.h"

RenderAnimAsset::RenderAnimAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath)
{
	_assetName = InAssetName;
	_assetPath = InAssetPath;
}

RenderAnimAsset::~RenderAnimAsset()
{
	if (_AnimRawData != nullptr)
	{
		for (RKFTrack& TrackItem : _AnimRawData->_Tracks)
		{
			if (TrackItem._TrackItems != nullptr)
			{
				free(TrackItem._TrackItems);
			}
		}
		delete _AnimRawData;
		_AnimRawData = nullptr;
	}
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
	SS_ASSERT_MSG(false, L"TODO: impl");
}

void RenderAnimAsset::RemoveAssetReference(const AssetInstanceReferencer& ReferencerName)
{
	SS_ASSERT_MSG(false, L"TODO: impl");
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
