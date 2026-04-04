#pragma once

class ICIBox;
class ICollisionWorld;

class ICollDevice : public INoncopyable
{
public:
	virtual ICollisionWorld* CreateCollWorld(SS::SHasherW InWorldName) const = 0;
	virtual ICIBox* CreateCollBox() = 0;
};
