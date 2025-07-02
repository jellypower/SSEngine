#pragma once
#include "SObject/Public/SObjectBase.h"
#include "SObject/Public/ModuleEntry/SObjectFactory.h"


class SWorld;
class SComponentBase;

class SGameObject : public SObjectBase
{
private:
	Transform _transform;
	XMMATRIX _CommittedWorldTransformMat;
	Quaternion _CommittedWorldRotation;

	SS::PooledList<SGameObject*, SS::InlineAllocator<8>> _Children;
	SS::PooledList<SComponentBase*, SS::InlineAllocator<8>> _Components;

	SObjHashCode _IncludedWorldHash = nullptr;
	SGameObject* _Parent = nullptr;

	uint64 _TransformCommitedFrameCnt = 0;
	bool _bIsHierarchyInitialized = false;
	bool _bTransformCommitReserved = false;


public:
	bool IsRootInWorld() const;

	SWorld* GetIncludedWorldRef() const;
	SObjHashCode GetIncludedWorldHash() const { return _IncludedWorldHash; }
	bool GetIsHierarchyInitialized() const { return _bIsHierarchyInitialized; }

	int32 GetChildCnt() const { return _Children.GetSize(); }
	SGameObject* GetChild(int32 ChildIdx) const { return _Children[ChildIdx]; }
	SGameObject* GetParent() const { return _Parent; }

	int32 GetComponentCnt() const { return _Components.GetSize(); }
	SComponentBase* GetComponentByIdx(int32 ComponentIdx) const { return _Components[ComponentIdx]; }

	const Transform& GetTransform() const { return _transform; }
	Transform GetWorldTransform() const;
	XMMATRIX GetWorldTransformMatrix() const;
	Quaternion GetWorldRot() const;

	uint64 GetTransformCommittedFrameCnt() const { return _TransformCommitedFrameCnt; }
	bool IsTransformCommitReserved() const { return _bTransformCommitReserved; }
	const XMMATRIX& GetCommittedWorldTransformMat() const { return _CommittedWorldTransformMat; }
	const Quaternion& GetCommittedWorldRotation() const { return _CommittedWorldRotation; }

public:
	void SetTransform(const Transform& InTransform);
	void SetPosition(const Vector4f& InPosition);
	void SetRotation(const Quaternion& InRotation);
	void SetScale(const Vector4f& InScale);

	void MarkTransformCommitNeeded();
	void CommitTransform(const XMMATRIX& ParentWorldTransformMat, const Quaternion& ParentRotation);

	void SetParent(SGameObject* InNewParent);
	void MarkHierarchyInitialized() { _bIsHierarchyInitialized = true; }

	void OnEnterTheWorld(SObjHashCode WorldHashCode);
	void OnExitTheWorld();

public:
	void AddComponent(SComponentBase* InComponent);
	template<typename T>
	T* CreateComponent(SS::SHasherW ComponentName)
	{
		static_assert(std::derived_from<T, SComponentBase>);
		T* NewComponent = NewSObject<T>(ComponentName);
		AddComponent(NewComponent);
		return NewComponent;
	}

};
