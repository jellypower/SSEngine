#pragma once

class ICollInstanceBase;

class ICollisionWorld : public INoncopyable
{
public:
	virtual bool IsAnyInstanceRemainInWorld() const = 0;
	virtual SS::SHasherW GetWorldName() const = 0;

	virtual void AddToWorld(ICollInstanceBase* InRenderInstance) = 0;
	virtual void RemoveFromWorld(SObjHashCode CollInstanceIDToRemove) = 0;

};
