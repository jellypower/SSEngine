#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/CollisionComp/SBoxColliderComponent.h"


#include "SSCollision/Public/CollInstance/ICIBox.h"
#include "SSCollision/Public/CollInstance/ICollInstanceBase.h"
#include "SSCollision/Public/CollInstance/CICreationDesc.h"
#include "SSCollision/Public/CollisionBase/ICollDevice.h"
#include "SSCollision/Public/ModuleEntry/SSCollisionGlobalVariableSet.h"

#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/SRenderContent/_DEBUG/SRenderDebugUtil.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/ICommonRenderAssetSet.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"


void SBoxColliderComponent::SetExtent(const Vector4f& Extent)
{
	_Extent = Extent;
	if (_CollInstance != nullptr)
	{
		_CollInstance->SetExtent(Extent);
	}
}

bool SBoxColliderComponent::ShouldProcessPerFrameInherently() const
{
	return false;
}

void SBoxColliderComponent::PerFrame(float DeltaTime)
{
	IMeshAsset* Cube = g_Renderer->GetCommonRenderAssetSet()->GetCube1mMesh();

	SGameObject* OwnerGameObject = GetGameObject();
	SWorld* IncludedWorld = OwnerGameObject->GetIncludedWorldRef();
	const XMMATRIX& WorldTransformMat = OwnerGameObject->GetCommittedWorldTransformMat();
	const Quaternion& WorldRot = OwnerGameObject->GetCommittedWorldRotation();
	const ICIBox* BoxCollider = static_cast<ICIBox*>(GetCollInstance());

	
	Transform DebugTransform;
	DebugTransform.Scale = BoxCollider->GetExtent() * 2;
	DebugTransform.Position = GetOffset();
	XMMATRIX DebugDrawExtent = DebugTransform.AsMatrix();
	DebugDrawExtent = DebugDrawExtent * WorldTransformMat;

	SRenderDebugUtil::DrawDebugMesh(
		IncludedWorld,
		DebugDrawExtent,
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
	CI_BOX_DESC Desc;
	Desc.InitialLclTransform = GetGameObject()->GetTransform();
	Desc.Offset = GetOffset();
	Desc.ComponentID = GetHashCode();
	Desc.Extent = _Extent;

	_CollInstance = g_CollDevice->CreateCollBox(Desc);
	
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

	_CollInstance->Release();
}
