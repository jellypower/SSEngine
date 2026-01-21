#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/SRenderContent/_DEBUG/SRenderDebugUtil.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderBase/ICommonRenderAssetSet.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

#include "SSContentsBase/Public/ContentBase/SWorld.h"
#include "SSContentsBase/Public/SRenderContent/_DEBUG/TimedDebugDrawDesc.h"


void SRenderDebugUtil::DrawDebugMesh(
	SWorld* WorldToDraw,
	const Transform& Transform,
	IMeshAsset* MeshToDraw,
	bool bUseDepth,
	const Vector4f& Color,
	float Time)
{
	XMMATRIX WMatrix = Transform.AsMatrix();
	XMMATRIX RotMatrix = Transform.Rotation.AsMatrix();

	WorldToDraw->DebugDrawMesh(WMatrix, RotMatrix, MeshToDraw, bUseDepth, Color, Time);

}

void SRenderDebugUtil::DrawDebugMesh(
	SWorld* WorldToDraw,
	const XMMATRIX& WMatrix,
	const XMMATRIX& RotMatrix,
	IMeshAsset* MeshToDraw,
	bool bUseDepth,
	const Vector4f& Color,
	float Time)
{
	WorldToDraw->DebugDrawMesh(WMatrix, RotMatrix, MeshToDraw, bUseDepth, Color, Time);
}

void SRenderDebugUtil::DrawDirectionalMesh(
	SWorld* WorldToDraw,
	const Vector4f& StartPos, 
	const Vector4f& EndPos,
	IMeshAsset* DirectionableMesh, 
	bool bUseDepth,
	float Thickness,
	const Vector4f& Color, 
	float Time)
{
	Vector4f Dir = EndPos - StartPos;
	float Dist = Dir.Get3DLength();
	Dir = Dir / Dist;

	Quaternion Rot = Quaternion::CalcPitchYawRotationFromDir(Dir);

	Vector4f Scale;
	Scale.Z = Dist;
	Scale.X = Thickness;
	Scale.Y = Thickness;

	Transform Transform;
	Transform.Scale = Scale;
	Transform.Rotation = Rot;
	Transform.Position = StartPos;

	XMMATRIX WMatrix = Transform.AsMatrix();
	XMMATRIX RotMatrix = Rot.AsMatrix();

	WorldToDraw->DebugDrawMesh(WMatrix, RotMatrix, DirectionableMesh, bUseDepth, Color, Time);
}

void SRenderDebugUtil::DrawDebugPose(
	SWorld* WorldToDraw,
	const XMMATRIX& PoseOriginMatrix,
	const PoseSlot& ResultPose,
	bool bUseDepth,
	const Vector4f& Color,
	float Time)
{
	ICommonRenderAssetSet* CommonAssets = g_Renderer->GetCommonRenderAssetSet();
	IMeshAsset* ArrowMesh = CommonAssets->GetArrowMesh();
	// TODO: 여기서 계속하기
}
