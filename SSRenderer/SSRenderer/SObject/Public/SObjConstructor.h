#pragma once
#include <concepts>

#include "SObjectBase.h"


class SGameObject;

class SObjConstructor
{
private:
	static void InitSObjectBaseInfo(SObjectBase* InNewObject, SS::SHasherW ObjectName);

	static void FinishConstructHierarchy_Recursion(SGameObject* CurGameObject);

	static void PreDestructHierarchy_Recursive(SGameObject* ObjToDestroy);
	static void Destroy_Recursive(SGameObject* ObjToDestroy);

	
public:
	template<typename T>
	inline static T* New(SS::SHasherW InNewObjName)
	{
		static_assert(std::derived_from<T, SObjectBase>, "Must be derived from SObjectBase");
		T* NewSObject = DBG_NEW T();
		InitSObjectBaseInfo(NewSObject, InNewObjName);
		return NewSObject;
	}
	static void Delete(SObjectBase* Obj);


	static void FinishConstructHierarchy(SGameObject* RootObject);
	static void DestroyAll(SGameObject* RootObjToDestroy);

};
