#pragma once

#include "resource.h"       // ÉÅÉCÉì ÉVÉìÉ{Éã
#include "comres.h"
#include "Common.h"
#include "Global.h"
#include "Dpi.h"

class CNmsApp : public CWinApp
{
public:
	CNmsApp();

protected:
	ULONG m_nCheckData;
	const char *m_pRegistryName;

	DECLARE_MESSAGE_MAP()
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnAppAbout();
	afx_msg void OnOnlineUpdate();
	afx_msg void OnOnlineManual();
	afx_msg void OnTechnicalSupport();
	afx_msg void OnDocumentQa();
	afx_msg void OnReadme();
	afx_msg void OnYmecStore();
	afx_msg void OnLicenseRegist();
	afx_msg void OnUpdateLicenseRegist(CCmdUI *pCmdUI);
};
