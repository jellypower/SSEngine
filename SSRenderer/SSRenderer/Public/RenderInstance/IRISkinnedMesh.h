#pragma once
#include "IRIMesh.h"


class IRISkinnedMesh : public IRIMesh
{
	virtual void UpdateSkeleton() = 0;
};
	