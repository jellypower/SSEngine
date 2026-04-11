#pragma once
#include "SSCollision/Public/CollInstance/ICIBox.h"

class CIBox : public ICIBox
{
private:
	// 무게중심 기준으로 XYZ좌표의 길이(XYZ = {0.5, 0.5, 0.5} 여야 1*1*1짜리 박스임)
	Vector4f _Extent;
	

	Vector4f _BBMin;
	Vector4f _BBMax;

	XMMATRIX _WorldMat;
	Quaternion _WorldRot;

	SObjHashCode _GameObjectHashCode = nullptr;

	ICollisionWorld* _IncludedCollWorld = nullptr;


public:
	virtual const Vector4f& GetExtent() const override;
	virtual void SetExtent(const Vector4f& InExtent) override;

public:
	virtual ECollShapeType GetCollShapeType() const override;


	virtual void SetWorldTransform(const XMMATRIX& WorldMat, const Quaternion& WorldRot) override;
	virtual void CommitTransform() override;

	virtual Vector4f GetWorldPos() const override;
	virtual const XMMATRIX& GetWorldTransformMat() const override;
	virtual const Quaternion& GetWorldRotTransformMat() const override;


	// 오브젝트의 WorldPos를 원점으로 Dir"방향"쪽으로 가장 멀리 나가있는 점 계산
	// return: WorldPosition
	virtual Vector4f CalcFurthest(const Vector4f& Dir) const override;
	virtual Vector4f GetBBMin() const override;
	virtual Vector4f GetBBMax() const override;

public:
	virtual SObjHashCode GetGameObjectID() const override;
	virtual void SetGameObjectIDXXX(SObjHashCode InHashCode) override;

	


	virtual void OnEnterTheCollWorld(ICollisionWorld* InCollWorld) override;
	virtual void OnExitFromCollWorld() override;
	virtual ICollisionWorld* GetIncludedCollWorld() const override;
};
