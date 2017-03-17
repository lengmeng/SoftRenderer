#pragma once
#include<Windows.h>
#include "Vector3.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 640

// ��������(ģ�����ꡢUV����)
struct Vertex {
	Vector3f position;
	Vector3f uv;
	Vertex(float x, float y, float z, float u, float v) {
		position = Vector3f(x, y, z, 1);
		uv = Vector3f(u, v, 1);
	}
};

// �������ݣ���Ļ���ꡢUV���꣩
struct VertexData {
	Vector3f pos;
	Vector3f uv;
	bool isCull = false;
	// ���ܻ����������編�߲�ֵ�����
};

struct Color {
	Color() {
		r = 0;
		g = 0;
		b = 0;
		a = 1;
	};
	Color(byte c1, byte c2, byte c3) {
		r = c1;
		g = c2;
		b = c3;
		a = 1;
	};
	Color(byte c1, byte c2, byte c3, byte c4) {
		r = c1;
		g = c2;
		b = c3;
		a = c4;
	};
	byte r;
	byte g;
	byte b;
	byte a;
};


struct Texture {
	byte *pBmpBuf;
	int bmpWidth;
	int bmpHeight;
	int biBitCount;
};