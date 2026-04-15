#pragma once
#include "SSEngineDefault/ModuleExportKeyword.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"


struct HasherPoolNode;
SSENGINEDEFAULT_MODULE_NATIVE const HasherPoolNode* FindOrAddHasherNode(const utf16* InStr);


SSENGINEDEFAULT_MODULE_NATIVE void DestroyGlobalHasherPool();
