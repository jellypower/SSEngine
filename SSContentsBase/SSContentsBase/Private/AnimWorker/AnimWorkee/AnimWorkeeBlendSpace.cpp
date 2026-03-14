#include "AnimWorkeeBlendSpace.h"

#include "SSEngineDefault/Public/RawProfiler/ScopeProfMacro.h"
#include "SSEngineDefault/Public/RawProfiler/SSFrameInfo.h"

#include "SObject/Public/SObjHashT.h"
#include "SSContentsBase/Public/AnimComponents/SBlendSpaceAnimTestComponent.h"

#include "SSContentsBase/Public/AnimWorker/AnimBase/AnimateUtilFunctions.h"
#include "SSContentsBase/Public/ContentBase/SGameObject.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/IAssetManager.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IRenderAnimAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderKeyFrameAnimData/RenderAnimData.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

AnimWorkeeBlendSpace::AnimWorkeeBlendSpace(const SBlendSpaceAnimTestComponent* AnimComp)
	: _BindingIdxByName(400, 400)
{
	const SS::PooledList<SObjHashT<SGameObject>>& Bindings =
		AnimComp->GetBoneBindings();

	const int32 BindingCnt = Bindings.GetSize();

	{
//		SCOPE_PROFILE(AnimWorkeeBlendSpace::BindName);

		for (int32 i = 0; i < BindingCnt; i++)
		{
			SGameObject* GOItem = Bindings[i].Get();
			if (GOItem == nullptr)
			{
				SS_ASSERT(false);
				continue;
			}

			SS::SHasherW ObjName = GOItem->GetObjectName();
			_BindingIdxByName.Add(ObjName, i);
		}
	}

	static const SS::SHasherW HasherResult = L"Result";
	_ResultPose.PoseName = HasherResult;
	_ResultPose.BoneNames.Reserve(BindingCnt);
	_ResultPose.ParentBoneIdx.Reserve(BindingCnt);
	_ResultPose.BoneTransforms.Reserve(BindingCnt);

	{
//		SCOPE_PROFILE(AnimWorkeeBlendSpace::InitPose);

		for (int32 i = 0; i < BindingCnt; i++)
		{
			SGameObject* GOItem = Bindings[i].Get();
			SGameObject* GOParent = GOItem->GetParent();

			SS::SHasherW ParentName = GOParent->GetObjectName();
			SS::SHasherW GOName = GOItem->GetObjectName();
			const Transform& GOTransform = GOItem->GetTransform();

			int32 ParentIdx = INVALID_IDX;

			int32* FoundIdx = _BindingIdxByName.Find(ParentName);
			if (FoundIdx != nullptr)
			{
				ParentIdx = *FoundIdx;
			}

			_ResultPose.BoneNames.PushBack(GOName);
			_ResultPose.ParentBoneIdx.PushBack(ParentIdx);
			_ResultPose.BoneTransforms.PushBack(GOTransform);
		}
	}

	{
		// InitBlendPose


		// 우선은 SetSizeDirectly를 통해 쓰레기 값을 넣어준다.
		// UpdateAnimation에서 무조건 Write를 하고 나서 Read를 할 거기 때문에 문제 없을 확률이 높다.
		static const SS::SHasherW NAME_BlendPose1 = L"BlendPose0";
		_BlendPose0.PoseName = NAME_BlendPose1;
		_BlendPose0.BoneNames.SetSizeDirectly(BindingCnt);
		_BlendPose0.ParentBoneIdx.SetSizeDirectly(BindingCnt);
		_BlendPose0.BoneTransforms.SetSizeDirectly(BindingCnt);

		static const SS::SHasherW NAME_BlendPose2 = L"BlendPose1";
		_BlendPose1.PoseName = NAME_BlendPose2;
		_BlendPose1.BoneNames.SetSizeDirectly(BindingCnt);
		_BlendPose1.ParentBoneIdx.SetSizeDirectly(BindingCnt);
		_BlendPose1.BoneTransforms.SetSizeDirectly(BindingCnt);

		static const SS::SHasherW NAME_BlendPose3 = L"BlendPose2";
		_BlendPose2.PoseName = NAME_BlendPose3;
		_BlendPose2.BoneNames.SetSizeDirectly(BindingCnt);
		_BlendPose2.ParentBoneIdx.SetSizeDirectly(BindingCnt);
		_BlendPose2.BoneTransforms.SetSizeDirectly(BindingCnt);


		const int32 NameDataSize = sizeof(SS::SHasherW) * BindingCnt;
		memcpy_s(
			_BlendPose0.BoneNames.GetData(), NameDataSize,
			_ResultPose.BoneNames.GetData(), NameDataSize
		);

		memcpy_s(
			_BlendPose1.BoneNames.GetData(), NameDataSize,
			_ResultPose.BoneNames.GetData(), NameDataSize
		);

		memcpy_s(
			_BlendPose2.BoneNames.GetData(), NameDataSize,
			_ResultPose.BoneNames.GetData(), NameDataSize
		);

		// =====================

		const int32 ParentBoneIdxSize = sizeof(int32) * BindingCnt;
		memcpy_s(
			_BlendPose0.ParentBoneIdx.GetData(), ParentBoneIdxSize,
			_ResultPose.ParentBoneIdx.GetData(), ParentBoneIdxSize
		);

		memcpy_s(
			_BlendPose1.ParentBoneIdx.GetData(), ParentBoneIdxSize,
			_ResultPose.ParentBoneIdx.GetData(), ParentBoneIdxSize
		);

		memcpy_s(
			_BlendPose2.ParentBoneIdx.GetData(), ParentBoneIdxSize,
			_ResultPose.ParentBoneIdx.GetData(), ParentBoneIdxSize
		);


		// ======================

		const int32 TransformDataSize = sizeof(Transform) * BindingCnt;

		memcpy_s(
			_BlendPose0.BoneTransforms.GetData(), TransformDataSize,
			_ResultPose.BoneTransforms.GetData(), TransformDataSize
		);

		memcpy_s(
			_BlendPose1.BoneTransforms.GetData(), TransformDataSize,
			_ResultPose.BoneTransforms.GetData(), TransformDataSize
		);

		memcpy_s(
			_BlendPose2.BoneTransforms.GetData(), TransformDataSize,
			_ResultPose.BoneTransforms.GetData(), TransformDataSize
		);
	}
}

