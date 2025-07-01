#pragma once
#include "SRenderComponentBase.h"

class SStaticMeshRenderComponent : public SRenderComponentBase
{
protected:
	virtual void ConstructRenderInstance() override;
	virtual void DestructRenderInstance() override;
};
