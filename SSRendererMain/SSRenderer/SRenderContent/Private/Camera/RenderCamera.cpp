#include "RenderCamera.h"

#include "SRenderContent/Public/Camera/SCameraComponent.h"
#include "SSContentsBase/SGameObject.h"
#include "SSContentsBase/SWorld.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

RenderCamera::RenderCamera(SCameraComponent* InOwnerCamera)
{
	_OwnerCamera = InOwnerCamera;
}

const IRenderWorld* RenderCamera::GetIcludedRenderWorld() const
{
	SGameObject* Parent = _OwnerCamera->GetParent();
	SWorld* World = Parent->GetIncludedWorldRef();
	if (World == nullptr)
	{
		DEBUG_BREAK();
		return nullptr;
	}

	return World->GetRenderWorld();
}

GALRenderTarget* RenderCamera::GetSpecificRenderTarget() const
{
	return _OwnerCamera->_RenderTarget;
}

const XMMATRIX& RenderCamera::GetVPMatrix() const
{
	return _OwnerCamera->GetVPMatrix();
}

const Transform& RenderCamera::GetCameraTransform() const
{
	SGameObject* GO = _OwnerCamera->GetParent();
	return GO->GetTransform();
}
