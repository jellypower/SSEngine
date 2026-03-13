#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/AnimWorker/AnimBase/AnimateUtilFunctions.h"

#include "SSContentsBase/Public/AnimWorker/AnimWorkee/IAnimWorkee.h"
#include "SSContentsBase/Public/AnimWorker/AnimBase/AnimPoseTypes.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderKeyFrameAnimData/RenderAnimData.h"


Transform EvaluateRenderKFTransform(const RenderAnimRawData* AnimRawData, int32 TrackIdx, float CurTimeRatio)
{
	Transform Result = Transform::Identity;

	const RKFTrack& Track = AnimRawData->_Tracks[TrackIdx];
	const int32 TrackItemCnt = Track._TrackItems.GetSize();

	

	int32 i = 0;
	float T1TimeRatio = 0;
	float T2TimeRatio = 0;
	float TimeSpan = 0;
	for (; i < TrackItemCnt - 1; i++)
	{
		T1TimeRatio = Track._TrackItems[i]._TimeRatio;
		T2TimeRatio = Track._TrackItems[i + 1]._TimeRatio;
		TimeSpan = T2TimeRatio - T1TimeRatio;

		if (T1TimeRatio <= CurTimeRatio && CurTimeRatio < T2TimeRatio)
		{
			break;
		}
	}

	if (i >= TrackItemCnt - 1)
	{
		i = TrackItemCnt - 2;
		T1TimeRatio = Track._TrackItems[i]._TimeRatio;
		T2TimeRatio = Track._TrackItems[i + 1]._TimeRatio;
		TimeSpan = T2TimeRatio - T1TimeRatio;
	}

	float CurTimeRatioAlphaInRange = (CurTimeRatio - T1TimeRatio) / TimeSpan;
	CurTimeRatioAlphaInRange = CurTimeRatioAlphaInRange > 1 ? 1 : CurTimeRatioAlphaInRange;

	const Transform& T1 = Track._TrackItems[i]._Transform;
	const Transform& T2 = Track._TrackItems[i + 1]._Transform;

	Result = SS::Lerp(T1, T2, CurTimeRatioAlphaInRange);


	return Result;
}

void EvaluatePose(
	PoseSlot& OutPose,
	const SS::HashMap<SS::SHasherW, int32>& PoseBoneIdxMap,
	const RenderAnimRawData* AnimRawData,
	float CurTimeRatio,
	SS::SHasherW RootToIgnore)
{
	const int32 TrackCnt = AnimRawData->_Header.TrackCnt;

	for (int32 TrackIdx = 0; TrackIdx < TrackCnt; TrackIdx++)
	{
		SS::SHasherW TrackNameItem = AnimRawData->_Tracks[TrackIdx]._TrackName;

		if (TrackNameItem == RootToIgnore)
		{
			continue;
		}

		const int32* pBindingIdx = PoseBoneIdxMap.Find(TrackNameItem);
		if (pBindingIdx == nullptr)
		{
			// 원래는 Assert가 나긴 해야되는데 일단은 패스하자.
			continue;
		}

		int32 BindingIdx = *pBindingIdx;

		Transform Result = EvaluateRenderKFTransform(AnimRawData, TrackIdx, CurTimeRatio);
		OutPose.BoneTransforms[BindingIdx] = Result;
	}
}

bool BlendThreePoses(
	PoseSlot& OutPose,
	const PoseSlot& InPose1, float Weight1,
	const PoseSlot& InPose2, float Weight2,
	const PoseSlot& InPose3, float Weight3)
{
	constexpr float TEMP_EPSILON = 0.01f;

	int32 ResultBoneCnt = OutPose.BoneTransforms.GetSize();
	int32 Pose1BoneCnt = InPose1.BoneTransforms.GetSize();
	int32 Pose2BoneCnt = InPose2.BoneTransforms.GetSize();
	int32 Pose3BoneCnt = InPose3.BoneTransforms.GetSize();


	bool bIsValidBlend =
		(ResultBoneCnt == Pose1BoneCnt) &&
		(Pose1BoneCnt == Pose2BoneCnt) &&
		(Pose2BoneCnt == Pose3BoneCnt);

	if (bIsValidBlend == false)
	{
		return false;
	}

	if (Weight1 + Weight2 + Weight3 < 1.0f - TEMP_EPSILON)
	{
		SS_ASSERT(false);
		return false;
	}


	if (Weight1 + Weight2 < TEMP_EPSILON)
	{
		for (int32 i = 0; i < ResultBoneCnt; i++)
		{
			OutPose.BoneTransforms[i] = InPose3.BoneTransforms[i];
		}

		return true;
	}
	else if (Weight1 + Weight3 < TEMP_EPSILON)
	{
		for (int32 i = 0; i < ResultBoneCnt; i++)
		{
			OutPose.BoneTransforms[i] = InPose2.BoneTransforms[i];
		}

		return true;
	}
	else if (Weight2 + Weight3 < TEMP_EPSILON)
	{
		for (int32 i = 0; i < ResultBoneCnt; i++)
		{
			OutPose.BoneTransforms[i] = InPose1.BoneTransforms[i];
		}

		return true;
	}

	for (int32 i = 0; i < ResultBoneCnt; i++)
	{
		const Transform& T1 = InPose1.BoneTransforms[i];
		const Transform& T2 = InPose2.BoneTransforms[i];
		const Transform& T3 = InPose3.BoneTransforms[i];

		float t1 = Weight2 / (Weight1 + Weight2);
		float w12 = 1 - (Weight1 + Weight2); // == (Weight1 + Weight2) / (Weight1 + Weight2 + Weight3)

		Transform Lerp1 = SS::Lerp(T1, T2, t1);
		Transform Lerp2 = SS::Lerp(Lerp1, T3, w12);

		OutPose.BoneTransforms[i] = Lerp2;
	}

	return true;
}

bool BlendPoses(PoseSlot& OutPose,
	const PoseSlot& InPose1,
	const PoseSlot& InPose2,
	float Alpha)
{
	constexpr float TEMP_EPSILON = 0.01f;

	int32 ResultBoneCnt = OutPose.BoneTransforms.GetSize();
	int32 Pose1BoneCnt = InPose1.BoneTransforms.GetSize();
	int32 Pose2BoneCnt = InPose2.BoneTransforms.GetSize();

	bool bIsValidBlend =
		(ResultBoneCnt == Pose1BoneCnt) &&
		(Pose1BoneCnt == Pose2BoneCnt);

	if (bIsValidBlend == false)
	{
		return false;
	}

	if (Alpha < TEMP_EPSILON)
	{
		for (int32 i = 0; i < ResultBoneCnt; i++)
		{
			OutPose.BoneTransforms[i] = InPose1.BoneTransforms[i];
		}

		return true;
	}
	else if (Alpha > 1 - TEMP_EPSILON)
	{
		for (int32 i = 0; i < ResultBoneCnt; i++)
		{
			OutPose.BoneTransforms[i] = InPose2.BoneTransforms[i];
		}

		return true;
	}

	for (int32 i = 0; i < ResultBoneCnt; i++)
	{
		const Transform& T1 = InPose1.BoneTransforms[i];
		const Transform& T2 = InPose2.BoneTransforms[i];

		OutPose.BoneTransforms[i] = SS::Lerp(T1, T2, Alpha);
	}

	return true;
}
