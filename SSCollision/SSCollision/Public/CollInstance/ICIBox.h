#pragma once
#include "ICollInstanceBase.h"

// ICollisionInstanceBox
class ICIBox : public  ICollInstanceBase
{
public:
	virtual const Vector4f& GetExtent() const = 0;
	virtual Vector4f GetVertexPos(int Idx) const = 0;
	virtual void SetExtent(const Vector4f& InExtent) = 0;
};
