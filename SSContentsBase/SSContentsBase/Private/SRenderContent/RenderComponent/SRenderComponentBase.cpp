#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SRenderComponentBase.h"

#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSRenderer/Public/RenderInstance/IRenderInstance.h"

void SRenderComponentBase::SetModelAsset(SS::SHasherW ModelAssetName)
{
	_ModelAssetName = ModelAssetName;
}


void SRenderComponentBase::PostConstructHierarchy()
{
	ConstructRenderInstance();
}

void SRenderComponentBase::OnEnterTheWorld()
{
}

void SRenderComponentBase::OnExitTheWorld()
{
}

void SRenderComponentBase::PreDestructHierarchy()
{
	DestructRenderInstance();
}

void SRenderComponentBase::OnGameObjectTransformCommited()
{
	SGameObject* Owner = GetGameObject();
	_RenderInstance->SetWorldTransformMatrix(Owner->GetCommittedWorldTransformMat());
	_RenderInstance->SetWorldRotation(Owner->GetCommittedWorldRotation());
}
