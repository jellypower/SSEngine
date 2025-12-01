#pragma once
#include "RenderKeyFrameTrackItemTypes/ERKFTrackItemType.h"
#include "RenderKeyFrameTrackItemTypes/RKFTrackItemTransform.h"


struct RKFTrack // RenderKeyFrameTrack
{
	ERKFTrackItemType _Type = ERKFTrackItemType::None;
	SS::SHasherW _TrackName;
	int _TrackItemCnt = 0;
	RKFTrackItemBase* _TrackItems = nullptr;
};


struct RenderAnimRawData
{
	SS::PooledList<RKFTrack> _Tracks;
	float _KeyFrameDuration = 0;
};