#include "stdafx.h"
#include "ScreenRender.h"
#include "resource.h"

HINSTANCE hInst;
SCRCFG cfg;

BOOL CALLBACK EnumScreens(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	HWND hCombo = (HWND) dwData;
	MONITORINFOEX mInfo;
	mInfo.cbSize = sizeof(MONITORINFOEX);
	if (GetMonitorInfo(hMonitor, &mInfo)) {
		int id = ComboBox_AddString(hCombo, mInfo.szDevice);
		if (id >= 0) { // errors are negative
			MONITORINFOEX* pInfo = (MONITORINFOEX*) malloc(sizeof(MONITORINFOEX));
			if (pInfo != NULL) {
				memcpy(pInfo, &mInfo, sizeof(MONITORINFOEX));
				ComboBox_SetItemData(hCombo, id, (LPARAM) pInfo);
			} else {
				ComboBox_SetItemData(hCombo, id, NULL);
			}
		}
	}
	return TRUE;
}

INT_PTR CALLBACK ScrSelProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hCombo = GetDlgItem(hDlg, IDC_SCREENS);
	MONITORINFOEX* pInfo;
	int count, id;
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		// Set up the combo box
		ComboBox_ResetContent(hCombo); // Just to be sure
		// Create the default value
		ComboBox_AddString(hCombo, TEXT("(none)"));
		ComboBox_SetItemData(hCombo, 0, NULL);
		// Enumerate Monitors
		EnumDisplayMonitors(NULL, NULL, EnumScreens, (LPARAM) hCombo);
		if (ComboBox_SelectString(hCombo, -1, cfg.szScreenName) == CB_ERR) 
			ComboBox_SetCurSel(hCombo, 0);
		return (INT_PTR)TRUE;
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			// Save data
			cfg.bShowCursor = false;
			id = ComboBox_GetCurSel(hCombo);
			if (id >= 0) { // Negative is CB_ERR
				pInfo = (MONITORINFOEX*) ComboBox_GetItemData(hCombo, id);
				if (pInfo != NULL) {
					StringCbCopy(cfg.szScreenName, sizeof(cfg.szScreenName), pInfo->szDevice);
					cfg.x = pInfo->rcMonitor.left;
					cfg.y = pInfo->rcMonitor.top;
					cfg.w = pInfo->rcMonitor.right - cfg.x;
					cfg.h = pInfo->rcMonitor.bottom - cfg.y;
					cfg.bActive = true;
				} else {
					cfg.bActive = false;
					cfg.szScreenName[0] = (TCHAR) 0;
				}
			}
			// NO BREAK
		case IDCANCEL:
			// Clean up the MonitorInfo structures
			count = ComboBox_GetCount(hCombo);
			for (int i=0; i<count; i++) {
				pInfo = (MONITORINFOEX*) ComboBox_GetItemData(hCombo, i);
				if (pInfo == NULL) continue;
				free(pInfo);
			}

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void Screen_Init(HINSTANCE hInstance)
{
	hInst = hInstance;

	cfg.bActive = false;
	cfg.szScreenName[0] = (TCHAR) 0; // Default to empty string
}

void Screen_OpenSettingsBox(HWND parent)
{
	DialogBox(hInst, MAKEINTRESOURCE(IDD_SELECT_SCREEN), parent, ScrSelProc);
}

void Screen_Draw(HDC hdc)
{
	TCHAR str[100];
	size_t len;
	HDC hDesktop = GetDC(NULL);
	if (cfg.bActive) {
		StringCbPrintf(str, sizeof(str), TEXT("On %s, x = %i, y = %i, w = %i, h = %i"), cfg.szScreenName, cfg.x, cfg.y, cfg.w, cfg.h);
	} else {
		StringCbCopy(str, sizeof(str), TEXT("No screen selected"));
	}
	StringCchLength(str, 100, &len);
	TextOut(hdc, 0, 0, str, (int)len);
	ReleaseDC(NULL, hDesktop);
}