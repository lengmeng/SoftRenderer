#pragma once
#include "Common.h"
// 只针对平底或平顶三角面，若是其他形状 则需要进行切割（目前cube都是平头和平底三角面 暂用）

// 绘制像素
void SetPixel(byte *backBuffer, int x, int y, byte r, byte g, byte b) {
	backBuffer[int(640 - y) * 800 * 3 + (int(x) + 1) * 3 - 1] = r;
	backBuffer[int(640 - y) * 800 * 3 + (int(x) + 1) * 3 - 2] = g;
	backBuffer[int(640 - y) * 800 * 3 + (int(x) + 1) * 3 - 3] = b;
}
void SetPixel(byte *backBuffer, int x, int y, Color color) {
	backBuffer[int(640 - y) * 800 * 3 + (int(x) + 1) * 3 - 1] = color.r;
	backBuffer[int(640 - y) * 800 * 3 + (int(x) + 1) * 3 - 2] = color.g;
	backBuffer[int(640 - y) * 800 * 3 + (int(x) + 1) * 3 - 3] = color.b;
}


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

// 绘制平底三角纹理 （顶 左下 右下）下
void RenderTexture1(byte *backBuffer, Texture *pBmpTex, VertexData vd1, VertexData vd2, VertexData vd3) {
	double x, y, xleft, xright;				// 插值x和y，左右线段x
	double oneoverz_left, oneoverz_right;	// 左右线段1/z
	double oneoverz_top, oneoverz_bottom;	// 上下顶点1/z
	double oneoverz, oneoverz_step;			// 插值1/z以及扫描线步长

	double soverz_top, soverz_bottom;		// 上下顶点s/z
	double toverz_top, toverz_bottom;		// 上下顶点t/z
	double soverz_left, soverz_right;		// 左右线段s/z
	double toverz_left, toverz_right;		// 左右线段t/z

	double soverz, soverz_step;				// 插值s/z以及扫描线步长
	double toverz, toverz_step;				// 插值t/z以及扫描线步长

	double s, t;							// 要求的原始s和t


	VertexData leftBottom, rightBottm, topVertex = vd1;
	if (vd2.pos.x > vd3.pos.x) {
		leftBottom = vd3;
		rightBottm = vd2;
	}
	else {
		leftBottom = vd2;
		rightBottm = vd3;
	}

	for (int y = leftBottom.pos.y; y < topVertex.pos.y; y++) {
		xleft = topVertex.pos.x + (topVertex.pos.x - leftBottom.pos.x) / (topVertex.pos.y - leftBottom.pos.y) * (y - topVertex.pos.y);
		xright = topVertex.pos.x + (topVertex.pos.x - rightBottm.pos.x) / (topVertex.pos.y - rightBottm.pos.y) * (y - topVertex.pos.y);

		// 根据证得的x,y与深度topVertex的线性关系：1/z = Ax + B 1/z = Ay + B
		oneoverz_top = 1.0 / topVertex.pos.z;		// 上方顶点的1/z
		oneoverz_bottom = 1.0 / leftBottom.pos.z;	// 左下方顶点的1/z

		oneoverz_left = (y - topVertex.pos.y) * (oneoverz_bottom - oneoverz_top) /
			(leftBottom.pos.y - topVertex.pos.y) + oneoverz_top; // 左边线段与y线交点的1/z

		oneoverz_bottom = 1.0 / rightBottm.pos.z;	// 右边下方顶点的1/z

		oneoverz_right = (y - topVertex.pos.y) * (oneoverz_bottom - oneoverz_top) /
			(rightBottm.pos.y - topVertex.pos.y) + oneoverz_top;	// 右边相对与y线交点的1/z

		// （当前y值对应的线上）x增量与1/z增量之比，用于计算和绘制出这一线 
		oneoverz_step = (oneoverz_right - oneoverz_left) / (xright - xleft);

		// 已证得纹理坐标s，t与x，y存在线性关系 
		// 顶点纹理UV(s0,t0),左下顶点纹理UV(s1,t1)，右下顶点纹理UV(s2,t2)
		float s0 = topVertex.uv.x, t0 = topVertex.uv.y, s1 = leftBottom.uv.x, t1 = leftBottom.uv.y, s2 = rightBottm.uv.x, t2 = rightBottm.uv.y;

		soverz_top = s0 / topVertex.pos.z;
		soverz_bottom = s1 / leftBottom.pos.z;
		soverz_left = (y - topVertex.pos.y) * (soverz_bottom - soverz_top) / (leftBottom.pos.y - topVertex.pos.y) + soverz_top;
		soverz_bottom = s2 / rightBottm.pos.z;
		soverz_right = (y - topVertex.pos.y) * (soverz_bottom - soverz_top) / (rightBottm.pos.y - topVertex.pos.y) + soverz_top;
		soverz_step = (soverz_right - soverz_left) / (xright - xleft);
		toverz_top = t0 / topVertex.pos.z;
		toverz_bottom = t1 / leftBottom.pos.z;
		toverz_left = (y - topVertex.pos.y) * (toverz_bottom - toverz_top) / (leftBottom.pos.y - topVertex.pos.y) + toverz_top;
		toverz_bottom = t2 / rightBottm.pos.z;
		toverz_right = (y - topVertex.pos.y) * (toverz_bottom - toverz_top) / (rightBottm.pos.y - topVertex.pos.y) + toverz_top;
		toverz_step = (toverz_right - toverz_left) / (xright - xleft);

		// 以计算出随x递增而增加的s/z，t/z和1/z的比，因此可以计算出它们
		for (x = xleft, oneoverz = oneoverz_left,
			soverz = soverz_left, toverz = toverz_left;
			x < xright; ++x, oneoverz += oneoverz_step,
			soverz += soverz_step, toverz += toverz_step)
		{
			s = soverz / oneoverz; // (s/z)/(1/z) = s
			t = toverz / oneoverz;
			Color c = GetColor(pBmpTex, s, t);
			SetPixel(backBuffer, x, y, c);
			//帧缓冲像素[x, y] = 纹理[s, t];
		}
	}
}

