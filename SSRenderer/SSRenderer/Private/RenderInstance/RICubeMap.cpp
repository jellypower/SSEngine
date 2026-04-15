#include "RICubeMap.h"

#include <SSEngineDefault/Public/RawProfiler/SSFrameInfo.h>

#include "SSGAL/Public/GALRenderInstance/GALRIMetadata.h"
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/ITextureAsset.h"
#include "SSRenderer/Private/RenderBase/SSRenderer.h"
#include "SSRenderer/Public/RenderCommon/SSRenderUtilFuncs.h"

float RICubeMap::GetCubeMapSize() const
{
	return 50.f;
}

ITextureAsset* RICubeMap::GetCubemapTexture() const
{
	return _TextureAsset;
}

void RICubeMap::SetCubemapTexture(ITextureAsset* InAsset)
{
	if (_IncludedRenderWorld == nullptr)
	{
		_TextureAsset = InAsset;
		return;
	}

	AssetInstanceReferencer AssetReferencer;
	AssetReferencer.Type = EAssetInstanceReferenceType::ObjectHashCode;
	AssetReferencer.ObjHashCode = _OwnerHashCode;


	_TextureAsset->RemoveAssetReference(AssetReferencer);
	InAsset->AddAssetReference(AssetReferencer);
	_TextureAsset = InAsset;
}



SObjHashCode RICubeMap::GetGameObjectID() const
{
	return _OwnerHashCode;
}

void RICubeMap::SetGameObjectIDXXX(SObjHashCode InHashCode)
{
	_OwnerHashCode = InHashCode;
}

ERenderInstanceType RICubeMap::GetRIType() const
{
	return ERenderInstanceType::CubeMap;
}

const XMMATRIX& RICubeMap::GetWorldTransformMatrix() const
{
	static Transform CUBEMAP_TRANSFORM;
	const float CubeMapSize = GetCubeMapSize();
	CUBEMAP_TRANSFORM.Scale = Vector4f(CubeMapSize, CubeMapSize, CubeMapSize, 0);

	return CUBEMAP_TRANSFORM.AsMatrix();
}

const XMMATRIX& RICubeMap::GetWorldRotationMatrix() const
{
	SS_ASSERT(false, L"Should not be called.");
	return XMMatrixIdentity();
}

void RICubeMap::SetWorldTransformMatrix(const XMMATRIX& InMatrix)
{
	// noop
}

void RICubeMap::SetWorldRotation(const Quaternion& InRotation)
{
	// noop
}

void RICubeMap::InjectGALMetadataXXX(GALRIMetadata* MetadataToHandover, int32 FrameMod)
{
	if (_MetaData[FrameMod] != nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	if (MetadataToHandover->GetMetadataRenderInstanceType() != ERenderInstanceType::CubeMap)
	{
		SS_INTERRUPT();
		return;
	}

	_MetaData[FrameMod] = MetadataToHandover;
}

GALRIMetadata* RICubeMap::GetGALMetadata(int32 FrameMod) const
{
	return _MetaData[FrameMod];
}

void RICubeMap::ReleaseGALMetaData()
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
			SS_ASSERT(false);
			continue;
		}

		const int32 DestroyDelay = 
			(ItemIdx + GAL_NESTED_FRAME_CNT) % // 중첩된 프레임 뒤에 지운다.
			DEFERRED_DESTROY_MOD; // 위 값도 리밋을 넘을 수 있으니까 모듈러 한 번 더 해줌.

		g_Renderer->ReserveDestory(_MetaData[ItemIdx], DestroyDelay);
		_MetaData[ItemIdx] = nullptr;
	}
}

void RICubeMap::OnEnterTheRenderWorldXXX(IRenderWorld* InRenderWorld)
{
	if (InRenderWorld == nullptr || _IncludedRenderWorld != nullptr)
	{
		SS_INTERRUPT();
	}

	_IncludedRenderWorld = InRenderWorld;

	AssetInstanceReferencer AssetReferencer;
	AssetReferencer.Type = EAssetInstanceReferenceType::ObjectHashCode;
	AssetReferencer.ObjHashCode = _OwnerHashCode;
	_TextureAsset->AddAssetReference(AssetReferencer);
}

void RICubeMap::OnExitFromRenderWorldXXX()
{
	if (_IncludedRenderWorld == nullptr)
	{
		SS_INTERRUPT();
	}
	_IncludedRenderWorld = nullptr;

	AssetInstanceReferencer AssetReferencer;
	AssetReferencer.Type = EAssetInstanceReferenceType::ObjectHashCode;
	AssetReferencer.ObjHashCode = _OwnerHashCode;
	_TextureAsset->RemoveAssetReference(AssetReferencer);
}

IRenderWorld* RICubeMap::GetIncludedRenderWorld() const
{
	return _IncludedRenderWorld;
}
