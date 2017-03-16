#pragma once
#include <math.h>
#include "Matrix4x4.h"

class Vector3f {
public:
	float x;
	float y;
	float z;
	float w;

	Vector3f();

	Vector3f(float x, float y, float z, float w);

	Vector3f(float x, float y, float z);

	double Length();

	Vector3f Normalize();

	static float Dot(Vector3f, Vector3f);

	static Vector3f Cross(Vector3f lhs, Vector3f rhs);
};

extern Vector3f operator*(Vector3f lhs, Matrix4x4 *rhs);

extern Vector3f operator-(Vector3f lhs, Vector3f rhs);

extern Vector3f operator+(Vector3f lhs, Vector3f rhs);