#pragma once
#include "SObject/Public/SObjectBase.h"

#include "SSContentsBase/ModuleExportKeyword.h"

class SWorld;
class SGameObject;

class SSCONTENTBASE_MODULE SComponentBase : public SObjectBase
{
private:
	SGameObject* _ParentGameObject = nullptr;

public:
	SGameObject* GetGameObject() const { return _ParentGameObject; }
	SWorld* GetIncludedWorld() const;

	void InitComponentWithParent(SGameObject* InParent);

public:
	virtual bool ShouldProcessPerFrameInherently() const { return false; }

	// virtual void PostConstruct() { };
	virtual void PostConstructHierarchy() { }
	virtual void OnEnterTheWorld() { }

	virtual void PerFrame(float DeltaTime) { }

	virtual void OnExitTheWorld() { }
	virtual void PreDestructHierarchy() { }
	// virtual void PreDestruct() { };


	virtual void OnGameObjectTransformCommited() { }
	virtual void OnChildrenGameObjectTransformCommitted() { }
};
