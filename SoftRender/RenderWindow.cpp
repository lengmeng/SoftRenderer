#include<Windows.h>
#include "Common.h"
#include "RenderCore.h"


#define WINDOW_TITLE   L"SoftRender"
#define bits 24

// 方法声明
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void Initlize3D(HWND hwnd); //初始化3D各种参数
void UpdateScene();			//更新场景(顶点位置,颜色，纹理等)数据
void Render();				//渲染函数
void ClearBackBuffer(byte r, byte g, byte b); // 情况背后缓存
void Release3D();			//释放资源
void InputUpdate();			//输入的更新

// 变量声明
int screen_w;
int screen_h;
int window_w;
int window_h;
static byte *BackBuffer;
static byte *DepthBuffer;
static HDC screen_hdc;
static HWND screen_hwnd;
static HDC hCompatibleDC; 
static HBITMAP hCompatibleBitmap; 
static HBITMAP hOldBitmap; 
static BITMAPINFO binfo; 

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	/* */
	WNDCLASSEX wndclass = { 0 };
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = L"SoftRender";
	if (!RegisterClassEx(&wndclass))
		return -1;
	HWND hwnd;

	hwnd = CreateWindow(L"SoftRender", WINDOW_TITLE, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, hInstance, NULL);
	//进行3D元素的初始化
	Initlize3D(hwnd);

	//注意客户区(屏幕)的大小和窗口的大小是不一样的
	MoveWindow(hwnd, 250, 80, window_w, window_h, true);

	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	RenderCore rc;
	float temp = 0.0;
	//消息循环msg
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// 以某种颜色清除背后缓存
			ClearBackBuffer(0, 125, 125);

			// 绘制
			rc.RenderView(BackBuffer, temp);

			// 根据背后缓存数据进行渲染
			Render();
		}
		temp += 0.05f;
		temp = temp>360?0:temp;
	}

	UnregisterClass(L"ForTheDream", wndclass.hInstance);

	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		ValidateRect(hwnd, NULL);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(hwnd);
		break;
	case WM_KEYUP:
		break;
	case WM_DESTROY:
		Release3D();
		PostQuitMessage(0);
		break;
	default: return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}

void Initlize3D(HWND hwnd)
{
	//获取屏幕大小
	screen_w = SCREEN_WIDTH;
	screen_h = SCREEN_HEIGHT;
	RECT rect = { 0, 0, screen_w, screen_h };

	//由屏幕大小获取窗口的大小
	AdjustWindowRect(&rect, GetWindowLong(hwnd, GWL_STYLE), 0);
	window_w = rect.right - rect.left;
	window_h = rect.bottom - rect.top;

	//填充结构体
	ZeroMemory(&binfo, sizeof(BITMAPINFO));
	binfo.bmiHeader.biBitCount = 24;      //每个像素多少位，也可直接写24(RGB)或者32(RGBA)
	binfo.bmiHeader.biCompression = BI_RGB;
	binfo.bmiHeader.biHeight = -SCREEN_HEIGHT;
	binfo.bmiHeader.biPlanes = 1;
	binfo.bmiHeader.biSizeImage = 0;
	binfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	binfo.bmiHeader.biWidth = SCREEN_WIDTH;

	//获取屏幕HDC
	screen_hwnd = hwnd;
	screen_hdc = GetDC(screen_hwnd);

	//获取兼容HDC和兼容Bitmap,兼容Bitmap选入兼容HDC(每个HDC内存每时刻仅能选入一个GDI资源,GDI资源要选入HDC才能进行绘制)
	hCompatibleDC = CreateCompatibleDC(screen_hdc);
	hCompatibleBitmap = CreateCompatibleBitmap(screen_hdc, SCREEN_WIDTH, SCREEN_HEIGHT);
	hOldBitmap = (HBITMAP)SelectObject(hCompatibleDC, hCompatibleBitmap);

	//分配背后缓存
	BackBuffer = new byte[SCREEN_WIDTH*SCREEN_HEIGHT*bits / 8];

	//分配深度缓存
	DepthBuffer = new byte[SCREEN_WIDTH*SCREEN_HEIGHT*bits / 8];
}

void Render()
{
	SetDIBits(screen_hdc, hCompatibleBitmap, 0, SCREEN_HEIGHT, BackBuffer, (BITMAPINFO*)&binfo, DIB_RGB_COLORS);
	BitBlt(screen_hdc, -1, -1, SCREEN_WIDTH, SCREEN_HEIGHT, hCompatibleDC, 0, 0, SRCCOPY);
}

void Release3D()
{
	SelectObject(hCompatibleDC, hOldBitmap);
	DeleteObject(hCompatibleDC);
	DeleteObject(hCompatibleDC);

	ReleaseDC(screen_hwnd, screen_hdc);

	delete[] BackBuffer;
}

void ClearBackBuffer(byte r, byte g, byte b)
{
	for (int i = 0; i < screen_h*screen_w; ++i)
	{

		BackBuffer[i * 3 + 0] = b;
		BackBuffer[i * 3 + 1] = g;
		BackBuffer[i * 3 + 2] = r;
	}
}


void UpdateScene()
{
	//进行输入的更新
	InputUpdate();
}

//输入的更新
void InputUpdate()
{


}