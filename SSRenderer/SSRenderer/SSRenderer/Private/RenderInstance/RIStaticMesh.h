#pragma once
#include "SSRenderer/Public/RenderInstance/IRIMesh.h"

class RIStaticMesh : public IRIMesh
{
public:
	virtual int64 GetGameObjectIDNative() const override;
	virtual ERenderInstanceType GetRIType() const override;
	virtual const GALRIMetadata* GetGALMetadata() const	override;
	virtual void ReleaseGALMetaData() override;
	virtual ModelAsset* GetModelAsset() const override;

	virtual const XMMATRIX& GetWorldTransformMatrix() const override;
	virtual const XMMATRIX& GetWorldRotationMatrix() const override;

public:
	SObjHashCode _GameObjectHashCode = nullptr;
	ModelAsset* _ModelRef = nullptr;
	GALRIMetadata* _MetaData = nullptr;
};
