#pragma once
#include <Windows.h>
#include "RenderTexture.h"
#include <fstream>

Texture *readBmp(char *bmpName)
{
	Texture *pBmpTex = new Texture();
	RGBQUAD *pColorTable;//颜色表指针  

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