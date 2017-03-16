#include "Vector3.h"

Vector3f::Vector3f() {
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 0.0f;
}

Vector3f::Vector3f(float x, float y, float z, float w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Vector3f::Vector3f(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = 0.0f;
}

double Vector3f::Length() {
	return sqrt(x * x + y * y + z * z);
}

Vector3f Vector3f::Normalize() {
	double length = Length();
	if (length != 0) {
		double s = 1 / length;
		x *= s;
		y *= s;
		z *= s;
	}
	return *this;
}

float Vector3f::Dot(Vector3f lhs, Vector3f rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}
Vector3f Vector3f::Cross(Vector3f lhs, Vector3f rhs)
{
	float x = lhs.y * rhs.z - lhs.z * rhs.y;
	float y = lhs.z * rhs.x - lhs.x * rhs.z;
	float z = lhs.x * rhs.y - lhs.y * rhs.x;
	return Vector3f(x, y, z, 0);
}
// Ê¸Á¿ * ¾ØÕó
Vector3f operator*(Vector3f lhs, Matrix4x4 *rhs)
{
	Vector3f v;
	v.x = lhs.x * rhs->_m[0][0] + lhs.y * rhs->_m[1][0] + lhs.z * rhs->_m[2][0] + lhs.w * rhs->_m[3][0];
	v.y = lhs.x * rhs->_m[0][1] + lhs.y * rhs->_m[1][1] + lhs.z * rhs->_m[2][1] + lhs.w * rhs->_m[3][1];
	v.z = lhs.x * rhs->_m[0][2] + lhs.y * rhs->_m[1][2] + lhs.z * rhs->_m[2][2] + lhs.w * rhs->_m[3][2];
	v.w = lhs.x * rhs->_m[0][3] + lhs.y * rhs->_m[1][3] + lhs.z * rhs->_m[2][3] + lhs.w * rhs->_m[3][3];
	return v;
}

Vector3f operator-(Vector3f lhs, Vector3f rhs)
{
	Vector3f v;
	v.x = lhs.x - rhs.x;
	v.y = lhs.y - rhs.y;
	v.z = lhs.z - rhs.z;
	v.w = 0;
	return v;
}

Vector3f operator+(Vector3f lhs, Vector3f rhs)
{
	Vector3f v;
	v.x = lhs.x + rhs.x;
	v.y = lhs.y + rhs.y;
	v.z = lhs.z + rhs.z;
	v.w = 0;
	return v;
}
