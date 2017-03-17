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
	// ʵ��Ӧ�ôӲ�������������������Ϣ
	RenderCore();
	~RenderCore();

	// ���ݸ���Ҫ��Ⱦ���������ݡ����󻺴桢��Ȼ���
	// ÿ��������д��ʱ������Ȳ��ԣ����Բ�ͨ�����޷�д��
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
	// ��������(��Щ����Ҫע�⵱�ж������ʱ)
	Matrix4x4 *mvpMat;
	Camera *camera;
	Cube *cube;
	byte *backBuffer;
	Color LineColor;
	Texture *pBmpTex;
};