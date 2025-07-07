#pragma once
#include "SObject/Public/SObjectBase.h"

class SGameObject;

class SComponentBase : public SObjectBase
{
private:
	SGameObject* _Parent = nullptr;

public:
	SGameObject* GetParent() const { return _Parent; }

	void InitComponentWithParent(SGameObject* InParent);

public:
	// virtual void PostConstruct() { };
	virtual void PostConstructHierarchy() { }
	virtual void OnEnterTheWorld() { }
	virtual void OnExitTheWorld() { }
	virtual void PreDestructHierarchy() { }
	// virtual void PreDestruct() { };


	virtual void OnGameObjectTransformCommited() { }
	virtual void OnChildrenGameObjectTransformCommitted() { }
};
