#include<Windows.h>
#include "Common.h"
#include "RenderCore.h"


#define WINDOW_TITLE   L"SoftRender"
#define bits 24

// ��������
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void Initlize3D(HWND hwnd); //��ʼ��3D���ֲ���
void UpdateScene();			//���³���(����λ��,��ɫ�������)����
void Render();				//��Ⱦ����
void ClearBackBuffer(byte r, byte g, byte b); // ������󻺴�
void Release3D();			//�ͷ���Դ
void InputUpdate();			//����ĸ���

// ��������
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
	//����3DԪ�صĳ�ʼ��
	Initlize3D(hwnd);

	//ע��ͻ���(��Ļ)�Ĵ�С�ʹ��ڵĴ�С�ǲ�һ����
	MoveWindow(hwnd, 250, 80, window_w, window_h, true);

	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	RenderCore rc;
	float temp = 0.0;
	//��Ϣѭ��msg
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
			// ��ĳ����ɫ������󻺴�
			ClearBackBuffer(0, 125, 125);

			// ����
			rc.RenderView(BackBuffer, temp);

			// ���ݱ��󻺴����ݽ�����Ⱦ
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
	//��ȡ��Ļ��С
	screen_w = SCREEN_WIDTH;
	screen_h = SCREEN_HEIGHT;
	RECT rect = { 0, 0, screen_w, screen_h };

	//����Ļ��С��ȡ���ڵĴ�С
	AdjustWindowRect(&rect, GetWindowLong(hwnd, GWL_STYLE), 0);
	window_w = rect.right - rect.left;
	window_h = rect.bottom - rect.top;

	//���ṹ��
	ZeroMemory(&binfo, sizeof(BITMAPINFO));
	binfo.bmiHeader.biBitCount = 24;      //ÿ�����ض���λ��Ҳ��ֱ��д24(RGB)����32(RGBA)
	binfo.bmiHeader.biCompression = BI_RGB;
	binfo.bmiHeader.biHeight = -SCREEN_HEIGHT;
	binfo.bmiHeader.biPlanes = 1;
	binfo.bmiHeader.biSizeImage = 0;
	binfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	binfo.bmiHeader.biWidth = SCREEN_WIDTH;

	//��ȡ��ĻHDC
	screen_hwnd = hwnd;
	screen_hdc = GetDC(screen_hwnd);

	//��ȡ����HDC�ͼ���Bitmap,����Bitmapѡ�����HDC(ÿ��HDC�ڴ�ÿʱ�̽���ѡ��һ��GDI��Դ,GDI��ԴҪѡ��HDC���ܽ��л���)
	hCompatibleDC = CreateCompatibleDC(screen_hdc);
	hCompatibleBitmap = CreateCompatibleBitmap(screen_hdc, SCREEN_WIDTH, SCREEN_HEIGHT);
	hOldBitmap = (HBITMAP)SelectObject(hCompatibleDC, hCompatibleBitmap);

	//���䱳�󻺴�
	BackBuffer = new byte[SCREEN_WIDTH*SCREEN_HEIGHT*bits / 8];

	//������Ȼ���
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
	//��������ĸ���
	InputUpdate();
}

//����ĸ���
void InputUpdate()
{


}