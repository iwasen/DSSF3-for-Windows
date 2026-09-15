#include "StdAfx.h"
#include "DllUxTheme.h"

CDllUxTheme::CDllUxTheme()
	: CDllCall("UxTheme.dll")
{
}

HRESULT  CDllUxTheme::CloseThemeData(HTHEME hTheme)
{
	typedef HRESULT  (__stdcall *LPFUNC)(HTHEME hTheme);

	LPFUNC pFunc = (LPFUNC)LoadFunction("CloseThemeData");
	if (pFunc != NULL)
		return pFunc(hTheme);
	else
		return NULL;
}

HRESULT CDllUxTheme::DrawThemeBackground(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pRect, const RECT *pClipRect)
{
	typedef HRESULT  (__stdcall *LPFUNC)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pRect, const RECT *pClipRect);

	LPFUNC pFunc = (LPFUNC)LoadFunction("DrawThemeBackground");
	if (pFunc != NULL)
		return pFunc(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
	else
		return NULL;
}

HRESULT CDllUxTheme::EnableThemeDialogTexture(HWND hWnd, DWORD dwFlags)
{
	typedef HRESULT (__stdcall *LPFUNC)(HWND hWnd, DWORD dwFlags);

	LPFUNC pFunc = (LPFUNC)LoadFunction("EnableThemeDialogTexture");
	if (pFunc != NULL)
		return pFunc(hWnd, dwFlags);
	else
		return NULL;
}

BOOL CDllUxTheme::IsThemeActive()
{
	typedef BOOL  (__stdcall *LPFUNC)();

	LPFUNC pFunc = (LPFUNC)LoadFunction("IsThemeActive");
	if (pFunc != NULL)
		return pFunc();
	else
		return FALSE;
}

HTHEME CDllUxTheme::OpenThemeData(HWND hWnd, LPCWSTR pszClassList)
{
	typedef HTHEME (__stdcall *LPFUNC)(HWND hWnd, LPCWSTR pszClassList);

	LPFUNC pFunc = (LPFUNC)LoadFunction("OpenThemeData");
	if (pFunc != NULL)
		return pFunc(hWnd, pszClassList);
	else
		return NULL;
}
