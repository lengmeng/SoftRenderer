#pragma once
#include<Windows.h>
class RenderCore {
public:
	// ʵ��Ӧ�ôӲ�������������������Ϣ
	RenderCore();

	~RenderCore();

	// ���ݸ���Ҫ��Ⱦ���������ݡ����󻺴桢��Ȼ���
	// ÿ��������д��ʱ������Ȳ��ԣ����Բ�ͨ�����޷�д��
	void RenderView(byte* buffer, float rotation);

};