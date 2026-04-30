#include "pch.h"
#define SSCOLLISION_MODULE_EXPORT
#include "SSCollision/Public/ModuleEntry/CollisionWorldFactory.h"

#include "SSCollision/Private/CollisionBase/CollDevice.h"
#include "SSCollision/Private/CollisionBase/CollisionWorld.h"


ICollDevice* g_CollDevice = nullptr;

ICollDevice* CreateCollDevice()
{
	if (g_CollDevice != nullptr)
	{
		SS_INTERRUPT();
		return nullptr;
	}
	g_CollDevice = DBG_NEW CollDevice();


	return g_CollDevice;
}
