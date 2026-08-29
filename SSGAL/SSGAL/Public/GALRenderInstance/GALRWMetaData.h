#pragma once

class IRenderWorld;

class GALRWMetaData : public ISSUnknown
{
public:
	virtual IRenderWorld* GetOwnerRenderWorld() const = 0;
};
