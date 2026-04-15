#include "SSContentsBase/Public/SRenderContent/RenderComponent/SRenderLightComponent.h"

#include "SSRenderer/Public/RenderInstance/Light/IRenderLight.h"

const Vector4f& SRenderLightComponent::GetLightIntensity() const
{
	return _CachedLightIntensity;
}

void SRenderLightComponent::SetLightIntensity(const Vector4f& InIntensity)
{
	_CachedLightIntensity = InIntensity;
	if (_RenderLight == nullptr)
	{
		return;
	}

	_RenderLight->SetLightIntensity(_CachedLightIntensity.SimdVec);
}

void SRenderLightComponent::PostConstruct()
{
	_CachedLightIntensity = Vector4f::One;
}
