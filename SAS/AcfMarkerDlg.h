#pragma once

#include "MyCtrl.h"

// AcfMarkerDlg ダイアログ

class CAcfMarkerDlg : public CDialogAF
{
	DECLARE_DYNAMIC(CAcfMarkerDlg)

public:
	CAcfMarkerDlg(CWnd* pParent = NULL);   // 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_ACF_MARKER };

	long m_nAcfID;
	long m_nAcfMarkerID;
	double m_fTime;

protected:
	CMyEdit m_cEditTitle;
	CMyEdit m_cEditComment;

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
