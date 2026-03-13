#pragma once
#define SSCONTENTBASE_MODULE_EXPORT
#include "SAnimatorBaseComponent.h"

enum class E8Dir;
class AnimWorkeeBlendSpace;

class SSCONTENTBASE_MODULE SBlendSpaceAnimTestComponent : public SAnimatorBaseComponent
{
private:
	AnimWorkeeBlendSpace* _AnimWorkee = nullptr;

	bool _bDrawDebugResultPose = false;


public:
	virtual IAnimWorkee* GetAnimWorkee() const override;
	virtual void OnAnimWorkerUpdateAnimationEnded() override;

public:
	virtual void ReconstructBoneBinding() override;
	virtual void PreDestructHierarchy() override;

public:
	bool IsOnPause() const;
	SS::SHasherW GetRenderAnimAssetName(E8Dir InDir) const;
	float GetAnimDuration() const;
	double GetWholeFrameTime() const;
	Vector2f GetBlendPoint() const;

	void SetWholeFrameTime(float Time);
	void SetPauseAnim(bool bIsPause);
	void SetRenderAnimAsset(SS::SHasherW RenderAnimAssetName, E8Dir InDir);
	void SetBlendPoint(Vector2f InPoint);


	// DEBUG
public:
	bool GetDrawDebugResultPose() const { return _bDrawDebugResultPose; }

	void SetDrawDebugResultPose(bool bDraw) { _bDrawDebugResultPose = bDraw; }
	// ~DEBUG
};
