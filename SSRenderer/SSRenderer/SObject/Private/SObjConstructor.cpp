#include "SObject/Public/SObjConstructor.h"

#include "SObject/Public/SComponentBase.h"
#include "SObject/Public/SGameObject.h"
#include "SObject/Public/SObjectGlobalHashMap.h"
#include "SObject/Public/GlobalVariableSet/SObjectGlobalVariableSet.h"


void SObjConstructor::InitSObjectBaseInfo(SObjectBase* InNewObject, SS::SHasherW ObjectName)
{
	InNewObject->_ObjectName = ObjectName;
	SObjHashCode NewHashCode = g_ObjectHashMap->InsertNewObject(InNewObject);
	InNewObject->_HashCode = NewHashCode;
	InNewObject->PostConstruct();
}

void SObjConstructor::FinishConstructHierarchy_Recursion(SGameObject* CurGameObject)
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

void SObjConstructor::Destroy_Recursive(SGameObject* ObjToDestroy)
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
		SObjConstructor::Delete(CompItem);
	}

	SObjConstructor::Delete(ObjToDestroy);
}

void SObjConstructor::PreDestructHierarchy_Recursive(SGameObject* ObjToDestroy)
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

void SObjConstructor::Delete(SObjectBase* Obj)
{
	Obj->PreDestruct();
	SObjHashCode HashCodeToRemove = Obj->GetHashCode();
	g_ObjectHashMap->RemoveObject(HashCodeToRemove);
	delete Obj;
}

void SObjConstructor::DestroyAll(SGameObject* RootObjToDestroy)
{
	PreDestructHierarchy_Recursive(RootObjToDestroy);
	Destroy_Recursive(RootObjToDestroy);
}

void SObjConstructor::FinishConstructHierarchy(SGameObject* RootObject)
{
	FinishConstructHierarchy_Recursion(RootObject);
}
