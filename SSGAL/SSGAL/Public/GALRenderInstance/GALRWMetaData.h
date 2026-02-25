#pragma once

class IRenderWorld;

class GALRWMetaData : public INoncopyable
{
public:
	virtual IRenderWorld* GetOwnerRenderWorld() const = 0;
};
