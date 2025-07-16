#include "RenderWorld.h"

#include "SSGAL/Public/SSGALCommonEnums.h"
#include "SSGAL/Public/GALRenderInstance/GALRWMetaData.h"

#include "SSRenderer.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IAssetBase.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IModelAsset.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSRenderer/Public/RenderInstance/IRIMesh.h"


RenderWorld::RenderWorld(const utf16* InWorldName) :
	_RenderInstanceByHashCode(RENDERWORLD_HASHMAP_SIZE, RENDERWORLD_BUCKET_CAPACITY)
{
	_RenderWorldName = InWorldName;
}

RenderWorld::~RenderWorld()
{
	SS_ASSERT(IsAnyInstanceRemainInWorld() == false);

	delete _GALMetadata;
	_GALMetadata = nullptr;
}

void RenderWorld::InitializeRenderWorld(IRenderer* OwnerRenderer)
{
	_OwnerRenderer = OwnerRenderer;
}

bool RenderWorld::IsAnyInstanceRemainInWorld() const
{
	return _RenderInstanceByHashCode.GetCnt() != 0;
}

SS::SHasherW RenderWorld::GetWorldName() const
{
	return _RenderWorldName;
}

void RenderWorld::AddToWorld(IRenderInstance* InRenderInstance) // TODO: RenderInstance에게 본인이 속한 RenderWorld 설정해주는 기능 만들기
{
	SObjHashCode GameObjectHashCode = InRenderInstance->GetGameObjectID();
	if (_RenderInstanceByHashCode.Find(GameObjectHashCode) != nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	_RenderInstanceByHashCode.Add(GameObjectHashCode, InRenderInstance);
	InRenderInstance->SetIncludedRenderWorldXXX(this);


	switch (InRenderInstance->GetRIType())
	{
	case ERenderInstanceType::StaticMesh:
	{
		IRIMesh* InRIMesh = (IRIMesh*)InRenderInstance;

		AssetInstanceReferencer AssetReferencer;
		AssetReferencer.Type = EAssetInstanceReferenceType::ObjectHashCode;
		AssetReferencer.ObjHashCode = GameObjectHashCode;

		IModelAsset* ModelAsest = InRIMesh->GetModelAsset();
		ModelAsest->AddAssetReference(AssetReferencer);
	}
	break;
	}
}


void RenderWorld::RemoveFromWorld(SObjHashCode RenderInstanceIDToRemove)
{
	IRenderInstance** ppRenderInstance = _RenderInstanceByHashCode.Find(RenderInstanceIDToRemove);
	if (ppRenderInstance == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	IRenderInstance* RenderInstanceToRemove = *ppRenderInstance;
	if (RenderInstanceToRemove == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	_RenderInstanceByHashCode.Remove(RenderInstanceIDToRemove);
	RenderInstanceToRemove->SetIncludedRenderWorldXXX(nullptr);


	switch (RenderInstanceToRemove->GetRIType())
	{
	case ERenderInstanceType::StaticMesh:
	{
		IRIMesh* RIMeshToRemove = (IRIMesh*)RenderInstanceToRemove;

		AssetInstanceReferencer AssetReferencer;
		AssetReferencer.Type = EAssetInstanceReferenceType::ObjectHashCode;
		AssetReferencer.ObjHashCode = RIMeshToRemove->GetGameObjectID();

		IModelAsset* ModelAsset = RIMeshToRemove->GetModelAsset();
		ModelAsset->RemoveAssetReference(AssetReferencer);
	}
	break;

	default:
	{
		SS_ASSERT(false);
		break;
	}
	}
}

GALRWMetaData* RenderWorld::GetGALMetadata() const
{
	return _GALMetadata;
}

void RenderWorld::InjectGALMetadataXXX(GALRWMetaData* InMetadata)
{
	_GALMetadata = InMetadata;
}