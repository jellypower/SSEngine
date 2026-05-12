#pragma once
#include "SSCollision/Public/CollInstance/ICISphere.h"

struct CI_SPHERE_DESC;
class IRigidBodyBase;

class CISphere : public ICISphere
{
private:
	SObjHashCode _GameObjectHashCode;

	float _Radius = 0.5f;
	Vector4f _Offset = Vector4f(0, 0, 0 , 1);
	Transform _ColliderTransform;

	IRigidBodyBase* _OwnerRigidBody = nullptr;

	physx::PxShape* _Shape = nullptr;


public:
	CISphere(const CI_SPHERE_DESC& Desc, physx::PxShape* InShape);
	virtual ~CISphere();

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


	// ICISphere
public:
	virtual float GetRadius() const override;
	virtual void SetRadius(float InRadius) override;


	// CISphere
public:
	physx::PxShape* GetPxShape() const { return _Shape; }

private:
	Transform CalcSphereTransform() const;
	void ApplyLocalTransformChange();
};
