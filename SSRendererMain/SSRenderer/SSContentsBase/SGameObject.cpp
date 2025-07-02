#include "SSContentsBase/SGameObject.h"

#include "SSContentsBase/SComponentBase.h"
#include "SSContentsBase/SWorld.h"
#include "SSEngineDefault/Public/RawProfiler/SSFrameInfo.h"

bool SGameObject::IsRootInWorld() const
{
	SGameObject* Parent = GetParent();
	if (Parent == nullptr)
	{
		return true; // 부모가 없는 경우에도 Root로 취급한다.
	}
	SWorld* World = GetIncludedWorldRef();

	SGameObject* WorldRoot = World->GetWorldRootObject();

	return Parent == WorldRoot;
}

SWorld* SGameObject::GetIncludedWorldRef() const
{
	SObjHashCode WorldHashCode = GetIncludedWorldHash();
	SObjectBase* WorldObj = WorldHashCode.GetSObject();
	SWorld* World = (SWorld*)WorldObj;

	return World;
}

Transform SGameObject::GetWorldTransform() const
{
	if (IsRootInWorld())
	{
		return _transform;
	}

	SGameObject* Parent = GetParent();
	Transform ParentTransform = Parent->GetWorldTransform();

	return _transform * ParentTransform;
}

XMMATRIX SGameObject::GetWorldTransformMatrix() const
{
	XMMATRIX TransformMat = _transform.AsMatrix();

	if (IsRootInWorld())
	{
		return TransformMat;
	}

	SGameObject* Parent = GetParent();
	XMMATRIX ParentWorldTransformMat = Parent->GetWorldTransformMatrix();

	return TransformMat * ParentWorldTransformMat;
}

Quaternion SGameObject::GetWorldRot() const
{
	if (IsRootInWorld())
	{
		return _transform.Rotation;
	}

	SGameObject* Parent = GetParent();

	return Parent->GetWorldRot() * _transform.Rotation;
}

void SGameObject::SetTransform(const Transform& InTransform)
{
	_transform = InTransform;
	_transform.Position.W = 1.f;
	MarkTransformCommitNeeded();
}

void SGameObject::SetPosition(const Vector4f& InPosition)
{
	_transform.Position = InPosition;
	_transform.Position.W = 1.f;
	MarkTransformCommitNeeded();
}

void SGameObject::SetRotation(const Quaternion& InRotation)
{
	_transform.Rotation = InRotation;
	MarkTransformCommitNeeded();
}

void SGameObject::SetScale(const Vector4f& InScale)
{
	_transform.Scale = InScale;
	MarkTransformCommitNeeded();
}

void SGameObject::SetParent(SGameObject* InNewParent)
{
	if (InNewParent->GetIncludedWorldHash() != GetIncludedWorldHash())
	{
		SS_ASSERT_MSG(false, L"Parent must be in the same World." );
		return;
	}

	if (_Parent != nullptr) // 이전 부모는 날린다
	{
		int32 PrevParentChildCnt = _Parent->_Children.GetSize();
		for (int32 i = 0; i < PrevParentChildCnt; i++)
		{
			SGameObject* ChildItem = _Parent->_Children[i];
			if (ChildItem == this)
			{
				_Parent->_Children.RemoveAtAndFillLast(i);
				break;
			}

			SS_ASSERT_MSG(i != (PrevParentChildCnt - 1), L"Parent does not contain this as Child.");
		}
	}

	_Parent = InNewParent;
	InNewParent->_Children.PushBack(this);
}

void SGameObject::AddComponent(SComponentBase* InComponent)
{
	if (InComponent->GetParent() != nullptr)
	{
		SS_ASSERT_MSG(false, L"Already has a Parent");
		return;
	}

	InComponent->InitComponentWithParent(this);
	_Components.PushBack(InComponent);
}


void SGameObject::OnEnterTheWorld(SObjHashCode WorldHashCode)
{
	_IncludedWorldHash = WorldHashCode;
	MarkTransformCommitNeeded();
}

void SGameObject::OnExitTheWorld()
{
	_IncludedWorldHash = nullptr;
}


void SGameObject::MarkTransformCommitNeeded()
{
	if (_bTransformCommitReserved == false)
	{
		_bTransformCommitReserved = true;
		SWorld* World = GetIncludedWorldRef();

		if (World != nullptr)
		{
			World->AddTransformCommitNeededObj(this);
		}
	}
}

void SGameObject::CommitTransform(const XMMATRIX& ParentWorldTransformMat, const Quaternion& ParentRotation)
{
	XMMATRIX ThisTransformMat = _transform.AsMatrix();
	_CommittedWorldTransformMat = ThisTransformMat * ParentWorldTransformMat;
	_CommittedWorldRotation =  ParentRotation * _transform.Rotation;
	_bTransformCommitReserved = false;
	_TransformCommitedFrameCnt = SSFrameInfo::GetFrameCnt();

	for (SComponentBase* ComponentItem : _Components)
	{
		ComponentItem->OnGameObjectTransformCommited();
	}

	for (SGameObject* ChildItem : _Children)
	{
		ChildItem->CommitTransform(_CommittedWorldTransformMat, _CommittedWorldRotation);
	}

	for (SComponentBase* ComponentItem : _Components)
	{
		ComponentItem->OnChildrenGameObjectTransformCommitted();
	}
}
