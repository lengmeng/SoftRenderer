#pragma once
#include<Windows.h>
class RenderCore {
public:
	// 实际应该从参数传入物体和摄像机信息
	RenderCore();

	~RenderCore();

	// 传递给需要渲染的物体数据、背后缓存、深度缓存
	// 每个物体在写入时进行深度测试，测试不通过则无法写入
	void RenderView(byte* buffer, float rotation);

};