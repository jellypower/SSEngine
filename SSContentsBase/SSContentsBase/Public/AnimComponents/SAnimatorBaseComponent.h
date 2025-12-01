#pragma once
#include "SObject/Public/SObjHashT.h"

#include "SSContentsBase/Public/ContentBase/SComponentBase.h"

class SSCONTENTBASE_MODULE SAnimatorBaseComponent : public SComponentBase
{
private:
	SS::SHasherW _RenderAnimAssetName;
	SS::PooledList<SObjHashT<SGameObject>> _BoneBindings;
	float _WholeFrameTime = 0;
	bool _bIsOnPause = false;


public:
	const SS::PooledList<SObjHashT<SGameObject>>& GetBoneBindings() const { return _BoneBindings; }
	bool IsOnPause() const { return _bIsOnPause; }
	float GetWholeFrameTime() const { return _WholeFrameTime; }
	SS::SHasherW GetRenderAnimAssetName() const { return _RenderAnimAssetName; }

	void SetPauseAnim(bool bIsPause);
	void SetRenderAnimAsset(SS::SHasherW RenderAnimAssetName);

	
	void ReconstructBoneBinding();


public:
	virtual void PostConstructHierarchy() override;
	virtual void UpdateAnimation();
};
