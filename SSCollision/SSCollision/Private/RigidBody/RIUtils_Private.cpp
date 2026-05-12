#include "pch.h"
#include "RIUtils_Private.h"

#include "SSCollision/Public/RigidBody/IRigidBodyBase.h"
#include "RigidBodyStatic.h"
#include "RigidBodyDynamic.h"
#include "RigidCharacterMovement.h"



physx::PxActor* ExtractPxActor(IRigidBodyBase* RI)
{
	switch (RI->GetRigidBodyType())
	{
	case ERigidBodyType::Static:            return static_cast<RigidBodyStatic*>(RI)->GetPxActor();
	case ERigidBodyType::Dynamic:           return static_cast<RigidBodyDynamic*>(RI)->GetPxActor();
	case ERigidBodyType::CharacterMovement: return static_cast<RigidCharacterMovement*>(RI)->GetPxActor();
	}

	SS_INTERRUPT();
	return nullptr;
}