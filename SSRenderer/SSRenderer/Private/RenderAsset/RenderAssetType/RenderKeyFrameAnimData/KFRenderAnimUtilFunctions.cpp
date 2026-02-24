#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderKeyFrameAnimData/KFRenderAnimUtilFunctions.h"


Transform EvaluateRenderKFTransform(const RenderAnimRawData* AnimRawData, int32 TrackIdx, float CurTime)
{
	Transform Result = Transform::Identity;

	const RKFTrack& Track = AnimRawData->_Tracks[TrackIdx];
	const int32 TrackItemCnt = Track._TrackItems.GetSize();

	const float Duration = AnimRawData->_KeyFrameDuration;

	float CurTimeRatio = CurTime / Duration;
	CurTimeRatio = CurTimeRatio < 0 ? 0 : CurTimeRatio; // clamp01
	CurTimeRatio = CurTimeRatio > 1 ? 1 : CurTimeRatio;


	int32 i = 0;
	float T1TimeRatio = 0;
	float T2TimeRatio = 0;
	float TimeSpan = 0;
	for (;i < TrackItemCnt - 1; i++)
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
