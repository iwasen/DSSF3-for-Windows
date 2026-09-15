#pragma once

class CMyDialog : public CDialog
{
public:
	CMyDialog(UINT IDD = NULL, CWnd* pParent = NULL);   // 標準のコンストラクタ

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

protected:
	virtual BOOL OnInitDialog();
	BOOL OnToolTipNotify(UINT id, NMHDR * pNMHDR, LRESULT * pResult);
	DECLARE_MESSAGE_MAP()
};
