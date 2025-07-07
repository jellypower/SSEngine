#pragma once
#include "SRenderComponentBase.h"

#include "SSContentsBase/ModuleExportKeyword.h"

class SSCONTENTBASE_MODULE SStaticMeshRenderComponent : public SRenderComponentBase
{
protected:
	virtual void ConstructRenderInstance() override;
	virtual void DestructRenderInstance() override;
};
