#define DLL_EXPORT
#include "SObject/Public/SObjHashCode.h"
#include "SObject/Public/SObjectBase.h"
#include "SObject/Public/SObjectGlobalHashMap.h"
#include "SObject/Public/GlobalVariableSet/SObjectGlobalVariableSet.h"


SObjHashCode::SObjHashCode(SObjectBase* InObject)
{
	_NativeValue = InObject->_HashCode._NativeValue;
}

SObjectBase* SObjHashCode::GetSObject() const
{
	SObjectBase* FoundObj = g_ObjectHashMap->FindObject(*this);
	return FoundObj;
}
