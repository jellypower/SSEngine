#include "RISkinnedMesh.h"

#include "SSGAL/Public/GALRenderInstance/GALRIMetadata.h"
#include "SSRenderer/Private/RenderBase/SSRenderer.h"
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMaterialAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSRenderer/Public/RenderCommon/SSRenderUtilFuncs.h"

RISkinnedMesh::RISkinnedMesh() :
	_SkeletonPose(200)
{
}

SObjHashCode RISkinnedMesh::GetGameObjectID() const
{
	return _GameObjectHashCode;
}

void RISkinnedMesh::SetGameObjectIDXXX(SObjHashCode InHashCode)
{
	_GameObjectHashCode = InHashCode;
}

ERenderInstanceType RISkinnedMesh::GetRIType() const
{
	return ERenderInstanceType::SkinnedMesh;
}

const XMMATRIX& RISkinnedMesh::GetWorldTransformMatrix() const
{
	return _WorldTransformMatrix;
}

const XMMATRIX& RISkinnedMesh::GetWorldRotationMatrix() const
{
	return _WorldRotationMatrix;
}

void RISkinnedMesh::SetWorldTransformMatrix(const XMMATRIX& InMatrix)
{
	_WorldTransformMatrix = InMatrix;
}

void RISkinnedMesh::SetWorldRotation(const Quaternion& InRotation)
{
	_WorldRotationMatrix = InRotation.AsMatrix();
}

void RISkinnedMesh::InjectGALMetadataXXX(GALRIMetadata* MetadataToHandover, int32 FrameMod)
{
	if (_MetaData[FrameMod] != nullptr)
	{
		SS_INTERRUPT();
		return;
	}

	if (MetadataToHandover->GetMetadataRenderInstanceType() != ERenderInstanceType::SkinnedMesh)
	{
		SS_INTERRUPT();
		return;
	}

	_MetaData[FrameMod] = MetadataToHandover;
}

GALRIMetadata* RISkinnedMesh::GetGALMetadata(int32 FrameMod) const
{
	return _MetaData[FrameMod];
}

void RISkinnedMesh::ReleaseGALMetaData()
{
	int32 CurFrameMod = RenderFrameInfo::GetFrameMod();

	for (int32 Offset = 0; Offset < GAL_NESTED_FRAME_CNT; Offset++)
	{
		const int32 ItemIdx =
			(CurFrameMod - Offset // CurFrameMod가 N이라고 하면 N-1번째 아이템은 CurFrame-1번째에 사용했던 녀석
				+ GAL_NESTED_FRAME_CNT) // CurFrameMod - Offset 값이 0보다 작을 수 있기 때문에 더해줌
			% GAL_NESTED_FRAME_CNT; // 그리고 다시 나눠줌

		if (_MetaData[ItemIdx] == nullptr)
		{
			continue;
		}

		const int32 DestroyDelay =
			(ItemIdx + GAL_NESTED_FRAME_CNT) % // 중첩된 프레임 뒤에 지운다.
			DEFERRED_DESTROY_MOD; // 위 값도 리밋을 넘을 수 있으니까 모듈러 한 번 더 해줌.

		g_Renderer->ReserveDestory(_MetaData[ItemIdx], DestroyDelay);
		_MetaData[ItemIdx] = nullptr;
	}
}

void RISkinnedMesh::OnEnterTheRenderWorldXXX(IRenderWorld* InRenderWorld)
{
	if (InRenderWorld == nullptr || _IncludedRenderWorld != nullptr)
	{
		SS_INTERRUPT();
	}

	_IncludedRenderWorld = InRenderWorld;

	AssetInstanceReferencer ThisAssetRef;
	ThisAssetRef.Type = EAssetInstanceReferenceType::ObjectHashCode;
	ThisAssetRef.ObjHashCode = _GameObjectHashCode;


	if (_MeshRef == nullptr)
	{
		SS_INTERRUPT();
		return;
	}
	_MeshRef->AddAssetReference(ThisAssetRef);

	int32 SubMeshCnt = _MeshRef->GetSubMeshCnt();
	for (int i = 0; i < SubMeshCnt; i++)
	{
		_MtlRef[i]->AddAssetReference(ThisAssetRef);
	}
}

