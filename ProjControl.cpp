// ProjControl.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ProjControl.h"
#include "ScreenRender.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE g_hInst;								// current instance
HWND	  g_hWnd;								// Primary window
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void FixAspectRatio(HWND hWnd, int resizeType, LPRECT size, double ratio);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PROJCONTROL, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROJCONTROL));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJCONTROL));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_PROJCONTROL);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   g_hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   g_hWnd = hWnd;

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_SCREEN:
			Screen_OpenSettingsBox(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		Screen_Draw(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_SIZING:
		FixAspectRatio(hWnd, (int) wParam, (LPRECT) lParam, Screen_GetRatio());
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void FixAspectRatio(HWND hWnd, int resizeType, LPRECT size, double ratio) {
	RECT rcClient, rcWindow;
	int newWidth, newHeight, ncWidth, ncHeight;
	double curRatio;
	if (ratio < 0.01) return; // Testing against 0 is generally a bad idea
	// Get nonclient size
	GetClientRect(hWnd, &rcClient);
	GetWindowRect(hWnd, &rcWindow);
	ncWidth = (rcWindow.right - rcWindow.left) - (rcClient.right);
	ncHeight = (rcWindow.bottom - rcWindow.top) - (rcClient.bottom);

	// One of these will not be used, but easier to calculate up here
	newWidth = (int) (((size->bottom - size->top - ncWidth) * ratio) + ncWidth);
	newHeight = (int) (((size->right - size->left - ncHeight) / ratio) + ncWidth);

	curRatio = (double)(size->right - size->left) / (double)(size->bottom - size->top);
	switch (resizeType) {
		case WMSZ_BOTTOM:
		case WMSZ_TOP:
			// Right side needs to match
			size->right = size->left + newWidth;
			break;
		case WMSZ_LEFT:
		case WMSZ_RIGHT:
			// Bottom side needs to match
			size->bottom = size->top + newHeight;
			break;
		case WMSZ_TOPLEFT:
			if (curRatio > ratio) {
				// Width needs to shrink
				size->left = size->right - newWidth;
			} else {
				size->top = size->bottom - newHeight;
			}
			break;
		case WMSZ_TOPRIGHT:
			if (curRatio > ratio) {
				size->right = size->left + newWidth;
			} else {
				size->top = size->bottom - newHeight;
			}
			break;
		case WMSZ_BOTTOMLEFT:
			if (curRatio > ratio) {
				size->left = size->right - newWidth;
			} else {
				size->bottom = size->top + newHeight;
			}
			break;
		case WMSZ_BOTTOMRIGHT:
			if (curRatio > ratio) {
				size->right = size->left + newWidth;
			} else {
				size->bottom = size->top + newHeight;
			}
			break;
	}
}
	

void StartTimedRedraw(int millis)
{
	// TODO: Setup the redraw timer
}

void StopTimedRedraw()
{
	// TODO: Stop the redraw timer
}

void ForceRedraw()
{
	InvalidateRect(g_hWnd, NULL, TRUE);
}

RECT GetClientRect()
{
	RECT rc;
	GetClientRect(g_hWnd, &rc);
	return rc;
}