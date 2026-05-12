#pragma once
#include "SSCollision/Public/CollInstance/ICIBox.h"


class IRigidBodyBase;
struct CI_BOX_DESC;

class CIBox : public ICIBox
{
private:
	SObjHashCode _GameObjectHashCode;

	// 무게중심 기준으로 XYZ좌표의 길이(XYZ = {0.5, 0.5, 0.5} 여야 1*1*1짜리 박스임)
	Vector4f _Extent = Vector4f(0.5f, 0.5f, 0.5f, 0);
	Vector4f _Offset = Vector4f(0, 0, 0, 1);;
	Transform _ColliderLclTransform;

	IRigidBodyBase* _OwnerRigidBody = nullptr;
	ICollisionWorld* _IncludedCollWorld = nullptr;

	physx::PxShape* _Shape = nullptr;

public:
	CIBox(const CI_BOX_DESC& Desc, physx::PxShape* InShape);
	virtual ~CIBox();

	// CI
public:
	virtual ECollShapeType GetCollShapeType() const override;

	virtual void SyncColliderLclTransform_ByContent(const Transform& LocalTransform) override;

	virtual const Vector4f& GetOffset() const override;
	virtual void SetOffset(const Vector4f& InOffset) override;

	virtual Vector4f CalcFurthest(const Vector4f& Dir) const override;
	virtual const AABBBox& GetBBox() const override;

	virtual SObjHashCode GetGameObjectID() const override;
	virtual ICollisionWorld* GetIncludedCollWorld() const override;


	// ICIBox
public:
	virtual const Vector4f& GetExtent() const override;
	virtual void SetExtent(const Vector4f& InExtent) override;


	// CIBox
public:
	physx::PxShape* GetPxShape() const { return _Shape; }

private:
	Transform CalcBoxTransform() const;
	void ApplyLocalTransformChange();
};
