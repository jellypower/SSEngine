#pragma once
#include "RenderAnimData.h"
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSRenderer/ModuleExportKeyword.h"


SSRENDERER_MODULE_NATIVE Transform EvaluateRenderKFTransform(const RenderAnimRawData* AnimRawData, int32 TrackIdx, float CurTime);
