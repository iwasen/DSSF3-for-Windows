#pragma once

#include "resource.h"       // ÉÅÉCÉì ÉVÉìÉ{Éã
#include "comres.h"
#include "Common.h"
#include "global.h"
#include "Dpi.h"

#define ID_VALUE		0x0fffffff
#define ID_KIND			0xf0000000
#define ID_SYSTEM		0x10000000
#define ID_DB_IR		0x20000000
#define ID_DB_ACF		0x30000000
#define ID_DB_NMS		0x40000000
#define ID_FOLDER		0x50000000
#define ID_DATA			0x60000000

#define PHINT_DELETE	(CObject *)1

#define IMAGE_FOLDER_IR		0
#define IMAGE_IR_BLUE		1
#define IMAGE_IR_RED		2
#define IMAGE_FOLDER_ACF	3
#define IMAGE_ACF_BLUE		4
#define IMAGE_ACF_RED		5
#define IMAGE_FOLDER_NMS	6
#define IMAGE_NMS_BLUE		7
#define IMAGE_NMS_RED		8
#define IMAGE_SYSTEM		9
#define IMAGE_DB_IR			10
#define IMAGE_DB_ACF		11
#define IMAGE_DB_NMS		12
#define IMAGE_FOLDER2_IR	13
#define IMAGE_FOLDER2_ACF	14
#define IMAGE_FOLDER2_NMS	15

extern CImageList imageList;

class SasApp : public CWinApp
{
public:
	SasApp();

protected:
	ULONG m_nCheckData;
	const char *m_pRegistryName;

	void AdjustDialogSize();
	void SetDialogFontSize(UINT nIDTemplate, int nFontSize);
	void RegistExportFileIcon();

	DECLARE_MESSAGE_MAP()
	virtual BOOL InitInstance();
	virtual LRESULT ProcessWndProcException(CException* e, const MSG* pMsg);
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
