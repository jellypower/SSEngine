#include "RenderLightDirectional.h"

#include "SSGAL/Public/GALRenderInstance/GALRIMetadata.h"

#include "SSGAL/Public/GALRenderTarget/GALRenderTarget.h"
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSRenderer/Public/RenderBase/IRenderWorld.h"
#include "SSRenderer/Public/RenderInstance/IRenderCamera.h"


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
	IRenderer* Renderer = _IncludedRenderWorld->GetOwnerRenderer();
	const IRenderCamera* RenderCam = Renderer->GetMainRenderCamera();
	const Transform& CamTransform = RenderCam->GetCameraTransform();
	float CamFOV = RenderCam->GetFOVWithRadians();
	float CamNearZ = RenderCam->GetNearZ();
	float CamFarZ = RenderCam->GetFarZ();
	float AspectRatio = RenderCam->GetAspectRatio();
	float CamPlaneCenterDist = (CamNearZ + CamFarZ) / 2.f;
	XMVECTOR CamForward = CamTransform.GetForward().SimdVec;
	XMVECTOR CamFrustumCenter = CamTransform.Position.SimdVec + (CamForward * CamPlaneCenterDist);
	


	// 카메라의 Frustum을 감싸는 직육면체의 크기
	float FarHeight = tan(CamFOV / 2) * CamFarZ * 2;
	float FarWidth = FarHeight * AspectRatio;
	float CamPlaneDepth = CamFarZ - CamNearZ;

	// 카메라 직육면체의 대각선 길이
	float CubiodMaxDist = FarHeight * FarHeight + FarWidth * FarHeight + CamPlaneDepth * CamPlaneDepth;
	CubiodMaxDist = sqrt(CubiodMaxDist);

	static const XMVECTOR DOWN_VECTOR = { 0, -1, 0 ,0 };
	static const XMVECTOR FORWARD_VECTOR = { 0, 0, 1, 0 };
	constexpr float SHADOWMAP_VIEWPOS_DIST = 10;
	XMVECTOR ShadowMapViewDir = XMVector4Transform(DOWN_VECTOR, _WorldRotationMatrix);
	XMVECTOR ShadowMapUpDir = XMVector4Transform(FORWARD_VECTOR, _WorldRotationMatrix);
	XMVECTOR ShadowMapViewPos = CamFrustumCenter - ShadowMapViewDir * CubiodMaxDist / 2;




	XMMATRIX ViewMat = XMMatrixLookToLH(
		ShadowMapViewPos,
		ShadowMapViewDir,
		ShadowMapUpDir);

	//XMMATRIX ProjMat2 = XMMatrixParallelLH(3, 3, 1.f, -1.f);
	// XMMATRIX ProjMat2 = XMMatrixPerspectiveFovLH(XM_PIDIV2, 1, 0.001, 1);
	XMMATRIX ProjMat2  = XMMatrixOrthographicLH(CubiodMaxDist / 2, CubiodMaxDist / 2, 0.001, CubiodMaxDist);
	XMMATRIX Result = ViewMat * ProjMat2;
	return Result;
}