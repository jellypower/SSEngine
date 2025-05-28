#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SObject/Public/SObjectBase.h"


void InitSObjectBaseInfo(SObjectBase* InNewObject, SS::SHasherW ObjectName);
void DelSObject(SObjectBase* Obj);

template<typename T>
inline static T* NewSObject(SS::SHasherW InNewObjName)
{
	static_assert(std::derived_from<T, SObjectBase>, "Must be derived from SObjectBase");
	T* NewSObject = DBG_NEW T();
	InitSObjectBaseInfo(NewSObject, InNewObjName);
	return NewSObject;
}