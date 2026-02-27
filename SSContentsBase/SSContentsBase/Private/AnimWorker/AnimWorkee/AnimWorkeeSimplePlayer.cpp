#include "AnimWorkeeSimplePlayer.h"

#include "SSEngineDefault/Public/RawProfiler/SSFrameInfo.h"

#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/AnimComponents/SSimpleAnimatorTestComponent.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderKeyFrameAnimData/KFRenderAnimUtilFunctions.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderKeyFrameAnimData/RenderAnimData.h"


#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IRenderAnimAsset.h"

AnimWorkeeSimplePlayer::AnimWorkeeSimplePlayer(const SSimpleAnimatorTestComponent* AnimComp)
{
	const SS::PooledList<SObjHashT<SGameObject>>& Bindings = AnimComp->GetBoneBindings();
	int BindingCnt = Bindings.GetSize();

	for (int32 i = 0; i < BindingCnt; i++)
	{
		SGameObject* GOItem = Bindings[i].Get();
		SGameObject* GOParent = GOItem->GetParent();

		SObjHashCode ParentHashCode = GOParent->GetHashCode();
		SS::SHasherW GOName = GOItem->GetObjectName();
		const Transform& GOTransform = GOItem->GetTransform();

		int32 ParentIdx = INVALID_IDX;
		for (int32 j = 0; j < BindingCnt; j++)
		{
			if (Bindings[j].GetHashCode() == ParentHashCode)
			{
				ParentIdx = j;
				break;
			}
		}

		_ResultPose.PoseName = L"Result";
		_ResultPose.BoneNames.PushBack(GOName);
		_ResultPose.ParentBoneIdx.PushBack(ParentIdx);
		_ResultPose.BoneTransforms.PushBack(GOTransform);
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

	int32 BindingCnt = _ResultPose.BoneTransforms.GetSize();



	for (int32 i = 0; i < BindingCnt; i++)
	{
		SS_ASSERT(AnimRawData->_Tracks[i]._TrackName == _ResultPose.BoneNames[i]);
		// TODO: 애니메이션과 BoneBinding의 인덱스가 같지 않아도 제대로 재생되도록 하는 기능이 나중에 필요할 수 있음.
		Transform Result = EvaluateRenderKFTransform(AnimRawData, i, Time);
		_ResultPose.BoneTransforms[i] = Result;
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