// 绘制平顶三角形（下 左上  右上）
void RenderTexture2(byte *backBuffer, Texture *pBmpTex, VertexData vd1, VertexData vd2, VertexData vd3) {
	double x, y, xleft, xright;				// 插值x和y，左右线段x
	double oneoverz_left, oneoverz_right;	// 左右线段1/z
	double oneoverz_top, oneoverz_bottom;	// 上下顶点1/z
	double oneoverz, oneoverz_step;			// 插值1/z以及扫描线步长

	double soverz_top, soverz_bottom;		// 上下顶点s/z
	double toverz_top, toverz_bottom;		// 上下顶点t/z
	double soverz_left, soverz_right;		// 左右线段s/z
	double toverz_left, toverz_right;		// 左右线段t/z

	double soverz, soverz_step;				// 插值s/z以及扫描线步长
	double toverz, toverz_step;				// 插值t/z以及扫描线步长

	double s, t;							// 要求的原始s和t

	VertexData leftTop, rightTop, bottomVertex = vd1;

	if (vd2.pos.x > vd3.pos.x) {
		leftTop = vd3;
		rightTop = vd2;
	}
	else {
		leftTop = vd2;
		rightTop = vd3;
	}

	for (int y = bottomVertex.pos.y; y < leftTop.pos.y; y++) {
		// x0y0是顶部顶点，x1y1是左下顶点、x2y2是右下顶点
		xleft = leftTop.pos.x + (leftTop.pos.x - bottomVertex.pos.x) / (leftTop.pos.y - bottomVertex.pos.y) * (y - leftTop.pos.y);
		xright = rightTop.pos.x + (rightTop.pos.x - bottomVertex.pos.x) / (rightTop.pos.y - bottomVertex.pos.y) * (y - rightTop.pos.y);

		// 根据证得的x,y与深度z的线性关系：1/z = Ax + B 1/z = Ay + B
		oneoverz_top = 1.0 / leftTop.pos.z;		// 左上方顶点的1/z
		oneoverz_bottom = 1.0 / bottomVertex.pos.z;	// 底部顶点的1/z

		oneoverz_left = (y - leftTop.pos.y) * (oneoverz_bottom - oneoverz_top) /
			(bottomVertex.pos.y - leftTop.pos.y) + oneoverz_top; // 左边线段与y线交点的1/ z

		oneoverz_top = 1.0 / rightTop.pos.z;	// 右上方顶点的1/z

		oneoverz_right = (y - leftTop.pos.y) * (oneoverz_bottom - oneoverz_top) /
			(bottomVertex.pos.y - rightTop.pos.y) + oneoverz_top;	// 右边相对与y线交点的1/z

											// （当前y值对应的线上）x增量与1/z增量之比，用于计算和绘制出这一线 
		oneoverz_step = (oneoverz_right - oneoverz_left) / (xright - xleft);

		// 已证得纹理坐标s，t与x，y存在线性关系 
		// 顶点纹理UV(s0,t0),左下顶点纹理UV(s1,t1)，右下顶点纹理UV(s2,t2)
		float s0 = leftTop.uv.x, t0 = leftTop.uv.y, s1 = bottomVertex.uv.x, t1 = bottomVertex.uv.y, s2 = rightTop.uv.x, t2 = rightTop.uv.y;
		//float s0 = 0, t0 = 0, s1 = 0, t1 = 1, s2 = 1, t2 = 0;

		soverz_top = s0 / leftTop.pos.z;
		soverz_bottom = s1 / bottomVertex.pos.z;
		soverz_left = (y - leftTop.pos.y) * (soverz_bottom - soverz_top) / (bottomVertex.pos.y - leftTop.pos.y) + soverz_top;
		soverz_top = s2 / rightTop.pos.z;
		soverz_right = (y - leftTop.pos.y) * (soverz_bottom - soverz_top) / (bottomVertex.pos.y - rightTop.pos.y) + soverz_top;
		soverz_step = (soverz_right - soverz_left) / (xright - xleft);
		toverz_top = t0 / leftTop.pos.z;
		toverz_bottom = t1 / bottomVertex.pos.z;
		toverz_left = (y - leftTop.pos.y) * (toverz_bottom - toverz_top) / (bottomVertex.pos.y - leftTop.pos.y) + toverz_top;
		toverz_top = t2 / rightTop.pos.z;
		toverz_right = (y - leftTop.pos.y) * (toverz_bottom - toverz_top) / (bottomVertex.pos.y - rightTop.pos.y) + toverz_top;
		toverz_step = (toverz_right - toverz_left) / (xright - xleft);

		// 以计算出随x递增而增加的s/z，t/z和1/z的比，因此可以计算出它们
		for (x = xleft, oneoverz = oneoverz_left,
			soverz = soverz_left, toverz = toverz_left;
			x < xright; ++x, oneoverz += oneoverz_step,
			soverz += soverz_step, toverz += toverz_step)
		{
			s = soverz / oneoverz; // (s/z)/(1/z) = s
			t = toverz / oneoverz;

			Color c = GetColor(pBmpTex, s, t);
			SetPixel(backBuffer, x, y, c);
			//SetPixel(backBuffer, x, y, s * 255, t * 255, 0);
			// 用一个Vector表示
			//帧缓冲像素[x, y] = 纹理[s, t];
		}
	}
}

