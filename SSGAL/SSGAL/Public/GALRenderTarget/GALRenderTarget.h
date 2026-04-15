#pragma once
#include "GALRTCommonEnums.h"
#include "SSGAL/Public/SSGALCommonEnums.h"
#include "SSEngineDefault/Public/GeometryType.h"

class GALRenderDeviceContext;
enum class ERenderTargetType : uint8;
class GALRenderDevice;



struct GALRenderTargetDesc
{
	int32 ResourceWidth = 0;
	int32 ResourceHeight = 0;
	BoundBox2f ScissorRectSize; // ScissorRects: 실제 전체 텍스쳐 영역에서 Scissor까지만 그림(LeftTop=Min, RightBottom=Max에 따라서)
	ViewportBox DrawBoxSize; // Viewports: 실제 전체 텍스쳐 영역에서 TopLeft기준으로 WidthHeight만큼 그림(WidthHeight가 늘었다 줄었다 하면 이미지 사이즈가 달라짐)
	ERTColorFormat Format = ERTColorFormat::None;
	EResourceStateType InitialResourceState = EResourceStateType::None;
	bool bUseUAV = false;
	bool bUseSRV = false;
};



class GALRenderTarget : public INoncopyable
{
public:
	const BoundBox2f& GetScissorRectSize() const { return _ScissorRectSize; }
	const ViewportBox& GetViewportBoxSize() const { return _ViewportBoxSize; }
	int32 GetCurRenderTargetIdx() const { return _CurRenderTargetIdx; }

public:
	virtual Vector2i32 GetResourceSize() const = 0;
	virtual int32 GetResourceRowPitch() const = 0;
	virtual ERenderTargetType GetRenderTargetType() const = 0;
	virtual ERTColorFormat GetRTColorFormat() const = 0;

	virtual void ResourceBarrier(GALRenderDeviceContext* InDeviceContext, EResourceStateType From, EResourceStateType To) = 0;


protected:
	GALRenderDevice* _OwnerRenderDevice = nullptr;
	// RSSetScissorRects: 실제 전체 텍스쳐 영역에서 Scissor까지만 그림(LeftTop=Min, RightBottom=Max에 따라서)
	BoundBox2f _ScissorRectSize;
	// RSSetViewports: 실제 전체 텍스쳐 영역에서 TopLeft기준으로 WidthHeight만큼 그림(WidthHeight가 늘었다 줄었다 하면 이미지 사이즈가 달라짐)
	ViewportBox _ViewportBoxSize;
	int32 _CurRenderTargetIdx = 0;
};
