#pragma once
#include "SObject/Public/SObjHashCode.h"

class GALRIMetadata;
enum class ERenderInstanceType : int32;
class ModelAsset;
class SRenderComponentBase;



/**
* RenderWorld에 포함되는 실제 렌더링 될 객체
* 실제 객체 타입(StaticMesh, SkinnedMesh, ParticleMesh ...)에 따라서 다른 RenderInstance 타입과 메타데이터를 가지도록 만드는 것이 목표이다.
*/
class BasicRenderInstance : public INoncopyable
{
public:
	void ReleaseGALRI();
	virtual ~BasicRenderInstance();

public:
	ERenderInstanceType _Type; 
	SObjHashCode _GameObjectHashCode = nullptr;
	ModelAsset* _ModelRef = nullptr;
	GALRIMetadata* _GALRIMetadata = nullptr;
};

