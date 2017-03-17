#pragma once
#include <Windows.h>
#include "RenderTexture.h"
#include <fstream>

Texture *readBmp(char *bmpName)
{
	Texture *pBmpTex = new Texture();
	RGBQUAD *pColorTable;//��ɫ��ָ��  

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