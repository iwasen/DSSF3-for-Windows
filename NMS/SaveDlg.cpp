// SaveDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Nms.h"
#include "SaveDlg.h"
#include "NmsDoc.h"
#include "NmsItem.h"
#include "Help\ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CSaveDlg ダイアログ

CSaveDlg::CSaveDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(CSaveDlg::IDD, pParent)
{
	m_bWaveNoSave = FALSE;
}

void CSaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FOLDER_LIST, m_cFolderList);
	DDX_Control(pDX, IDC_DATE, m_cDate);
	DDX_Control(pDX, IDC_NAME, m_cName);
	DDX_Control(pDX, IDC_PLACE, m_cPlace);
	DDX_Control(pDX, IDC_TITLE, m_cTitle);
	DDX_Control(pDX, IDC_COMMENT, m_cComment);
	DDX_Check(pDX, IDC_WAVE_NOSAVE, m_bWaveNoSave);
}

BEGIN_MESSAGE_MAP(CSaveDlg, CDialogAF)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaveDlg メッセージ ハンドラ

BOOL CSaveDlg::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	struct LISTCOLUMN {
		UINT textID;
		int width;
	};
	static const LISTCOLUMN tColumn[] = {
		{IDS_TITLE, 150},
		{IDS_NAME, 100},
		{IDS_PLACE, 100},
		{IDS_DATE, 100},
		{IDS_COMMENT, 200}
	};
	int i;
	LV_COLUMN lvc;
	CDbFolder dbFolder;
	DbFolderRec dbFolderRec;
	CString str;

	m_cFolderList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	for (i = 0; i < sizeof(tColumn) / sizeof(LISTCOLUMN); i++) {
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		lvc.fmt = LVCFMT_LEFT;
		str.LoadString(tColumn[i].textID);
		lvc.pszText = (LPTSTR)(LPCSTR)str;
		lvc.cx = CDpi::AdjustDpi(tColumn[i].width, 95);
		m_cFolderList.InsertColumn(i, &lvc);
	}

	if (!dbFolder.Open())
		return TRUE;

	CString strFilter;
	strFilter.Format("TYPE=%c", FOLDER_TYPE_NMS);
	dbFolder.DBSetFilter(strFilter);

	for (i = 0; ; i++) {
		if (!dbFolder.ReadRecNext(&dbFolderRec))
			break;

		m_cFolderList.InsertItem(i, dbFolderRec.sTitle);
		m_cFolderList.SetItemText(i, 1, dbFolderRec.sName);
		m_cFolderList.SetItemText(i, 2, dbFolderRec.sPlace);
		m_cFolderList.SetItemText(i, 3, FormatTime(dbFolderRec.sDate));
		m_cFolderList.SetItemText(i, 4, dbFolderRec.sComment);

		m_cFolderList.SetItemData(i, dbFolderRec.nFolderID);
	}

	m_nSortItem = 0;
	m_cFolderList.SortItems(CompareItems, (LPARAM)this);

	SYSTEMTIME st;
	::GetLocalTime(&st);
	m_cDate.Format("%04d/%02d/%02d", st.wYear, st.wMonth, st.wDay);

	m_cTitle.SetMaxLength(LEN_TITLE);
	m_cName.SetMaxLength(LEN_NAME);
	m_cPlace.SetMaxLength(LEN_PLACE);
	m_cComment.SetMaxLength(LEN_COMMENT);

	return TRUE;
}

int CALLBACK CSaveDlg::CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CSaveDlg *pDlg = (CSaveDlg *)lParamSort;
	CListCtrl &dataList = pDlg->m_cFolderList;
	LV_FINDINFO findInfo;
	int index1, index2;

	findInfo.flags = LVFI_PARAM;
	findInfo.lParam = lParam1;
	index1 = dataList.FindItem(&findInfo);

	findInfo.flags = LVFI_PARAM;
	findInfo.lParam = lParam2;
	index2 = dataList.FindItem(&findInfo);

	return dataList.GetItemText(index1, pDlg->m_nSortItem).Compare(dataList.GetItemText(index2, pDlg->m_nSortItem)) * pDlg->m_nSortDir;
}

