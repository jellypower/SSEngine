#include "RenderLightDirectional.h"

#include "SSGAL/Public/GALRenderInstance/GALRIMetadata.h"

RenderLightDirectional::RenderLightDirectional(const RenderLightDirectionalDesc& InDesc)
{
	_Desc = InDesc;
}

SObjHashCode RenderLightDirectional::GetGameObjectID() const
{
	return _GameObjectHashCode;
}

void RenderLightDirectional::SetGameObjectIDXXX(SObjHashCode InHashCode)
{
	_GameObjectHashCode = InHashCode;
}

ERenderInstanceType RenderLightDirectional::GetRIType() const
{
	return ERenderInstanceType::Light;
}

const XMMATRIX& RenderLightDirectional::GetWorldTransformMatrix() const
{
	return _WorldTransformMatrix;
}

const XMMATRIX& RenderLightDirectional::GetWorldRotationMatrix() const
{
	return _WorldRotationMatrix;
}

void RenderLightDirectional::SetWorldTransformMatrix(const XMMATRIX& InMatrix)
{
	_WorldTransformMatrix = InMatrix;
}

void RenderLightDirectional::SetWorldRotation(const Quaternion& InRotation)
{
	_WorldRotationMatrix = InRotation.AsMatrix();
}

void RenderLightDirectional::InjectGALMetadataXXX(GALRIMetadata* MetadataToHandover)
{
	SS_ASSERT_MSG(false, L"Metadata를 생성할 필요가 없습니다. RenderWorld의 Metadata를 공유합니다.");
}

const GALRIMetadata* RenderLightDirectional::GetGALMetadata() const
{
	SS_ASSERT_MSG(false, L"Metadata를 생성할 필요가 없습니다. RenderWorld의 Metadata를 공유합니다.");
	return nullptr;
}

void RenderLightDirectional::ReleaseGALMetaData()
{
	SS_ASSERT_MSG(false, L"Metadata를 생성할 필요가 없습니다. RenderWorld의 Metadata를 공유합니다.");
}

void RenderLightDirectional::SetIncludedRenderWorldXXX(IRenderWorld* InRenderWorld)
{
	SS_ASSERT(InRenderWorld == nullptr || _IncludedRenderWorld == nullptr);
	_IncludedRenderWorld = InRenderWorld;
}

IRenderWorld* RenderLightDirectional::GetIncludedRenderWorld() const
{
	return _IncludedRenderWorld;
}

ELightType RenderLightDirectional::GetLightType() const
{
	return ELightType::Directional;
}

bool RenderLightDirectional::IsShadowMapEnabled() const
{
	return _Desc.bEnableShadowMap;
}

void RenderLightDirectional::SetEnableShadowMap(bool bEnable)
{
	_Desc.bEnableShadowMap = bEnable;

	if (bEnable == false)
	{
		ReleaseShadowMap();
	}
}

XMMATRIX RenderLightDirectional::CalcShadowMapVPMatrix() const
{
	XMVECTOR LightDir = {0,0,1,1};
	LightDir = XMVector4Transform(LightDir, _WorldRotationMatrix);
}
