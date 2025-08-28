#pragma once
#include "SSContentsBase/Public/ContentBase/SComponentBase.h"

class IRenderLight;

class SSCONTENTBASE_MODULE SRenderLightComponent : public SComponentBase
{
protected:
	IRenderLight* _RenderLight = nullptr;

private:
	Vector4f _CachedLightIntensity;

public:
	const Vector4f& GetLightIntensity() const;
	void SetLightIntensity(const Vector4f& InIntensity);

public:
	void PostConstruct() override;
};