void RISkinnedMesh::OnExitFromRenderWorldXXX()
{
	_IncludedRenderWorld = nullptr;

	AssetInstanceReferencer ThisAssetRef;
	ThisAssetRef.Type = EAssetInstanceReferenceType::ObjectHashCode;
	ThisAssetRef.ObjHashCode = _GameObjectHashCode;

	if (_MeshRef == nullptr)
	{
		SS_INTERRUPT();
		return;
	}
	_MeshRef->RemoveAssetReference(ThisAssetRef);

	int32 SubMeshCnt = _MeshRef->GetSubMeshCnt();
	for (int i = 0; i < SubMeshCnt; i++)
	{
		_MtlRef[i]->RemoveAssetReference(ThisAssetRef);
	}
}

IRenderWorld* RISkinnedMesh::GetIncludedRenderWorld() const
{
	return _IncludedRenderWorld;
}

IMeshAsset* RISkinnedMesh::GetMeshAsset() const
{
	return _MeshRef;
}

IMaterialAsset* RISkinnedMesh::GetMaterialAsset(int MtlIdx) const
{
	if (_MeshRef->GetSubMeshCnt() <= MtlIdx)
	{
		return nullptr;
	}

	return _MtlRef[MtlIdx];
}

void RISkinnedMesh::SetMeshAsset(IMeshAsset* InAsset)
{
	if (_IncludedRenderWorld == nullptr)
	{
		_MeshRef = InAsset;
		const int32 NewSubMeshCnt = _MeshRef->GetSubMeshCnt();
		_MtlRef.SetSizeDirectly(NewSubMeshCnt);
		return;
	}

	AssetInstanceReferencer ThisAssetRef;
	ThisAssetRef.Type = EAssetInstanceReferenceType::ObjectHashCode;
	ThisAssetRef.ObjHashCode = _GameObjectHashCode;

	const int32 PrevSubMeshCnt = _MeshRef->GetSubMeshCnt();
	_MeshRef->RemoveAssetReference(ThisAssetRef);
	_MeshRef = InAsset;
	const int32 NewSubMeshCnt = _MeshRef->GetSubMeshCnt();
	_MeshRef->AddAssetReference(ThisAssetRef);


	for (int32 i = NewSubMeshCnt; i < PrevSubMeshCnt; i++)
	{
		_MtlRef[i]->RemoveAssetReference(ThisAssetRef);
	}
	_MtlRef.SetSizeDirectly(NewSubMeshCnt);
}

void RISkinnedMesh::SetMaterialAsset(IMaterialAsset* InAsset, int32 MtlIdx)
{
	if (_MeshRef == nullptr)
	{
		return;
	}

	const int32 SubMeshCnt = _MeshRef->GetSubMeshCnt();
	if (SubMeshCnt <= MtlIdx)
	{
		return;
	}

	if (_IncludedRenderWorld == nullptr)
	{
		_MtlRef[MtlIdx] = InAsset;
		return;
	}

	AssetInstanceReferencer ThisAssetRef;
	ThisAssetRef.Type = EAssetInstanceReferenceType::ObjectHashCode;
	ThisAssetRef.ObjHashCode = _GameObjectHashCode;

	_MtlRef[MtlIdx]->RemoveAssetReference(ThisAssetRef);
	_MtlRef[MtlIdx] = InAsset;
	InAsset->AddAssetReference(ThisAssetRef);
}

const SS::PooledList<SBASkinningJointMatrix>& RISkinnedMesh::GetSkeletonPose() const
{
	return _SkeletonPose;
}

void RISkinnedMesh::UpdateSkeletonPose(int32 BoneIdx, const XMMATRIX& WMatrix, const XMMATRIX& RotMatrix)
{
	if (BoneIdx >= _SkeletonPose.GetSize())
	{
		_SkeletonPose.Resize(BoneIdx + 1);
	}

	_SkeletonPose[BoneIdx].WMatrix = WMatrix;
	_SkeletonPose[BoneIdx].RotMatrix = RotMatrix;
}
