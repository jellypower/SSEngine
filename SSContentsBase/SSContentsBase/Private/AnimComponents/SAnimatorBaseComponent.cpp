#include "SSContentsBase/Public/AnimComponents/SAnimatorBaseComponent.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IRenderAnimAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderKeyFrameAnimData/RenderAnimData.h"

#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSEngineDefault/Public/RawProfiler/SSFrameInfo.h"


void SAnimatorBaseComponent::SetPauseAnim(bool bIsPause)
{
	_bIsOnPause = bIsPause;
}

void SAnimatorBaseComponent::SetRenderAnimAsset(SS::SHasherW RenderAnimAssetName)
{
	_RenderAnimAssetName = RenderAnimAssetName;
	ReconstructBoneBinding();
}


void SAnimatorBaseComponent::ReconstructBoneBinding()
{
	const SGameObject* GO = GetGameObject();
	
	if (GO == nullptr || GO->GetIsHierarchyInitialized() == false)
	{
		return;
	}

	if (_RenderAnimAssetName.IsEmpty())
	{
		return;
	}

	IAssetManager* AssetManager = g_Renderer->GetAssetManager();
	IRenderAnimAsset* FoundRenderAnimAsset = AssetManager->FindAssetByName<IRenderAnimAsset>(_RenderAnimAssetName);
	if (FoundRenderAnimAsset == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	const RenderAnimRawData* AnimData = FoundRenderAnimAsset->GetKeyFrameAnimData();
	if (AnimData == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	SS::PooledList<SGameObject*> ScrapedDecendants(200);
	GO->ScrapAllDescendants(ScrapedDecendants);
	_BoneBindings.Reserve(200);

	for (const RKFTrack& TrackItem : AnimData->_Tracks)
	{
		SGameObject* MatchingObject = nullptr;

		for (SGameObject* Item : ScrapedDecendants)
		{
			if (TrackItem._TrackName.IsEmpty() == false &&
				TrackItem._TrackName == Item->GetObjectName())
			{
				MatchingObject = Item;
				break;
			}
		}

		_BoneBindings.PushBack(MatchingObject); // nullptr도 들어감
	}
	
	int32 a = 0;
}

void SAnimatorBaseComponent::PostConstructHierarchy()
{
	ReconstructBoneBinding();
}


void SAnimatorBaseComponent::UpdateAnimation()
{
	_WholeFrameTime += SSFrameInfo::GetDeltaTime();
}
