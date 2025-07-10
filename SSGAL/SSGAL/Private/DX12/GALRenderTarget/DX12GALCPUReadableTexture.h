#pragma once
#include "Public/GALRenderTarget/GALCPUReadableTexture.h"

class DX12GALRenderDevice;

class DX12GALCPUReadableTexture : public GALCPUReadableTexture
{
public:
	DX12GALCPUReadableTexture(DX12GALRenderDevice* InOwnerDevice, ERTColorFormat InColorFormat,  Vector2i32 InWidthHeight, int32 Pitch, const utf16* ResourceName);
	~DX12GALCPUReadableTexture();

public:
	virtual const Vector2i32& GetResourceWidthHeight() const override;
	virtual const ERTColorFormat GetColorFormat() const override;
	virtual void* GetDataAt(int32 X, int32 Y) const override;
	virtual void* GetDataAtRatio(float RatioX, float RatioY) const override;
	
	virtual void BeginRead() override;
	virtual void EndRead() override;

public:
	ID3D12Resource* GetCurrentResource() const { return _TexResource; }

private:
	DX12GALRenderDevice* _OwnerRenderDevice = nullptr;
	Vector2i32 _ResourceWidthHeight;
	ERTColorFormat _ColorFormat;
	int32 _Pitch;

	ID3D12Resource* _TexResource = nullptr;
	byte* _MappedData = nullptr;
};
