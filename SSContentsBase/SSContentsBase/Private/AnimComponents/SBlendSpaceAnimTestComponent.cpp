#include "SSContentsBase/Public/AnimComponents/SBlendSpaceAnimTestComponent.h"

#include "SSContentsBase/Private/AnimWorker/AnimWorkee/AnimWorkeeBlendSpace.h"
#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/SRenderContent/_DEBUG/SRenderDebugUtil.h"

#include "SSEngineDefault/Public/RawProfiler/ScopeProfMacro.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IRenderAnimAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderKeyFrameAnimData/RenderAnimData.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

IAnimWorkee* SBlendSpaceAnimTestComponent::GetAnimWorkee() const
{
	return _AnimWorkee;
}

void SBlendSpaceAnimTestComponent::OnAnimWorkerUpdateAnimationEnded()
{
	if (_AnimWorkee == nullptr)
	{
		return;
	}

	if (_bDrawDebugResultPose == false)
	{
		return;
	}

	const PoseSlot& ResultPose = _AnimWorkee->GetResultPose();
	SGameObject* OwnerGameObject = GetGameObject();
	XMMATRIX WorldTransformOrigin = OwnerGameObject->CalcWorldTransformMatrix();
	SWorld* IncludedWorld = OwnerGameObject->GetIncludedWorldRef();

	SRenderDebugUtil::DrawDebugPose(IncludedWorld, WorldTransformOrigin, ResultPose, false, 0.5);
}

void SBlendSpaceAnimTestComponent::ReconstructBoneBinding()
{
	SCOPE_PROFILE(SBlendSpaceAnimTestComponent::ReconstructBoneBinding);
	SAnimatorBaseComponent::ReconstructBoneBinding();

	if (_AnimWorkee != nullptr)
	{
		delete _AnimWorkee;
	}

	_AnimWorkee = DBG_NEW AnimWorkeeBlendSpace(this);
	_AnimWorkee->SetRootIgnoreName(L"root");
}

void SBlendSpaceAnimTestComponent::PreDestructHierarchy()
{
	if (_AnimWorkee != nullptr)
	{
		delete _AnimWorkee;
	}
}

bool SBlendSpaceAnimTestComponent::IsOnPause() const
{
	if (_AnimWorkee == nullptr)
	{
		return true;
	}

	return _AnimWorkee->IsOnPause();
}

SS::SHasherW SBlendSpaceAnimTestComponent::GetRenderAnimAssetName(E8Dir InDir) const
{
	return _AnimWorkee->GetRenderAnimAssetName(InDir);
}

float SBlendSpaceAnimTestComponent::GetAnimDuration() const
{
	SS::SHasherW MainAnimName = GetRenderAnimAssetName(E8Dir::None);
	if (MainAnimName.IsEmpty())
	{
		return 0;
	}

	IAssetManager* AssetManager = g_Renderer->GetAssetManager();
	IRenderAnimAsset* FoundRenderAnimAsset = AssetManager->FindAssetByName<IRenderAnimAsset>(MainAnimName);
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

double SBlendSpaceAnimTestComponent::GetWholeFrameTime() const
{
	if (_AnimWorkee == nullptr)
	{
		return 0;
	}

	return _AnimWorkee->GetWholeFrameTime();
}

Vector2f SBlendSpaceAnimTestComponent::GetBlendPoint() const
{
	if (_AnimWorkee == nullptr)
	{
		return { 0, 0 };
	}

	return _AnimWorkee->GetBlendPoint();
}

void SBlendSpaceAnimTestComponent::SetWholeFrameTime(float Time)
{
	if (_AnimWorkee == nullptr)
	{
		return;
	}

	_AnimWorkee->SetWholeFrameTime(Time);
}

void SBlendSpaceAnimTestComponent::SetPauseAnim(bool bIsPause)
{
	if (_AnimWorkee == nullptr)
	{
		return;
	}

	_AnimWorkee->SetPauseAnim(bIsPause);
}

void SBlendSpaceAnimTestComponent::SetRenderAnimAsset(SS::SHasherW RenderAnimAssetName, E8Dir InDir)
{
	if (_AnimWorkee == nullptr)
	{
		return;
	}

	_AnimWorkee->SetRenderAnimAsset(RenderAnimAssetName, InDir);
}

void SBlendSpaceAnimTestComponent::SetBlendPoint(Vector2f InPoint)
{
	if (_AnimWorkee == nullptr)
	{
		return;
	}

	_AnimWorkee->SetBlendPoint(InPoint);
}
