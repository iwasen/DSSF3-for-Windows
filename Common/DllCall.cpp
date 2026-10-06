#include "StdAfx.h"
#include "DllCall.h"

CDllCall::CDllCall(LPCSTR pDllFile)
{
	m_hLibModule = ::LoadLibrary(pDllFile);
}

CDllCall::~CDllCall()
{
	if (m_hLibModule != NULL)
		::FreeLibrary(m_hLibModule);
}

FARPROC CDllCall::LoadFunction(LPCSTR pFuncName) const
{
	if (m_hLibModule == NULL)
		return NULL;

	return ::GetProcAddress(m_hLibModule, pFuncName);
}
