#pragma once
#include "SSContentsBase/Public/ContentBase/SComponentBase.h"

class CharacterComponent : public SComponentBase
{
private:
	Vector2f _MoveVelocity;
	float _BaseAccel = 0;
	float _AutoDecl = 0;
	float _MaxSpeed = 0;


public:
	bool ShouldProcessPerFrameInherently() const override;
	void PerFrame() override;


public:
	void AddAccel(Vector2f Dir);
};
