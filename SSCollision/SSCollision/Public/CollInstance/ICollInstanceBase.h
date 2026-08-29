#pragma once
#include "SSEngineDefault/Public/Collision/AABBBox.h"

// class ISpatialAccelerationStructure;
class ICollisionWorld;

enum class ECollShapeType : int32
{
	None,

	Box,
	Sphere
};

class ICollInstanceBase : public ISSUnknown
{
public:
	virtual ECollShapeType GetCollShapeType() const = 0;

	// 기본적으로 GameObject의 World_Scale, Local_Rotation, Local_Pos이 들어감 
	virtual void SyncColliderLclTransform_ByContent(const Transform& LocalTransform) = 0;

	virtual const Vector4f& GetOffset() const = 0;
	virtual void SetOffset(const Vector4f& InOffset) = 0;

	// 오브젝트의 WorldPos를 원점으로 Dir"방향"쪽으로 가장 멀리 나가있는 점 계산
	// return: WorldPosition
	virtual Vector4f CalcFurthest(const Vector4f& Dir) const = 0;
	virtual const AABBBox& GetBBox() const = 0;


	virtual SObjHashCode GetGameObjectID() const = 0;
	virtual ICollisionWorld* GetIncludedCollWorld() const = 0;

//	virtual void OnEnterTheSAS(ISpatialAccelerationStructure* InSAS) = 0;
//	virtual void OnExitTheSAS() = 0;
//	virtual ISpatialAccelerationStructure* GetIncludedSAS() const = 0;
//	virtual void SetSASProxyIdx(int64 InSASProxyIdx) = 0;
//	virtual int64 GetSASProxyIdx() const = 0;
};
