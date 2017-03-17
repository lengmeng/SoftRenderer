#include <cmath>
#include <fstream>  
#include "RenderCore.h"
#include "RenderTexture.h"
#include "Common.h"
#include "LoadTexture.h"


// 纹理采样
// 获取pmbBuff颜色值 (坐标位置问题 实现后再考虑下) 
Color GetColor(Texture *pBmpTex, float rx, float ry) {
	int x = rx * pBmpTex->bmpWidth;
	int y = ry * pBmpTex->bmpHeight;

	int lineByte = (pBmpTex->bmpWidth*pBmpTex->biBitCount / 8 + 3) / 4 * 4;

	//彩色图像
	Color c = Color(
		*(pBmpTex->pBmpBuf + x*lineByte + y * 3 + 2),
		*(pBmpTex->pBmpBuf + x*lineByte + y * 3 + 1),
		*(pBmpTex->pBmpBuf + x*lineByte + y * 3));

	return c;
}

// 绘制像素
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

// 绘制直线
int RenderCore::DrawLine(Vector3f v1, Vector3f v2)
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
void RenderCore::GetMvpMatrix(Matrix4x4 *mvpMat) {
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


// 背部剔除
void RenderCore::BackFaceCulling(VertexData * vertexs) {
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

// 齐次裁剪
void FrustumCulling(VertexData *vertexs, int vectexCount) {
	// 先使用最简单的CVV裁剪
	for (int i = 0; i < vectexCount; ++i) {
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

// 传递给需要渲染的物体数据、背后缓存、深度缓存
// 每个物体在写入时进行深度测试，测试不通过则无法写入
void RenderCore::RenderView(byte* buffer, float rotation) {
	backBuffer = buffer;
	cube->rotation = rotation;
	int vecCount = cube->vectexCount;
	// 读取纹理
	pBmpTex = readBmp(cube->textureName);

	GetMvpMatrix(mvpMat);
	
	// 从cube获得完整的顶点信息  现在这种做法会有多于的顶点存在
	VertexData *vertexs = new VertexData[vecCount]; // 顶点信息

	// 将顶点数据交由顶点处理
	for (int i = 0; i < vecCount; ++i) {
		VertexPro(&(vertexs[i]), cube->Vertexs[i]);
	}

	// 裁剪
	FrustumCulling(vertexs, vecCount);

	// 齐次除法得到屏幕坐标x,y，深度值z
	for (int i = 0; i < vecCount; ++i) {
		// +1 使用屏幕左下角为原点的坐标系
		vertexs[i].pos.x = int((vertexs[i].pos.x / vertexs[i].pos.w + 1) * (SCREEN_WIDTH/2));
		vertexs[i].pos.y = int((vertexs[i].pos.y / vertexs[i].pos.w + 1) * (SCREEN_HEIGHT/2));
		vertexs[i].pos.z = vertexs[i].pos.z / vertexs[i].pos.w;
	}

	// 背部剔除(防止浮点误差，使用屏幕坐标进行计算)
	BackFaceCulling(vertexs);

	// 添加纹理
	RenderTexture *rt = new RenderTexture(this);
	rt->AddTexture(vertexs, pBmpTex, vecCount);

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

// 顶点处理器
void RenderCore::VertexPro(VertexData *vd, Vertex v) {
	vd->pos = v.position * mvpMat;
	vd->uv = v.uv;


}

// 片元处理器 屏幕坐标x,y 纹理坐标u,v
void RenderCore::Fragment(int x, int y, float u, float v) {
	Color c = GetColor(pBmpTex, u, v);
	SetBuffer(x,  y, c);
}

