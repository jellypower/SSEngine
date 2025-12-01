#include "SSContentsBase/Public/AnimComponents/SSimpleAnimatorTestComponent.h"

#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderKeyFrameAnimData/RenderAnimData.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderKeyFrameAnimData/KFRenderAnimUtilFunctions.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IRenderAnimAsset.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"


void SSimpleAnimatorTestComponent::UpdateAnimation()
{
	__super::UpdateAnimation();
	UpdateNodesAnimation();
}

void SSimpleAnimatorTestComponent::UpdateNodesAnimation()
{
	if (GetRenderAnimAssetName().IsEmpty())
	{
		SS_ASSERT(false);
		return;
	}

	IAssetManager* AssetManager = g_Renderer->GetAssetManager();
	IRenderAnimAsset* FoundRenderAnimAsset = AssetManager->FindAssetByName<IRenderAnimAsset>(GetRenderAnimAssetName());
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


	float Time = GetWholeFrameTime();
	Time = fmod(Time, AnimRawData->_KeyFrameDuration);

	const SS::PooledList<SObjHashT<SGameObject>>& Bindings = GetBoneBindings();
	int32 BindingCnt = Bindings.GetSize();

	for (int32 i = 0; i < BindingCnt; i++)
	{
		SObjHashT<SGameObject> ItemHashPtr = Bindings[i];
		SGameObject* Item = ItemHashPtr.Get();
		if (Item == nullptr)
		{
			continue;
		}

		Transform Result = EvaluateRenderKFTransform(AnimRawData, i, Time);
		Item->SetTransform(Result);
	}
}
