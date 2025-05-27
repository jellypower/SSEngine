#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"


struct ConstantBufferWrapper;
constexpr int32 DEFAULT_PAGE_SIZE = 64 * 1024; // DX12 Committed Resource Default Alignment Size

struct ConstantBufferPoolDesc
{
	int32 BufferItemSize;
	int32 DefaultPoolPageCnt;
	uint32 ResourceSizePerPage = DEFAULT_PAGE_SIZE;
};

struct PageLocation
{
	int32 PageNo = 0;
	int32 Location = 0;
};

class ConstantBufferPool : public INoncopyable
{
public:
	ConstantBufferPool(const ConstantBufferPoolDesc& Desc);
	virtual ~ConstantBufferPool();

	virtual bool IsValid() const = 0;
	virtual ConstantBufferWrapper* RentConstantBuffer() = 0;
	virtual void ResetAllocState() = 0;

protected:
	uint32 _resourceSizePerPage = 0;
	int32 _constantBufferitemSize = 0;
	int32 _itemCntPerPage = 0;
	int32 _bufferAlignmentPaddingSize = 0;
};