void DrawTriangle(byte *backBuffer, Texture *pBmpTex, VertexData vd1, VertexData vd2, VertexData vd3) // 画任意实心三角形  
{
	if (vd1.pos.y == vd2.pos.y)
	{
		if (vd3.pos.y > vd1.pos.y) // 平底
		{
			RenderTexture1(backBuffer, pBmpTex, vd3, vd1, vd2);
		}
		else // 平顶  
		{
			RenderTexture2(backBuffer, pBmpTex, vd3, vd1, vd2);
		}
	}
	else if (vd1.pos.y == vd3.pos.y)
	{
		if (vd2.pos.y > vd1.pos.y) // 平底  
		{
			RenderTexture1(backBuffer, pBmpTex, vd2, vd1, vd3);
		}
		else // 平顶  
		{
			RenderTexture2(backBuffer, pBmpTex, vd2, vd1, vd3);
		}
	}
	else if (vd2.pos.y == vd3.pos.y)
	{
		if (vd1.pos.y > vd2.pos.y) // 平底  
		{
			RenderTexture1(backBuffer, pBmpTex, vd1, vd2, vd3);
		}
		else // 平顶  
		{
			RenderTexture2(backBuffer, pBmpTex, vd1, vd2, vd3);
		}
	}
	else
	{
		VertexData topV, middleV, bottomV, newV;
		if (vd1.pos.y >= vd2.pos.y && vd2.pos.y >= vd3.pos.y) // v1.y v2.y v3.y  
		{
			topV = vd1;
			middleV = vd2;
			bottomV = vd3;
		}
		else if (vd1.pos.y >= vd3.pos.y && vd3.pos.y >= vd2.pos.y) // v1.y v3.y v2.y  
		{
			topV = vd1;
			middleV = vd3;
			bottomV = vd2;
		}
		else if (vd2.pos.y >= vd1.pos.y && vd1.pos.y >= vd3.pos.y) // v2.y v1.y v3.y  
		{
			topV = vd2;
			middleV = vd1;
			bottomV = vd3;
		}
		else if (vd2.pos.y >= vd3.pos.y && vd3.pos.y >= vd1.pos.y) // v2.y v3.y v1.y  
		{
			topV = vd2;
			middleV = vd3;
			bottomV = vd1;
		}
		else if (vd3.pos.y >= vd1.pos.y && vd1.pos.y >= vd2.pos.y) // v3.y v1.y v2.y  
		{
			topV = vd3;
			middleV = vd1;
			bottomV = vd2;
		}
		else if (vd3.pos.y >= vd2.pos.y && vd2.pos.y >= vd1.pos.y) // v3.y v2.y v1.y  
		{
			topV = vd3;
			middleV = vd2;
			bottomV = vd1;
		}

		int xl; // 长边在ymiddle时的x，来决定长边是在左边还是右边  
		xl = (middleV.pos.y - topV.pos.y) * (bottomV.pos.x - topV.pos.x) / (bottomV.pos.y - topV.pos.y) + topV.pos.x + 0.5;

		float z = (middleV.pos.y - topV.pos.y) * (bottomV.pos.z - topV.pos.z) / (bottomV.pos.y - topV.pos.y) + topV.pos.z;
		float u = (middleV.pos.y - topV.pos.y) * (bottomV.uv.x - topV.uv.x) / (bottomV.pos.y - topV.pos.y) + topV.uv.x;
		float v = (middleV.pos.y - topV.pos.y) * (bottomV.uv.y - topV.uv.y) / (bottomV.pos.y - topV.pos.y) + topV.uv.y;
		newV.pos = Vector3f(xl, middleV.pos.y, z);
		newV.uv = Vector3f(u, v, 0);

		//if (xl <= middleV.x) // 左三角形  
		//{

		// 画平底  
 		RenderTexture1(backBuffer, pBmpTex, topV, newV, middleV);

		// 画平顶  
		RenderTexture2(backBuffer, pBmpTex, bottomV, newV, middleV);

		//}
		//else // 右三角形  
		//{
		//	// 画平底  
		//	RenderTexture1(xtop, ytop, xmiddle, ymiddle, xl, ymiddle, color);

		//	// 画平顶  
		//	RenderTexture2(xmiddle, ymiddle, xl, ymiddle, xbottom, ybottom, color);
		//}
	}
}