const PoseSlot& AnimWorkeeBlendSpace::GetResultPose() const
{
	return _ResultPose;
}

uint64 AnimWorkeeBlendSpace::GetLastUpdateFrame() const
{
	return _LastUpdateFrame;
}

bool AnimWorkeeBlendSpace::ShouldUpdateAnim() const
{
	return _bIsAnimPause == false || _bIsOneTimeUpdateRequested;
}

void AnimWorkeeBlendSpace::UpdateAnimation(float DeltaTime)
{
	IAssetManager* AssetManager = g_Renderer->GetAssetManager();


	float BPSqrLen = _BlendPoint.GetSqrLength();
	if (BPSqrLen < 0.0001f)
	{
		IRenderAnimAsset* FoundRenderAnimAsset = AssetManager->FindAssetByName<IRenderAnimAsset>(_RenderAnimAssetNames[static_cast<int32>(E8Dir::None)]);
		const RenderAnimRawData* AnimRawData = FoundRenderAnimAsset->GetKeyFrameAnimData();

		const float MainAnimDuration = AnimRawData->_Header.KeyFrameDuration;

		if (_bIsOneTimeUpdateRequested == false)
		{
			_WholeFrameTime += DeltaTime;
		}
		double TimeRatio = fmod(_WholeFrameTime, MainAnimDuration);
		TimeRatio = TimeRatio / MainAnimDuration;

		EvaluatePose(_ResultPose, _BindingIdxByName, AnimRawData, TimeRatio, _RootToIgnore);
	}
	else
	{
		float AngleInRad = atan2(_BlendPoint.Y, _BlendPoint.X);


		AngleInRad += XM_2PI;
		AngleInRad = fmod(AngleInRad, XM_2PI); // 각도를 0~2PI로 클램핑


		constexpr int32 TEMP_COUNT = (int32)E8Dir::Count;
		static const Vector4f BlendPosByDir[TEMP_COUNT] =
		{
	{0, 0, 0 ,0},		// None,
	{0, 1, 0, 0},		// U,
	{1, 1, 0, 0},		// UR,
	{1, 0, 0, 0},		// R,
	{1, -1, 0, 0},		// RD,
	{0, -1, 0, 0},		// D,
	{-1, -1, 0, 0},		// DL,
	{-1, 0, 0, 0},		// L,
	{-1, 1, 0, 0}		// LU,
		};


		constexpr E8Dir Dir0 = E8Dir::None;
		E8Dir Dir1;
		E8Dir Dir2;

		if (AngleInRad < XM_PIDIV4) // 0~45
		{
			Dir1 = E8Dir::UR;
			Dir2 = E8Dir::R;
		}
		else if (AngleInRad < XM_PIDIV4 * 2) // 45~90
		{
			Dir1 = E8Dir::U;
			Dir2 = E8Dir::UR;
		}
		else if (AngleInRad < XM_PIDIV4 * 3) // 90~135
		{
			Dir1 = E8Dir::UL;
			Dir2 = E8Dir::U;
		}
		else if (AngleInRad < XM_PIDIV4 * 4) // 135~180
		{
			Dir1 = E8Dir::L;
			Dir2 = E8Dir::UL;
		}
		else if (AngleInRad < XM_PIDIV4 * 5) // 180~225
		{
			Dir1 = E8Dir::DL;
			Dir2 = E8Dir::L;
		}
		else if (AngleInRad < XM_PIDIV4 * 6) // 225~270
		{
			Dir1 = E8Dir::D;
			Dir2 = E8Dir::DL;
		}
		else if (AngleInRad < XM_PIDIV4 * 7) // 270~315
		{
			Dir1 = E8Dir::DR;
			Dir2 = E8Dir::D;
		}
		else // 315 ~ 360
		{
			Dir1 = E8Dir::R;
			Dir2 = E8Dir::DR;
		}


		const Vector4f& v0 = BlendPosByDir[(int32)Dir0];
		const Vector4f& v1 = BlendPosByDir[(int32)Dir1];
		const Vector4f& v2 = BlendPosByDir[(int32)Dir2];

		Vector4f BlendPoint4f = { _BlendPoint.X, _BlendPoint.Y, 0, 0 };

		Vector4f Weights = SS::CalcBarycentricWeight(BlendPoint4f, v0, v1, v2);

		float w0 = Weights.X;
		float w1 = Weights.Y;
		float w2 = Weights.Z;

		IRenderAnimAsset* FoundRenderAnimAsset0 =
			AssetManager->FindAssetByName<IRenderAnimAsset>(_RenderAnimAssetNames[static_cast<int32>(Dir0)]);
		const RenderAnimRawData* AnimRawData0 = FoundRenderAnimAsset0->GetKeyFrameAnimData();

		IRenderAnimAsset* FoundRenderAnimAsset1 =
			AssetManager->FindAssetByName<IRenderAnimAsset>(_RenderAnimAssetNames[static_cast<int32>(Dir1)]);
		const RenderAnimRawData* AnimRawData1 = FoundRenderAnimAsset1->GetKeyFrameAnimData();

		IRenderAnimAsset* FoundRenderAnimAsset2 =
			AssetManager->FindAssetByName<IRenderAnimAsset>(_RenderAnimAssetNames[static_cast<int32>(Dir2)]);
		const RenderAnimRawData* AnimRawData2 = FoundRenderAnimAsset2->GetKeyFrameAnimData();


		const float MainAnimDuration = AnimRawData0->_Header.KeyFrameDuration;
		const float Duration1 = AnimRawData1->_Header.KeyFrameDuration;
		const float Duration2 = AnimRawData2->_Header.KeyFrameDuration;
		const float BlendedAnimDuration = 
			w0 * MainAnimDuration +
			w1 * Duration1 + 
			w2 * Duration2;

		// 메인 애님 듀레이션 기준으로 배속해준다.
		const float BlendedPlayRate = MainAnimDuration / BlendedAnimDuration;

		if (_bIsOneTimeUpdateRequested == false)
		{
			_WholeFrameTime += (DeltaTime * BlendedPlayRate);
		}


		double TimeRatio = fmod(_WholeFrameTime, MainAnimDuration);
		TimeRatio = TimeRatio / MainAnimDuration;

		EvaluatePose(_BlendPose0, _BindingIdxByName, AnimRawData0, TimeRatio, _RootToIgnore);
		EvaluatePose(_BlendPose1, _BindingIdxByName, AnimRawData1, TimeRatio, _RootToIgnore);
		EvaluatePose(_BlendPose2, _BindingIdxByName, AnimRawData2, TimeRatio, _RootToIgnore);

		BlendThreePoses(
			_ResultPose,
			_BlendPose0, w0,
			_BlendPose1, w1,
			_BlendPose2, w2
		);
	}

	_bIsOneTimeUpdateRequested = false;
	_LastUpdateFrame = SSFrameInfo::GetFrameCnt();
}

