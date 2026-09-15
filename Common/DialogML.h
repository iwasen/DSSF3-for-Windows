#pragma once

#include "Dpi.h"

class CDialogML : public CDialogAF
{
public:
	CDialogML(UINT nIDTemplate, CWnd* pParent = NULL);   // 標準のコンストラクタ

	BOOL Create(CWnd *pParentWnd = NULL);

protected:
	UINT m_nIDTemplate;
	CWnd *m_pParent;

	int MessageBox(UINT nIDPrompt, UINT nType = MB_OK);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual void PostNcDestroy();
	virtual void OnCancel();
	virtual void OnOK();
};
