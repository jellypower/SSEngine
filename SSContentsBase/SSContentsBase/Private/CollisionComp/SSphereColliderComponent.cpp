#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/CollisionComp/SSphereColliderComponent.h"

#include "SSCollision/Public/CollInstance/ICISphere.h"
#include "SSCollision/Public/CollisionBase/ICollDevice.h"
#include "SSCollision/Public/ModuleEntry/SSCollisionGlobalVariableSet.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/ICommonRenderAssetSet.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/SRenderContent/_DEBUG/SRenderDebugUtil.h"

void SSphereColliderComponent::SetRadius(float InRadius)
{
	_CollInstance->SetRadius(InRadius);
}

bool SSphereColliderComponent::ShouldProcessPerFrameInherently() const
{
	return true;
}

void SSphereColliderComponent::PerFrame(float DeltaTime)
{
	IMeshAsset* Sphere = g_Renderer->GetCommonRenderAssetSet()->GetSphere1mMesh();

	SGameObject* OwnerGameObject = GetGameObject();
	SWorld* IncludedWorld = OwnerGameObject->GetIncludedWorldRef();
	const XMMATRIX& WorldTransformMat = OwnerGameObject->GetCommittedWorldTransformMat();

	float BiggestScale = SS::CalcBiggestScaleAxis(WorldTransformMat);
	float Radius = _CollInstance->GetRadius();

	Transform transform;

	transform.Scale = Vector4f( 2, 2, 2, 0 ) * (BiggestScale * Radius);
	transform.Position = WorldTransformMat.r[3];

	SRenderDebugUtil::DrawDebugMesh(
		IncludedWorld,
		transform,
		Sphere,
		true);
}

Vector4f SSphereColliderComponent::CalcFurthest(const Vector4f& Dir) const
{
	return _CollInstance->CalcFurthest(Dir);
}

ICollInstanceBase* SSphereColliderComponent::GetCollInstance() const
{
	return _CollInstance;
}

void SSphereColliderComponent::ConstructCollInstance()
{
	_CollInstance = g_CollDevice->CreateCollSphere();
	_CollInstance->SetGameObjectIDXXX(GetHashCode());
}

void SSphereColliderComponent::DestructCollInstance()
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
