#pragma once

#include "dllcall.h"
#include <uxtheme.h>

class CDllUxTheme : public CDllCall
{
public:
	CDllUxTheme();

	HRESULT EnableThemeDialogTexture(HWND hWnd, DWORD dwFlags);
	HRESULT DrawThemeBackground(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pRect, const RECT *pClipRect);
	HRESULT CloseThemeData(HTHEME hTheme);
	BOOL IsThemeActive();
	HTHEME OpenThemeData(HWND hWnd, LPCWSTR pszClassList);
};
