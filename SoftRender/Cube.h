#pragma once
#include "Vector3.h"
#include "Common.h"

// 每个Cube以参数形式传递给Render去渲染
// 昨晚想了下，认为M2W的变换矩阵应该由物体来提供
// 因为也只是测试模式，让每个模型自带Vertex信息好了
class Cube {
public:
	float scale = 1.0f;
	float rotation = -45;
	char * textureName = "1117.bmp";

	int vectexCount = 12 * 3;
	// 正方形顶点（模型空间坐标）
	//	Vector3f(-1,1,-1,1), Vector3f(1,1,-1,1), Vector3f(1,-1,-1,1), Vector3f(-1,-1,-1,1)
	//	Vector3f(-1,1,1,1),  Vector3f(1,1,1,1),  Vector3f(1,-1,1,1),  Vector3f(-1,-1,1,1)

	// 实际上模型是以三角面的形式存在，而不是之后才被拆为三角面
	Vertex Vertexs[12 * 3] = {
		// 1,2,3,3,4,1, 前
		Vertex(-1,1,-1,0,0), Vertex(1,1,-1,0,1), Vertex(1,-1,-1,1,1),
		Vertex(1,-1,-1,1,1), Vertex(-1,-1,-1,1,0), Vertex(-1,1,-1,0,0),
		// 1,4,5,4,8,5, 左
		Vertex(-1,1,-1,0,0), Vertex(-1,-1,-1,0,1), Vertex(-1,1,1,1,0),
		Vertex(-1,-1,-1,0,1), Vertex(-1,-1,1,1,1), Vertex(-1,1,1,1,0),
		// 1,5,6,6,2,1, 上
		Vertex(-1,1,-1,0,0), Vertex(-1,1,1,1,0), Vertex(1,1,1,1,1),
		Vertex(1,1,1,1,1), Vertex(1,1,-1,0,1), Vertex(-1,1,-1,0,0),
		// 3,7,4,7,8,4, 下
		Vertex(1,-1,-1,0,1), Vertex(1,-1,1,1,1), Vertex(-1,-1,-1,0,0),
		Vertex(1,-1,1,1,1), Vertex(-1,-1,1,1,0), Vertex(-1,-1,-1,0,0),
		// 2,6,7,7,3,2,  右
		Vertex(1,1,-1,0,0), Vertex(1,1,1,0,1), Vertex(1,-1,1,1,1),
		Vertex(1,-1,1,1,1), Vertex(1,-1,-1,1,0), Vertex(1,1,-1,0,0),
		// 5,7,6,5,8,7 后
		Vertex(-1,1,1,0,1), Vertex(1,-1,1,1,0), Vertex(1,1,1,0,0),
		Vertex(-1,1,1,0,1), Vertex(-1,-1,1,1,1), Vertex(1,-1,1,1,0),
	};

	// 正方形坐标（空间中坐标）
	Vector3f position = Vector3f(0,-2,5,1); 



	// 根据子坐标系原点坐标和旋转获得坐标空间转换矩阵（无旋转和缩放）
	Matrix4x4* Model2World() {
		// 平移
		Matrix4x4 *translationMat = new Matrix4x4
			(1, 0, 0, position.x,
			0, 1, 0, position.y,
			0, 0, 1, position.z,
			0, 0, 0, 1);
		return translationMat;
	}

	void Model2World2(Matrix4x4* result) {
		result->SetZero();
		// 平移 旋转(Y轴) 缩放
		Matrix4x4 *translationMat = Matrix4x4::TranslationMat(position.x, position.y, position.z);
		Matrix4x4 *rotationMat = Matrix4x4::RotateByY(rotation);
		Matrix4x4 *scaleMat = Matrix4x4::ScaleMat(scale, scale, scale);

		Matrix4x4 *temp = new Matrix4x4();
		temp->MatrixMul(*translationMat, *rotationMat);
		// 这里得到的就是C->P 也就是从模型空间转到世界空间 不需要逆转 切记不要混淆
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
