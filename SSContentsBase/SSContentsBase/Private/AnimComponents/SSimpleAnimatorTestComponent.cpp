#include "SSContentsBase/Public/AnimComponents/SSimpleAnimatorTestComponent.h"

#include "SSEngineDefault/Public/RawProfiler/ScopeProfMacro.h"

#include "SSContentsBase/Public/AnimWorker/AnimBase/AnimateUtilFunctions.h"

#include "SSContentsBase/Private/AnimWorker/AnimWorkee/AnimWorkeeSimplePlayer.h"
#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/SRenderContent/_DEBUG/SRenderDebugUtil.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderKeyFrameAnimData/RenderAnimData.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IRenderAnimAsset.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"


void SSimpleAnimatorTestComponent::SetWholeFrameTime(float Time)
{
	if (_AnimWorkee == nullptr)
	{
		return;
	}

	_AnimWorkee->SetWholeFrameTime(Time);
}

void SSimpleAnimatorTestComponent::SetPauseAnim(bool bIsPause)
{
	if (_AnimWorkee == nullptr)
	{
		return;
	}

	_AnimWorkee->SetPauseAnim(bIsPause);
}

void SSimpleAnimatorTestComponent::SetRenderAnimAsset(SS::SHasherW RenderAnimAssetName)
{
	_RenderAnimAssetName = RenderAnimAssetName;

	if (_AnimWorkee != nullptr)
	{
		_AnimWorkee->SetRenderAnimAsset(RenderAnimAssetName);
	}
}


IAnimWorkee* SSimpleAnimatorTestComponent::GetAnimWorkee() const
{
	return _AnimWorkee;
}

void SSimpleAnimatorTestComponent::OnAnimWorkerUpdateAnimationEnded()
{
	IAnimWorkee* AnimWorkee = GetAnimWorkee();
	if (AnimWorkee == nullptr)
	{
		return;
	}

	if (_bDrawDebugResultPose == false)
	{
		return;
	}

	const PoseSlot& ResultPose = AnimWorkee->GetResultPose();
	SGameObject* OwnerGameObject = GetGameObject();
	XMMATRIX WorldTransformOrigin = OwnerGameObject->CalcWorldTransformMatrix();
	SWorld* IncludedWorld = OwnerGameObject->GetIncludedWorldRef();

	SRenderDebugUtil::DrawDebugPose(IncludedWorld, WorldTransformOrigin, ResultPose, false, 0.5);
}

void SSimpleAnimatorTestComponent::ReconstructBoneBinding()
{
	SCOPE_PROFILE(SSimpleAnimatorTestComponent::ReconstructBoneBinding);
	SAnimatorBaseComponent::ReconstructBoneBinding();

	if (_AnimWorkee != nullptr)
	{
		delete _AnimWorkee;
	}

	_AnimWorkee = DBG_NEW AnimWorkeeSimplePlayer(this);
}

void SSimpleAnimatorTestComponent::PreDestructHierarchy()
{
	if (_AnimWorkee != nullptr)
	{
		delete _AnimWorkee;
	}
}

bool SSimpleAnimatorTestComponent::IsOnPause() const
{
	if (_AnimWorkee == nullptr)
	{
		return true;
	}

	return _AnimWorkee->IsOnPause();
}

SS::SHasherW SSimpleAnimatorTestComponent::GetRenderAnimAssetName() const
{
	return _RenderAnimAssetName;
}

float SSimpleAnimatorTestComponent::GetAnimDuration() const
{
	if (GetRenderAnimAssetName().IsEmpty())
	{
		SS_ASSERT(false);
		return 0;
	}

	IAssetManager* AssetManager = g_Renderer->GetAssetManager();
	IRenderAnimAsset* FoundRenderAnimAsset = AssetManager->FindAssetByName<IRenderAnimAsset>(GetRenderAnimAssetName());
	if (FoundRenderAnimAsset == nullptr)
	{
		SS_ASSERT(false);
		return 0;
	}

	const RenderAnimRawData* AnimRawData = FoundRenderAnimAsset->GetKeyFrameAnimData();
	if (AnimRawData == nullptr)
	{
		SS_ASSERT(false);
		return 0;
	}

	return AnimRawData->_Header.KeyFrameDuration;
}

double SSimpleAnimatorTestComponent::GetWholeFrameTime() const
{
	if (_AnimWorkee == nullptr)
	{
		return 0;
	}

	return _AnimWorkee->GetWholeFrameTime();
}
