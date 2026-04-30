#pragma once
#include "SColliderBaseComponent.h"

class ICIBox;

class SSCONTENTBASE_MODULE SBoxColliderComponent : public SColliderBaseComponent
{
private:
	ICIBox* _CollInstance = nullptr;

public:
	void SetExtent(const Vector4f& Extent);

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
