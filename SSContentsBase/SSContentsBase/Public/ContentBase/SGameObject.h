#pragma once
#include "SComponentBase.h"
#include "SObject/Public/SObjectBase.h"
#include "SObject/Public/SObjHashT.h"
#include "SObject/Public/ModuleEntry/SObjectFactory.h"

#include "SSContentsBase/ModuleExportKeyword.h"

class SWorld;
class SComponentBase;

// TODO: StrongBind라는 개념 만들기 -> SkinnedMesh나 스크립트를 통해 만들어진 게임오브젝트들은 라이프사이클을 무조건 통일 할 수 있게

class SSCONTENTBASE_MODULE SGameObject : public SObjectBase
{
private:
	Transform _transform;
	XMMATRIX _CommittedWorldTransformMat;
	Quaternion _CommittedWorldRotation;

	SS::PooledList<SGameObject*, SS::InlineAllocator<8>> _Children;
	SS::PooledList<SComponentBase*, SS::InlineAllocator<8>> _Components;

	SObjHashT<SGameObject> _StrongBindAncestor;

	SObjHashCode _IncludedWorldHash;
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
	SGameObject* FindChildOfName(SS::SHasherW Name, bool bIncludeHieararchy = false) const;
	void ScrapAllDescendants(SS::PooledList<SGameObject*>& OutDescendants) const;

	SGameObject* GetStrongBindAncestor() const { return _StrongBindAncestor.Get(); }
	bool IsStronglyBound() const { return _StrongBindAncestor.Get() != nullptr; }
	bool IsStrongBindAncestor() const { return _StrongBindAncestor.Get() == this; }


	int32 GetComponentCnt() const { return _Components.GetSize(); }
	SComponentBase* GetComponentByIdx(int32 ComponentIdx) const { return _Components[ComponentIdx]; }

	const Transform& GetTransform() const { return _transform; }
	Transform CalcWorldTransform() const;
	XMMATRIX CalcWorldTransformMatrix() const;
	Quaternion CalcWorldRot() const;

	uint64 GetTransformCommittedFrameCnt() const { return _TransformCommitedFrameCnt; }
	bool IsTransformCommitReserved() const { return _bTransformCommitReserved; }
	const XMMATRIX& GetCommittedWorldTransformMat() const { return _CommittedWorldTransformMat; }
	const Quaternion& GetCommittedWorldRotation() const { return _CommittedWorldRotation; }

public:
	void SetTransform(const Transform& InTransform);
	void SetWorldTransform(const Transform& InWorldTransform);
	void SetPosition(const Vector4f& InPosition);
	void SetRotation(const Quaternion& InRotation);
	void SetScale(const Vector4f& InScale);

	void MarkTransformCommitNeeded();
	void CommitTransform(const XMMATRIX& ParentWorldTransformMat, const Quaternion& ParentRotation);

	void SetParent(SGameObject* InNewParent);
	void MarkHierarchyInitialized() { _bIsHierarchyInitialized = true; }

	void SetStrongBindAncestor(SGameObject* InAncestor);

	void OnEnterTheWorld(SObjHashCode WorldHashCode);
	void OnExitTheWorld();

private:
	static void ScrapAllDescendant_Recursion(SS::PooledList<SGameObject*>& OutDescendants, const SGameObject* ParentToScrap);

public:
	void AddComponent(SComponentBase* InComponent);


public:
	template<typename T>
	T* CreateComponent(SS::SHasherW ComponentName)
	{
		static_assert(std::derived_from<T, SComponentBase>);
		T* NewComponent = NewSObject<T>(ComponentName);
		AddComponent(NewComponent);
		return NewComponent;
	}

	template<typename T>
	T* FindComponent() const
	{
		static_assert(std::derived_from<T, SComponentBase>);
		for (SComponentBase* CompItem : _Components)
		{
			T* CastedComp = dynamic_cast<T*>(CompItem);

			if (CastedComp != nullptr)
			{
				return CastedComp;
			}
		}

		return nullptr;
	}

};
