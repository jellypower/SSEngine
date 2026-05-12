#pragma once
#include "SSCollision/Public/RigidBody/IRigidBodyBase.h"


enum class ERigidBodySimType
{
	None,

	Static,
	Dynamic,
	CustomSim
};


inline ERigidBodySimType GetRigidBodySimType(ERigidBodyType Type)
{
	switch (Type)
	{
	case ERigidBodyType::Static:
		return ERigidBodySimType::Static;

	case ERigidBodyType::Dynamic:
		return ERigidBodySimType::Dynamic;

	case ERigidBodyType::CharacterMovement:
		return ERigidBodySimType::CustomSim;

	default:
		SS_ASSERT(false);
		return ERigidBodySimType::None;
	}

}
