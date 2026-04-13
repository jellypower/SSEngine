#pragma once
#include "SSContentsBase/Public/ContentBase/SComponentBase.h"

class IRigidBodyBase;

class SSCONTENTBASE_MODULE SRigidBodyBaseComponent : public SComponentBase
{
public:
	// virtual void PostConstruct() override;
	virtual void PostConstructHierarchy() override;
	virtual void OnEnterTheWorld() override;
	virtual void OnExitTheWorld() override;
	virtual void PreDestructHierarchy() override;
	// virtual void PreDestruct() override;

	void OnGameObjectTransformCommited(EFramePhase CommitPhase) override;


public:
	virtual IRigidBodyBase* GetRigidBodyInstance() const = 0;

protected:
	virtual void ConstructRigidBodyInstance() = 0;
	virtual void DestructRigidBodyInstance() = 0;
	
};
