#pragma once

class GALRenderDevice;

enum class ERTColorFormat : int32;

class GALCPUReadableTexture : public INoncopyable
{
public:
	virtual const Vector2i32& GetResourceWidthHeight() const = 0;
	virtual const ERTColorFormat GetColorFormat() const = 0;
	virtual void* GetDataAt(int32 X, int32 Y) const = 0;
	virtual void* GetDataAtRatio(float RatioX, float RatioY) const = 0;

	virtual void BeginRead() = 0;
	virtual void EndRead() = 0;
};
