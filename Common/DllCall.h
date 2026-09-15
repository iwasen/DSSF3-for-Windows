#pragma once

class CDllCall
{
public:
	CDllCall(LPCTSTR pDllFile);
	virtual ~CDllCall();

	BOOL IsLoaded() { return m_hLibModule != NULL; }

protected:
	HMODULE m_hLibModule;

	FARPROC LoadFunction(LPCTSTR pFuncName);
};
