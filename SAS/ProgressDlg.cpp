// ProgressDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "ProgressDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg ダイアログ

CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/, LPCSTR title)
	: CDialogAF(CProgressDlg::IDD, pParent)
{
	m_nMaxValue = 100;
	m_bAbort = FALSE;

	Create(IDD_PROGRESS, pParent);

	if (pParent != NULL) {
		CRect rectParent;
		pParent->GetWindowRect(rectParent);
		CRect rectThis;
		GetWindowRect(rectThis);
		SetWindowPos(NULL, (rectParent.left + rectParent.right - rectThis.Width()) / 2,
					(rectParent.top + rectParent.bottom - rectThis.Height()) / 2,
					0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}

	if (title != NULL)
		SetWindowText(title);

	ShowWindow(SW_SHOW);
	UpdateWindow();
}

CProgressDlg::~CProgressDlg()
{
	DestroyWindow();
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
	DDX_Text(pDX, IDC_MESSAGE, m_sMessage);
}

BEGIN_MESSAGE_MAP(CProgressDlg, CDialogAF)
	ON_BN_CLICKED(IDC_ABORT, OnAbort)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg メッセージ ハンドラ

void CProgressDlg::SetRange(int nMaxValue)
{
	m_nMaxValue = nMaxValue;
}

void CProgressDlg::SetProgressBar(int nPos)
{
	m_nPos = 100 * nPos / m_nMaxValue;
	m_Progress.SetPos(m_nPos);
}

void CProgressDlg::SetProgressBarSub(int percent)
{
	m_Progress.SetPos(m_nPos + percent / m_nMaxValue);
}

void CProgressDlg::SetMessage(LPCSTR msg)
{
	m_sMessage = msg;
	UpdateData(FALSE);
	UpdateWindow();
}

BOOL CProgressDlg::CheckAbort()
{
	MSG	msg;

	for (;;) {
		if (!::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			break;

		::TranslateMessage((LPMSG)&msg);
		::DispatchMessage((LPMSG)&msg);
	}

	if (m_bAbort) {
		if (::AfxMessageBox(IDS_MSG_CANCELACTION, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
			m_bAbort = FALSE;
	}

	return m_bAbort;
}

void CProgressDlg::OnAbort()
{
	m_bAbort = TRUE;
}
