#define _USE_MATH_DEFINES
#include "Matrix4x4.h"
#include "Vector3.h"


void Matrix4x4::MatrixMul(Matrix4x4 lhs, Matrix4x4 rhs){
	this->SetZero();
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				this->_m[i][j] += lhs._m[i][k] * rhs._m[k][j];
			}
		}
	}
}

float Matrix4x4::Determinate(float **m, int n) {
	if (n == 1)
	{//递归出口
		return m[0][0];
	}
	else
	{
		float result = 0;
		float **tempM = new float*[n - 1];
		for (int i = 0; i<n-1; ++i) {
			tempM[i] = new float[n - 1];
		}
		for (int i = 0; i < n; i++)
		{
			//求代数余子式
			for (int j = 0; j < n - 1; j++)//行
			{
				for (int k = 0; k < n - 1; k++)//列
				{
					int x = j + 1;//原矩阵行
					int y = k >= i ? k + 1 : k;//原矩阵列
					tempM[j][k] = m[x][y];
				}
			}

			result += (float)pow(-1, 1 + (1 + i)) * m[0][i] * Determinate(tempM, n - 1);
			
		}
		for (int i = 0; i<n-1; ++i) {
			delete[]tempM[i];
		}
		delete []tempM;
		return result;
	}
}

void  Matrix4x4::InitMatrix4x4() {
	_m = new float*[4];
	for (int i = 0; i<4; ++i) {
		_m[i] = new float[4];
	}
}

Matrix4x4::Matrix4x4() {
	InitMatrix4x4();
}

Matrix4x4::Matrix4x4(float a1, float a2, float a3, float a4,
	float b1, float b2, float b3, float b4,
	float c1, float c2, float c3, float c4,
	float d1, float d2, float d3, float d4) {
	InitMatrix4x4();
	_m[0][0] = a1;  _m[0][1] = a2;  _m[0][2] = a3;  _m[0][3] = a4;
	_m[1][0] = b1;  _m[1][1] = b2;  _m[1][2] = b3;  _m[1][3] = b4;
	_m[2][0] = c1;  _m[2][1] = c2;  _m[2][2] = c3;  _m[2][3] = c4;
	_m[3][0] = d1;  _m[3][1] = d2;  _m[3][2] = d3;  _m[3][3] = d4;
}

void Matrix4x4::DeleteMatrix4x4() {
	for (int i = 0; i<4; ++i) {
		delete[]_m[i];
	}
	delete[] _m;
}
// 置0
void Matrix4x4::SetZero()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			_m[i][j] = 0;
		}
	}
}
// 单位化
void Matrix4x4::Identity()
{
	_m[0][0] = 1; _m[0][1] = 0; _m[0][2] = 0;  _m[0][3] = 0;
	_m[1][0] = 0; _m[1][1] = 1; _m[1][2] = 0;  _m[1][3] = 0;
	_m[2][0] = 0; _m[2][1] = 0; _m[2][2] = 1; _m[2][3] = 0;
	_m[3][0] = 0; _m[3][1] = 0; _m[3][2] = 0; _m[3][3] = 1;
}
// 转置
void Matrix4x4::Transpose()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = i; j < 4; j++)
		{
			float temp = _m[i][j];
			_m[i][j] = _m[j][i];
			_m[j][i] = temp;
		}
	}
}
// 行列式
float Matrix4x4::Determinate()
{
	return Determinate(_m, 4);
}
// 伴随矩阵
Matrix4x4 *Matrix4x4::GetAdjoint()
{
	int x, y;
	float **tempM = new float*[3];
	for (int i = 0; i<3; ++i) {
		tempM[i] = new float[3];
	}
	Matrix4x4 *result = new Matrix4x4();
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				for (int t = 0; t < 3; ++t)
				{
					x = k >= i ? k + 1 : k;
					y = t >= j ? t + 1 : t;

					tempM[k][t] = _m[x][y];
				}
			}
			result->_m[i][j] = (float)pow(-1, (1 + j) + (1 + i)) * Determinate(tempM, 3);
		}
	}

	for (int i = 0; i<3; ++i) {
		delete(tempM[i]);
	}
	delete(tempM);
	result->Transpose();

	return result;
}
// 逆矩阵
Matrix4x4 *Matrix4x4::Inverse()
{
	float a = 1;// Determinate();
	if (a == 0)
	{
		return nullptr;
	}

	Matrix4x4 *adj = GetAdjoint();//伴随矩阵
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			adj->_m[i][j] = adj->_m[i][j] / a;
		}
	}
	return adj;
}

float* Matrix4x4::operator[](const int i)
{
	return _m[i];
}

// 平移矩阵
Matrix4x4* Matrix4x4::TranslationMat(float x, float y, float z) {
	Matrix4x4 *mat = new Matrix4x4();
	mat->SetZero();
	mat->_m[0][0] = 1;
	mat->_m[1][1] = 1;
	mat->_m[2][2] = 1;
	mat->_m[3][3] = 1;

	mat->_m[0][3] = x;
	mat->_m[1][3] = y;
	mat->_m[2][3] = z;
	return mat;
}

// 旋转矩阵
Matrix4x4* Matrix4x4::RotateByX(float angle) {
	float a = (angle * M_PI) / (180.0f);
	Matrix4x4 *mat = new Matrix4x4();
	mat->SetZero();
	mat->_m[0][0] = 1;
	mat->_m[1][1] = cos(a);
	mat->_m[1][2] = -sin(a);
	mat->_m[2][2] = cos(a);
	mat->_m[2][1] = sin(a);
	mat->_m[3][3] = 1;
	return mat;
}

Matrix4x4* Matrix4x4::RotateByY(float angle) {
	float a = (angle * M_PI) / (180.0f);
	Matrix4x4 *mat = new Matrix4x4();
	mat->SetZero();
	mat->_m[0][0] = cos(a);
	mat->_m[1][1] = 1;
	mat->_m[2][2] = cos(a);
	mat->_m[0][2] = sin(a);
	mat->_m[2][0] = -sin(a);
	mat->_m[3][3] = 1;
	return mat;
}

Matrix4x4* Matrix4x4::RotateByZ(float angle) {
	float a = (angle * M_PI) / (180.0f);
	Matrix4x4 *mat = new Matrix4x4();
	mat->SetZero();
	mat->_m[0][0] = cos(a);
	mat->_m[1][1] = cos(a);
	mat->_m[2][2] = 1;
	mat->_m[3][3] = 1;
	mat->_m[0][1] = -sin(a);
	mat->_m[1][0] = sin(a);
	return mat;
}

// 缩放矩阵
Matrix4x4* Matrix4x4::ScaleMat(float x, float y, float z) {
	Matrix4x4 *mat = new Matrix4x4();
	mat->SetZero();
	mat->_m[0][0] = x;
	mat->_m[1][1] = y;
	mat->_m[2][2] = z;
	mat->_m[3][3] = 1;
	return mat;
}