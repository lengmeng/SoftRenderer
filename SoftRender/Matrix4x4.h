#pragma once
#include <math.h>

class Matrix4x4 {
private:
	float Determinate(float **m, int n);
	void InitMatrix4x4();
public:
	float **_m;

	Matrix4x4();

	Matrix4x4(float a1, float a2, float a3, float a4,
		float b1, float b2, float b3, float b4,
		float c1, float c2, float c3, float c4,
		float d1, float d2, float d3, float d4);
	void DeleteMatrix4x4();
	// ��0
	void SetZero();
	// ��λ��
	void Identity();
	// ת��
	void Transpose();
	// ����ʽ
	float Determinate();
	// �������
	Matrix4x4 *GetAdjoint();
	// �����
	Matrix4x4 *Inverse();

	float* operator[](const int i);

	void MatrixMul(Matrix4x4 lhs, Matrix4x4 rhs);

	// ƽ�ƾ���
	static Matrix4x4* TranslationMat(float x, float y, float z);
	// ��ת����
	static Matrix4x4* RotateByX(float angle);
	static Matrix4x4* RotateByY(float angle);
	static Matrix4x4* RotateByZ(float angle);
	// ���ž���
	static Matrix4x4* ScaleMat(float x, float y, float z);

};
