#pragma once

struct SelfExcludeFilter : physx::PxQueryFilterCallback
{
	physx::PxRigidActor* _Self;

	explicit SelfExcludeFilter(physx::PxRigidActor* Self) : _Self(Self)
	{
	}

	physx::PxQueryHitType::Enum preFilter(
		const physx::PxFilterData&,
		const physx::PxShape*,
		const physx::PxRigidActor* Actor,
		physx::PxHitFlags&) override
	{
		return (Actor == _Self) ? physx::PxQueryHitType::eNONE : physx::PxQueryHitType::eBLOCK;
	}

	physx::PxQueryHitType::Enum postFilter(
		const physx::PxFilterData& filterData, 
		const physx::PxQueryHit& hit,
		const physx::PxShape* shape, 
		const physx::PxRigidActor* actor) override
	{
		return physx::PxQueryHitType::eBLOCK;
	}
};
