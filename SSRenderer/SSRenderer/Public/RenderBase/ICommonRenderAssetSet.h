#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

class IMaterialAsset;
class ITextureAsset;

class ICommonRenderAssetSet : public INoncopyable
{
public:
	ITextureAsset* GetTexEMPTY() const { return _TexEMPTY; }
	ITextureAsset* GetTexWHITE() const { return _TexWHITE; }
	ITextureAsset* GetTexBLACK() const { return _TexBLACK; }
	ITextureAsset* GetTexEMPTYNORMAL() const { return _TexEMPTYNORMAL; }
	IMaterialAsset* GetEmptyPBRMaterial() const { return _EmptyPBRMaterial; }


public:
	virtual void CacheCommonRenderAssets() = 0;
	virtual void AddRefCachedAssets() = 0;
	virtual void ReleaseCachedAssets() = 0;

protected:
	ITextureAsset* _TexEMPTY = nullptr;
	ITextureAsset* _TexWHITE = nullptr;
	ITextureAsset* _TexBLACK = nullptr;
	ITextureAsset* _TexEMPTYNORMAL = nullptr;

	IMaterialAsset* _EmptyPBRMaterial = nullptr;
};
