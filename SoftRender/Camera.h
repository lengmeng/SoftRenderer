#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"

// �������Ϊ��������
class Camera {
public:
	static Camera* GetInstance();
	Vector3f position = Vector3f(0, 0, -5, 1);	// ����
	Vector3f lookAt = Vector3f(0,0,-1,1);
	Vector3f up = Vector3f(0,1,0,1);

	float fov = 60;								// �Ƕ�
	float nearPlan = 1;							// ���ü���
	float farPlan = 50;							// Զ�ü���
	float aspect = 1.25;						// ��߱�

	Matrix4x4* World2View();
	Matrix4x4* View2Projection();
	~Camera();
private:
	Camera();
	static Camera *instance;
};
