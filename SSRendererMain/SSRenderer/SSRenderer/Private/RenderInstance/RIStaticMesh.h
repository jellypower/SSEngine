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
	virtual ModelAsset* GetModelAsset() const override;
	virtual const XMMATRIX& GetWorldTransformMatrix() const override;
	virtual const XMMATRIX& GetWorldRotationMatrix() const override;

	void InjectGALMetadataXXX(GALRIMetadata* MetadataToHandover) override;


public:
	SObjHashCode _GameObjectHashCode = nullptr;
	ModelAsset* _ModelRef = nullptr;
	GALRIMetadata* _MetaData = nullptr;
};
