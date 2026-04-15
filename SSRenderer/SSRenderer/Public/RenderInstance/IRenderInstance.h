#pragma once
#include "SSGAL/Public/SSGALInlineSettings.h"

#include "SSEngineDefault/Public/SSEngineDefault.h"

#include "SObject/Public/SObjHashCode.h"

class IModelAsset;
class GALRIMetadata;
class ModelAsset;
class IRenderWorld;

enum class ERenderInstanceType : int32
{
	None = -1,

	StaticMesh,
	SkinnedMesh,

	Light,

	CubeMap	
};


/**
* RenderWorld에 포함되는 실제 렌더링 될 객체
* 실제 객체 타입(StaticMesh, SkinnedMesh, ParticleMesh ...)에 따라서 다른 RenderInstance 타입과 메타데이터를 가지도록 만드는 것이 목표이다.
*/
class IRenderInstance : public INoncopyable
{
public:

	virtual SObjHashCode GetGameObjectID() const = 0;
	virtual void SetGameObjectIDXXX(SObjHashCode InHashCode) = 0;

	virtual ERenderInstanceType GetRIType() const = 0;

	virtual const XMMATRIX& GetWorldTransformMatrix() const	= 0;
	virtual const XMMATRIX& GetWorldRotationMatrix() const = 0;
	virtual void SetWorldTransformMatrix(const XMMATRIX& InMatrix) = 0;
	virtual void SetWorldRotation(const Quaternion& InRotation) = 0;

	virtual void InjectGALMetadataXXX(GALRIMetadata* MetadataToHandover, int32 FrameMod) = 0;
	virtual GALRIMetadata* GetGALMetadata(int32 FrameMod) const = 0;
	virtual void ReleaseGALMetaData() = 0;

	virtual void OnEnterTheRenderWorldXXX(IRenderWorld* InRenderWorld) = 0;
	virtual void OnExitFromRenderWorldXXX() = 0;

	virtual IRenderWorld* GetIncludedRenderWorld() const = 0;
};

