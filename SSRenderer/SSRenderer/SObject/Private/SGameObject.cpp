#include "SObject/Public/SGameObject.h"

#include "SObject/Public/SComponentBase.h"
#include "SObject/Public/SWorld.h"
#include "SSEngineDefault/Public/SSFrameInfo.h"

bool SGameObject::IsRootInWorld() const
{
	SWorld* World = GetIncludedWorldRef();

	SGameObject* Parent = GetParent();
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
	XMMATRIX ParentTransformMat = Parent->GetWorldTransformMatrix();

	return TransformMat * ParentTransformMat;
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
	MarkTransformUpdateNeeded();
}

void SGameObject::SetPosition(const Vector4f& InPosition)
{
	_transform.Position = InPosition;
	_transform.Position.W = 1.f;
	MarkTransformUpdateNeeded();
}

void SGameObject::SetRotation(const Quaternion& InRotation)
{
	_transform.Rotation = InRotation;
	MarkTransformUpdateNeeded();
}

void SGameObject::SetScale(const Vector4f& InScale)
{
	_transform.Scale = InScale;
	MarkTransformUpdateNeeded();
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
}

void SGameObject::OnExitTheWorld()
{
	_IncludedWorldHash = nullptr;
}


void SGameObject::MarkTransformUpdateNeeded()
{
	uint64 CurFrameCnt = SSFrameInfo::GetFrameCnt();
	if (_LastTransformUpdateFrameCnt != CurFrameCnt)
	{
		_LastTransformUpdateFrameCnt = CurFrameCnt;

		SWorld* World = GetIncludedWorldRef();

		World->AddObjectNeedToUpdateTransform(this);

		int32 ChildCnt = GetChildCnt();
		for (int32 i=0;i<ChildCnt;i++)
		{
			SGameObject* ChildItem = GetChild(i);
			ChildItem->MarkTransformUpdateNeeded();
		}
	}
}