SS::SHasherW AnimWorkeeBlendSpace::GetRenderAnimAssetName(E8Dir Dir)
{
	return _RenderAnimAssetNames[static_cast<int32>(Dir)];
}

void AnimWorkeeBlendSpace::SetPauseAnim(bool bIsPause)
{
	_bIsAnimPause = bIsPause;
}

void AnimWorkeeBlendSpace::SetRootIgnoreName(SS::SHasherW InName)
{
	_RootToIgnore = InName;
}

void AnimWorkeeBlendSpace::SetRenderAnimAsset(SS::SHasherW RenderAnimAssetName, E8Dir Dir)
{
	if (Dir == E8Dir::Count)
	{
		SS_INTERRUPT();
	}

	_RenderAnimAssetNames[(int32)Dir] = RenderAnimAssetName;
}

void AnimWorkeeBlendSpace::SetWholeFrameTime(float Time)
{
	_WholeFrameTime = Time;
	_bIsOneTimeUpdateRequested = true;
}

void AnimWorkeeBlendSpace::SetBlendPoint(Vector2f InPoint)
{
	InPoint.X = InPoint.X < -1 ? -1 : InPoint.X;
	InPoint.X = InPoint.X > 1 ? 1 : InPoint.X;

	InPoint.Y = InPoint.Y < -1 ? -1 : InPoint.Y;
	InPoint.Y = InPoint.Y > 1 ? 1 : InPoint.Y;

	_bIsOneTimeUpdateRequested = true;
	_BlendPoint = InPoint;
}