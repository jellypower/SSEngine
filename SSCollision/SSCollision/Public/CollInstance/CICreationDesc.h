#pragma once


class ICollisionWorld;

struct COLL_INSTANCE_CREATION_DESC
{
	Transform InitialLclTransform;
	Vector4f Offset;
	SObjHashCode ComponentID;
};

struct CI_BOX_DESC : COLL_INSTANCE_CREATION_DESC
{
	Vector4f Extent;
};

struct CI_SPHERE_DESC : COLL_INSTANCE_CREATION_DESC
{
	float Radius;
};