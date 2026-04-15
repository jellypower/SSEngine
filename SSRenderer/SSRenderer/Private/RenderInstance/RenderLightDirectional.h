#pragma once
#include "SSRenderer/Public/RenderInstance/Light/IRenderLightDirectional.h"
#include "SSRenderer/Public/RenderInstance/Descriptors/LightDesc.h"

class RenderLightDirectional : public IRenderLightDirectional
{
public:
	RenderLightDirectional(const RenderLightDirectionalDesc& InDesc);

public:
	// IRenderInstance
	virtual SObjHashCode GetGameObjectID() const override;
	virtual void SetGameObjectIDXXX(SObjHashCode InHashCode) override;

	virtual ERenderInstanceType GetRIType() const override;

	virtual const XMMATRIX& GetWorldTransformMatrix() const override;
	virtual const XMMATRIX& GetWorldRotationMatrix() const override;
	virtual void SetWorldTransformMatrix(const XMMATRIX& InMatrix) override;
	virtual void SetWorldRotation(const Quaternion& InRotation) override;

	virtual void InjectGALMetadataXXX(GALRIMetadata* MetadataToHandover, int32 FrameMod) override;
	virtual GALRIMetadata* GetGALMetadata(int32 FrameMod) const override;
	virtual void ReleaseGALMetaData() override;

	virtual void OnEnterTheRenderWorldXXX(IRenderWorld* InRenderWorld) override;
	virtual void OnExitFromRenderWorldXXX() override;
	virtual IRenderWorld* GetIncludedRenderWorld() const override;
	// ~IRenderInstance


	// IRenderLight
	ELightType GetLightType() const override;
	virtual bool IsShadowMapEnabled() const override;

	virtual XMVECTOR GetLightColor() const override;
	virtual void SetLightIntensity(const XMVECTOR& InLightIntensity) override;

	virtual const RenderLightDirectionalDesc& GetDirectionalLightDesc() const override;
	virtual XMVECTOR CalcDirectionalLightDirection() const override;
	virtual XMMATRIX CalcShadowMapVPMatrix(const IRenderCamera* CameraToUseShadowMap) const override;
	// ~IRenderLight


private:
	XMMATRIX _WorldRotationMatrix;
	SObjHashCode _GameObjectHashCode = nullptr;
	IRenderWorld* _IncludedRenderWorld = nullptr;
	GALRIMetadata* _ShadowMapMetaData[GAL_NESTED_FRAME_CNT] = { nullptr, };
	XMVECTOR _LightIntensity;

	RenderLightDirectionalDesc _Desc;
};
