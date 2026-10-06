// NoiseSourceDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Nms.h"
#include "NoiseSourceDlg.h"
#include "FileIO.h"
#include "Help\ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CNoiseSourceDlg ダイアログ

CNoiseSourceDlg::CNoiseSourceDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(CNoiseSourceDlg::IDD, pParent)
{
}

void CNoiseSourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NOISE_SOURCE_NAME, m_sNoiseSource);
}

BEGIN_MESSAGE_MAP(CNoiseSourceDlg, CDialogAF)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNoiseSourceDlg メッセージ ハンドラ

void CNoiseSourceDlg::OnOK()
{
	UpdateData(TRUE);

	if (m_sNoiseSource.IsEmpty()) {
		::AfxMessageBox(IDS_ERR_NOISE_SOURCE, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	CDbNsTmp dbNsTmp;
	if (!dbNsTmp.Open())
		return;

	DbNsTmpRec dbNsTmpRec;
	if (!dbNsTmp.GetNewID(&dbNsTmpRec.nNsTmpID))
		return;

	NsTmpData nsTmpData{};
	dbNsTmpRec.sName = m_sNoiseSource;

	dbNsTmp.StoreRec(&dbNsTmpRec, &nsTmpData);

	CDialogAF::OnOK();
}

BOOL CNoiseSourceDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDOK, IDH_NOISE_SOURCE_OK,
		IDCANCEL, IDH_NOISE_SOURCE_CANCEL,
		IDC_NOISE_SOURCE_NAME, IDH_NOISE_SOURCE_NAME,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
