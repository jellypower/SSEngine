#pragma once
#include "SObject/ModuleExportKeyword.h"

#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SObject/Public/SObjectBase.h"


SOBJECT_MODULE_NATIVE void Internal_InitSObjectBaseInfo(SObjectBase* InNewObject, SS::SHasherW ObjectName);
SOBJECT_MODULE_NATIVE void DelSObject(SObjectBase* Obj);


template<typename T>
inline static T* NewSObject(SS::SHasherW InNewObjName)
{
	static_assert(std::derived_from<T, SObjectBase>, "Must be derived from SObjectBase");
	T* NewSObject = DBG_NEW T();
	Internal_InitSObjectBaseInfo(NewSObject, InNewObjName);
	return NewSObject;
}