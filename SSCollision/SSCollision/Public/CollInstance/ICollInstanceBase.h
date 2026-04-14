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

	virtual void CollProcess_MoveObjecet(const Vector4f& MoveDelta) = 0;
	virtual void CollProcess_RotateObjecet(const Quaternion& RotDelta) = 0;

	virtual void SyncWorldTransform_ByContent(const XMMATRIX& WorldMat, const Quaternion& WorldRot) = 0;

	virtual void SetOffset(const Vector4f& InOffset) = 0;

	virtual Vector4f GetWorldPos() const = 0;
	virtual const Quaternion& GetWorldRot() const = 0;
	virtual const XMMATRIX& GetWorldTransformMat() const = 0;

	// 오브젝트의 pivot기준으로 Dir방향쪽으로 가장 멀리 나가있는 점 찾아주는 코드
	virtual Vector4f CalcFurthest(const Vector4f& Dir) const = 0;
	virtual Vector4f GetBBMin() const = 0; // Get BoundBox Min
	virtual Vector4f GetBBMax() const = 0;

public:
	virtual SObjHashCode GetGameObjectID() const = 0;
	virtual void SetGameObjectIDXXX(SObjHashCode InHashCode) = 0;



	virtual void OnEnterTheCollWorld(ICollisionWorld* InRenderWorld) = 0;
	virtual void OnExitFromCollWorld() = 0;

	virtual ICollisionWorld* GetIncludedCollWorld() const = 0;

};
