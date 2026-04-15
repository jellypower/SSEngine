#pragma once
#include <SSEngineDefault/Public/SSContainer/HashMap.h>

#include "SObject/Public/SObjHashT.h"

#include "SSContentsBase/Public/ContentBase/SComponentBase.h"

class IAnimWorkee;

class SSCONTENTBASE_MODULE SAnimatorBaseComponent : public SComponentBase
{
private:
	SS::PooledList<SObjHashT<SGameObject>> _BoneBindings;


public:
	SAnimatorBaseComponent();

public:
	virtual void OnEnterTheWorld() override;
	virtual void PostConstructHierarchy() override;
	virtual void OnExitTheWorld() override;


public:
	const SS::PooledList<SObjHashT<SGameObject>>& GetBoneBindings() const { return _BoneBindings; }


public:
	virtual IAnimWorkee* GetAnimWorkee() const = 0;
	virtual void OnAnimWorkerUpdateAnimationEnded() { }

	virtual void ReconstructBoneBinding();
	virtual void ApplyAnimWorkeeTransform();


};
