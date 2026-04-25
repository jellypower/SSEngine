#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SRenderComponentBase.h"

#include <SSEngineDefault/Public/RawProfiler/IFrameInfoProcessor.h>
#include <SSRenderer/Public/RenderBase/IRenderWorld.h>

#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/ContentBase/SWorld.h"
#include "SSRenderer/Public/RenderInstance/IRenderInstance.h"


void SRenderComponentBase::PostConstructHierarchy()
{
	ConstructRenderInstance();
}

void SRenderComponentBase::OnEnterTheWorld()
{
	SWorld* IncludedWorld = GetIncludedWorld();
	IRenderWorld* RenderWorld = IncludedWorld->GetRenderWorld();

	RenderWorld->AddToWorld(GetRenderInstance());
}

void SRenderComponentBase::OnExitTheWorld()
{
	SWorld* IncludedWorld = GetIncludedWorld();
	IRenderWorld* RenderWorld = IncludedWorld->GetRenderWorld();

	IRenderInstance* RenderInstance = GetRenderInstance();
	SObjHashCode GOID = RenderInstance->GetGameObjectID();
	RenderWorld->RemoveRenderInstanceFromWorld(GOID);
}

void SRenderComponentBase::PreDestructHierarchy()
{
	DestructRenderInstance();
}

void SRenderComponentBase::OnGameObjectTransformCommited(EFramePhase CommitPhase)
{
	SGameObject* Owner = GetGameObject();
	_RenderInstance->SetWorldTransformMatrix(Owner->GetCommittedWorldTransformMat());
	_RenderInstance->SetWorldRotation(Owner->GetCommittedWorldRotation());
}
