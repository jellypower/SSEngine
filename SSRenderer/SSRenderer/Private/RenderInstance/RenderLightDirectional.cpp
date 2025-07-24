#include "RenderLightDirectional.h"

#include "SSGAL/Public/GALRenderTarget/GALRenderTarget.h"


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
	return XMMatrixIdentity();
}

const XMMATRIX& RenderLightDirectional::GetWorldRotationMatrix() const
{
	return _WorldRotationMatrix;
}

void RenderLightDirectional::SetWorldTransformMatrix(const XMMATRIX& InMatrix)
{
	// DirectionalLight는 Rotation을 제외한 Transform이 필요없음
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

const RenderLightDirectionalDesc& RenderLightDirectional::GetDirectionalLightDesc()
{
	return _Desc;
}

XMVECTOR RenderLightDirectional::CalcDirectionalLightDirection() const
{
	XMVECTOR LightDir = {0,1,0,1}; // UpDirection
	LightDir = XMVector4Transform(LightDir, _WorldRotationMatrix);

	return LightDir;
}

void RenderLightDirectional::InjectShadowMapXXX(GALRenderTarget* ShadowMapToHandover)
{
	if (_ShadowMap != nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	_ShadowMap = ShadowMapToHandover;
}

GALRenderTarget* RenderLightDirectional::GetShadowMap() const
{
	return _ShadowMap;
}

void RenderLightDirectional::ReleaseShadowMap()
{
	if (_ShadowMap == nullptr)
	{
		return;
	}

	delete _ShadowMap;
	_ShadowMap = nullptr;

}
