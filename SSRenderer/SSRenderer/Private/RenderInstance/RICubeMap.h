#pragma once
#include "SSRenderer/Public/RenderInstance/IRICubeMap.h"

class RICubeMap : public IRICubeMap
{
private:
	SObjHashCode _OwnerHashCode;
	ITextureAsset* _TextureAsset = nullptr;
	GALRIMetadata* _MetaData[GAL_NESTED_FRAME_CNT] = { nullptr, };
	IRenderWorld* _IncludedRenderWorld = nullptr;

public:
	void Release() override;


public:
	virtual float GetCubeMapSize() const override;
	virtual ITextureAsset* GetCubemapTexture() const override;
	virtual void SetCubemapTexture(ITextureAsset* InAsset) override;


public:
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

};
