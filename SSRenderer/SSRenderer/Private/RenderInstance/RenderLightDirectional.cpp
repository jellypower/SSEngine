#include "RenderLightDirectional.h"

#include <SSGAL/Public/GALRenderInstance/GALRIMetadata.h>

#include "SSGAL/Public/GALRenderTarget/GALRenderTarget.h"
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"


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
	if (MetadataToHandover->GetMetadataRenderInstanceType() != ERenderInstanceType::Light)
	{
		SS_ASSERT(false);
		return;
	}

	_ShadowMapMetaData = MetadataToHandover;
}

GALRIMetadata* RenderLightDirectional::GetGALMetadata() const
{
	return _ShadowMapMetaData;
}

void RenderLightDirectional::ReleaseGALMetaData()
{
	if (_ShadowMapMetaData == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	delete _ShadowMapMetaData;
	_ShadowMapMetaData = nullptr;
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
		ReleaseGALMetaData();
	}
}

const RenderLightDirectionalDesc& RenderLightDirectional::GetDirectionalLightDesc() const
{
	return _Desc;
}

XMVECTOR RenderLightDirectional::CalcDirectionalLightDirection() const
{
	XMVECTOR LightDir = {0,1,0,1}; // UpDirection
	LightDir = XMVector4Transform(LightDir, _WorldRotationMatrix);

	return LightDir;
}

XMMATRIX RenderLightDirectional::CalcShadowMapVPMatrix() const
{
	constexpr float WORLD_BOUNDARY_RADIUS = 100000000;

	static const XMVECTOR UP_VECTOR = { 0, 1, 0 ,1 };
	XMVECTOR ShadowMapCamPos = { 0,1,0,1 }; // UpDirection
	ShadowMapCamPos = XMVector4Transform(ShadowMapCamPos, _WorldRotationMatrix);

	XMMATRIX ViewMat = XMMatrixLookToLH(
		ShadowMapCamPos * WORLD_BOUNDARY_RADIUS,
		-ShadowMapCamPos,
		UP_VECTOR);


	XMMATRIX ProjMat = XMMatrixOrthographicLH(_Desc.ShadowMapSize.X, _Desc.ShadowMapSize.X, 0.001, WORLD_BOUNDARY_RADIUS);

	return ViewMat * ProjMat;
}