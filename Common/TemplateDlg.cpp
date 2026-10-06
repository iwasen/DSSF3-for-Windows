// TemplateDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "comres.h"
#include "Common.h"
#include "FileIO.h"
#include "TemplateDlg.h"
#include "TempExportDlg.h"
#include "TempImportDlg.h"
#include "Help\CommonHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CTemplateDlg ダイアログ

CTemplateDlg::CTemplateDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(CTemplateDlg::IDD, pParent)
{
}

void CTemplateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NOISE_SOURCE, m_cNoiseSource);
	DDX_Control(pDX, IDC_TEMPLATE_LIST, m_cTemplateList);
	DDX_Control(pDX, IDC_TAB, m_cTab);
}

BEGIN_MESSAGE_MAP(CTemplateDlg, CDialogAF)
	ON_BN_CLICKED(IDC_DELETE_TEMPLATE, OnDeleteTemplate)
	ON_LBN_SELCHANGE(IDC_TEMPLATE_LIST, OnSelchangeTemplateList)
	ON_BN_CLICKED(IDC_UPDATE_TEMPLATE, OnUpdateTemplate)
	ON_BN_CLICKED(IDC_IMPORT, OnImport)
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplateDlg メッセージ ハンドラ

BOOL CTemplateDlg::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	SetNsTmpList(-1);

	m_cTab.AddPage(&m_TempAcf, IDD_TEMP_ACF, "ACF", TRUE);
	m_cTab.AddPage(&m_TempIAcf, IDD_TEMP_IACF, "IACF", TRUE);
	m_cTab.SetPage(0);

	return TRUE;
}

void CTemplateDlg::OnOK()
{
	SaveNoiseTemplate(-1);
}

void CTemplateDlg::OnUpdateTemplate()
{
	int nIndex;

	if ((nIndex = m_cTemplateList.GetCurSel()) == LB_ERR)
		return;

	SaveNoiseTemplate((long)m_cTemplateList.GetItemData(nIndex));
}

void CTemplateDlg::OnDeleteTemplate()
{
	int nIndex;

	if ((nIndex = m_cTemplateList.GetCurSel()) == LB_ERR)
		return;

	CDbNsTmp dbNsTmp;
	if (!dbNsTmp.Open())
		return;

	dbNsTmp.DeleteRec((long)m_cTemplateList.GetItemData(nIndex));
	SetNsTmpList(-1);
}

void CTemplateDlg::OnSelchangeTemplateList()
{
	int nIndex;

	if ((nIndex = m_cTemplateList.GetCurSel()) == LB_ERR)
		return;

	CDbNsTmp dbNsTmp;
	if (!dbNsTmp.Open())
		return;

	DbNsTmpRec dbNsTmpRec;
	NsTmpData nsTmpData;
	if (!dbNsTmp.ReadRecID((long)m_cTemplateList.GetItemData(nIndex), &dbNsTmpRec, &nsTmpData))
		return;

	m_cNoiseSource = dbNsTmpRec.sName;

	// データ表示
	m_TempAcf.SetData(nsTmpData);
	m_TempIAcf.SetData(nsTmpData);
}

void CTemplateDlg::SetNsTmpList(long nID)
{
	m_cTemplateList.ResetContent();

	CDbNsTmp dbNsTmp;
	if (!dbNsTmp.Open())
		return;

	DbNsTmpRec dbNsTmpRec;
	int nIndex;

	for (int i = 0; ; i++) {
		if (!dbNsTmp.ReadRecNext(&dbNsTmpRec, NULL))
			break;

		nIndex = m_cTemplateList.AddString(dbNsTmpRec.sName);
		m_cTemplateList.SetItemData(nIndex, dbNsTmpRec.nNsTmpID);

		if (dbNsTmpRec.nNsTmpID == nID)
			m_cTemplateList.SetCurSel(nIndex);
	}
}

void CTemplateDlg::SaveNoiseTemplate(long nID)
{
	if (m_cNoiseSource == "") {
		::AfxMessageBox(IDS_ERR_NOISE_SOURCE, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	CDbNsTmp dbNsTmp;
	if (!dbNsTmp.Open())
		return;

	DbNsTmpRec dbNsTmpRec;
	NsTmpData nsTmpData;

	if (nID == -1) {
		if (!dbNsTmp.GetNewID(&dbNsTmpRec.nNsTmpID))
			return;
	} else
		dbNsTmpRec.nNsTmpID = nID;

	dbNsTmpRec.sName = m_cNoiseSource;

	// データ取得
	m_TempAcf.GetData(nsTmpData);
	m_TempIAcf.GetData(nsTmpData);

	nsTmpData.nSum = 0;

	if (nID == -1)
		dbNsTmp.StoreRec(&dbNsTmpRec, &nsTmpData);
	else
		dbNsTmp.UpdateRec(&dbNsTmpRec, &nsTmpData);

	SetNsTmpList(dbNsTmpRec.nNsTmpID);
}

void CTemplateDlg::OnImport()
{
	CTempImportDlg dlg(this);

	if (dlg.DoModal() == IDOK) {
		int nIndex;
		long nID;
		if ((nIndex = m_cTemplateList.GetCurSel()) != LB_ERR)
			nID = (long)m_cTemplateList.GetItemData(nIndex);
		else
			nID = -1;
		SetNsTmpList(nID);
		OnSelchangeTemplateList();

		m_TempAcf.ShowWeightings();
		m_TempIAcf.ShowWeightings();
	}
}

void CTemplateDlg::OnExport()
{
	CTempExportDlg dlg(this);
	int nIndex;

	if ((nIndex = m_cTemplateList.GetCurSel()) != LB_ERR)
		dlg.m_nSelectID = (long)m_cTemplateList.GetItemData(nIndex);

	dlg.DoModal();
}

BOOL CTemplateDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDOK, IDH_TEMPLATE_OK,
		IDC_UPDATE_TEMPLATE, IDH_TEMPLATE_UPDATE_TEMPLATE,
		IDC_DELETE_TEMPLATE, IDH_TEMPLATE_DELETE_TEMPLATE,
		IDC_EXPORT, IDH_TEMPLATE_EXPORT,
		IDC_IMPORT, IDH_TEMPLATE_IMPORT,
		IDCANCEL, IDH_TEMPLATE_CANCEL,
		IDC_TEMPLATE_LIST, IDH_TEMPLATE_TEMPLATE_LIST,
		IDC_NOISE_SOURCE, IDH_TEMPLATE_NOISE_SOURCE,
		IDC_TAB, IDH_TEMPLATE_TAB,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
