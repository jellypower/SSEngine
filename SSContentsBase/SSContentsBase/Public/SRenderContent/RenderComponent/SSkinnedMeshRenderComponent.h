#pragma once
#include "SObject/Public/SObjHashT.h"

#include "SRenderComponentBase.h"

class SSkinnedMeshRenderComponent : public SRenderComponentBase
{
private:
	SS::PooledList<SObjHashT<SGameObject>> _BoneBindings;
	SObjHashT<SGameObject> _RootBone;

protected:
	virtual bool ShouldProcessPerFrameInherently() const override;
	virtual void PerFrame() override;

	virtual void OnGameObjectTransformCommited() override;
	virtual void OnChildrenGameObjectTransformCommitted() override;

	virtual void PostConstructHierarchy() override;;

	virtual void ConstructRenderInstance() override;
	virtual void DestructRenderInstance() override;

public:
	SS::SHasherW GetModelAssetName() const { return _ModelAssetName; }

private:
	void ReconstructBoneBinding(SGameObject* RootBoneGameObject);
	void UpdateRenderInstanceBonePose();
};
