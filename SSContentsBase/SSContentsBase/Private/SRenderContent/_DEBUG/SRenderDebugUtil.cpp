#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/SRenderContent/_DEBUG/SRenderDebugUtil.h"

#include "SSContentsBase/Public/AnimWorker/AnimWorkee/IAnimWorkee.h"
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
	if (Dist < SS_EPSILON)
	{
		return;
	}
	Dir = Dir / Dist;

	Quaternion Rot = Quaternion::CalcPitchYawRotationFromDir(Dir);

	Vector4f Scale;
	Scale.Z = Dist;
	Scale.X = Dist * Thickness;
	Scale.Y = Dist * Thickness;

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
	const PoseSlot& InPose,
	bool bUseDepth,
	float Thickness,
	const Vector4f& Color,
	float Time)
{
	ICommonRenderAssetSet* CommonAssets = g_Renderer->GetCommonRenderAssetSet();
	IMeshAsset* ArrowMesh = CommonAssets->GetArrowMesh();


	SS::PooledList<XMMATRIX> WorldTransformMatrices;

	int BoneCnt = InPose.BoneTransforms.GetSize();
	WorldTransformMatrices.Reserve(BoneCnt);
	for (int i = 0; i < BoneCnt; i++)
	{
		int ParentIdx = InPose.ParentBoneIdx[i];

		XMMATRIX WorldTransform = XMMatrixIdentity();

		while (ParentIdx != -1)
		{
			const Transform& ParentTransform = InPose.BoneTransforms[ParentIdx];
			WorldTransform = WorldTransform * ParentTransform.AsMatrix();

			ParentIdx = InPose.ParentBoneIdx[ParentIdx];
		}

		WorldTransform = WorldTransform * PoseOriginMatrix;
		WorldTransformMatrices.PushBack(WorldTransform);
	}


	for (int i = 0; i < BoneCnt; i++)
	{
		Vector4f EndPos = WorldTransformMatrices[i].r[3];
		int ParentIdx = InPose.ParentBoneIdx[i];

		Vector4f StartPos;
		if (ParentIdx == -1)
		{
			StartPos = PoseOriginMatrix.r[3];
		}
		else
		{
			StartPos = WorldTransformMatrices[ParentIdx].r[3];
		}

		DrawDirectionalMesh(WorldToDraw, StartPos, EndPos, ArrowMesh, bUseDepth, Thickness, Color, Time);
	}
}
