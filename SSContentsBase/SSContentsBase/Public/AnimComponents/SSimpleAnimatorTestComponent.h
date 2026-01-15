#pragma once
#include "SAnimatorBaseComponent.h"


class AnimWorkeeSimplePlayer;

class SSCONTENTBASE_MODULE SSimpleAnimatorTestComponent : public SAnimatorBaseComponent
{
private:
	AnimWorkeeSimplePlayer* _AnimWorkee = nullptr;
	SS::SHasherW _RenderAnimAssetName;



public:


	virtual IAnimWorkee* GetAnimWorkee() const override;
	virtual void ReconstructBoneBinding() override;
	virtual void PreDestructHierarchy() override;

public:
	bool IsOnPause() const;
	SS::SHasherW GetRenderAnimAssetName() const;
	float GetAnimDuration() const;
	double GetWholeFrameTime() const;

	void SetWholeFrameTime(float Time);
	void SetPauseAnim(bool bIsPause);
	void SetRenderAnimAsset(SS::SHasherW RenderAnimAssetName);

};

