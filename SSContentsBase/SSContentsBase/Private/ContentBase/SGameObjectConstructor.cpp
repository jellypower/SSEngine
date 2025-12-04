#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/ContentBase/SGameObjectConstructor.h"


#include "SSContentsBase/Public/ContentBase/SComponentBase.h"
#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SObject/Public/SObjectGlobalHashMap.h"
#include "SObject/Public/ModuleEntry/SObjectFactory.h"


void SGameObjectConstructor::FinishConstructHierarchy_Recursion(SGameObject* CurGameObject)
{
	CurGameObject->MarkHierarchyInitialized();

	int32 CompCnt = CurGameObject->GetComponentCnt();
	for (int32 i = 0; i < CompCnt; i++)
	{
		SComponentBase* ComponentItem = CurGameObject->GetComponentByIdx(i);
		ComponentItem->PostConstructHierarchy();
	}

	int32 ChildCnt = CurGameObject->GetChildCnt();
	for (int32 i = 0; i < ChildCnt; i++)
	{
		SGameObject* ChildItem = CurGameObject->GetChild(i);
		FinishConstructHierarchy_Recursion(ChildItem);
	}
}

void SGameObjectConstructor::Destroy_Recursive(SGameObject* ObjToDestroy)
{
	int32 ChildCnt = ObjToDestroy->GetChildCnt();
	for (int32 i = ChildCnt - 1; i >= 0; i--)
	{
		SGameObject* ChildItem = ObjToDestroy->GetChild(i);
		Destroy_Recursive(ChildItem);
	}

	int32 CompCnt = ObjToDestroy->GetComponentCnt();
	for (int32 i = CompCnt - 1; i >= 0; i--)
	{
		SComponentBase* CompItem = ObjToDestroy->GetComponentByIdx(i);
		DelSObject(CompItem);
	}

	DelSObject(ObjToDestroy);
}

void SGameObjectConstructor::PreDestructHierarchy_Recursive(SGameObject* ObjToDestroy)
{
	int32 ChildCnt = ObjToDestroy->GetChildCnt();
	for (int32 i = ChildCnt - 1; i >= 0; i--)
	{
		SGameObject* ChildItem = ObjToDestroy->GetChild(i);
		PreDestructHierarchy_Recursive(ChildItem);
	}

	int32 CompCnt = ObjToDestroy->GetComponentCnt();
	for (int32 i = CompCnt - 1; i>= 0 ; i--)
	{
		SComponentBase* CompItem = ObjToDestroy->GetComponentByIdx(i);
		CompItem->PreDestructHierarchy();
	}
}


void SGameObjectConstructor::DestroyAll(SGameObject* RootObjToDestroy)
{
	if (RootObjToDestroy->IsStronglyBound() &&
		RootObjToDestroy->IsStrongBindAncestor() == false)
	{
		SS_ASSERT(false);
		return;
	}

	PreDestructHierarchy_Recursive(RootObjToDestroy);
	Destroy_Recursive(RootObjToDestroy);
}

void SGameObjectConstructor::FinishConstructHierarchy(SGameObject* RootObject)
{
	FinishConstructHierarchy_Recursion(RootObject);
}
