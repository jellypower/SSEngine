#pragma once
#include "SSContentsBase/Public/AnimWorker/AnimBase/AnimPoseTypes.h"
#include "SSContentsBase/Public/AnimWorker/AnimWorkee/IAnimWorkee.h"


class SSimpleAnimatorTestComponent;

class AnimWorkeeSimplePlayer : public IAnimWorkee
{
private:
	SS::SHasherW _RenderAnimAssetName;

	int _LastUpdateFrame = 0;
	double _WholeFrameTime = 0;
	bool _bIsOneTimeUpdateRequested = false;
	bool _bIsAnimPause = false;

	PoseSlot _ResultPose;

	SS::HashMap<SS::SHasherW, int32> _BindingIdxByName;

public:
	AnimWorkeeSimplePlayer(const SSimpleAnimatorTestComponent* AnimComp);

public:
	virtual const PoseSlot& GetResultPose() const override;
	virtual uint64 GetLastUpdateFrame() const override;
	virtual bool ShouldUpdateAnim() const override;
	virtual void UpdateAnimation(float DeltaTime) override;

public:
	bool IsOnPause() const { return _bIsAnimPause;  }
	double GetWholeFrameTime() const { return _WholeFrameTime; }

	void SetPauseAnim(bool bIsPause);
	void SetRenderAnimAsset(SS::SHasherW RenderAnimAssetName);
	void SetWholeFrameTime(float Time);
;
};
