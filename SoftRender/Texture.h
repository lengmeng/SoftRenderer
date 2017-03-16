#pragma once
#include "Common.h"
// ֻ���ƽ�׻�ƽ�������棬����������״ ����Ҫ�����иĿǰcube����ƽͷ��ƽ�������� ���ã�

// ��������
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

// ����ƽ���������� ���� ���� ���£���
void RenderTexture1(byte *backBuffer, Texture *pBmpTex, VertexData vd1, VertexData vd2, VertexData vd3) {
	double x, y, xleft, xright;				// ��ֵx��y�������߶�x
	double oneoverz_left, oneoverz_right;	// �����߶�1/z
	double oneoverz_top, oneoverz_bottom;	// ���¶���1/z
	double oneoverz, oneoverz_step;			// ��ֵ1/z�Լ�ɨ���߲���

	double soverz_top, soverz_bottom;		// ���¶���s/z
	double toverz_top, toverz_bottom;		// ���¶���t/z
	double soverz_left, soverz_right;		// �����߶�s/z
	double toverz_left, toverz_right;		// �����߶�t/z

	double soverz, soverz_step;				// ��ֵs/z�Լ�ɨ���߲���
	double toverz, toverz_step;				// ��ֵt/z�Լ�ɨ���߲���

	double s, t;							// Ҫ���ԭʼs��t


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

		// ����֤�õ�x,y�����topVertex�����Թ�ϵ��1/z = Ax + B 1/z = Ay + B
		oneoverz_top = 1.0 / topVertex.pos.z;		// �Ϸ������1/z
		oneoverz_bottom = 1.0 / leftBottom.pos.z;	// ���·������1/z

		oneoverz_left = (y - topVertex.pos.y) * (oneoverz_bottom - oneoverz_top) /
			(leftBottom.pos.y - topVertex.pos.y) + oneoverz_top; // ����߶���y�߽����1/z

		oneoverz_bottom = 1.0 / rightBottm.pos.z;	// �ұ��·������1/z

		oneoverz_right = (y - topVertex.pos.y) * (oneoverz_bottom - oneoverz_top) /
			(rightBottm.pos.y - topVertex.pos.y) + oneoverz_top;	// �ұ������y�߽����1/z

		// ����ǰyֵ��Ӧ�����ϣ�x������1/z����֮�ȣ����ڼ���ͻ��Ƴ���һ�� 
		oneoverz_step = (oneoverz_right - oneoverz_left) / (xright - xleft);

		// ��֤����������s��t��x��y�������Թ�ϵ 
		// ��������UV(s0,t0),���¶�������UV(s1,t1)�����¶�������UV(s2,t2)
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

		// �Լ������x���������ӵ�s/z��t/z��1/z�ıȣ���˿��Լ��������
		for (x = xleft, oneoverz = oneoverz_left,
			soverz = soverz_left, toverz = toverz_left;
			x < xright; ++x, oneoverz += oneoverz_step,
			soverz += soverz_step, toverz += toverz_step)
		{
			s = soverz / oneoverz; // (s/z)/(1/z) = s
			t = toverz / oneoverz;
			Color c = GetColor(pBmpTex, s, t);
			SetPixel(backBuffer, x, y, c);
			//֡��������[x, y] = ����[s, t];
		}
	}
}

