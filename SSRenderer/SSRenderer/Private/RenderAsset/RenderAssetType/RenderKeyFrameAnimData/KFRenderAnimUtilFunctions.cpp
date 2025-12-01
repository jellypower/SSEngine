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
	for (int32 i = 0; i < Track._TrackItemCnt - 1; i++)
	{
		float T1TimeRatio = TransformTrack[i]._TimeRatio;
		float T2TimeRatio = TransformTrack[i + 1]._TimeRatio;
		float TimeSpan = T2TimeRatio - T1TimeRatio;


		if (TransformTrack[i]._TimeRatio > CurTimeRatio ||
			TransformTrack[i + 1]._TimeRatio < CurTimeRatio )
		{
			continue;
		}

		float CurTimeRatioAlphaInRange = (CurTimeRatio - T1TimeRatio) / TimeSpan;

		const Transform& T1 = TransformTrack[i]._Transform;
		const Transform& T2 = TransformTrack[i + 1]._Transform;

		Result = SS::Lerp(T1, T2, CurTimeRatioAlphaInRange);
		break;
	}


	return Result;
}
