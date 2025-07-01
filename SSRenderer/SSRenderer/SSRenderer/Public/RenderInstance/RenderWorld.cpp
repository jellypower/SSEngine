#include "RenderWorld.h"

#include "IRenderInstance.h"
#include "IRIMesh.h"
#include "SSGAL/Public/SSGALCommonEnums.h"
#include "SSRenderer/Public/RenderAsset/MeshAssetManager.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/ModelAsset.h"
#include "SSRenderer/Public/RenderBase/SSRenderer.h"
#include "SSRenderer/Public/SObjectBase/SRenderComponentBase.h"

RenderWorld::RenderWorld() :
	_RenderInstanceByHashCode(RENDERWORLD_HASHMAP_SIZE, RENDERWORLD_BUCKET_CAPACITY)
{

}

void RenderWorld::InitializeRenderWorld(SSRenderer* OwnerRenderer)
{
	_OwnerRenderer = OwnerRenderer;
}

bool RenderWorld::IsAnyInstanceRemainInWorld() const
{
	return _RenderInstanceByHashCode.GetCnt() != 0;
}

void RenderWorld::AddToWorld(IRenderInstance* InRenderInstance)
{
	MeshAssetManager* MeshAssetManager = _OwnerRenderer->GetMeshAssetManager();

	SObjHashCode GameObjectHashCode = SObjHashCode(InRenderInstance->GetGameObjectIDNative());
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
		_OwnerRenderer->AddModelInstanceReference(InRIMesh->GetModelAsset(), AssetReferencer);
	}
	break;
	}
}


void RenderWorld::RemoveFromWorld(SObjHashCode RenderInstanceIDToRemove)
{
	IRenderInstance** ppBasicRenderInstance = _RenderInstanceByHashCode.Find(RenderInstanceIDToRemove);
	if (ppBasicRenderInstance == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	IRenderInstance* RenderInstanceToRemove = *ppBasicRenderInstance;
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
		SObjHashCode HashCode(RIMeshToRemove->GetGameObjectIDNative());
		AssetReferencer.ObjHashCode = HashCode;
		_OwnerRenderer->RemoveModelInstanceReference(RIMeshToRemove->GetModelAsset(), AssetReferencer);

		_RenderInstanceByHashCode.Remove(RenderInstanceIDToRemove);
	}
	break;
	}
}

void RenderWorld::CommitRenderInstanceTransform()
{

}
