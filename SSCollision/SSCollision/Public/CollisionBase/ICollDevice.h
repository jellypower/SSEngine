#pragma once

class ICISphere;
class ICollInstanceBase;
class ICIBox;
class ICollisionWorld;

class ICollDevice : public INoncopyable
{
public:
	virtual ICollisionWorld* CreateCollWorld(SS::SHasherW InWorldName) const = 0;
	virtual ICIBox* CreateCollBox() = 0;
	virtual ICISphere* CreateCollSphere() = 0;


	// Static Functions
public:
	virtual bool AreColliding(const ICollInstanceBase* c1, const ICollInstanceBase* c2) = 0;


	// Debug
public:

};
