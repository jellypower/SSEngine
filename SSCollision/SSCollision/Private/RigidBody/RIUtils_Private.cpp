#include "pch.h"
#include "RIUtils_Private.h"

#include "SSCollision/Public/RigidBody/IRigidBodyBase.h"
#include "RigidCharacterMovement.h"



physx::PxActor* ExtractPxActor(IRigidBodyBase* RI)
{
	switch (RI->GetRigidBodyType())
	{
	case ERigidBodyType::CharacterMovement: return		static_cast<RigidCharacterMovement*>(RI)->GetPxActor();
	}

	SS_INTERRUPT();
	return nullptr;
}