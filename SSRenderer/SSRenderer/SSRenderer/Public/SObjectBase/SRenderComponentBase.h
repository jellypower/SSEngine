#pragma once
#include "SObject/Public/SComponentBase.h"
#include "SObject/Public/SObjectBase.h"

class BasicRenderInstance;
class ModelAsset;
class SRenderComponentBase;

class SRenderComponentBase : public SComponentBase
{
protected:
	SS::SHasherW _ModelAssetName;
	BasicRenderInstance* _RenderInstance = nullptr;


public:
	BasicRenderInstance* GetRenderInstance() const { return _RenderInstance; }

	void SetModelAsset(SS::SHasherW ModelAssetName);

	// virtual void PostConstruct() override;
	virtual void PostConstructHierarchy() override;
	virtual void OnEnterTheWorld() override;
	virtual void OnExitTheWorld() override;
	virtual void PreDestructHierarchy() override;
	// virtual void PreDestruct() override;

protected:
	virtual void ConstructRenderInstance() = 0;
	virtual void DestructRenderInstance() = 0;

};
