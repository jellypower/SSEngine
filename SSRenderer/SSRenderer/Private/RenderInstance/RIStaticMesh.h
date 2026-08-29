#pragma once
#include "SSGAL/Public/SSGALCommonEnums.h"

#include "SObject/Public/SObjHashCode.h"

#include "SSRenderer/Public/RenderInstance/IRIMesh.h"


class IMaterialAsset;
class IMeshAsset;

class RIStaticMesh : public IRIMesh
{
public:
	void Release() override;

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


public:
	virtual IMeshAsset* GetMeshAsset() const override;
	virtual IMaterialAsset* GetMaterialAsset(int MtlIdx) const override;

	virtual void SetMeshAsset(IMeshAsset* InAsset) override;
	virtual void SetMaterialAsset(IMaterialAsset* InAsset, int32 MtlIdx) override;


private:
	IMeshAsset* _MeshRef = nullptr;
	SS::PooledList<IMaterialAsset*, SS::InlineAllocator<8>> _MtlRef;

private:
	XMMATRIX _WorldTransformMatrix;
	XMMATRIX _WorldRotationMatrix;
	SObjHashCode _GameObjectHashCode = nullptr;
	
	GALRIMetadata* _MetaData[GAL_NESTED_FRAME_CNT] = { nullptr, };
	IRenderWorld* _IncludedRenderWorld = nullptr;

};
