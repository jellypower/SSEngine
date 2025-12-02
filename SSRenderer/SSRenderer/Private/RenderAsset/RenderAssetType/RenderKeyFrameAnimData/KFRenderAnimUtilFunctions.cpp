#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderKeyFrameAnimData/KFRenderAnimUtilFunctions.h"


Transform EvaluateRenderKFTransform(const RenderAnimRawData* AnimRawData, int32 TrackIdx, float CurTime)
{
	Transform Result = Transform::Identity;

	const RKFTrack& Track = AnimRawData->_Tracks[TrackIdx];
	if (Track._Type != ERKFTrackItemType::BoneTransform)
	{
		SS_INTERRUPT();
		return Transform::Identity;
	}

	float Duration = AnimRawData->_KeyFrameDuration;

	float CurTimeRatio = CurTime / Duration;
	CurTimeRatio = CurTimeRatio < 0 ? 0 : CurTimeRatio; // clamp01
	CurTimeRatio = CurTimeRatio > 1 ? 1 : CurTimeRatio;


	RKFTrackItemTransform* TransformTrack = static_cast<RKFTrackItemTransform*>(Track._TrackItems);

	int32 i = 0;
	float T1TimeRatio = 0;
	float T2TimeRatio = 0;
	float TimeSpan = 0;
	for (;i < Track._TrackItemCnt - 1; i++)
	{
		T1TimeRatio = TransformTrack[i]._TimeRatio;
		T2TimeRatio = TransformTrack[i + 1]._TimeRatio;
		TimeSpan = T2TimeRatio - T1TimeRatio;

		if (T1TimeRatio < CurTimeRatio && CurTimeRatio < T2TimeRatio)
		{
			break;
		}
	}

	if (i >= Track._TrackItemCnt - 1)
	{
		i = Track._TrackItemCnt - 2;
		T1TimeRatio = TransformTrack[i]._TimeRatio;
		T2TimeRatio = TransformTrack[i + 1]._TimeRatio;
		TimeSpan = T2TimeRatio - T1TimeRatio;
	}

	float CurTimeRatioAlphaInRange = (CurTimeRatio - T1TimeRatio) / TimeSpan;
	CurTimeRatioAlphaInRange = CurTimeRatioAlphaInRange > 1 ? 1 : CurTimeRatioAlphaInRange;

	const Transform& T1 = TransformTrack[i]._Transform;
	const Transform& T2 = TransformTrack[i + 1]._Transform;

	Result = SS::Lerp(T1, T2, CurTimeRatioAlphaInRange);


	return Result;
}
