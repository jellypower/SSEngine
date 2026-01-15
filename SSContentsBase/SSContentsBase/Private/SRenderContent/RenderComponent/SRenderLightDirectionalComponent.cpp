#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/SRenderContent/RenderComponent/SRenderLightDirectionalComponent.h"

#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/ContentBase/SWorld.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderBase/IRenderWorld.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSRenderer/Public/RenderInstance/Light/IRenderLightDirectional.h"
#include "SSRenderer/Public/RenderInstance/Descriptors/LightDesc.h"

void SRenderLightDirectionalComponent::PostConstructHierarchy()
{
	if (_Desc.ShadowMapSize.X <= 0 || _Desc.ShadowMapSize.Y <= 0)
	{
		_Desc.ShadowMapSize.X = 1024.f;
		_Desc.ShadowMapSize.Y = 1024.f;
	}

	_RenderLight = g_Renderer->CreateDirectionalLight(_Desc);
	SGameObject* Parent = GetGameObject();
	_RenderLight->SetGameObjectIDXXX(Parent->GetHashCode());
	_RenderLight->SetLightIntensity(GetLightIntensity().SimdVec);
}

void SRenderLightDirectionalComponent::OnEnterTheWorld()
{
	SGameObject* Parent = GetGameObject();
	SWorld* World = Parent->GetIncludedWorldRef();
	IRenderWorld* RenderWorld = World->GetRenderWorld();

	RenderWorld->AddToWorld(_RenderLight);
}

void SRenderLightDirectionalComponent::OnExitTheWorld()
{
	SGameObject* Parent = GetGameObject();
	SWorld* World = Parent->GetIncludedWorldRef();
	IRenderWorld* RenderWorld = World->GetRenderWorld();

	SObjHashCode GOID = Parent->GetHashCode();
	RenderWorld->RemoveRenderInstanceFromWorld(GOID);
}

void SRenderLightDirectionalComponent::PreDestructHierarchy()
{
	_RenderLight->ReleaseGALMetaData();
	delete _RenderLight;
	_RenderLight = nullptr;
}

void SRenderLightDirectionalComponent::OnGameObjectTransformCommited()
{
	SGameObject* Owner = GetGameObject();
	const Transform& WorldTransform = Owner->CalcWorldTransform();
	_RenderLight->SetWorldRotation(WorldTransform.Rotation);
}
