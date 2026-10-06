// TempImportDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "comres.h"
#include "Common.h"
#include "FileIO.h"
#include "TempImportDlg.h"
#include "NsTemp.h"
#include "Help\CommonHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CTempImportDlg ダイアログ

CTempImportDlg::CTempImportDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(CTempImportDlg::IDD, pParent)
{
	m_iAppendMode = 0;
}

void CTempImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TEMPLATE_LIST, m_cTemplateList);
	DDX_Radio(pDX, IDC_MERGE, m_iAppendMode);
}

BEGIN_MESSAGE_MAP(CTempImportDlg, CDialogAF)
	ON_BN_CLICKED(IDC_ALL_SELECT, OnAllSelect)
	ON_BN_CLICKED(IDC_ALL_REMOVE, OnAllRemove)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTempImportDlg メッセージ ハンドラ

BOOL CTempImportDlg::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	if (!ReadTemplateFile()) {
		EndDialog(IDCANCEL);
		return TRUE;
	}

	return TRUE;
}

void CTempImportDlg::OnAllSelect()
{
	const int nCount = m_cTemplateList.GetCount();
	for (int i = 0; i < nCount; i++)
		m_cTemplateList.SetSel(i);
}

void CTempImportDlg::OnAllRemove()
{
	const int nCount = m_cTemplateList.GetCount();
	for (int i = 0; i < nCount; i++)
		m_cTemplateList.SetSel(i, FALSE);
}

void CTempImportDlg::OnOK()
{
	if (ImportTemplate())
		EndDialog(IDOK);
}

BOOL CTempImportDlg::ReadTemplateFile()
{
	CFileDialog fileDlg(TRUE, "nst", NULL,
			OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
			"Template File (*.nst)|*.nst||", this, 0);

	if (fileDlg.DoModal() == IDCANCEL)
		return FALSE;

	m_PathName = fileDlg.GetPathName();

	CFile importFile;
	if (!importFile.Open(m_PathName, CFile::modeRead | CFile::shareExclusive)) {
		::AfxMessageBox(IDS_ERR_IMPORT_FILE);
		return FALSE;
	}

	NsTmpData nsTmpData;
	char name[LEN_NSTMPNAME + 1];
	while (importFile.Read(name, sizeof(name)) == sizeof(name)) {
		const int nIndex = m_cTemplateList.AddString(name);
		m_cTemplateList.SetSel(nIndex);
		importFile.Read(&nsTmpData, sizeof(nsTmpData));
	}

	return TRUE;
}

BOOL CTempImportDlg::ImportTemplate()
{
	CDbNsTmp dbNsTmp;
	if (!dbNsTmp.Open())
		return FALSE;

	CFile importFile;
	if (!importFile.Open(m_PathName, CFile::modeRead | CFile::shareExclusive)) {
		::AfxMessageBox(IDS_ERR_IMPORT_FILE);
		return FALSE;
	}

	DbNsTmpRec dbNsTmpRec;
	NsTmpData nsTmpData1;
	NsTmpData nsTmpData2;
	int find;
	char name[LEN_NSTMPNAME + 1];
	char nameBuf[LEN_NSTMPNAME];

	UpdateData(TRUE);

	for (int i = 0; importFile.Read(name, sizeof(name)) == sizeof(name); i++) {
		importFile.Read(&nsTmpData1, sizeof(nsTmpData1));
		if (m_cTemplateList.GetSel(i) > 0) {
			memset(nameBuf, ' ', sizeof(nameBuf));
			memcpy(nameBuf, name, strlen(name));
			dbNsTmp.DBChgIdx(dbNsTmp.m_nIdxNsTmpName);
			if (dbNsTmp.DBSearch2(nameBuf, LEN_NSTMPNAME, &find) == 0 && find == 1) {
				if (!dbNsTmp.ReadRecID(-1, &dbNsTmpRec, &nsTmpData2))
					return FALSE;

				if (m_iAppendMode == 0)
					MergeTemplate(&nsTmpData1, &nsTmpData2);

				if (!dbNsTmp.UpdateRec(&dbNsTmpRec, &nsTmpData1))
					return FALSE;
			} else {
				if (!dbNsTmp.GetNewID(&dbNsTmpRec.nNsTmpID))
					return FALSE;

				dbNsTmpRec.sName = name;

				if (!dbNsTmp.StoreRec(&dbNsTmpRec, &nsTmpData1))
					return FALSE;
			}
		}
	}

	::UpdateNsWeightData();

	return TRUE;
}

void CTempImportDlg::MergeTemplate(NsTmpData *pNsTmpData1, const NsTmpData *pNsTmpData2)
{
	const int nSum = pNsTmpData1->nSum + pNsTmpData2->nSum;
	if (nSum == 0)
		return;

	pNsTmpData1->fPhi0Standard = (pNsTmpData1->fPhi0Standard * pNsTmpData1->nSum + pNsTmpData2->fPhi0Standard * pNsTmpData2->nSum) / nSum;
	pNsTmpData1->fPhi0Sum2 += pNsTmpData2->fPhi0Sum2;

	pNsTmpData1->fTaueStandard = (pNsTmpData1->fTaueStandard * pNsTmpData1->nSum + pNsTmpData2->fTaueStandard * pNsTmpData2->nSum) / nSum;
	pNsTmpData1->fTaueSum2 += pNsTmpData2->fTaueSum2;

	pNsTmpData1->fTau1Standard = (pNsTmpData1->fTau1Standard * pNsTmpData1->nSum + pNsTmpData2->fTau1Standard * pNsTmpData2->nSum) / nSum;
	pNsTmpData1->fTau1Sum2 += pNsTmpData2->fTau1Sum2;

	pNsTmpData1->fPhi1Standard = (pNsTmpData1->fPhi1Standard * pNsTmpData1->nSum + pNsTmpData2->fPhi1Standard * pNsTmpData2->nSum) / nSum;
	pNsTmpData1->fPhi1Sum2 += pNsTmpData2->fPhi1Sum2;

	pNsTmpData1->nSum = nSum;
}

BOOL CTempImportDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDOK, IDH_TEMP_IMPORT_OK,
		IDCANCEL, IDH_TEMP_IMPORT_CANCEL,
		IDC_TEMPLATE_LIST, IDH_TEMP_IMPORT_TEMPLATE_LIST,
		IDC_MERGE, IDH_TEMP_IMPORT_MERGE,
		IDC_REPLACE, IDH_TEMP_IMPORT_REPLACE,
		IDC_ALL_SELECT, IDH_TEMP_IMPORT_ALL_SELECT,
		IDC_ALL_REMOVE, IDH_TEMP_IMPORT_ALL_REMOVE,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
