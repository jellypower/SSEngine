#include "SObject/Public/ModuleEntry/SObjectFactory.h"

#include "SObject/Public/SObjectGlobalHashMap.h"
#include "SObject/Public/GlobalVariableSet/SObjectGlobalVariableSet.h"

void InitSObjectBaseInfo(SObjectBase* InNewObject, SS::SHasherW ObjectName)
{
	InNewObject->SetObjectNameXXX(ObjectName);
	SObjHashCode NewHashCode = g_ObjectHashMap->InsertNewObject(InNewObject);
	InNewObject->SetHashCodeXXX(NewHashCode);
	InNewObject->PostConstruct();
}

void DelSObject(SObjectBase* Obj)
{
	Obj->PreDestruct();
	SObjHashCode HashCodeToRemove = Obj->GetHashCode();
	g_ObjectHashMap->RemoveObject(HashCodeToRemove);
	delete Obj;
}
