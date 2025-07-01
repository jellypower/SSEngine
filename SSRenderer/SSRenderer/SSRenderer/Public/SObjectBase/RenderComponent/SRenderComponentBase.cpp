#include "SRenderComponentBase.h"
#include "SSGAL/Public/GALRenderDevice/GALRenderDeviceContext.h"

void SRenderComponentBase::SetModelAsset(SS::SHasherW ModelAssetName)
{
	_ModelAssetName = ModelAssetName;
}


void SRenderComponentBase::PostConstructHierarchy()
{
	ConstructRenderInstance();
}

void SRenderComponentBase::OnEnterTheWorld()
{
}

void SRenderComponentBase::OnExitTheWorld()
{
}

void SRenderComponentBase::PreDestructHierarchy()
{
	DestructRenderInstance();
}