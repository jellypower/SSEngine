#include "RenderWorld.h"

#include "BasicRenderInstance.h"
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

void RenderWorld::AddToWorld(BasicRenderInstance* RenderInstance)
{
	MeshAssetManager* MeshAssetManager = _OwnerRenderer->GetMeshAssetManager();

	SObjHashCode GameObjectHashCode = RenderInstance->_GameObjectHashCode;
	if (_RenderInstanceByHashCode.Find(GameObjectHashCode) != nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	_RenderInstanceByHashCode.Add(GameObjectHashCode, RenderInstance);

	AssetInstanceReferencer AssetReferencer;
	AssetReferencer.Type = EAssetInstanceReferenceType::ObjectHashCode;
	AssetReferencer.ObjHashCode = RenderInstance->_GameObjectHashCode;
	_OwnerRenderer->AddModelInstanceReference(RenderInstance->_ModelRef, AssetReferencer);
}


void RenderWorld::RemoveFromWorld(SObjHashCode RenderInstanceToRemove)
{
	BasicRenderInstance** ppBasicRenderInstance = _RenderInstanceByHashCode.Find(RenderInstanceToRemove);
	if (ppBasicRenderInstance == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	BasicRenderInstance* RenderInstance = *ppBasicRenderInstance;
	if (RenderInstance == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	AssetInstanceReferencer AssetReferencer;
	AssetReferencer.Type = EAssetInstanceReferenceType::ObjectHashCode;
	AssetReferencer.ObjHashCode = RenderInstance->_GameObjectHashCode;
	_OwnerRenderer->RemoveModelInstanceReference(RenderInstance->_ModelRef, AssetReferencer);

	_RenderInstanceByHashCode.Remove(RenderInstanceToRemove);
}

void RenderWorld::CommitRenderInstanceTransform()
{

}
