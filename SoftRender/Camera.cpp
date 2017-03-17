#define _USE_MATH_DEFINES
#include "Camera.h"

Camera::Camera() {

}

Camera::~Camera() {

}

Camera *Camera::instance = new Camera();

Camera* Camera::GetInstance() {
	return instance;
}

// ��ȡ������ռ�ת�����۲�ռ�ı任���� ������������ϵ
Matrix4x4* Camera::World2View() {
	Matrix4x4 temp = Matrix4x4( 1, 0, 0, position.x,
						0, 1, 0, position.y,
						0, 0, -1, position.z, // -1��ʾ�������ͷ ���򿴲�������
						0, 0, 0, 1 );
	Matrix4x4 *w2vMat2 = temp.Inverse(); // ����� ��Ϊ����õ�����V2W����ʵ����Ҫ����W2V
	temp.DeleteMatrix4x4();
	return w2vMat2;
}

//Matrix4x4* Camera::World2View() {
//	if (!w2vMat) {
//		w2vMat = new Matrix4x4();
//		Vector3f dir = lookAt - position;
//		Vector3f right = Vector3f::Cross(up, dir);
//		right.Normalize();
//		dir.Normalize();
//		Matrix4x4 t = Matrix4x4
//			(1, 0, 0, position.x,
//			 0, 1, 0, position.y,
//			 0, 0, 1, position.z,
//			 0, 0, 0, 1);
//		Matrix4x4 r = Matrix4x4
//			(right.x, up.x, dir.x, 0,
//			right.y, up.y, dir.y, 0,
//			right.z, up.z, dir.z, 0,
//			0, 0, 0, 1);
//		Matrix4x4 mat;
//		mat.MatrixMul(r, t);
//		Matrix4x4 rightMat = {
//			1,0,0,0,
//			0,1,0,0,
//			0,0,-1,0,
//			0,0,0,1
//		};
//		Matrix4x4 *temp = new Matrix4x4();
//		temp->MatrixMul(rightMat, mat);
//		w2vMat = temp->Inverse();
//	}
//	return w2vMat;
//}

// ��ȡ�ӹ۲�ռ�ת�����ü��ռ��ͶӰ����
Matrix4x4* Camera::View2Projection() {
	float angle = (fov * M_PI) / (180.0f);
	Matrix4x4 *v2pMat = new Matrix4x4();
	v2pMat->SetZero();
	v2pMat->_m[0][0] = 1 / (tan(angle / 2.0f) * aspect);
	v2pMat->_m[1][1] = 1 / tan(angle / 2.0f);
	v2pMat->_m[2][2] = -(farPlan + nearPlan) / (farPlan - nearPlan);
	v2pMat->_m[2][3] = -(2 * nearPlan * farPlan) / (farPlan - nearPlan);
	v2pMat->_m[3][2] = -1;
	return v2pMat;
}
