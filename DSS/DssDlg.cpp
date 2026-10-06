// DssDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Dss.h"
#include "DssDlg.h"
#include "VerRes.h"

// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CVerRes		res;
	SetDlgItemText(IDC_PRODUCT, res.GetProductName() + " Version " + res.GetProductVersion());
	SetDlgItemText(IDC_COPYRIGHT, res.GetLegalCopyright());

	return TRUE;
}

// CDssDlg ダイアログ

CDssDlg::CDssDlg(CWnd* pParent /*=NULL*/)
	: CPropertySheet(IDS_DSS_DIALOG, pParent)
{
	m_hIcon = ::AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	AddPage(&m_cFolder);
	AddPage(&m_cInitialize);
	AddPage(&m_cMmlib);

	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	m_psh.dwFlags &= ~PSH_HASHELP;
	m_cFolder.m_psp.dwFlags &= ~PSP_HASHELP;
	m_cInitialize.m_psp.dwFlags &= ~PSP_HASHELP;
}

BEGIN_MESSAGE_MAP(CDssDlg, CPropertySheet)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()

// CDssDlg メッセージ ハンドラ

BOOL CDssDlg::OnInitDialog()
{
	CPropertySheet::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	CString strOnlineUpdate;
	strOnlineUpdate.LoadString(IDS_ONLINE_UPDATE);
	if (!strOnlineUpdate.IsEmpty())
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ONLINE_UPDATE, strOnlineUpdate);
	}

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CDssDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	} else if (nID == IDM_ONLINE_UPDATE) {
		((CDssApp *)::AfxGetApp())->OnOnlineUpdate();
	} else {
		CPropertySheet::OnSysCommand(nID, lParam);
	}
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CDssDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		const int cxIcon = GetSystemMetrics(SM_CXICON);
		const int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		const int x = (rect.Width() - cxIcon + 1) / 2;
		const int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPropertySheet::OnPaint();
	}
}

//ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CDssDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
