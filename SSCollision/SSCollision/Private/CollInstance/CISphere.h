#pragma once
#include "SSCollision/Public/CollInstance/ICISphere.h"

class CISphere : public ICISphere
{
private:
	float _Radius = 0.5f;
	Vector4f _Offset = Vector4f(0, 0, 0 , 1);

	XMMATRIX _WorldMat;
	Quaternion _WorldRot;

	SObjHashCode _GameObjectHashCode = nullptr;

	ICollisionWorld* _IncludedCollWorld = nullptr;

public:
	float GetRadius() const override;
	void SetRadius(float InRadius) override;

	virtual ECollShapeType GetCollShapeType() const override;

	virtual void CollProcess_MoveObjecet(const Vector4f& MoveDelta) override;
	virtual void CollProcess_RotateObjecet(const Quaternion& RotDelta) override;

	virtual void SyncWorldTransform_ByContent(const XMMATRIX& WorldMat, const Quaternion& WorldRot) override;

	virtual void SetOffset(const Vector4f& InOffset) override;

	virtual Vector4f GetWorldPos() const override;
	const XMMATRIX& GetWorldTransformMat() const override;
	const Quaternion& GetWorldRot() const override;

	// 오브젝트의 pivot기준으로 Dir방향쪽으로 가장 멀리 나가있는 점 찾아주는 코드
	virtual Vector4f CalcFurthest(const Vector4f& Dir) const override;
	virtual Vector4f GetBBMin() const override;
	virtual Vector4f GetBBMax() const override;

public:
	virtual SObjHashCode GetGameObjectID() const override;
	virtual void SetGameObjectIDXXX(SObjHashCode InHashCode) override;

	virtual void OnEnterTheCollWorld(ICollisionWorld* InRenderWorld) override;
	virtual void OnExitFromCollWorld() override;

	virtual ICollisionWorld* GetIncludedCollWorld() const override;

};
