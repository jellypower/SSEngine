#pragma once
#include "SSEngineDefault/ModuleExportKeyword.h"


struct StringWSerializerContainer;

SSENGINEDEFAULT_MODULE_NATIVE void FillDataFromStrings(StringWSerializerContainer& Container);
SSENGINEDEFAULT_MODULE_NATIVE bool FillStringFromData(StringWSerializerContainer& Container);