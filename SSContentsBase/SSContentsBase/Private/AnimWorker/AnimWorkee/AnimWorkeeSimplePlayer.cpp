#include "AnimWorkeeSimplePlayer.h"

#include <SSEngineDefault/Public/RawProfiler/ScopeProfMacro.h>

#include "SSEngineDefault/Public/RawProfiler/SSFrameInfo.h"

#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/AnimComponents/SSimpleAnimatorTestComponent.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderKeyFrameAnimData/KFRenderAnimUtilFunctions.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderKeyFrameAnimData/RenderAnimData.h"


#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IRenderAnimAsset.h"

AnimWorkeeSimplePlayer::AnimWorkeeSimplePlayer(const SSimpleAnimatorTestComponent* AnimComp)
	: _BindingIdxByName(400, 400)
{
	const SS::PooledList<SObjHashT<SGameObject>>& Bindings = 
		AnimComp->GetBoneBindings();

	const int32 BindingCnt = Bindings.GetSize();

	{
		SCOPE_PROFILE(TEST1);

		for (int32 i = 0; i < BindingCnt; i++)
		{
			SGameObject* GOItem = Bindings[i].Get();
			if (GOItem == nullptr)
			{
				SS_ASSERT(false);
				continue;
			}

			SS::SHasherW ObjName = GOItem->GetObjectName();
			_BindingIdxByName.Add(ObjName, i);
		}
	}

	static const SS::SHasherW HasherResult = L"Result";
	_ResultPose.PoseName = HasherResult;
	_ResultPose.BoneNames.Reserve(BindingCnt);
	_ResultPose.ParentBoneIdx.Reserve(BindingCnt);
	_ResultPose.BoneTransforms.Reserve(BindingCnt);

	{
		SCOPE_PROFILE(TEST2);

		for (int32 i = 0; i < BindingCnt; i++)
		{
			SGameObject* GOItem = Bindings[i].Get();
			SGameObject* GOParent = GOItem->GetParent();

			SS::SHasherW ParentName = GOParent->GetObjectName();
			SS::SHasherW GOName = GOItem->GetObjectName();
			const Transform& GOTransform = GOItem->GetTransform();

			int32 ParentIdx = INVALID_IDX;

			int32* FoundIdx = _BindingIdxByName.Find(ParentName);
			if (FoundIdx != nullptr)
			{
				ParentIdx = *FoundIdx;
			}

			_ResultPose.BoneNames.PushBack(GOName);
			_ResultPose.ParentBoneIdx.PushBack(ParentIdx);
			_ResultPose.BoneTransforms.PushBack(GOTransform);
		}
	}

	_RenderAnimAssetName = AnimComp->GetRenderAnimAssetName();
}

const PoseSlot& AnimWorkeeSimplePlayer::GetResultPose() const
{
	return _ResultPose;
}

uint64 AnimWorkeeSimplePlayer::GetLastUpdateFrame() const
{
	return _LastUpdateFrame;
}

bool AnimWorkeeSimplePlayer::ShouldUpdateAnim() const
{
	return _bIsAnimPause == false || _bIsOneTimeUpdateRequested;
}


void AnimWorkeeSimplePlayer::SetPauseAnim(bool bIsPause)
{
	_bIsAnimPause = bIsPause;
}

void AnimWorkeeSimplePlayer::UpdateAnimation(float DeltaTime)
{
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

	const RenderAnimRawData* AnimRawData = FoundRenderAnimAsset->GetKeyFrameAnimData();
	if (AnimRawData == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	if (_bIsOneTimeUpdateRequested)
	{
		_bIsOneTimeUpdateRequested = false;
	}
	else
	{
		_WholeFrameTime += DeltaTime;
	}


	double Time = _WholeFrameTime;
	Time = fmod(Time, AnimRawData->_Header.KeyFrameDuration);


	const int32 TrackCnt = AnimRawData->_Header.TrackCnt;



	for (int32 TrackIdx = 0; TrackIdx < TrackCnt; TrackIdx++)
	{
		int32* pBindingIdx = _BindingIdxByName.Find(AnimRawData->_Tracks[TrackIdx]._TrackName);
		if (pBindingIdx == nullptr)
		{
			// 원래는 Assert가 나긴 해야되는데 일단은 패스하자.
			continue;
		}

		int32 BindingIdx = *pBindingIdx;

		Transform Result = EvaluateRenderKFTransform(AnimRawData, TrackIdx, Time);
		_ResultPose.BoneTransforms[BindingIdx] = Result;
	}

	_LastUpdateFrame = SSFrameInfo::GetFrameCnt();
}

void AnimWorkeeSimplePlayer::SetRenderAnimAsset(SS::SHasherW RenderAnimAssetName)
{
	_RenderAnimAssetName = RenderAnimAssetName;
}

void AnimWorkeeSimplePlayer::SetWholeFrameTime(float Time)
{
	_WholeFrameTime = Time;
	_bIsOneTimeUpdateRequested = true;
}
