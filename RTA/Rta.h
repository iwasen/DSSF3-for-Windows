#pragma once

#include "resource.h"		// ÉÅÉCÉì ÉVÉìÉ{Éã
#include "comres.h"

#include "Common.h"
#include "Global.h"
#include "Dpi.h"

class CRtaApp : public CWinApp
{
public:
	CRtaApp();

	int m_nPresetID;
	class CRtaDlg *m_pWndRta;

	void CloseFuncWindow(CWnd *pWnd);
	void OnOnlineUpdate();

protected:
	ULONG m_nCheckData;
	const char *m_pRegistryName;

	DECLARE_MESSAGE_MAP()
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT ProcessWndProcException(CException* e, const MSG* pMsg);
};
