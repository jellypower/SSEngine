#pragma once
#include "SColliderBaseComponent.h"

class ICISphere;

class SSCONTENTBASE_MODULE SSphereColliderComponent : public SColliderBaseComponent
{
private:
	ICISphere* _CollInstance = nullptr;

public:
	void SetRadius(float InRadius);

public:
	virtual bool ShouldProcessPerFrameInherently() const override;
	virtual void PerFrame(float DeltaTime) override;

public:
	virtual Vector4f CalcFurthest(const Vector4f& Dir) const override;
	virtual ICollInstanceBase* GetCollInstance() const override;

protected:
	virtual void ConstructCollInstance() override;
	virtual void DestructCollInstance() override;
};
