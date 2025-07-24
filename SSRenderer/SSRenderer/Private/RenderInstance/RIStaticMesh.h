#pragma once
#include "SSGAL/Public/SSGALCommonEnums.h"

#include "SObject/Public/SObjHashCode.h"

#include "SSRenderer/Public/RenderInstance/IRIMesh.h"


class RIStaticMesh : public IRIMesh
{
public:
	virtual SObjHashCode GetGameObjectID() const override;
	virtual void SetGameObjectIDXXX(SObjHashCode InHashCode) override;

	virtual ERenderInstanceType GetRIType() const override;

	virtual const XMMATRIX& GetWorldTransformMatrix() const override;
	virtual const XMMATRIX& GetWorldRotationMatrix() const override;
	virtual void SetWorldTransformMatrix(const XMMATRIX& InMatrix) override;
	virtual void SetWorldRotation(const Quaternion& InRotation) override;

	virtual void InjectGALMetadataXXX(GALRIMetadata* MetadataToHandover) override;
	virtual GALRIMetadata* GetGALMetadata() const override;
	virtual void ReleaseGALMetaData() override;

	virtual void SetIncludedRenderWorldXXX(IRenderWorld* InRenderWorld) override;
	virtual IRenderWorld* GetIncludedRenderWorld() const override;



	virtual IModelAsset* GetModelAsset() const override;
	virtual void SetModelAsset(IModelAsset* InAsset) override;


private:
	XMMATRIX _WorldTransformMatrix;
	XMMATRIX _WorldRotationMatrix;
	SObjHashCode _GameObjectHashCode = nullptr;
	IModelAsset* _ModelRef = nullptr;
	GALRIMetadata* _MetaData = nullptr;
	IRenderWorld* _IncludedRenderWorld = nullptr;
};
