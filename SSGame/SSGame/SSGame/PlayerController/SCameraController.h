#pragma once
#include <SObject/Public/SObjHashT.h>

#include "SSContentsBase/Public/ContentBase/SComponentBase.h"

class SCameraComponent;

class SCameraController : public SComponentBase
{
public:
	virtual bool ShouldProcessPerFrameInherently() const override;
	void PerFrame(float DeltaTime) override;

	virtual void PostConstructHierarchy() override;
	virtual void OnEnterTheWorld() override;


public:
	void BindFollowTarget(SGameObject* Target);

	void SetCamTargetPitchYaw(float Pitch, float Yaw);
	void SetCamTargetFOV(float NewFOVInDeg);


private:
	void BlendCamera(float DeltaTime);

private:
	SCameraComponent* _PlayerCameraComp = nullptr;

	SObjHashT<SGameObject> _FollowTarget;
	float _CamBlendSpeed = 20.f;
	float _TargetArmLen = 5.f;
	float _ArmHeight = 1.5f;


	float _CamCurFovInDeg = 0;
	float _CamCurYaw = 0;
	float _CamCurPitch = 0;

	float _CamTargetFOVInDeg = 60;
	float _CamYawTarget = 0;
	float _CamPitchTarget = 0;
};

