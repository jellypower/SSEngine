#define SSENGINEDEFAULT_MODULE_EXPORT
#include "SSEngineDefault/Public/SSVector.h"

Vector4f const Vector4f::Forward = Vector4f(0, 0, 1, 0);
Vector4f const Vector4f::Back = Vector4f(0, 0, -1, 0);
Vector4f const Vector4f::Up = Vector4f(0, 1, 0, 0);
Vector4f const  Vector4f::Down = Vector4f(0, -1, 0, 0);
Vector4f const Vector4f::Left = Vector4f(-1, 0, 0, 0);
Vector4f const Vector4f::Right = Vector4f(1, 0, 0, 0);
Vector4f const Vector4f::Zero = Vector4f(0, 0, 0, 0);
Vector4f const Vector4f::One = Vector4f(1, 1, 1, 1);
Vector4f const Vector4f::Half = Vector4f(.5f, .5f, .5f, .5f);

Vector2f const Vector2f::Zero = Vector2f(0, 0);
Vector2f const Vector2f::One = Vector2f(1, 1);

Vector2i32 const Vector2i32::Zero = Vector2i32(0, 0);

Transform const Transform::Identity = Transform();