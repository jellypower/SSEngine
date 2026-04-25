#pragma once

class ICollInstanceBase;
class SimplexV4;
class ICIBox;

static class CollCalc_Private
{
public:

	static bool GJK(const ICollInstanceBase* c1, const ICollInstanceBase* c2);
	static Vector4f GJK_Support(const ICollInstanceBase* c1, const ICollInstanceBase* c2, Vector4f Dir);
};