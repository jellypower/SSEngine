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

	_DirectionalLight = g_Renderer->CreateDirectionalLight(_Desc);
	SGameObject* Parent = GetParent();
	_DirectionalLight->SetGameObjectIDXXX(Parent->GetHashCode());
}

void SRenderLightDirectionalComponent::OnEnterTheWorld()
{
	SGameObject* Parent = GetParent();
	SWorld* World = Parent->GetIncludedWorldRef();
	IRenderWorld* RenderWorld = World->GetRenderWorld();

	RenderWorld->AddToWorld(_DirectionalLight);
}

void SRenderLightDirectionalComponent::OnExitTheWorld()
{
	SGameObject* Parent = GetParent();
	SWorld* World = Parent->GetIncludedWorldRef();
	IRenderWorld* RenderWorld = World->GetRenderWorld();

	SObjHashCode GOID = Parent->GetHashCode();
	RenderWorld->RemoveRenderInstanceFromWorld(GOID);
}

void SRenderLightDirectionalComponent::PreDestructHierarchy()
{
	_DirectionalLight->ReleaseGALMetaData();
	delete _DirectionalLight;
	_DirectionalLight = nullptr;
}

void SRenderLightDirectionalComponent::OnGameObjectTransformCommited()
{
	SGameObject* Owner = GetParent();
	const Transform& WorldTransform = Owner->GetWorldTransform();
	_DirectionalLight->SetWorldRotation(WorldTransform.Rotation);
}
