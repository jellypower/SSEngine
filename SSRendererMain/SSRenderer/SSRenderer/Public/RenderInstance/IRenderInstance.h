#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

class GALRIMetadata;
enum class ERenderInstanceType : int32;
class ModelAsset;



/**
* RenderWorld에 포함되는 실제 렌더링 될 객체
* 실제 객체 타입(StaticMesh, SkinnedMesh, ParticleMesh ...)에 따라서 다른 RenderInstance 타입과 메타데이터를 가지도록 만드는 것이 목표이다.
*/
class IRenderInstance : public INoncopyable
{
public:
	virtual SObjHashCode GetGameObjectID() const = 0;
	virtual ERenderInstanceType GetRIType() const = 0;
	virtual const GALRIMetadata* GetGALMetadata() const = 0;
	virtual void ReleaseGALMetaData() = 0;
	virtual const XMMATRIX& GetWorldTransformMatrix() const	= 0;
	virtual const XMMATRIX& GetWorldRotationMatrix() const = 0;

	virtual void InjectGALMetadataXXX(GALRIMetadata* MetadataToHandover) = 0;
};

