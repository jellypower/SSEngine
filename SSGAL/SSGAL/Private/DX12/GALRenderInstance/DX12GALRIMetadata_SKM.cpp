#include "pch.h"
#include "DX12GALRIMetadata_SKM.h"

#include <SSRenderer/Public/RenderInstance/IRenderInstance.h>

DX12GALRIMetadata_SKM::DX12GALRIMetadata_SKM(GALRenderDevice* InRenderDevice, const IRIMesh* InOwnerRenderInstance) :
	DX12GALRIMetadata_SM(InRenderDevice, InOwnerRenderInstance)
{
}

ERenderInstanceType DX12GALRIMetadata_SKM::GetMetadataRenderInstanceType()
{
	return ERenderInstanceType::SkinnedMesh;
}
