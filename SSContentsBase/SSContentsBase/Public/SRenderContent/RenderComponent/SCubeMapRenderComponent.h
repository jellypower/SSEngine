#pragma once
#include "SRenderComponentBase.h"

class SSCONTENTBASE_MODULE SCubeMapRenderComponent : public SRenderComponentBase
{
private:
	SS::SHasherW _TextureAssetName;


public:
	SS::SHasherW GetCubeMapTextureAssetName() const;
	void SetCubeMapTextureAssetName(SS::SHasherW InTexName);

protected:
	virtual void ConstructRenderInstance() override;
	virtual void DestructRenderInstance() override;
};
