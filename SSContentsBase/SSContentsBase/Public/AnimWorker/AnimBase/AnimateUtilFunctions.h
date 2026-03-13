#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

#include "SSEngineDefault/Public/SSContainer/HashMap.h"

#include "SSContentsBase/ModuleExportKeyword.h"


struct PoseSlot;
class RenderAnimRawData;

SSCONTENTBASE_MODULE Transform EvaluateRenderKFTransform(const RenderAnimRawData* AnimRawData, int32 TrackIdx, float CurTimeRatio);

SSCONTENTBASE_MODULE void EvaluatePose(
	PoseSlot& OutPose,
	const SS::HashMap<SS::SHasherW, int32>& PoseBoneIdxMap,
	const RenderAnimRawData* AnimRawData,
	float CurTimeRatio,
	SS::SHasherW RootToIgnore = SS::SHasherW::GetEmpty());


SSCONTENTBASE_MODULE bool BlendThreePoses(
	PoseSlot& OutPose,
	const PoseSlot& InPose1, float Weight1,
	const PoseSlot& InPose2, float Weight2,
	const PoseSlot& InPose3, float Weight3);

SSCONTENTBASE_MODULE bool BlendPoses(
	PoseSlot& OutPose,
	const PoseSlot& InPose1,
	const PoseSlot& InPose2,
	float Alpha);