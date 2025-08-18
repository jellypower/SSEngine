#pragma once
#include "SRenderComponentBase.h"

class SSkinnedMeshRenderComponent : public SRenderComponentBase
{
protected:
	virtual void ConstructRenderInstance() override;
	virtual void DestructRenderInstance() override;
};
