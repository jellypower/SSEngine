#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"


class IModelAsset;
class IMeshAsset;
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


	IModelAsset* GetCube1mModel() const { return _Cube1mModel; }
	IModelAsset* GetSphere1mModel() const { return _Sphere1mModel; }

	IMeshAsset* GetCube1mMesh() const { return _Cube1mMesh; }
	IMeshAsset* GetSphere1mMesh() const { return _Sphere1mMesh; }
	IMeshAsset* GetArrowMesh() const { return _ArrowMesh; }

public:
	virtual void InitializeCommonAssets() = 0;
	virtual void ReleaseCachedAssets() = 0;

protected:
	ITextureAsset* _TexEMPTY = nullptr;
	ITextureAsset* _TexWHITE = nullptr;
	ITextureAsset* _TexBLACK = nullptr;
	ITextureAsset* _TexEMPTYNORMAL = nullptr;

	IMaterialAsset* _EmptyPBRMaterial = nullptr;

	IModelAsset* _Cube1mModel = nullptr;
	IModelAsset* _Sphere1mModel = nullptr;

	IMeshAsset* _Cube1mMesh = nullptr;
	IMeshAsset* _Sphere1mMesh = nullptr;

	IMeshAsset* _ArrowMesh = nullptr;
};
