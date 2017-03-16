#include <cmath>
#include <fstream>  
#include "RenderCore.h"
#include "Cube.h"
#include "Camera.h"
#include "Texture.h"
#include "Common.h"

// ��������(��Щ����Ҫע�⵱�ж������ʱ)
Matrix4x4 *mvpMat = new Matrix4x4();
Camera *camera;
Cube *cube;
byte *backBuffer;
Color LineColor = Color(0,0,0);


Texture * readBmp(char *bmpName)
{
	Texture *pBmpTex = new Texture();
	RGBQUAD *pColorTable;//��ɫ��ָ��  

	//unsigned char *pBmpBuf;//����ͼ�����ݵ�ָ��

	FILE *fp;
	fopen_s(&fp, bmpName, "rb");//�����ƶ���ʽ��ָ����ͼ���ļ�  

	if (fp == 0)
		return nullptr;

	//����λͼ�ļ�ͷ�ṹBITMAPFILEHEADER  
	fseek(fp, sizeof(BITMAPFILEHEADER), 0);

	//����λͼ��Ϣͷ�ṹ��������ȡλͼ��Ϣͷ���ڴ棬����ڱ���head��  
	BITMAPINFOHEADER head;

	fread(&head, sizeof(BITMAPINFOHEADER), 1, fp); //��ȡͼ����ߡ�ÿ������ռλ������Ϣ  

	pBmpTex->bmpWidth = head.biWidth;
	pBmpTex->bmpHeight = head.biHeight;
	pBmpTex->biBitCount = head.biBitCount;//�������������ͼ��ÿ��������ռ���ֽ�����������4�ı�����  

	int lineByte = (pBmpTex->bmpWidth * pBmpTex->bmpHeight / 8 + 3) / 4 * 4;//�Ҷ�ͼ������ɫ������ɫ�����Ϊ256  

	if (pBmpTex->biBitCount == 8)
	{
		//������ɫ������Ҫ�Ŀռ䣬����ɫ����ڴ�  
		pColorTable = new RGBQUAD[256];
		fread(pColorTable, sizeof(RGBQUAD), 256, fp);
	}

	//����λͼ��������Ҫ�Ŀռ䣬��λͼ���ݽ��ڴ�  
	pBmpTex->pBmpBuf = new unsigned char[lineByte * pBmpTex->bmpHeight];
	fread(pBmpTex->pBmpBuf, 1, lineByte * pBmpTex->bmpHeight, fp);

	fclose(fp);//�ر��ļ�  

	return pBmpTex;
}

// ��������
void SetBuffer(int x, int y, byte r, byte g, byte b) {
	backBuffer[int(SCREEN_HEIGHT - y) * SCREEN_WIDTH * 3 + (int(x) + 1) * 3 - 1] = r;
	backBuffer[int(SCREEN_HEIGHT - y) * SCREEN_WIDTH * 3 + (int(x) + 1) * 3 - 2] = g;
	backBuffer[int(SCREEN_HEIGHT - y) * SCREEN_WIDTH * 3 + (int(x) + 1) * 3 - 3] = b;
}

void SetBuffer(int x, int y, Color color) {
	backBuffer[int(SCREEN_HEIGHT - y) * SCREEN_WIDTH * 3 + (int(x) + 1) * 3 - 1] = color.r;
	backBuffer[int(SCREEN_HEIGHT - y) * SCREEN_WIDTH * 3 + (int(x) + 1) * 3 - 2] = color.g;
	backBuffer[int(SCREEN_HEIGHT - y) * SCREEN_WIDTH * 3 + (int(x) + 1) * 3 - 3] = color.b;
}

// ����ֱ��
int DrawLine(Vector3f v1, Vector3f v2)
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
void GetMvpMatrix(Matrix4x4 *mvpMat) {
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
void EdgeBuffers(VertexData *vertexs) {
	for (int i = 0; i < cube->vectexCount/3; ++i) {
		int index = i * 3;

		if (vertexs[index].isCull || vertexs[index+1].isCull || vertexs[index+2].isCull)
			continue;

		DrawLine(vertexs[index].pos, vertexs[index+1].pos);
		DrawLine(vertexs[index+1].pos, vertexs[index+2].pos);
		DrawLine(vertexs[index+2].pos, vertexs[index].pos);
	}
}

void AddTexture(VertexData *vertexs, Texture *bmpTex) {
	for (int i = 0; i < cube->vectexCount / 3; ++i) {
		int index = i * 3;

		if (vertexs[index].isCull || vertexs[index + 1].isCull || vertexs[index + 2].isCull)
			continue;
		DrawTriangle(backBuffer, bmpTex, vertexs[index], vertexs[index+1], vertexs[index+2]);
	}
}

// �����޳�
void BackFaceCulling(VertexData * vertexs) {
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

void FrustumCulling(VertexData *vertexs) {
	// ��ʹ����򵥵�CVV�ü�
	for (int i = 0; i < cube->vectexCount; ++i) {
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
	//GetMvpMatrix(mvpMat);
}

RenderCore::~RenderCore() {
	mvpMat->DeleteMatrix4x4();
	delete(camera);
	delete(cube);
	delete(mvpMat);
	
}

// ���ݸ���Ҫ��Ⱦ���������ݡ����󻺴桢��Ȼ���
// ÿ��������д��ʱ������Ȳ��ԣ����Բ�ͨ�����޷�д��
void RenderCore::RenderView(byte* buffer, float rotation) {
	backBuffer = buffer;
	cube->rotation = rotation;

	// ��ȡ����
	Texture *pBmpTex = readBmp("1117.bmp");

	GetMvpMatrix(mvpMat);
	
	// ��cube��������Ķ�����Ϣ  ���������������ж��ڵĶ������
	VertexData *vertexs = new VertexData[cube->vectexCount]; // ������Ϣ

	// �ҳ�MVP���� ת����οռ�
	for (int i = 0; i < cube->vectexCount; ++i) {
		// ���ÿ�������ڲü��ռ��е����� ע�⣺��ʱw�����Ѿ���������
		vertexs[i].pos = cube->Vertexs[i].position * mvpMat;
		vertexs[i].uv = cube->Vertexs[i].uv;
	}

	// �ü�
	FrustumCulling(vertexs);

	// ��γ����õ���Ļ����x,y�����ֵz
	for (int i = 0; i < cube->vectexCount; ++i) {
		// +1 ʹ����Ļ���½�Ϊԭ�������ϵ
		vertexs[i].pos.x = int((vertexs[i].pos.x / vertexs[i].pos.w + 1) * (SCREEN_WIDTH/2));
		vertexs[i].pos.y = int((vertexs[i].pos.y / vertexs[i].pos.w + 1) * (SCREEN_HEIGHT/2));
		vertexs[i].pos.z = vertexs[i].pos.z / vertexs[i].pos.w;
	}

	// �����޳�(��ֹ������ʹ����Ļ������м���)
	BackFaceCulling(vertexs);

	// �������
	AddTexture(vertexs, pBmpTex);

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
