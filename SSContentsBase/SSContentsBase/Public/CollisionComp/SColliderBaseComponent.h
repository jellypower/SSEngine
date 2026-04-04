#pragma once
#include "SSContentsBase/Public/ContentBase/SComponentBase.h"


class ICollInstanceBase;

class SSCONTENTBASE_MODULE SColliderBaseComponent : public SComponentBase
{
public:
	// virtual void PostConstruct() override;
	virtual void PostConstructHierarchy() override;
	virtual void OnEnterTheWorld() override;
	virtual void OnExitTheWorld() override;
	virtual void PreDestructHierarchy() override;
	// virtual void PreDestruct() override;

	void OnGameObjectTransformCommited() override;


public:
	virtual Vector4f CalcFurthest(const Vector4f& Dir) const = 0;
	virtual ICollInstanceBase* GetCollInstance() const = 0;

protected:
	virtual void ConstructCollInstance() = 0;
	virtual void DestructCollInstance() = 0;
};
