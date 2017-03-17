#pragma once
#include<Windows.h>
#include "Cube.h"
#include "Camera.h"

class IRenderCore {
public:
	virtual void Fragment(int x, int y, float u, float v) = 0;
};

class RenderCore: IRenderCore {
public:
	// 实际应该从参数传入物体和摄像机信息
	RenderCore();
	~RenderCore();

	// 传递给需要渲染的物体数据、背后缓存、深度缓存
	// 每个物体在写入时进行深度测试，测试不通过则无法写入
	void RenderView(byte* buffer, float rotation);

	void VertexPro(VertexData *vd, Vertex v);
	virtual void Fragment(int x, int y, float u, float v);

	void SetBuffer(int x, int y, byte r, byte g, byte b);
	void SetBuffer(int x, int y, Color color);
	int DrawLine(Vector3f v1, Vector3f v2);
	void GetMvpMatrix(Matrix4x4 *mvpMat);
	void EdgeBuffers(VertexData *vertexs);
	void BackFaceCulling(VertexData * vertexs);
private :
	// 变量声明(这些数据要注意当有多个物体时)
	Matrix4x4 *mvpMat;
	Camera *camera;
	Cube *cube;
	byte *backBuffer;
	Color LineColor;
	Texture *pBmpTex;
};