// ����ƽ�������Σ��� ����  ���ϣ�
void RenderTexture2(byte *backBuffer, Texture *pBmpTex, VertexData vd1, VertexData vd2, VertexData vd3) {
	double x, y, xleft, xright;				// ��ֵx��y�������߶�x
	double oneoverz_left, oneoverz_right;	// �����߶�1/z
	double oneoverz_top, oneoverz_bottom;	// ���¶���1/z
	double oneoverz, oneoverz_step;			// ��ֵ1/z�Լ�ɨ���߲���

	double soverz_top, soverz_bottom;		// ���¶���s/z
	double toverz_top, toverz_bottom;		// ���¶���t/z
	double soverz_left, soverz_right;		// �����߶�s/z
	double toverz_left, toverz_right;		// �����߶�t/z

	double soverz, soverz_step;				// ��ֵs/z�Լ�ɨ���߲���
	double toverz, toverz_step;				// ��ֵt/z�Լ�ɨ���߲���

	double s, t;							// Ҫ���ԭʼs��t

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
		// x0y0�Ƕ������㣬x1y1�����¶��㡢x2y2�����¶���
		xleft = leftTop.pos.x + (leftTop.pos.x - bottomVertex.pos.x) / (leftTop.pos.y - bottomVertex.pos.y) * (y - leftTop.pos.y);
		xright = rightTop.pos.x + (rightTop.pos.x - bottomVertex.pos.x) / (rightTop.pos.y - bottomVertex.pos.y) * (y - rightTop.pos.y);

		// ����֤�õ�x,y�����z�����Թ�ϵ��1/z = Ax + B 1/z = Ay + B
		oneoverz_top = 1.0 / leftTop.pos.z;		// ���Ϸ������1/z
		oneoverz_bottom = 1.0 / bottomVertex.pos.z;	// �ײ������1/z

		oneoverz_left = (y - leftTop.pos.y) * (oneoverz_bottom - oneoverz_top) /
			(bottomVertex.pos.y - leftTop.pos.y) + oneoverz_top; // ����߶���y�߽����1/ z

		oneoverz_top = 1.0 / rightTop.pos.z;	// ���Ϸ������1/z

		oneoverz_right = (y - leftTop.pos.y) * (oneoverz_bottom - oneoverz_top) /
			(bottomVertex.pos.y - rightTop.pos.y) + oneoverz_top;	// �ұ������y�߽����1/z

											// ����ǰyֵ��Ӧ�����ϣ�x������1/z����֮�ȣ����ڼ���ͻ��Ƴ���һ�� 
		oneoverz_step = (oneoverz_right - oneoverz_left) / (xright - xleft);

		// ��֤����������s��t��x��y�������Թ�ϵ 
		// ��������UV(s0,t0),���¶�������UV(s1,t1)�����¶�������UV(s2,t2)
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

		// �Լ������x���������ӵ�s/z��t/z��1/z�ıȣ���˿��Լ��������
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
			// ��һ��Vector��ʾ
			//֡��������[x, y] = ����[s, t];
		}
	}
}

void DrawTriangle(byte *backBuffer, Texture *pBmpTex, VertexData vd1, VertexData vd2, VertexData vd3) // ������ʵ��������  
{
	if (vd1.pos.y == vd2.pos.y)
	{
		if (vd3.pos.y > vd1.pos.y) // ƽ��
		{
			RenderTexture1(backBuffer, pBmpTex, vd3, vd1, vd2);
		}
		else // ƽ��  
		{
			RenderTexture2(backBuffer, pBmpTex, vd3, vd1, vd2);
		}
	}
	else if (vd1.pos.y == vd3.pos.y)
	{
		if (vd2.pos.y > vd1.pos.y) // ƽ��  
		{
			RenderTexture1(backBuffer, pBmpTex, vd2, vd1, vd3);
		}
		else // ƽ��  
		{
			RenderTexture2(backBuffer, pBmpTex, vd2, vd1, vd3);
		}
	}
	else if (vd2.pos.y == vd3.pos.y)
	{
		if (vd1.pos.y > vd2.pos.y) // ƽ��  
		{
			RenderTexture1(backBuffer, pBmpTex, vd1, vd2, vd3);
		}
		else // ƽ��  
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

		int xl; // ������ymiddleʱ��x������������������߻����ұ�  
		xl = (middleV.pos.y - topV.pos.y) * (bottomV.pos.x - topV.pos.x) / (bottomV.pos.y - topV.pos.y) + topV.pos.x + 0.5;

		float z = (middleV.pos.y - topV.pos.y) * (bottomV.pos.z - topV.pos.z) / (bottomV.pos.y - topV.pos.y) + topV.pos.z;
		float u = (middleV.pos.y - topV.pos.y) * (bottomV.uv.x - topV.uv.x) / (bottomV.pos.y - topV.pos.y) + topV.uv.x;
		float v = (middleV.pos.y - topV.pos.y) * (bottomV.uv.y - topV.uv.y) / (bottomV.pos.y - topV.pos.y) + topV.uv.y;
		newV.pos = Vector3f(xl, middleV.pos.y, z);
		newV.uv = Vector3f(u, v, 0);

		//if (xl <= middleV.x) // ��������  
		//{

		// ��ƽ��  
 		RenderTexture1(backBuffer, pBmpTex, topV, newV, middleV);

		// ��ƽ��  
		RenderTexture2(backBuffer, pBmpTex, bottomV, newV, middleV);

		//}
		//else // ��������  
		//{
		//	// ��ƽ��  
		//	RenderTexture1(xtop, ytop, xmiddle, ymiddle, xl, ymiddle, color);

		//	// ��ƽ��  
		//	RenderTexture2(xmiddle, ymiddle, xl, ymiddle, xbottom, ybottom, color);
		//}
	}
}

