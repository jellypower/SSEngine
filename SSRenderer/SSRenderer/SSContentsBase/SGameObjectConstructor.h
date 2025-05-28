#pragma once
#include "SSContentsBase/SGameObject.h"


class SGameObjectConstructor
{
private:
	static void FinishConstructHierarchy_Recursion(SGameObject* CurGameObject);
	static void PreDestructHierarchy_Recursive(SGameObject* ObjToDestroy);
	static void Destroy_Recursive(SGameObject* ObjToDestroy);

public:
	static void FinishConstructHierarchy(SGameObject* RootObject);
	static void DestroyAll(SGameObject* RootObjToDestroy);
};
