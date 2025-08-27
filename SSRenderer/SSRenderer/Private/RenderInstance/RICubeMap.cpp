#include "RICubeMap.h"

#include "SSGAL/Public/GALRenderInstance/GALRIMetadata.h"

ITextureAsset* RICubeMap::GetCubemapTexture() const
{
	return _TextureAsset;
}

void RICubeMap::SetCubemapTexture(ITextureAsset* InAsset)
{
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
	CUBEMAP_TRANSFORM.Scale = Vector4f(50, 50, 50, 0);

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

void RICubeMap::SetIncludedRenderWorldXXX(IRenderWorld* InRenderWorld)
{
	_IncludedRenderWorld = InRenderWorld;
}

IRenderWorld* RICubeMap::GetIncludedRenderWorld() const
{
	return _IncludedRenderWorld;
}
