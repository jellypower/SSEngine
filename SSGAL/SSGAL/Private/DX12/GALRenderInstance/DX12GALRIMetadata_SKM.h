#pragma once
#include "DX12GALRIMetadata_SM.h"

class DX12GALRIMetadata_SKM : public DX12GALRIMetadata_SM
{
public:
	DX12GALRIMetadata_SKM(GALRenderDevice* InRenderDevice, const IRIMesh* InOwnerRenderInstance);

public:
	virtual ERenderInstanceType GetMetadataRenderInstanceType() override;
};
