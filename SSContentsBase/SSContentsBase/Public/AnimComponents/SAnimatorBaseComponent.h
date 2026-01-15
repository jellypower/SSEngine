#pragma once
#include "SObject/Public/SObjHashT.h"

#include "SSContentsBase/Public/ContentBase/SComponentBase.h"

class IAnimWorkee;

class SSCONTENTBASE_MODULE SAnimatorBaseComponent : public SComponentBase
{
private:
	SS::PooledList<SObjHashT<SGameObject>> _BoneBindings;


public:
	const SS::PooledList<SObjHashT<SGameObject>>& GetBoneBindings() const { return _BoneBindings; }


public:
	virtual IAnimWorkee* GetAnimWorkee() const = 0;


	virtual void ApplyAnimWorkeeTransform();



	
	virtual void ReconstructBoneBinding();


public:
	virtual void OnEnterTheWorld() override;
	virtual void PostConstructHierarchy() override;
	virtual void OnExitTheWorld() override;
};
