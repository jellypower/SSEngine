#pragma once
#include "SSCollision/ModuleExportKeyword.h"

class ICollDevice;
SSCOLLISION_MODULE_NATIVE ICollDevice* CreateCollDevice();
typedef decltype(&CreateCollDevice) FuncPtr_CreateCollDevice;
