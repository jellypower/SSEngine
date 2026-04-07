#pragma once


class ICollisionWorld;

enum class ECollShapeType : int32
{
	None,

	Box,
	Sphere
};

class ICollInstanceBase : public INoncopyable
{
public:
	virtual ECollShapeType GetCollShapeType() const = 0;

	virtual void SetWorldTransform(const XMMATRIX& WorldMat, const Quaternion& WorldRot) = 0;
	virtual void CommitTransform() = 0;

	// 오브젝트의 pivot기준으로 Dir방향쪽으로 가장 멀리 나가있는 점 찾아주는 코드
	virtual Vector4f CalcFurthest(const Vector4f& Dir) const = 0;

public:
	virtual SObjHashCode GetGameObjectID() const = 0;
	virtual void SetGameObjectIDXXX(SObjHashCode InHashCode) = 0;



	virtual void OnEnterTheCollWorld(ICollisionWorld* InRenderWorld) = 0;
	virtual void OnExitFromCollWorld() = 0;

	virtual ICollisionWorld* GetIncludedCollWorld() const = 0;

};
