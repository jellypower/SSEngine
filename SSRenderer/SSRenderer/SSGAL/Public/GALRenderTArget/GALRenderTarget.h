#pragma once
#include "SSEngineDefault/Public/GeometryType.h"
#include "SSEngineDefault/Public/SSEngineDefault.h"

class GALRenderDeviceContext;
enum class EResourceStateType : uint8;
enum class ERTColorFormat : int32;
enum class ERenderTargetType : uint8;
class GALRenderDevice;



class GALRenderTarget : public INoncopyable
{
public:
	const BoundBox2f& GetScissorRectSize() const { return _ScissorRectSize; }
	const ViewportBox& GetViewportBoxSize() const { return _ViewportBoxSize; }
	int32 GetCurRenderTargetIdx() const { return _CurRenderTargetIdx; }


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
