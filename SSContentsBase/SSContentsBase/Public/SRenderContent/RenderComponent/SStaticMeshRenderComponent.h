#pragma once
#include "SMeshRenderComponentBase.h"

#include "SSContentsBase/ModuleExportKeyword.h"

class SSCONTENTBASE_MODULE SStaticMeshRenderComponent : public SMeshRenderComponentBase
{
protected:
	virtual void ConstructRenderInstance() override;
	virtual void DestructRenderInstance() override;
};
