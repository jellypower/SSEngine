#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

#include "SSContentsBase/ModuleExportKeyword.h"

struct PoseSlot;
class IMeshAsset;
class SWorld;

class SSCONTENTBASE_MODULE SRenderDebugUtil
{
public:
	static void DrawDebugMesh(
		SWorld* WorldToDraw, 
		const Transform& Transform, 
		IMeshAsset* MeshToDraw,
		bool bUseDepth,
		const Vector4f& Color = Vector4f::Zero, 
		float Time = 0);

	static void DrawDebugMesh(
		SWorld* WorldToDraw,
		const XMMATRIX& WMatrix,
		const XMMATRIX& RotMatrix,
		IMeshAsset* MeshToDraw,
		bool bUseDepth,
		const Vector4f& Color = Vector4f::Zero,
		float Time = 0
	);

	static void DrawDirectionalMesh(
		SWorld* WorldToDraw,
		const Vector4f& StartPos,
		const Vector4f& EndPos,
		IMeshAsset* DirectionableMesh,
		bool bUseDepth,
		float Thickness = 0.5,
		const Vector4f& Color = Vector4f::Zero,
		float Time = 0
	);

	static void DrawLine(
		SWorld* WorldToDraw,
		const Vector4f& StartPos,
		const Vector4f& EndPos,
		bool bUseDepth,
		float Thickness = 0.3,
		const Vector4f& Color = Vector4f::Zero,
		float Time = 0
	);


	static void DrawDebugPose(
		SWorld* WorldToDraw,
		const XMMATRIX& PoseOriginMatrix,
		const PoseSlot& InPose,
		bool bUseDepth,
		float Thickness = 0.5,
		const Vector4f& Color = Vector4f::Zero,
		float Time = 0);
};