void CSaveDlg::OnOK()
{
	long folderID;

	if (m_cTitle.IsEmpty()) {
		::AfxMessageBox(IDS_ERR_TITLE, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	if (::AfxMessageBox(IDS_MSG_SAVEDATA, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
		return;

	if (!MakeFolder(folderID))
		return;

	if (!SaveData(folderID))
		return;

	CDialogAF::OnOK();
}

BOOL CSaveDlg::MakeFolder(long &nFolderID)
{
	CDbFolder dbFolder;

	if (!dbFolder.Open())
		return FALSE;

	if (!dbFolder.GetNewID(&nFolderID))
		return FALSE;

	DbFolderRec dbFolderRec;
	dbFolderRec.nFolderID = nFolderID;
	dbFolderRec.sType = FOLDER_TYPE_NMS;
	dbFolderRec.sName = m_cName;
	dbFolderRec.sPlace = m_cPlace;
	CString sDate = m_cDate;
	dbFolderRec.sDate = sDate.Left(4) + sDate.Mid(5, 2) + sDate.Mid(8, 2);
	dbFolderRec.sTitle = m_cTitle;
	dbFolderRec.sComment = m_cComment;
	dbFolderRec.fScale = 1.0;

	if (!dbFolder.StoreRec(&dbFolderRec))
		return FALSE;

	return TRUE;
}

BOOL CSaveDlg::SaveData(long nFolderID)
{
	CDbNms dbNms;
	if (!dbNms.Open())
		return FALSE;

	DbNmsRec dbNmsRec;
	CNmsItem *pNmsItem;
	CString tempFileName;
	HWAVEDATA hWaveData;
	PWAVEDATA pWaveData;

	POSITION pos = m_pItemList->GetHeadPosition();

	while (pos != NULL) {
		pNmsItem = (CNmsItem *)m_pItemList->GetNext(pos);

		if (pNmsItem->m_bSaved)
			continue;

		if (!dbNms.GetNewID(&dbNmsRec.nNmsID))
			return FALSE;

		tempFileName.Format("%s\\%d.WAV", (LPCSTR)g_sTempDir, pNmsItem->m_nIndex);
		if (MMWaveReadFile(&hWaveData, (LPSTR)(LPCSTR)tempFileName) != 0)
			return FALSE;

		pWaveData = (PWAVEDATA)::GlobalLock(hWaveData);

		dbNmsRec.nFolderID = nFolderID;
		dbNmsRec.nSampling = pWaveData->nSamplesPerSec;
		dbNmsRec.nChannel = pWaveData->nChannels | (pWaveData->nBitsPerSample >= 24 ? 0x04 : 0);
		dbNmsRec.nSampleNum = pWaveData->dataSize / (pWaveData->nChannels * pWaveData->nBitsPerSample / 8);
		dbNmsRec.nNumber = pNmsItem->m_nIndex;
		dbNmsRec.fDataTime = pNmsItem->m_fDataTime;
		dbNmsRec.sTime = pNmsItem->m_cTime.Format("%Y%m%d%H%M%S");

		::GlobalUnlock(hWaveData);

		dbNmsRec.nConditionSize = sizeof(NmsConditionData);
		dbNmsRec.nFactorSize = sizeof(NmsFactorData);
		dbNmsRec.nNmsFactor = pNmsItem->m_nNmsFactorData;

		if (!dbNms.StoreRec(&dbNmsRec))
			return FALSE;

		if (!m_bWaveNoSave) {
			if (MMWaveWriteDB(hWaveData, dbNms.GetFCB(), "WAVEDATA", MMTYPE_WAV) != 0) {
				::AfxMessageBox(IDS_ERR_WRITEDATA, MB_OK | MB_ICONEXCLAMATION);
				return FALSE;
			}
		}

		dbNms.DBWriteBinary("NMSCOND", &pNmsItem->m_NmsConditionData, sizeof(NmsConditionData));
		if (pNmsItem->m_pNmsFactorData != NULL)
			dbNms.DBWriteBinary("NMSFACTOR", pNmsItem->m_pNmsFactorData, sizeof(NmsFactorData) * pNmsItem->m_nNmsFactorData);
		dbNms.DBWriteBinary("NOISESRC", &pNmsItem->m_NoiseSrcData, sizeof(NoiseSrcData));

		pNmsItem->m_bSaved = TRUE;
	}

	return TRUE;
}

BOOL CSaveDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDOK, IDH_SAVE_OK,
		IDCANCEL, IDH_SAVE_CANCEL,
		IDC_FOLDER_LIST, IDH_SAVE_FOLDER_LIST,
		IDC_TITLE, IDH_SAVE_TITLE,
		IDC_WAVE_NOSAVE, IDH_SAVE_WAVE_NOSAVE,
		IDC_TITLE, IDH_SAVE_TITLE,
		IDC_NAME, IDH_SAVE_NAME,
		IDC_PLACE, IDH_SAVE_PLACE,
		IDC_COMMENT, IDH_SAVE_COMMENT,
		IDC_DATE, IDH_SAVE_DATE,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
