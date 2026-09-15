#pragma once

class CDialogExt : public CDialogAF
{
	struct SDlgCtlPosition {
		HWND hWnd;
		CRect rectCtl;
		double fLeft;
		double fTop;
		double fRight;
		double fBottom;
	};
	typedef CArray <SDlgCtlPosition, SDlgCtlPosition&> CDlgCtlPositionArray;

public:
	CDialogExt(UINT nID, CWnd* pParent);   // 標準コンストラクタ

	void SetCtlPosition(UINT nID, double fLeft, double fTop, double fRight, double fBottom);
	void SetCtlPosition(CWnd *pWnd, double fLeft, double fTop, double fRight, double fBottom);
	void SetMaximunControl(HWND hMaximunCtrl);
	void CreateStatusBar();
	void SetStatusBar(UINT nID);
	void SetStatusBar(LPCTSTR pText);

protected:
	CStatusBar  m_wndStatusBar;
	CSize m_sizeWindow;
	CSize m_sizeClient;
	HWND m_hMaximunCtrl;
	CDlgCtlPositionArray m_aDlgCtlPosition;
	//CFont m_font;

	void SaveWindowSize();
	void RepositionControls(int cx, int cy);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
