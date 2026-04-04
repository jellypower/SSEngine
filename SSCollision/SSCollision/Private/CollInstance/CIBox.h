#pragma once
#include "SSCollision/Public/CollInstance/ICIBox.h"

class CIBox : public ICIBox
{
private:
	// 무게중심 기준으로 XYZ좌표의 길이(XYZ = {0.5, 0.5, 0.5} 여야 1*1*1짜리 박스임)
	Vector4f _Extent;
	// 아랫면면 왼쪽 아래로 시작해서 시계방향 4개 + 아랫면 왼쪽 아래로 시작해서 시계방향 4개
	Vector4f _Vertices[8]; 

	XMMATRIX _WorldMat;
	Quaternion _WorldRot;

	SObjHashCode _GameObjectHashCode = nullptr;

	ICollisionWorld* _IncludedCollWorld = nullptr;


public:
	virtual const Vector4f& GetExtent() const override;
	virtual Vector4f GetVertexPos(int Idx) const override;
	virtual void SetExtent(const Vector4f& InExtent) override;

public:
	virtual ECollShapeType GetCollShapeType() const override;


	virtual void SetWorldTransform(const XMMATRIX& WorldMat, const Quaternion& WorldRot) override;
	virtual void CommitTransform() override;

	// 오브젝트의 WorldPos를 원점으로 Dir"방향"쪽으로 가장 멀리 나가있는 점 계산
	// return: WorldPosition
	virtual Vector4f CalcFurthest(const Vector4f& Dir) const override;

public:
	virtual SObjHashCode GetGameObjectID() const override;
	virtual void SetGameObjectIDXXX(SObjHashCode InHashCode) override;

	


	virtual void OnEnterTheCollWorld(ICollisionWorld* InCollWorld) override;
	virtual void OnExitFromCollWorld() override;
	virtual ICollisionWorld* GetIncludedCollWorld() const override;
};
