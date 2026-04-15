#define SOBJECT_MODULE_EXPORT
#include "SObject/Public/SObjHashCode.h"
#include "SObject/Public/SObjectBase.h"
#include "SObject/Public/SObjectGlobalHashMap.h"
#include "SObject/Public/GlobalVariableSet/SObjectGlobalVariableSet.h"


SObjHashCode::SObjHashCode(SObjectBase* InObject)
{
	if (InObject == nullptr)
	{
		return;
	}

	_NativeValue = InObject->GetHashCode()._NativeValue;
}

SObjectBase* SObjHashCode::GetSObject() const
{
	SObjectBase* FoundObj = g_ObjectHashMap->FindObject(*this);
	return FoundObj;
}
