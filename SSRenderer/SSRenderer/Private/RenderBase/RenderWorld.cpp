#include "RenderWorld.h"


#include "SSRenderer.h"
#include "SSGAL/Public/SSGALCommonEnums.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IAssetBase.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSRenderer/Public/RenderInstance/IRIMesh.h"

RenderWorld::RenderWorld() :
	_RenderInstanceByHashCode(RENDERWORLD_HASHMAP_SIZE, RENDERWORLD_BUCKET_CAPACITY)
{

}

void RenderWorld::InitializeRenderWorld(IRenderer* OwnerRenderer)
{
	_OwnerRenderer = OwnerRenderer;
}

bool RenderWorld::IsAnyInstanceRemainInWorld() const
{
	return _RenderInstanceByHashCode.GetCnt() != 0;
}

void RenderWorld::AddToWorld(IRenderInstance* InRenderInstance)
{

	SObjHashCode GameObjectHashCode = InRenderInstance->GetGameObjectID();
	if (_RenderInstanceByHashCode.Find(GameObjectHashCode) != nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	_RenderInstanceByHashCode.Add(GameObjectHashCode, InRenderInstance);


	switch (InRenderInstance->GetRIType())
	{
	case ERenderInstanceType::StaticMesh:
	{
		IRIMesh* InRIMesh = (IRIMesh*)InRenderInstance;

		AssetInstanceReferencer AssetReferencer;
		AssetReferencer.Type = EAssetInstanceReferenceType::ObjectHashCode;
		AssetReferencer.ObjHashCode = GameObjectHashCode;

		SSRenderer* OwnerRenderer = (SSRenderer*)_OwnerRenderer;
		OwnerRenderer->AddModelInstanceReference(InRIMesh->GetModelAsset(), AssetReferencer);
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


	switch (RenderInstanceToRemove->GetRIType())
	{
	case ERenderInstanceType::StaticMesh:
	{
		IRIMesh* RIMeshToRemove = (IRIMesh*)RenderInstanceToRemove;

		AssetInstanceReferencer AssetReferencer;
		AssetReferencer.Type = EAssetInstanceReferenceType::ObjectHashCode;
		AssetReferencer.ObjHashCode = RIMeshToRemove->GetGameObjectID();

		SSRenderer* OwnerRenderer = (SSRenderer*)_OwnerRenderer;
		OwnerRenderer->RemoveModelInstanceReference(RIMeshToRemove->GetModelAsset(), AssetReferencer);
		_RenderInstanceByHashCode.Remove(RenderInstanceIDToRemove);
	}
	break;
	}
}