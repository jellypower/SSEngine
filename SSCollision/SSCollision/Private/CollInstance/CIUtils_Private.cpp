#include "pch.h"
#include "CIUtils_Private.h"


#include "SSCollision/Public/CollInstance/ICollInstanceBase.h"
#include "CIBox.h"
#include "CISphere.h"



physx::PxShape* ExtractPxShape(ICollInstanceBase* CI)
{
	switch (CI->GetCollShapeType())
	{
	case ECollShapeType::Box: return		static_cast<CIBox*>(CI)->GetPxShape();
	case ECollShapeType::Sphere:return		static_cast<CISphere*>(CI)->GetPxShape();
	}

	SS_INTERRUPT();
	return nullptr;
}
