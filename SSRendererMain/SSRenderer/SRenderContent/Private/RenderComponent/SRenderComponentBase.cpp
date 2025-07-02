#include "SRenderContent/Public/RenderComponent/SRenderComponentBase.h"

#include "SSContentsBase/SGameObject.h"
#include "SSGAL/Public/GALRenderDevice/GALRenderDeviceContext.h"
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
	SGameObject* Owner = GetParent();
	_RenderInstance->SetWorldTransformMatrix(Owner->GetCommittedWorldTransformMat());
	_RenderInstance->SetWorldRotation(Owner->GetCommittedWorldRotation());
}
