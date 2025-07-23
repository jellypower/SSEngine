#pragma once
#include "SSRenderer/Public/RenderInstance/Light/IRenderLightDirectional.h"

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

	virtual void InjectGALMetadataXXX(GALRIMetadata* MetadataToHandover) override;
	virtual const GALRIMetadata* GetGALMetadata() const override;
	virtual void ReleaseGALMetaData() override;

	virtual void SetIncludedRenderWorldXXX(IRenderWorld* InRenderWorld) override;
	virtual IRenderWorld* GetIncludedRenderWorld() const override;

	// ~IRenderInstance
	ELightType GetLightType() const override;
	virtual bool IsShadowMapEnabled() const override;

	void SetEnableShadowMap(bool bEnable) override;

	XMMATRIX CalcShadowMapVPMatrix() const override;

	void InjectShadowMapXXX(GALRenderTarget* ShadowMapToHandover) override;
	GALRenderTarget* GetShadowMap() const override;
	void ReleaseShadowMap() override;
	// IRenderLight



	// ~IRenderLight


private:
	XMMATRIX _WorldTransformMatrix;
	XMMATRIX _WorldRotationMatrix;
	SObjHashCode _GameObjectHashCode = nullptr;
	IModelAsset* _ModelRef = nullptr;
	IRenderWorld* _IncludedRenderWorld = nullptr;

	RenderLightDirectionalDesc _Desc;
};
