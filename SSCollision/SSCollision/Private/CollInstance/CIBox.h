#pragma once
#include "SSCollision/Public/CollInstance/ICIBox.h"

class CIBox : public ICIBox
{
private:
	// 무게중심 기준으로 XYZ좌표의 길이(XYZ = {0.5, 0.5, 0.5} 여야 1*1*1짜리 박스임)
	Vector4f _Extent = Vector4f(0.5f, 0.5f, 0.5f, 0);
	Vector4f _Offset = Vector4f(0, 0, 0, 1);;

	AABBBox _BBox;


	Quaternion _WorldRot;
	XMMATRIX _WorldMat;


	SObjHashCode _GameObjectHashCode = nullptr;

	ICollisionWorld* _IncludedCollWorld = nullptr;

public:
	virtual const Vector4f& GetExtent() const override;
	virtual void SetExtent(const Vector4f& InExtent) override;

public:
	virtual ECollShapeType GetCollShapeType() const override;

	virtual void CollProcess_MoveObjecet(const Vector4f& MoveDelta) override;
	virtual void CollProcess_RotateObjecet(const Quaternion& RotDelta) override;

	virtual void SyncWorldTransform_ByContent(const XMMATRIX& WorldMat, const Quaternion& WorldRot) override;

	virtual const Vector4f& GetOffset() const override;
	virtual void SetOffset(const Vector4f& InOffset) override;

	virtual Vector4f GetWorldPos() const override;
	virtual const XMMATRIX& GetWorldTransformMat() const override;
	virtual const Quaternion& GetWorldRot() const override;


	// 오브젝트의 WorldPos를 원점으로 Dir"방향"쪽으로 가장 멀리 나가있는 점 계산
	// return: WorldPosition
	virtual Vector4f CalcFurthest(const Vector4f& Dir) const override;
	virtual const AABBBox& GetBBox() const override;

public:
	virtual void* GetInternalHandle() const override;

	virtual SObjHashCode GetGameObjectID() const override;
	virtual void SetGameObjectIDXXX(SObjHashCode InHashCode) override;

	virtual void OnEnterTheCollWorld(ICollisionWorld* InCollWorld) override;
	virtual void OnExitFromCollWorld() override;
	virtual ICollisionWorld* GetIncludedCollWorld() const override;
};
