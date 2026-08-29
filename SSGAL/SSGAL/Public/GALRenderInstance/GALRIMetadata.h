#pragma once

class IRenderInstance;
enum class ERenderInstanceType;

// GAL Render Instance Metadata
class GALRIMetadata : public ISSUnknown
{
protected:
	const IRenderInstance* _OwnerRenderInstance = nullptr;

public:
	virtual ERenderInstanceType GetMetadataRenderInstanceType() = 0;

};

