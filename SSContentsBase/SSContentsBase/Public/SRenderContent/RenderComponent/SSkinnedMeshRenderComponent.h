#pragma once
#include <SSEngineDefault/Public/SSContainer/HashMap.h>

#include "SObject/Public/SObjHashT.h"
#include "SMeshRenderComponentBase.h"


class SSCONTENTBASE_MODULE SSkinnedMeshRenderComponent : public SMeshRenderComponentBase
{
private:
	SS::HashMap<SS::SHasherW, SObjHashT<SGameObject>> _ChildsByName;
	SS::PooledList<SObjHashT<SGameObject>> _BoneBindings;
	SObjHashT<SGameObject> _RootBone;

public:
	SSkinnedMeshRenderComponent();

protected:
	virtual bool ShouldProcessPerFrameInherently() const override;
	virtual void PerFrame(float DeltaTime) override;

	virtual void OnGameObjectTransformCommited() override;
	virtual void OnChildrenGameObjectTransformCommitted() override;

	virtual void PostConstructHierarchy() override;;

	virtual void ConstructRenderInstance() override;
	virtual void DestructRenderInstance() override;

private:
	void ReconstructBoneBinding(SGameObject* RootBoneGameObject);
	void UpdateRenderInstanceBonePose();
};
