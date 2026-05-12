#pragma once
#include "SRigidBodyBaseComponent.h"

class IRigidBodyBase;


class SSCONTENTBASE_MODULE SRigidBodyStaticComponent : public SRigidBodyBaseComponent
{
private:
	IRigidBodyBase* _RigidBodyStatic = nullptr;

public:
	// SRigidBodyBaseComponent
public:
	IRigidBodyBase* GetRigidBodyInstance() const override;
	void PostCollision_SyncTransform() override;

protected:
	void ConstructRigidBodyInstance() override;
	void DestructRigidBodyInstance() override;
};
