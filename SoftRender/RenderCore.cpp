#include <cmath>
#include <fstream>  
#include "RenderCore.h"
#include "Cube.h"
#include "Camera.h"
#include "Texture.h"
#include "Common.h"

// 变量声明(这些数据要注意当有多个物体时)
Matrix4x4 *mvpMat = new Matrix4x4();
Camera *camera;
Cube *cube;
byte *backBuffer;
Color LineColor = Color(0,0,0);


Texture * readBmp(char *bmpName)
{
	Texture *pBmpTex = new Texture();
	RGBQUAD *pColorTable;//颜色表指针  

	//unsigned char *pBmpBuf;//读入图像数据的指针

	FILE *fp;
	fopen_s(&fp, bmpName, "rb");//二进制读方式打开指定的图像文件  

	if (fp == 0)
		return nullptr;

	//跳过位图文件头结构BITMAPFILEHEADER  
	fseek(fp, sizeof(BITMAPFILEHEADER), 0);

	//定义位图信息头结构变量，读取位图信息头进内存，存放在变量head中  
	BITMAPINFOHEADER head;

	fread(&head, sizeof(BITMAPINFOHEADER), 1, fp); //获取图像宽、高、每像素所占位数等信息  

	pBmpTex->bmpWidth = head.biWidth;
	pBmpTex->bmpHeight = head.biHeight;
	pBmpTex->biBitCount = head.biBitCount;//定义变量，计算图像每行像素所占的字节数（必须是4的倍数）  

	int lineByte = (pBmpTex->bmpWidth * pBmpTex->bmpHeight / 8 + 3) / 4 * 4;//灰度图像有颜色表，且颜色表表项为256  

	if (pBmpTex->biBitCount == 8)
	{
		//申请颜色表所需要的空间，读颜色表进内存  
		pColorTable = new RGBQUAD[256];
		fread(pColorTable, sizeof(RGBQUAD), 256, fp);
	}

	//申请位图数据所需要的空间，读位图数据进内存  
	pBmpTex->pBmpBuf = new unsigned char[lineByte * pBmpTex->bmpHeight];
	fread(pBmpTex->pBmpBuf, 1, lineByte * pBmpTex->bmpHeight, fp);

	fclose(fp);//关闭文件  

	return pBmpTex;
}

// 绘制像素
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

// 绘制直线
int DrawLine(Vector3f v1, Vector3f v2)
{
	int x1 = v1.x, x2 = v2.x, y1 = v1.y, y2 = v2.y;

	int dx = x2 - x1;
	int dy = y2 - y1;
	int ux = ((dx > 0) << 1) - 1;//x的增量方向，取或-1
	int uy = ((dy > 0) << 1) - 1;//y的增量方向，取或-1
	int x = x1, y = y1, eps;//eps为累加误差

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


// 获取MVP矩阵 先计算矩阵总乘积，再倒置与向量右乘 能够得到正确的结果
void GetMvpMatrix(Matrix4x4 *mvpMat) {
	// v2p * w2v * m2w * p
	Matrix4x4 mvMat;
	Matrix4x4 *m2wMat = new Matrix4x4();
	cube->Model2World2(m2wMat);

	Matrix4x4 *w2vMat = camera->World2View();
	// 以下存在内存泄漏
	Matrix4x4 *v2pMat = camera->View2Projection();

	mvMat.MatrixMul(*w2vMat, *m2wMat); 
	mvpMat->MatrixMul(*v2pMat, mvMat);
	mvpMat->Transpose(); // 倒置矩阵 因为将右乘变为左乘

	mvMat.DeleteMatrix4x4();
	m2wMat->DeleteMatrix4x4();
	w2vMat->DeleteMatrix4x4();
	v2pMat->DeleteMatrix4x4();
	delete(m2wMat);
	delete(w2vMat);
	delete(v2pMat);
}


// 绘制三角边
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

// 背部剔除
void BackFaceCulling(VertexData * vertexs) {
	int triangleCount = cube->vectexCount / 3;
	for (int i = 0; i < triangleCount; ++i) {
		int index = i * 3;
		// 先使用简单的计算
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
	// 先使用最简单的CVV裁剪
	for (int i = 0; i < cube->vectexCount; ++i) {
		Vector3f v = vertexs[i].pos;
		if (!(-abs(v.w) <= v.x && v.x <= abs(v.w)
			&& -abs(v.w) <= v.y && v.y <= abs(v.w)
			&& -abs(v.w) <= v.z && v.z <= abs(v.w))) {
			vertexs[i].isCull = true;
		}
	}
}

// 实际应该从参数传入物体和摄像机信息
RenderCore::RenderCore() {
	cube = new Cube(); // 正方形 暂时这里生成
	camera = Camera::GetInstance();
	//GetMvpMatrix(mvpMat);
}

RenderCore::~RenderCore() {
	mvpMat->DeleteMatrix4x4();
	delete(camera);
	delete(cube);
	delete(mvpMat);
	
}

// 传递给需要渲染的物体数据、背后缓存、深度缓存
// 每个物体在写入时进行深度测试，测试不通过则无法写入
void RenderCore::RenderView(byte* buffer, float rotation) {
	backBuffer = buffer;
	cube->rotation = rotation;

	// 读取纹理
	Texture *pBmpTex = readBmp("1117.bmp");

	GetMvpMatrix(mvpMat);
	
	// 从cube获得完整的顶点信息  现在这种做法会有多于的顶点存在
	VertexData *vertexs = new VertexData[cube->vectexCount]; // 顶点信息

	// 右乘MVP矩阵 转入齐次空间
	for (int i = 0; i < cube->vectexCount; ++i) {
		// 获得每个顶点在裁剪空间中的坐标 注意：此时w分量已经有了意义
		vertexs[i].pos = cube->Vertexs[i].position * mvpMat;
		vertexs[i].uv = cube->Vertexs[i].uv;
	}

	// 裁剪
	FrustumCulling(vertexs);

	// 齐次除法得到屏幕坐标x,y，深度值z
	for (int i = 0; i < cube->vectexCount; ++i) {
		// +1 使用屏幕左下角为原点的坐标系
		vertexs[i].pos.x = int((vertexs[i].pos.x / vertexs[i].pos.w + 1) * (SCREEN_WIDTH/2));
		vertexs[i].pos.y = int((vertexs[i].pos.y / vertexs[i].pos.w + 1) * (SCREEN_HEIGHT/2));
		vertexs[i].pos.z = vertexs[i].pos.z / vertexs[i].pos.w;
	}

	// 背部剔除(防止浮点误差，使用屏幕坐标进行计算)
	BackFaceCulling(vertexs);

	// 添加纹理
	AddTexture(vertexs, pBmpTex);

	//描边（还缺少绘制时的深度测试）
	//EdgeBuffers(vertexs);

	// 测试 画顶点
	//for (int i = 0; i < cube->vectexCount; ++i) {
	//	SetBuffer((int)vertexs[i].pos.x, (int)vertexs[i].pos.y, 0, 0, 0);
	//}
	
	delete(vertexs);
	delete(pBmpTex->pBmpBuf);
	delete(pBmpTex);
}
