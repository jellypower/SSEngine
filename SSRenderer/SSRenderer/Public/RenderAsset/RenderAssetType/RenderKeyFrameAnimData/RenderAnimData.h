#pragma once
#include "RKFTrackItem.h"


struct AnimDataHeader
{
	int32 TrackCnt = 0;
	float KeyFrameDuration = 0;

	int64 PAD02 = 0;
	int64 PAD03 = 0;
	int64 PAD04 = 0;
};

struct RKFTrack // RenderKeyFrameTrack
{
	SS::SHasherW _TrackName;
	SS::PooledList<RKFTrackItem> _TrackItems;
};


struct RenderAnimRawData
{
	AnimDataHeader _Header;
	SS::PooledList<RKFTrack> _Tracks;
};