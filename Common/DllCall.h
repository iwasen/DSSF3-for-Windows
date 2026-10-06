#pragma once

class CDllCall
{
public:
	CDllCall(LPCSTR pDllFile);
	virtual ~CDllCall();

	BOOL IsLoaded() const { return m_hLibModule != NULL; }

protected:
	HMODULE m_hLibModule;

	FARPROC LoadFunction(LPCSTR pFuncName) const;
};
