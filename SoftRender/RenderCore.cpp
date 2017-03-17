#include <cmath>
#include <fstream>  
#include "RenderCore.h"
#include "RenderTexture.h"
#include "Common.h"
#include "LoadTexture.h"


// �������
// ��ȡpmbBuff��ɫֵ (����λ������ ʵ�ֺ��ٿ�����) 
Color GetColor(Texture *pBmpTex, float rx, float ry) {
	int x = rx * pBmpTex->bmpWidth;
	int y = ry * pBmpTex->bmpHeight;

	int lineByte = (pBmpTex->bmpWidth*pBmpTex->biBitCount / 8 + 3) / 4 * 4;

	//��ɫͼ��
	Color c = Color(
		*(pBmpTex->pBmpBuf + x*lineByte + y * 3 + 2),
		*(pBmpTex->pBmpBuf + x*lineByte + y * 3 + 1),
		*(pBmpTex->pBmpBuf + x*lineByte + y * 3));

	return c;
}

// ��������
void RenderCore::SetBuffer(int x, int y, byte r, byte g, byte b) {
	backBuffer[int(SCREEN_HEIGHT - y) * SCREEN_WIDTH * 3 + (int(x) + 1) * 3 - 1] = r;
	backBuffer[int(SCREEN_HEIGHT - y) * SCREEN_WIDTH * 3 + (int(x) + 1) * 3 - 2] = g;
	backBuffer[int(SCREEN_HEIGHT - y) * SCREEN_WIDTH * 3 + (int(x) + 1) * 3 - 3] = b;
}

void RenderCore::SetBuffer(int x, int y, Color color) {
	backBuffer[int(SCREEN_HEIGHT - y) * SCREEN_WIDTH * 3 + (int(x) + 1) * 3 - 1] = color.r;
	backBuffer[int(SCREEN_HEIGHT - y) * SCREEN_WIDTH * 3 + (int(x) + 1) * 3 - 2] = color.g;
	backBuffer[int(SCREEN_HEIGHT - y) * SCREEN_WIDTH * 3 + (int(x) + 1) * 3 - 3] = color.b;
}

// ����ֱ��
int RenderCore::DrawLine(Vector3f v1, Vector3f v2)
{
	int x1 = v1.x, x2 = v2.x, y1 = v1.y, y2 = v2.y;

	int dx = x2 - x1;
	int dy = y2 - y1;
	int ux = ((dx > 0) << 1) - 1;//x����������ȡ��-1
	int uy = ((dy > 0) << 1) - 1;//y����������ȡ��-1
	int x = x1, y = y1, eps;//epsΪ�ۼ����

	eps = 0; dx = abs(dx); dy = abs(dy);
	if (dx > dy)
	{
		for (x = x1; x != x2; x += ux)
		{
			SetBuffer(x, y, LineColor);
			eps += dy;
			if ((eps << 1) >= dx)
			{
				y += uy; eps -= dx;
			}
		}
	}
	else
	{
		for (y = y1; y != y2; y += uy)
		{
			SetBuffer(x, y, LineColor);
			eps += dx;
			if ((eps << 1) >= dy)
			{
				x += ux; eps -= dy;
			}
		}
	}
	return 1;
}


// ��ȡMVP���� �ȼ�������ܳ˻����ٵ����������ҳ� �ܹ��õ���ȷ�Ľ��
void RenderCore::GetMvpMatrix(Matrix4x4 *mvpMat) {
	// v2p * w2v * m2w * p
	Matrix4x4 mvMat;
	Matrix4x4 *m2wMat = new Matrix4x4(); 
	cube->Model2World2(m2wMat);

	Matrix4x4 *w2vMat = camera->World2View();
	// ���´����ڴ�й©
	Matrix4x4 *v2pMat = camera->View2Projection();

	mvMat.MatrixMul(*w2vMat, *m2wMat); 
	mvpMat->MatrixMul(*v2pMat, mvMat);
	mvpMat->Transpose(); // ���þ��� ��Ϊ���ҳ˱�Ϊ���

	mvMat.DeleteMatrix4x4();
	m2wMat->DeleteMatrix4x4();
	w2vMat->DeleteMatrix4x4();
	v2pMat->DeleteMatrix4x4();
	delete(m2wMat);
	delete(w2vMat);
	delete(v2pMat);
}

// �������Ǳ�
void RenderCore::EdgeBuffers(VertexData *vertexs) {
	for (int i = 0; i < cube->vectexCount/3; ++i) {
		int index = i * 3;

		if (vertexs[index].isCull || vertexs[index+1].isCull || vertexs[index+2].isCull)
			continue;

		DrawLine(vertexs[index].pos, vertexs[index+1].pos);
		DrawLine(vertexs[index+1].pos, vertexs[index+2].pos);
		DrawLine(vertexs[index+2].pos, vertexs[index].pos);
	}
}


