#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/ContentBase/SGameObject.h"


#include "SSContentsBase/Public/ContentBase/SComponentBase.h"
#include "SSContentsBase/Public/ContentBase/SWorld.h"
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

SGameObject* SGameObject::FindChildOfName(SS::SHasherW Name, bool bIncludeHieararchy) const
{
	if (bIncludeHieararchy == false)
	{
		for (SGameObject* ChildItem : _Children)
		{
			if (ChildItem->GetObjectName() == Name)
			{
				return ChildItem;
			}
		}

		return nullptr;
	}

	for (SGameObject* ChildItem : _Children)
	{
		if (ChildItem->GetObjectName() == Name)
		{
			return ChildItem;
		}

		SGameObject* Descendant = ChildItem->FindChildOfName(Name, bIncludeHieararchy);
		if (Descendant != nullptr)
		{
			return Descendant;
		}
	}

	return nullptr;
}

void SGameObject::ScrapAllDescendants(SS::PooledList<SGameObject*>& OutDescendants) const
{
	ScrapAllDescendant_Recursion(OutDescendants, this);
}

Transform SGameObject::CalcWorldTransform() const
{
	if (IsRootInWorld())
	{
		return _transform;
	}

	SGameObject* Parent = GetParent();
	Transform ParentTransform = Parent->CalcWorldTransform();

	return _transform * ParentTransform;
}

XMMATRIX SGameObject::CalcWorldTransformMatrix() const
{
	XMMATRIX TransformMat = _transform.AsMatrix();

	if (IsRootInWorld())
	{
		return TransformMat;
	}

	SGameObject* Parent = GetParent();
	XMMATRIX ParentWorldTransformMat = Parent->CalcWorldTransformMatrix();

	return TransformMat * ParentWorldTransformMat;
}

Quaternion SGameObject::CalcWorldRot() const
{
	if (IsRootInWorld())
	{
		return _transform.Rotation;
	}

	SGameObject* Parent = GetParent();

	return _transform.Rotation * Parent->CalcWorldRot();
}

void SGameObject::SetTransform(const Transform& InTransform)
{
	_transform = InTransform;
	_transform.Position.W = 1.f;
	MarkTransformCommitNeeded();
}

void SGameObject::SetWorldTransform(const Transform& InWorldTransform)
{
	const SGameObject* Parent = GetParent();
	if (Parent == nullptr)
	{
		SetTransform(InWorldTransform);
		return;
	}

	Transform ParentWorldTransform = Parent->CalcWorldTransform();
	Transform ParentWorldInverseTransform = ParentWorldTransform.Inverse();

	Transform TransformToApply = InWorldTransform * ParentWorldInverseTransform;

	SetTransform(TransformToApply);
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
	if (InComponent->GetGameObject() != nullptr)
	{
		SS_ASSERT_MSG(false, L"Already has a Parent");
		return;
	}

	InComponent->InitComponentWithParent(this);
	_Components.PushBack(InComponent);
}


void SGameObject::SetStrongBindAncestor(SGameObject* InAncestor)
{
	if (InAncestor == this)
	{
		_StrongBindAncestor = InAncestor;
		return;
	}

	SGameObject* Parent = GetParent();
	if (Parent != nullptr && // This오브젝트가 아직 Enter World하지 않고 생성되는 중간인 경우에는 Parent가 없을 수도 있음
		Parent->GetStrongBindAncestor() != InAncestor)
	{
		SS_ASSERT(false); 
		return;
	}

	_StrongBindAncestor = InAncestor;
}

void SGameObject::OnEnterTheWorld(SObjHashCode WorldHashCode)
{
	_IncludedWorldHash = WorldHashCode;


	// AddToWorld전에 SetTransform하고 AddToWorld하면 _bTransformCommitReserved 가 true로 설정된다.
	// 그러면 World->AddTransformCommitNeededObj 가 실행되지 않기 때문에 여기선 그냥 임의로 실행해준다.
	_bTransformCommitReserved = true;
	SWorld* World = GetIncludedWorldRef();
	World->AddTransformCommitNeededObj(this);
}

void SGameObject::OnExitTheWorld()
{
	_IncludedWorldHash = nullptr;
}

void SGameObject::ScrapAllDescendant_Recursion(SS::PooledList<SGameObject*>& OutDescendants, const SGameObject* ParentToScrap)
{
	for (SGameObject* ChildItem : ParentToScrap->_Children)
	{
		OutDescendants.PushBack(ChildItem);
		ParentToScrap->ScrapAllDescendant_Recursion(OutDescendants, ChildItem);
	}
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
	uint64 ThisFrameCnt = SSFrameInfo::GetFrameCnt();
	if (ThisFrameCnt == _TransformCommitedFrameCnt)
	{
		// 이미 커밋된 트랜스폼은 패스한다.
		// 커밋은 모든 움직임이 끝나고 게임오브젝트당 1번 만 하는게 목표.
		return;
	}

	XMMATRIX ThisTransformMat = _transform.AsMatrix();
	_CommittedWorldTransformMat = ThisTransformMat * ParentWorldTransformMat;
	_CommittedWorldRotation =  _transform.Rotation * ParentRotation;
	_bTransformCommitReserved = false;
	_TransformCommitedFrameCnt = ThisFrameCnt;

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
