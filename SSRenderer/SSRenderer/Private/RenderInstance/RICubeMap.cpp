#include "RICubeMap.h"

#include "SSGAL/Public/GALRenderInstance/GALRIMetadata.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/ITextureAsset.h"

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

void RICubeMap::InjectGALMetadataXXX(GALRIMetadata* MetadataToHandover)
{
	_MetaData = MetadataToHandover;
}

GALRIMetadata* RICubeMap::GetGALMetadata() const
{
	return _MetaData;
}

void RICubeMap::ReleaseGALMetaData()
{
	delete _MetaData;
	_MetaData = nullptr;
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
