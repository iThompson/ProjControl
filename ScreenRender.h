#pragma once

void Screen_Init(HINSTANCE hInstance);
void Screen_OpenSettingsBox(HWND parent);
double Screen_GetRatio();
void Screen_Draw(HDC hdc);

typedef struct _SCRCFG {
	bool bShowCursor;
	bool bActive;
	UINT iUpdateInterval;
	int x;
	int y;
	int w;
	int h;
	TCHAR szScreenName[CCHDEVICENAME];
} SCRCFG;
