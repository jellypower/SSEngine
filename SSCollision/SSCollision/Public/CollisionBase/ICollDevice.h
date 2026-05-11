#pragma once

struct RIGID_CHARACTERMOVEMENT_DESC;
struct CI_SPHERE_DESC;
struct CI_BOX_DESC;
class IRigidCahracterMovement;
class ICISphere;
class ICollInstanceBase;
class ICIBox;
class ICollisionWorld;

class ICollDevice : public INoncopyable
{
public:
	virtual ICollisionWorld* CreateCollWorld(SS::SHasherW InWorldName) const = 0;
	virtual ICIBox* CreateCollBox(const CI_BOX_DESC& InDesc) = 0;
	virtual ICISphere* CreateCollSphere(const CI_SPHERE_DESC& InDesc) = 0;
	virtual IRigidCahracterMovement* CreateCharacterMovement(const RIGID_CHARACTERMOVEMENT_DESC& InDesc) = 0;


	// Static Functions
public:
	virtual bool AreColliding(const ICollInstanceBase* c1, const ICollInstanceBase* c2) = 0;


	// Debug
public:

};
