#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"

// 摄像机作为单例对象
class Camera {
public:
	static Camera* GetInstance();
	Vector3f position = Vector3f(0, 0, -5, 1);	// 坐标
	Vector3f lookAt = Vector3f(0,0,-1,1);
	Vector3f up = Vector3f(0,1,0,1);

	float fov = 60;								// 角度
	float nearPlan = 1;							// 近裁剪面
	float farPlan = 50;							// 远裁剪面
	float aspect = 1.25;						// 宽高比

	Matrix4x4* World2View();
	Matrix4x4* View2Projection();
	~Camera();
private:
	Camera();
	static Camera *instance;
};
