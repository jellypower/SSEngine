#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/CollisionComp/SBoxColliderComponent.h"

#include "SSCollision/Public/CollInstance/ICIBox.h"
#include "SSCollision/Public/CollInstance/ICollInstanceBase.h"
#include "SSCollision/Public/CollisionBase/ICollDevice.h"
#include "SSCollision/Public/ModuleEntry/SSCollisionGlobalVariableSet.h"

#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/SRenderContent/_DEBUG/SRenderDebugUtil.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/ICommonRenderAssetSet.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"


void SBoxColliderComponent::SetExtent(const Vector4f& Extent)
{
	_CollInstance->SetExtent(Extent);
}

bool SBoxColliderComponent::ShouldProcessPerFrameInherently() const
{
	return false;
}

void SBoxColliderComponent::PerFrame(float DeltaTime)
{
	IMeshAsset* Cube = g_Renderer->GetCommonRenderAssetSet()->GetCube1mMesh();
	IMeshAsset* Sphere = g_Renderer->GetCommonRenderAssetSet()->GetSphere1mMesh();

	SGameObject* OwnerGameObject = GetGameObject();
	XMMATRIX WorldTransformMat = OwnerGameObject->GetCommittedWorldTransformMat();
	Quaternion WorldRot = OwnerGameObject->GetCommittedWorldRotation();
	SWorld* IncludedWorld = OwnerGameObject->GetIncludedWorldRef();



	SRenderDebugUtil::DrawDebugMesh(
		IncludedWorld,
		WorldTransformMat,
		WorldRot.AsMatrix(),
		Cube,
		true);
}

Vector4f SBoxColliderComponent::CalcFurthest(const Vector4f& Dir) const
{
	return _CollInstance->CalcFurthest(Dir);
}

ICollInstanceBase* SBoxColliderComponent::GetCollInstance() const
{
	return _CollInstance;
}

void SBoxColliderComponent::ConstructCollInstance()
{
	_CollInstance = g_CollDevice->CreateCollBox();
	_CollInstance->SetGameObjectIDXXX(GetHashCode());
}

void SBoxColliderComponent::DestructCollInstance()
{
	if (_CollInstance == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	if (GetIncludedWorld() != nullptr)
	{
		SS_INTERRUPT(); // Must be removed from world before destruct.
		return;
	}

	delete _CollInstance;
}
