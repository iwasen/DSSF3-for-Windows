// ChangeNoiseSrcDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Nms.h"
#include "NmsDoc.h"
#include "ChangeNoiseSrcDlg.h"
#include "NoiseSourceDlg.h"
#include "Help\ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CChangeNoiseSrcDlg ダイアログ

CChangeNoiseSrcDlg::CChangeNoiseSrcDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(CChangeNoiseSrcDlg::IDD, pParent)
{
}

void CChangeNoiseSrcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NOISE_SRC_LIST, m_cNoiseSrcList);
}

BEGIN_MESSAGE_MAP(CChangeNoiseSrcDlg, CDialogAF)
	ON_BN_CLICKED(IDC_ADD_NEW, OnAddNew)
	ON_LBN_DBLCLK(IDC_NOISE_SRC_LIST, OnDblclkNoiseSrcList)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeNoiseSrcDlg メッセージ ハンドラ

BOOL CChangeNoiseSrcDlg::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	SetNsTmpList(m_pNmsItem->m_nNoiseTmpID);

	return TRUE;
}

void CChangeNoiseSrcDlg::OnOK()
{
	int nIndex;

	if ((nIndex = m_cNoiseSrcList.GetCurSel()) == LB_ERR)
		return;

	BOOL bLearning = m_pNmsItem->m_bLearning;
	m_pNmsItem->UnLearningNoiseSrc();

	m_pNmsItem->m_nNoiseTmpID = (int)m_cNoiseSrcList.GetItemData(nIndex);
	m_pNmsItem->m_NoiseSrcData.aName[0] = '=';
	m_cNoiseSrcList.GetText(nIndex, m_pNmsItem->m_NoiseSrcData.aName + 1);

	if (bLearning)
		m_pNmsItem->LearningNoiseSrc();

	CDialogAF::OnOK();
}

void CChangeNoiseSrcDlg::SetNsTmpList(long nID)
{
	CDbNsTmp dbNsTmp;
	DbNsTmpRec dbNsTmpRec;
	int i, nIndex;
	CString str;

	m_cNoiseSrcList.ResetContent();

	str.LoadString(IDS_UNKNOWN);
	str = "(" + str + ")";
	nIndex = m_cNoiseSrcList.AddString(str);
	m_cNoiseSrcList.SetItemData(nIndex, (DWORD_PTR)-1);
	if (nID == -1)
		m_cNoiseSrcList.SetCurSel(-1);

	if (!dbNsTmp.Open())
		return;

	for (i = 0; ; i++) {
		if (!dbNsTmp.ReadRecNext(&dbNsTmpRec, NULL))
			break;

		nIndex = m_cNoiseSrcList.AddString(dbNsTmpRec.sName);
		m_cNoiseSrcList.SetItemData(nIndex, dbNsTmpRec.nNsTmpID);

		if (dbNsTmpRec.nNsTmpID == nID)
			m_cNoiseSrcList.SetCurSel(nIndex);
	}
}

void CChangeNoiseSrcDlg::OnAddNew()
{
	CNoiseSourceDlg dlg;

	dlg.DoModal();

	SetNsTmpList(m_pNmsItem->m_nNoiseTmpID);
}

void CChangeNoiseSrcDlg::OnDblclkNoiseSrcList()
{
	OnOK();
}

BOOL CChangeNoiseSrcDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDOK, IDH_CHANGE_NOISE_OK,
		IDCANCEL, IDH_CHANGE_NOISE_CANCEL,
		IDC_ADD_NEW, IDH_CHANGE_NOISE_ADD_NEW,
		IDC_NOISE_SRC_LIST, IDH_CHANGE_NOISE_SRC_LIST,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
