#pragma once
#include "SObject/Public/SObjHashCode.h"

#include "SSRenderer/Public/RenderInstance/IRIMesh.h"

class RIStaticMesh : public IRIMesh
{
public:
	virtual SObjHashCode GetGameObjectID() const override;
	virtual ERenderInstanceType GetRIType() const override;
	virtual const GALRIMetadata* GetGALMetadata() const	override;
	virtual void ReleaseGALMetaData() override;
	virtual IModelAsset* GetModelAsset() const override;
	virtual const XMMATRIX& GetWorldTransformMatrix() const override;
	virtual const XMMATRIX& GetWorldRotationMatrix() const override;

	virtual void InjectGALMetadataXXX(GALRIMetadata* MetadataToHandover) override;
	virtual void SetWorldTransformMatrix(const XMMATRIX& InMatrix) override;
	virtual void SetWorldRotation(const Quaternion& InRotation) override;


public:
	XMMATRIX _WorldTransformMatrix;
	XMMATRIX _WorldRotationMatrix;
	SObjHashCode _GameObjectHashCode = nullptr;
	ModelAsset* _ModelRef = nullptr;
	GALRIMetadata* _MetaData = nullptr;
};
