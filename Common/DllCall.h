#pragma once

class CDllCall
{
public:
	CDllCall(LPCTSTR pDllFile);
	virtual ~CDllCall();

	BOOL IsLoaded() const { return m_hLibModule != NULL; }

protected:
	HMODULE m_hLibModule;

	FARPROC LoadFunction(LPCTSTR pFuncName) const;
};
