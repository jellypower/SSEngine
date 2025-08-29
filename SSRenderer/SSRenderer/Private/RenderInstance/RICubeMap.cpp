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
	_TEMP_PrevTextureAsset = _TextureAsset;
	_TextureAsset = InAsset;
}

void RICubeMap::SyncCubeMapTexture() // TODO: 함수 없애기
{
	if (_TEMP_PrevTextureAsset != _TextureAsset)
	{
		AssetInstanceReferencer ThisAssetRef;
		ThisAssetRef.Type = EAssetInstanceReferenceType::ObjectHashCode;
		ThisAssetRef.ObjHashCode = _OwnerHashCode;

		if (_TEMP_PrevTextureAsset != nullptr)
		{
			_TEMP_PrevTextureAsset->RemoveAssetReference(ThisAssetRef);
		}

		if (_TextureAsset != nullptr)
		{
			_TextureAsset->AddAssetReference(ThisAssetRef);
		}
	}
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

void RICubeMap::SetIncludedRenderWorldXXX(IRenderWorld* InRenderWorld)
{
	_IncludedRenderWorld = InRenderWorld;
}

IRenderWorld* RICubeMap::GetIncludedRenderWorld() const
{
	return _IncludedRenderWorld;
}
