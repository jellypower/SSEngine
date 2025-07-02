#include "SSContentsBase/SWorld.h"

#include "SSContentsBase/SComponentBase.h"
#include "SSContentsBase/SGameObject.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/ModelAsset.h"
#include "SSRenderer/Public/RenderInstance/IRenderInstance.h"

#include "SSRenderer/Public/RenderBase/RenderWorld.h"
#include "SRenderContent/Public/RenderComponent/SRenderComponentBase.h"


SWorld::SWorld() :
	_ObjectsByHashCode(WORLD_OBJECTMAP_HASHMAP_SIZE, WORLD_OBJECTMAP_HASHBUCKET_SIZE),
	_ObjectsNeedToUpdateTransform(TRANSFORM_UPDATE_HASHMAP_SIZE, TRANSFORM_UPDATE_HASHBUCKET_SIZE)
{
}

SWorld::~SWorld()
{
	bool Remain = _RenderWorld->IsAnyInstanceRemainInWorld();
	SS_ASSERT(Remain == false);

	delete _RenderWorld;
	_RenderWorld = nullptr;
}

void SWorld::PostConstruct()
{
	_WorldRootObject = NewSObject<SGameObject>(L"WorldRoot");
	SObjHashCode WorldHashCode = GetHashCode();
	AddWorldRootObject(_WorldRootObject);
}

void SWorld::PreDestruct()
{
	DelSObject(_WorldRootObject);
	_WorldRootObject = nullptr;
}

void SWorld::InitializeWorld(RenderWorld* InRenderWorld)
{
	_RenderWorld = InRenderWorld;
}

bool SWorld::IsAnyObjectRemainInWorld() const
{
	for (const SS::pair<SObjHashCode, SGameObject*>& item : _ObjectsByHashCode)
	{
		if (item.second != nullptr)
		{
			return true;
		}
	}
	return false;
}

void SWorld::DestroyAllObjectsInWorld()
{
	int32 ChildCnt = _WorldRootObject->GetChildCnt();
	for (int32 i = 0; i < ChildCnt; i++)
	{
		SGameObject* ChildItem = _WorldRootObject->GetChild(i);
		RemoveFromWorld(ChildItem);

		SGameObjectConstructor::DestroyAll(ChildItem);
	}
}

void SWorld::ClearObjectsNeedToUpdateTransformList()
{
	_ObjectsNeedToUpdateTransform.Clear();
}

void SWorld::AddObjectNeedToUpdateTransform(SGameObject* InObj)
{
	SObjHashCode WorldHashCode = GetHashCode();
	if (WorldHashCode != InObj->GetIncludedWorldHash())
	{
		SS_INTERRUPT();
	}

	SObjHashCode InObjHashCode = InObj->GetHashCode();
	SGameObject** ppFoundObj = _ObjectsNeedToUpdateTransform.Find(InObjHashCode);
	if (ppFoundObj != nullptr)
	{
		// 부모 오브젝트의 위치가 업데이트 되면서 자식 오브젝트를 포함시켰으면 이미 존재할 수도 있음
		return;
	}

	_ObjectsNeedToUpdateTransform.Add(InObjHashCode, InObj);
}

void SWorld::AddToWorld(SGameObject* InNewWorldObject, SGameObject* ParentObject)
{
	if (InNewWorldObject->GetIncludedWorldHash() != nullptr)
	{
		SS_ASSERT_MSG(false, L"To add to world, GameObject must not be included in any world.");
		return;
	}


	if (ParentObject != nullptr)
	{
		if (GetHashCode() != ParentObject->GetIncludedWorldHash())
		{
			SS_ASSERT_MSG(false, L"Parent must be included same world.");
			return;
		}

		AddGameObjectItem(InNewWorldObject);
		InNewWorldObject->SetParent(ParentObject);
	}
	else
	{
		AddGameObjectItem(InNewWorldObject);
		InNewWorldObject->SetParent(_WorldRootObject);
	}

	int32 ChildCnt = InNewWorldObject->GetChildCnt();
	for (int32 i = 0; i < ChildCnt; i++)
	{
		SGameObject* ChildObj = InNewWorldObject->GetChild(i);
		AddToWorld_Recursion(ChildObj);
	}
}

void SWorld::RemoveFromWorld(SGameObject* InObjectToRemove)
{
	RemoveFromWorld_Recursion(InObjectToRemove);
}

void SWorld::AddToWorld_Recursion(SGameObject* InNewObject)
{
	SGameObject** ppFoundGameObject = _ObjectsByHashCode.Find(InNewObject->GetHashCode());

	if (ppFoundGameObject != nullptr && (*ppFoundGameObject) != nullptr)
	{
		SS_ASSERT_MSG(false, L"Already smae object exists.");
		return;
	}

	// ==============================
	AddGameObjectItem(InNewObject);
	// ==============================

	int32 ChildCnt = InNewObject->GetChildCnt();
	for (int32 i = 0; i < ChildCnt; i++)
	{
		SGameObject* ChildObj = InNewObject->GetChild(i);
		AddToWorld_Recursion(ChildObj);
	}
}

void SWorld::RemoveFromWorld_Recursion(SGameObject* InObjectToRemove)
{
	int32 ChildCnt = InObjectToRemove->GetChildCnt();
	for (int32 i = ChildCnt - 1; i >= 0; i--)
	{
		SGameObject* ChildObj = InObjectToRemove->GetChild(i);
		RemoveFromWorld_Recursion(ChildObj);
	}

	// ==============================
	RemoveGameObjectItem(InObjectToRemove);
	// ==============================
}


void SWorld::AddGameObjectItem(SGameObject* InNewObject)
{
	InNewObject->OnEnterTheWorld(GetHashCode());

	_ObjectsByHashCode.Add(InNewObject->GetHashCode(), InNewObject);

	int32 CompCnt = InNewObject->GetComponentCnt();
	for (int32 i = 0; i < CompCnt; i++)
	{
		SComponentBase* CompItem = InNewObject->GetComponentByIdx(i);
		CompItem->OnEnterTheWorld();

		if (SRenderComponentBase* RenderComponent = dynamic_cast<SRenderComponentBase*>(CompItem))
		{
			_RenderWorld->AddToWorld(RenderComponent->GetRenderInstance());
		}
	}
}

void SWorld::RemoveGameObjectItem(SGameObject* InObjectToRemove)
{
	SGameObject** ppFoundGameObject = _ObjectsByHashCode.Find(InObjectToRemove->GetHashCode());
	if (ppFoundGameObject == nullptr)
	{
		SS_ASSERT_MSG(false, L"Object already removed.");
		return;
	}

	int32 CompCnt = InObjectToRemove->GetComponentCnt();
	for (int32 i = CompCnt - 1; i >= 0; i--)
	{
		SComponentBase* CompItem = InObjectToRemove->GetComponentByIdx(i);
		CompItem->OnExitTheWorld();

		if (SRenderComponentBase* RenderComponent = dynamic_cast<SRenderComponentBase*>(CompItem))
		{
			IRenderInstance* RenderInstance = RenderComponent->GetRenderInstance();
			SObjHashCode GOID =  RenderInstance->GetGameObjectID();
			_RenderWorld->RemoveFromWorld(GOID);
		}
	}

	InObjectToRemove->OnExitTheWorld();
	_ObjectsByHashCode.Remove(InObjectToRemove->GetHashCode());
}

void SWorld::AddWorldRootObject(SGameObject* InWorldRootObject)
{
	_WorldRootObject = InWorldRootObject;
	_WorldRootObject->MarkHierarchyInitialized();
	_WorldRootObject->OnEnterTheWorld(GetHashCode());
}
