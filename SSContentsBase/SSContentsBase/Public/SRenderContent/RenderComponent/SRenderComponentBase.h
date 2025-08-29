#pragma once
#include "SSContentsBase/Public/ContentBase/SComponentBase.h"

#include "SSContentsBase/ModuleExportKeyword.h"

class IRenderInstance;
class SRenderComponentBase;

class SSCONTENTBASE_MODULE SRenderComponentBase : public SComponentBase
{
protected:
	SS::SHasherW _ModelAssetName;
	IRenderInstance* _RenderInstance = nullptr;


public:
	IRenderInstance* GetRenderInstance() const { return _RenderInstance; }

	void SetModelAsset(SS::SHasherW ModelAssetName);

	// virtual void PostConstruct() override;
	virtual void PostConstructHierarchy() override;
	virtual void OnEnterTheWorld() override;
	virtual void OnExitTheWorld() override;
	virtual void PreDestructHierarchy() override;
	// virtual void PreDestruct() override;

	void OnGameObjectTransformCommited() override;

protected:
	virtual void ConstructRenderInstance() = 0;
	virtual void DestructRenderInstance() = 0;

};
