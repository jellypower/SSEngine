#pragma once
#include "SSCollision/Public/CollisionBase/ICollDevice.h"



class CollDevice : public ICollDevice
{
public:
	virtual ICollisionWorld* CreateCollWorld(SS::SHasherW InWorldName) const override;
	virtual ICIBox* CreateCollBox() override;
	virtual ICISphere* CreateCollSphere() override;



	// Static Functions
public:
	virtual bool AreColliding(const ICollInstanceBase* c1, const ICollInstanceBase* c2) override;
};
