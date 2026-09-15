#pragma once

class CDpi
{
public:
	static void InitDpi();
	static int GetDpi();
	static int AdjustDpi(int n, int nAdjust = 100);
	static int OriginalDpi(int n, int nAdjust = 100);

protected:
	static int m_nDpi;
};

// CDialogAF ダイアログ

class CDialogAF : public CDialog
{
	DECLARE_DYNAMIC(CDialogAF)

public:
	CDialogAF();
	CDialogAF(UINT nID, CWnd* pParent = NULL);   // 標準コンストラクタ

protected:
	CFont m_oFont;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
};
