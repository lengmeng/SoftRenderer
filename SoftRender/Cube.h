#pragma once
#include "Vector3.h"
#include "Common.h"

// ÿ��Cube�Բ�����ʽ���ݸ�Renderȥ��Ⱦ
// ���������£���ΪM2W�ı任����Ӧ�����������ṩ
// ��ΪҲֻ�ǲ���ģʽ����ÿ��ģ���Դ�Vertex��Ϣ����
class Cube {
public:
	float scale = 1.0f;
	float rotation = -45;
	char * textureName = "1117.bmp";

	int vectexCount = 12 * 3;
	// �����ζ��㣨ģ�Ϳռ����꣩
	//	Vector3f(-1,1,-1,1), Vector3f(1,1,-1,1), Vector3f(1,-1,-1,1), Vector3f(-1,-1,-1,1)
	//	Vector3f(-1,1,1,1),  Vector3f(1,1,1,1),  Vector3f(1,-1,1,1),  Vector3f(-1,-1,1,1)

	// ʵ����ģ���������������ʽ���ڣ�������֮��ű���Ϊ������
	Vertex Vertexs[12 * 3] = {
		// 1,2,3,3,4,1, ǰ
		Vertex(-1,1,-1,0,0), Vertex(1,1,-1,0,1), Vertex(1,-1,-1,1,1),
		Vertex(1,-1,-1,1,1), Vertex(-1,-1,-1,1,0), Vertex(-1,1,-1,0,0),
		// 1,4,5,4,8,5, ��
		Vertex(-1,1,-1,0,0), Vertex(-1,-1,-1,0,1), Vertex(-1,1,1,1,0),
		Vertex(-1,-1,-1,0,1), Vertex(-1,-1,1,1,1), Vertex(-1,1,1,1,0),
		// 1,5,6,6,2,1, ��
		Vertex(-1,1,-1,0,0), Vertex(-1,1,1,1,0), Vertex(1,1,1,1,1),
		Vertex(1,1,1,1,1), Vertex(1,1,-1,0,1), Vertex(-1,1,-1,0,0),
		// 3,7,4,7,8,4, ��
		Vertex(1,-1,-1,0,1), Vertex(1,-1,1,1,1), Vertex(-1,-1,-1,0,0),
		Vertex(1,-1,1,1,1), Vertex(-1,-1,1,1,0), Vertex(-1,-1,-1,0,0),
		// 2,6,7,7,3,2,  ��
		Vertex(1,1,-1,0,0), Vertex(1,1,1,0,1), Vertex(1,-1,1,1,1),
		Vertex(1,-1,1,1,1), Vertex(1,-1,-1,1,0), Vertex(1,1,-1,0,0),
		// 5,7,6,5,8,7 ��
		Vertex(-1,1,1,0,1), Vertex(1,-1,1,1,0), Vertex(1,1,1,0,0),
		Vertex(-1,1,1,0,1), Vertex(-1,-1,1,1,1), Vertex(1,-1,1,1,0),
	};

	// ���������꣨�ռ������꣩
	Vector3f position = Vector3f(0,-2,5,1); 



	// ����������ϵԭ���������ת�������ռ�ת����������ת�����ţ�
	Matrix4x4* Model2World() {
		// ƽ��
		Matrix4x4 *translationMat = new Matrix4x4
			(1, 0, 0, position.x,
			0, 1, 0, position.y,
			0, 0, 1, position.z,
			0, 0, 0, 1);
		return translationMat;
	}

	void Model2World2(Matrix4x4* result) {
		result->SetZero();
		// ƽ�� ��ת(Y��) ����
		Matrix4x4 *translationMat = Matrix4x4::TranslationMat(position.x, position.y, position.z);
		Matrix4x4 *rotationMat = Matrix4x4::RotateByY(rotation);
		Matrix4x4 *scaleMat = Matrix4x4::ScaleMat(scale, scale, scale);

		Matrix4x4 *temp = new Matrix4x4();
		temp->MatrixMul(*translationMat, *rotationMat);
		// ����õ��ľ���C->P Ҳ���Ǵ�ģ�Ϳռ�ת������ռ� ����Ҫ��ת �мǲ�Ҫ����
		result->MatrixMul(*temp, *scaleMat); 

		translationMat->DeleteMatrix4x4();
		rotationMat->DeleteMatrix4x4();
		scaleMat->DeleteMatrix4x4();
		temp->DeleteMatrix4x4();
		delete(translationMat);
		delete(rotationMat);
		delete(scaleMat);
		delete(temp);
	}
};
