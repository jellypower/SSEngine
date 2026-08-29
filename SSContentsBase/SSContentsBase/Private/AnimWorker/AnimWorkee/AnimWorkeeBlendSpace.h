#pragma once
#include "SSContentsBase/Public/AnimWorker/AnimBase/AnimPoseTypes.h"
#include "SSContentsBase/Public/AnimWorker/AnimWorkee/IAnimWorkee.h"

#include "SSEngineDefault/Public/CommonTypes/DirEnums.h"


class SBlendSpaceAnimTestComponent;
class SSimpleAnimatorTestComponent;

// TODO: 나중에 실제 BlendSpace로 작동 가능하도록 구현 바꾸기
class AnimWorkeeBlendSpace : public IAnimWorkee
{
private:
	SS::SHasherW _RenderAnimAssetNames[(int32)E8Dir::Count];
	SS::SHasherW _RootToIgnore;


	int _LastUpdateFrame = 0;
	double _WholeFrameTime = 0;
	bool _bIsOneTimeUpdateRequested = false;
	bool _bIsAnimPause = true;

	PoseSlot _ResultPose;
	PoseSlot _BlendPose0;
	PoseSlot _BlendPose1;
	PoseSlot _BlendPose2;

	Vector2f _BlendPoint; // X[0, 1], Y[0, 1]

	SS::HashMap<SS::SHasherW, int32> _BindingIdxByName;


public:
	AnimWorkeeBlendSpace(const SBlendSpaceAnimTestComponent* AnimComp);
	void Release() override;

public:

	const PoseSlot& GetPose0() const { return _BlendPose0; }
	const PoseSlot& GetPose1() const { return _BlendPose1; }
	const PoseSlot& GetPose2() const { return _BlendPose2; }

	const PoseSlot& GetResultPose() const override;
	uint64 GetLastUpdateFrame() const override;
	bool ShouldUpdateAnim() const override;
	void UpdateAnimation(float DeltaTime) override;

	SS::SHasherW GetRenderAnimAssetName(E8Dir Dir);

public:
	bool IsOnPause() const { return _bIsAnimPause; }
	double GetWholeFrameTime() const { return _WholeFrameTime; }
	Vector2f GetBlendPoint() const { return _BlendPoint; }

	void SetPauseAnim(bool bIsPause);
	void SetRootIgnoreName(SS::SHasherW InName);
	void SetRenderAnimAsset(SS::SHasherW RenderAnimAssetName, E8Dir Dir);
	void SetWholeFrameTime(float Time);


	void SetBlendPoint(Vector2f InPoint);
};
