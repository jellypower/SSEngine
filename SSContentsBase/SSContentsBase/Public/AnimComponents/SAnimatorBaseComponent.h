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

	bool _bIsOneTimeUpdateRequested = false;


public:
	const SS::PooledList<SObjHashT<SGameObject>>& GetBoneBindings() const { return _BoneBindings; }
	float GetWholeFrameTime() const { return _WholeFrameTime; }
	SS::SHasherW GetRenderAnimAssetName() const { return _RenderAnimAssetName; }
	bool IsOnPause() const { return _bIsOnPause; }

	virtual bool ShouldUpdateAnimation() const;

	void SetPauseAnim(bool bIsPause);
	void SetWholeFrameTime(float Time);
	void SetRenderAnimAsset(SS::SHasherW RenderAnimAssetName);

	
	void ReconstructBoneBinding();


public:
	virtual void OnEnterTheWorld() override;

	virtual void PostConstructHierarchy() override;
	virtual void UpdateAnimation();

	virtual void OnExitTheWorld() override;
};
