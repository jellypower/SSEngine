#include "RenderLightDirectional.h"

#include <SSEngineDefault/Public/RawProfiler/SSFrameInfo.h>

#include "SSGAL/Public/GALRenderInstance/GALRIMetadata.h"

#include "SSGAL/Public/GALRenderTarget/GALRenderTarget.h"
#include "SSRenderer/Private/RenderBase/SSRenderer.h"
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSRenderer/Public/RenderBase/IRenderWorld.h"
#include "SSRenderer/Public/RenderCommon/SSRenderUtilFuncs.h"
#include "SSRenderer/Public/RenderInstance/IRenderCamera.h"


RenderLightDirectional::RenderLightDirectional(const RenderLightDirectionalDesc& InDesc)
{
	_Desc = InDesc;
	_LightIntensity = { 1,1,1,1 };
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

void RenderLightDirectional::InjectGALMetadataXXX(GALRIMetadata* MetadataToHandover, int32 FrameMod)
{
	if (_ShadowMapMetaData[FrameMod] != nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	if (MetadataToHandover->GetMetadataRenderInstanceType() != ERenderInstanceType::Light)
	{
		SS_ASSERT(false);
		return;
	}

	_ShadowMapMetaData[FrameMod] = MetadataToHandover;
}

GALRIMetadata* RenderLightDirectional::GetGALMetadata(int32 FrameMod) const
{
	return _ShadowMapMetaData[FrameMod];
}

void RenderLightDirectional::ReleaseGALMetaData()
{
	int32 CurFrameMod = RenderFrameInfo::GetFrameMod();

	for (int32 Offset = 0; Offset < GAL_NESTED_FRAME_CNT; Offset++)
	{
		const int32 ItemIdx =
			(CurFrameMod - Offset // CurFrameMod가 N이라고 하면 N-1번째 아이템은 CurFrame-1번째에 사용했던 녀석
				+ GAL_NESTED_FRAME_CNT) // CurFrameMod - Offset 값이 0보다 작을 수 있기 때문에 더해줌
			% GAL_NESTED_FRAME_CNT; // 그리고 다시 나눠줌

		if (_ShadowMapMetaData[ItemIdx] == nullptr)
		{
			continue;
		}

		const int32 DestroyDelay =
			(ItemIdx + GAL_NESTED_FRAME_CNT) % // 중첩된 프레임 뒤에 지운다.
			DEFERRED_DESTROY_MOD; // 위 값도 리밋을 넘을 수 있으니까 모듈러 한 번 더 해줌.

		g_Renderer->ReserveDestory(_ShadowMapMetaData[ItemIdx], DestroyDelay);
		_ShadowMapMetaData[ItemIdx] = nullptr;
	}
}

void RenderLightDirectional::OnEnterTheRenderWorldXXX(IRenderWorld* InRenderWorld)
{
	SS_ASSERT(InRenderWorld != nullptr && _IncludedRenderWorld == nullptr);
	_IncludedRenderWorld = InRenderWorld;
}

void RenderLightDirectional::OnExitFromRenderWorldXXX()
{
	SS_ASSERT(_IncludedRenderWorld != nullptr);
	_IncludedRenderWorld = nullptr;
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

XMVECTOR RenderLightDirectional::GetLightColor() const
{
	return _LightIntensity;
}

void RenderLightDirectional::SetLightIntensity(const XMVECTOR& InLightIntensity)
{
	_LightIntensity = InLightIntensity;
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

XMMATRIX RenderLightDirectional::CalcShadowMapVPMatrix(const IRenderCamera* CameraToUseShadowMap) const
{
	const Transform& CamTransform = CameraToUseShadowMap->GetCameraTransform();
	float CamFOV = CameraToUseShadowMap->GetFOVWithRadians();
	float CamNearZ = CameraToUseShadowMap->GetNearZ();
	float CamFarZ = CameraToUseShadowMap->GetFarZ();
	float AspectRatio = CameraToUseShadowMap->GetAspectRatio();
	float CamPlaneCenterDist = (CamNearZ + CamFarZ) / 2.f;
	XMVECTOR CamForward = CamTransform.GetForward().SimdVec;
	XMVECTOR CamFrustumCenter = CamTransform.Position.SimdVec + (CamForward * CamPlaneCenterDist);
	


	// 카메라의 Frustum을 감싸는 직육면체의 크기
	float FarHeight = tan(CamFOV / 2) * CamFarZ * 2;
	float FarWidth = FarHeight * AspectRatio;
	float CamPlaneDepth = CamFarZ - CamNearZ;

	// 카메라 직육면체의 대각선 길이
	float CubiodMaxDist = FarHeight * FarHeight + FarWidth * FarWidth + CamPlaneDepth * CamPlaneDepth;
	CubiodMaxDist = sqrt(CubiodMaxDist);

	// Light는 기본적으로 Down을 바라보고 위쪽은 Z+다.
	static constexpr XMVECTOR DOWN_VECTOR = { 0, -1, 0 ,0 };
	static constexpr XMVECTOR FORWARD_VECTOR = { 0, 0, 1, 0 };
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