// �����޳�
void RenderCore::BackFaceCulling(VertexData * vertexs) {
	int triangleCount = cube->vectexCount / 3;
	for (int i = 0; i < triangleCount; ++i) {
		int index = i * 3;
		// ��ʹ�ü򵥵ļ���
		int c = (vertexs[index + 2].pos.x - vertexs[index].pos.x)*(vertexs[index + 1].pos.y - vertexs[index].pos.y) +
			(vertexs[index + 2].pos.y - vertexs[index].pos.y) * (vertexs[index].pos.x - vertexs[index + 1].pos.x);
		if (c <= 0) {
			vertexs[index].isCull = true;
			vertexs[index + 1].isCull = true;
			vertexs[index + 2].isCull = true;
		}
	}
}

// ��βü�
void FrustumCulling(VertexData *vertexs, int vectexCount) {
	// ��ʹ����򵥵�CVV�ü�
	for (int i = 0; i < vectexCount; ++i) {
		Vector3f v = vertexs[i].pos;
		if (!(-abs(v.w) <= v.x && v.x <= abs(v.w)
			&& -abs(v.w) <= v.y && v.y <= abs(v.w)
			&& -abs(v.w) <= v.z && v.z <= abs(v.w))) {
			vertexs[i].isCull = true;
		}
	}
}

// ʵ��Ӧ�ôӲ�������������������Ϣ
RenderCore::RenderCore() {
	cube = new Cube(); // ������ ��ʱ��������
	camera = Camera::GetInstance();
	mvpMat = new Matrix4x4();
	LineColor = Color(0, 0, 0);
	GetMvpMatrix(mvpMat);
}

RenderCore::~RenderCore() {
	delete camera;
	delete(cube);

	mvpMat->DeleteMatrix4x4();
	delete(mvpMat);
}

// ���ݸ���Ҫ��Ⱦ���������ݡ����󻺴桢��Ȼ���
// ÿ��������д��ʱ������Ȳ��ԣ����Բ�ͨ�����޷�д��
void RenderCore::RenderView(byte* buffer, float rotation) {
	backBuffer = buffer;
	cube->rotation = rotation;
	int vecCount = cube->vectexCount;
	// ��ȡ����
	pBmpTex = readBmp(cube->textureName);

	GetMvpMatrix(mvpMat);
	
	// ��cube��������Ķ�����Ϣ  ���������������ж��ڵĶ������
	VertexData *vertexs = new VertexData[vecCount]; // ������Ϣ

	// ���������ݽ��ɶ��㴦��
	for (int i = 0; i < vecCount; ++i) {
		VertexPro(&(vertexs[i]), cube->Vertexs[i]);
	}

	// �ü�
	FrustumCulling(vertexs, vecCount);

	// ��γ����õ���Ļ����x,y�����ֵz
	for (int i = 0; i < vecCount; ++i) {
		// +1 ʹ����Ļ���½�Ϊԭ�������ϵ
		vertexs[i].pos.x = int((vertexs[i].pos.x / vertexs[i].pos.w + 1) * (SCREEN_WIDTH/2));
		vertexs[i].pos.y = int((vertexs[i].pos.y / vertexs[i].pos.w + 1) * (SCREEN_HEIGHT/2));
		vertexs[i].pos.z = vertexs[i].pos.z / vertexs[i].pos.w;
	}

	// �����޳�(��ֹ������ʹ����Ļ������м���)
	BackFaceCulling(vertexs);

	// �������
	RenderTexture *rt = new RenderTexture(this);
	rt->AddTexture(vertexs, pBmpTex, vecCount);

	//��ߣ���ȱ�ٻ���ʱ����Ȳ��ԣ�
	//EdgeBuffers(vertexs);

	// ���� ������
	//for (int i = 0; i < cube->vectexCount; ++i) {
	//	SetBuffer((int)vertexs[i].pos.x, (int)vertexs[i].pos.y, 0, 0, 0);
	//}
	
	delete(vertexs);
	delete(pBmpTex->pBmpBuf);
	delete(pBmpTex);
}

// ���㴦����
void RenderCore::VertexPro(VertexData *vd, Vertex v) {
	vd->pos = v.position * mvpMat;
	vd->uv = v.uv;


}

// ƬԪ������ ��Ļ����x,y ��������u,v
void RenderCore::Fragment(int x, int y, float u, float v) {
	Color c = GetColor(pBmpTex, u, v);
	SetBuffer(x,  y, c);
}

