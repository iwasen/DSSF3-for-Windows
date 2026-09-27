// DataRecordDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "FftWnd.h"
#include "DataRecordDlg.h"
#include "Help\ContextHelp.h"

// CDataRecordDlg ダイアログ

CDataRecordDlg::CDataRecordDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(CDataRecordDlg::IDD, pParent)
{
	m_nSortItem = 0;
	m_nSortDir = 1;
	m_nOctBand = 0;

	m_pParentWnd = pParent;
	Create(IDD, NULL);
	ShowWindow(SW_SHOW);
}

void CDataRecordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATA_LIST, m_cDataList);
	DDX_Control(pDX, IDC_TIMING_CYCLE, m_cTimingCycle);
	DDX_Control(pDX, IDC_TIMING_MANUAL, m_cTimingManual);
	DDX_Control(pDX, IDC_TIMING_INTERVAL, m_cTimingInterval);
	DDX_Control(pDX, IDC_TIMING_RECORD, m_cTimingRecord);
	DDX_Control(pDX, IDC_DELETE_AT_START, m_cDeleteAtStart);
	DDX_Control(pDX, IDC_ALWAYS_NEW_DATA, m_cAlwaysNewData);
	DDX_Control(pDX, IDC_FILE_SAVE, m_cFileSave);
	DDX_Control(pDX, IDC_TIMING_SPIN, m_cTimingSpin);
}

BEGIN_MESSAGE_MAP(CDataRecordDlg, CDialogAF)
	ON_BN_CLICKED(IDC_FILE_SAVE, OnBnClickedFileSave)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnBnClickedOK)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_TIMING_CYCLE, OnBnClickedTimingCycle)
	ON_BN_CLICKED(IDC_TIMING_MANUAL, OnBnClickedTimingManual)
	ON_EN_CHANGE(IDC_TIMING_INTERVAL, OnEnChangeTimingInterval)
	ON_BN_CLICKED(IDC_TIMING_RECORD, OnBnClickedTimingRecord)
	ON_BN_CLICKED(IDC_DELETE_DATA, OnBnClickedDeleteData)
	ON_BN_CLICKED(IDC_DELETE_AT_START, OnBnClickedDeleteAtStart)
	ON_BN_CLICKED(IDC_ALWAYS_NEW_DATA, OnBnClickedAlwaysNewData)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_DATA_LIST, OnLvnColumnclickDataList)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

// CDataRecordDlg メッセージ ハンドラ

BOOL CDataRecordDlg::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	if (g_oSetData.Dtr.nPosLeft != 0 || g_oSetData.Dtr.nPosTop != 0)
		SetWindowPos(NULL, g_oSetData.Dtr.nPosLeft, g_oSetData.Dtr.nPosTop, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	m_cTimingCycle.SetCheck(g_oSetData.Dtr.nTimingMode == TIMING_CYCLE);
	m_cTimingManual.SetCheck(g_oSetData.Dtr.nTimingMode == TIMING_MANUAL);

	if (g_oSetData.Dtr.nTimingInterval == 0)
		g_oSetData.Dtr.nTimingInterval = 1;
	m_cTimingInterval = g_oSetData.Dtr.nTimingInterval;

	m_cTimingSpin.SetRange(1, 9999);

	m_cDeleteAtStart = g_oSetData.Dtr.bDeleteAtStart;
	m_cAlwaysNewData = g_oSetData.Dtr.bAlwaysNewData;

	EnableControls();

	m_cTimingInterval.SetValidChar(VC_NUM);

	return TRUE;
}

void CDataRecordDlg::OnBnClickedFileSave()
{
	CFileDialog fileDlg(FALSE, "csv", NULL,
			OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
			"CSV File (*.csv)|*.csv|Text File (*.txt)|*.txt|All Files (*.*)|*.*||", this, 0);

	if (fileDlg.DoModal() == IDCANCEL)
		return;

	FILE *fp;
	if (fopen_s(&fp, fileDlg.GetPathName(), "w") == 0) {
		int i, j;
		CHeaderCtrl *pHeaderCtrl = m_cDataList.GetHeaderCtrl();
		int nItem = m_cDataList.GetItemCount();
		int nColumn = pHeaderCtrl->GetItemCount();
		char textBuf[32];

		for (j = 0; j < nColumn; j++) {
			if (j != 0)
				fprintf(fp, ",");
			HDITEM hdItem{
				.mask = HDI_TEXT,
				.pszText = textBuf,
				.cchTextMax = sizeof(textBuf)
			};
			pHeaderCtrl->GetItem(j, &hdItem);
			fprintf(fp, "%s", textBuf);
		}
		fprintf(fp, "\n");

		for (i = 0; i < nItem; i++) {
			for (j = 0; j < nColumn; j++) {
				if (j != 0)
					fprintf(fp, ",");
				fprintf(fp, "%s", (LPCSTR)m_cDataList.GetItemText(i, j));
			}
			fprintf(fp, "\n");
		}
		fclose(fp);
	} else
		::AfxMessageBox(IDS_ERR_OPENOUTPUTFILE);
}

void CDataRecordDlg::OnBnClickedCancel()
{
	m_pParentWnd->SendMessage(WM_DATA_RECORD, DR_CLOSE);
}

void CDataRecordDlg::OnBnClickedOK()
{
}

void CDataRecordDlg::PostNcDestroy()
{
	delete this;
}

void CDataRecordDlg::InitDataList(int nOctBand, const CString *pFreqScale)
{
	int i;

	m_nOctBand = nOctBand;

	DeleteRecordData();

	while (m_cDataList.DeleteColumn(0))
		;

	m_cDataList.InsertColumn(0, "No.", LVCFMT_RIGHT, 55);
	m_cDataList.InsertColumn(1, "Sec.", LVCFMT_RIGHT, 55);

	if (g_oSetData.Fft.nChannel != CH_STEREO) {
		if (!g_oSetData.Fft.bPeakHold) {
			for (i = 0; i < m_nOctBand; i++) {
				m_cDataList.InsertColumn(i + 2, pFreqScale[i], LVCFMT_RIGHT, 55);
			}
		} else {
			for (i = 0; i < m_nOctBand; i++) {
				m_cDataList.InsertColumn(i * 2 + 2, pFreqScale[i], LVCFMT_RIGHT, 55);
				m_cDataList.InsertColumn(i * 2 + 3, pFreqScale[i] + "-p", LVCFMT_RIGHT, 55);
			}
		}
	} else {
		if (!g_oSetData.Fft.bPeakHold) {
			for (i = 0; i < m_nOctBand; i++) {
				m_cDataList.InsertColumn(i * 2 + 2, pFreqScale[i] + "-L", LVCFMT_RIGHT, 55);
				m_cDataList.InsertColumn(i * 2 + 3, pFreqScale[i] + "-R", LVCFMT_RIGHT, 55);
			}
		} else {
			for (i = 0; i < m_nOctBand; i++) {
				m_cDataList.InsertColumn(i * 4 + 2, pFreqScale[i] + "-L", LVCFMT_RIGHT, 55);
				m_cDataList.InsertColumn(i * 4 + 3, pFreqScale[i] + "-Lp", LVCFMT_RIGHT, 55);
				m_cDataList.InsertColumn(i * 4 + 4, pFreqScale[i] + "-R", LVCFMT_RIGHT, 55);
				m_cDataList.InsertColumn(i * 4 + 5, pFreqScale[i] + "-Rp", LVCFMT_RIGHT, 55);
			}
		}
	}
}

void CDataRecordDlg::OnDestroy()
{
	CRect rect;

	GetWindowRect(rect);
	g_oSetData.Dtr.nPosLeft = rect.left;
	g_oSetData.Dtr.nPosTop = rect.top;

	CDialogAF::OnDestroy();
}

void CDataRecordDlg::OnBnClickedTimingCycle()
{
	g_oSetData.Dtr.nTimingMode = TIMING_CYCLE;
	EnableControls();
}

void CDataRecordDlg::OnBnClickedTimingManual()
{
	g_oSetData.Dtr.nTimingMode = TIMING_MANUAL;
	EnableControls();
}

void CDataRecordDlg::OnEnChangeTimingInterval()
{
	if (m_cTimingInterval.m_hWnd != NULL) {
		g_oSetData.Dtr.nTimingInterval = m_cTimingInterval;
		if (g_oSetData.Dtr.nTimingInterval == 0) {
			g_oSetData.Dtr.nTimingInterval = 1;
			m_cTimingInterval = g_oSetData.Dtr.nTimingInterval;
		}
	}
}

void CDataRecordDlg::EnableControls()
{
	if (g_oSetData.Dtr.nTimingMode == TIMING_CYCLE) {
		m_cTimingInterval.EnableWindow(TRUE);
		m_cTimingRecord.EnableWindow(FALSE);
	} else {
		m_cTimingInterval.EnableWindow(FALSE);
		m_cTimingRecord.EnableWindow(TRUE);
	}
}
void CDataRecordDlg::OnBnClickedTimingRecord()
{
	m_pParentWnd->SendMessage(WM_DATA_RECORD, DR_RECORD);
}

void CDataRecordDlg::OnBnClickedDeleteData()
{
	DeleteRecordData();
}

void CDataRecordDlg::RecordData(int sec, FFTDATA *pFftData)
{
	CString str;
	int i;
	int nItem = m_cDataList.GetItemCount();

	if (nItem == 0)
		m_cFileSave.EnableWindow(TRUE);

	str.Format("%d", nItem + 1);
	m_cDataList.InsertItem(nItem, str);
	m_cDataList.SetItemData(nItem, nItem);

	str.Format("%d", sec);
	m_cDataList.SetItemText(nItem, 1, str);

	if (g_oSetData.Fft.nChannel != CH_STEREO) {
		if (g_oSetData.Fft.nChannel == CH_RIGHT_ONLY)
			pFftData++;

		if (!g_oSetData.Fft.bPeakHold) {
			for (i = 0; i < m_nOctBand; i++) {
				m_cDataList.SetItemText(nItem, 2 + i, FormatData(pFftData[0].m_fOctData[i]));
			}
		} else {
			for (i = 0; i < m_nOctBand; i++) {
				m_cDataList.SetItemText(nItem, 2 + i * 2, FormatData(pFftData[0].m_fOctData[i]));
				m_cDataList.SetItemText(nItem, 3 + i * 2, FormatData(pFftData[0].m_fOctPeakLevel[i]));
			}
		}
	} else {
		if (!g_oSetData.Fft.bPeakHold) {
			for (i = 0; i < m_nOctBand; i++) {
				m_cDataList.SetItemText(nItem, 2 + i * 2, FormatData(pFftData[0].m_fOctData[i]));
				m_cDataList.SetItemText(nItem, 3 + i * 2, FormatData(pFftData[1].m_fOctData[i]));
			}
		} else {
			for (i = 0; i < m_nOctBand; i++) {
				m_cDataList.SetItemText(nItem, 2 + i * 4, FormatData(pFftData[0].m_fOctData[i]));
				m_cDataList.SetItemText(nItem, 3 + i * 4, FormatData(pFftData[0].m_fOctPeakLevel[i]));
				m_cDataList.SetItemText(nItem, 4 + i * 4, FormatData(pFftData[1].m_fOctData[i]));
				m_cDataList.SetItemText(nItem, 5 + i * 4, FormatData(pFftData[1].m_fOctPeakLevel[i]));
			}
		}
	}

	if (g_oSetData.Dtr.bAlwaysNewData)
		m_cDataList.EnsureVisible(nItem, FALSE);
}

CString CDataRecordDlg::FormatData(double fData)
{
	CString str;

	if (fData != -1000)
		str.Format("%.2f", fData);

	return str;
}

void CDataRecordDlg::OnBnClickedDeleteAtStart()
{
	g_oSetData.Dtr.bDeleteAtStart = m_cDeleteAtStart;
}

void CDataRecordDlg::OnBnClickedAlwaysNewData()
{
	g_oSetData.Dtr.bAlwaysNewData = m_cAlwaysNewData;
}

void CDataRecordDlg::DeleteRecordData()
{
	m_cDataList.DeleteAllItems();
	m_cFileSave.EnableWindow(FALSE);
}

void CDataRecordDlg::OnLvnColumnclickDataList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (m_nSortItem != pNMLV->iSubItem)
		m_nSortDir = 1;
	else
		m_nSortDir *= -1;

	m_nSortItem = pNMLV->iSubItem;

	m_cDataList.SortItems(CompareItems, (LPARAM)this);

	*pResult = 0;
}

int CALLBACK CDataRecordDlg::CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CDataRecordDlg *pDlg = (CDataRecordDlg *)lParamSort;
	CListCtrl &dataList = pDlg->m_cDataList;

	LV_FINDINFO findInfo1{
		.flags = LVFI_PARAM,
		.lParam = lParam1
	};
	int index1 = dataList.FindItem(&findInfo1);
	double fData1 = atof(dataList.GetItemText(index1, pDlg->m_nSortItem));

	LV_FINDINFO findInfo2{
		.flags = LVFI_PARAM,
		.lParam = lParam2
	};
	int index2 = dataList.FindItem(&findInfo2);
	double fData2 = atof(dataList.GetItemText(index2, pDlg->m_nSortItem));

	double fComp = (fData1 - fData2) * pDlg->m_nSortDir;
	if (fComp > 0)
		return 1;
	else if (fComp < 0)
		return -1;
	else
		return 0;
}

BOOL CDataRecordDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_DATA_LIST, IDH_FFT_REC_DATA_LIST,
		IDC_TIMING_CYCLE, IDH_FFT_REC_TIMING_CYCLE,
		IDC_TIMING_INTERVAL, IDH_FFT_REC_TIMING_INTERVAL,
		IDC_TIMING_MANUAL, IDH_FFT_REC_TIMING_MANUAL,
		IDC_TIMING_RECORD, IDH_FFT_REC_TIMING_RECORD,
		IDC_DELETE_DATA, IDH_FFT_REC_DELETE_DATA,
		IDC_DELETE_AT_START, IDH_FFT_REC_DELETE_AT_START,
		IDC_ALWAYS_NEW_DATA, IDH_FFT_REC_ALWAYS_NEW_DATA,
		IDC_FILE_SAVE, IDH_FFT_REC_FILE_SAVE,
		IDCANCEL, IDH_FFT_REC_CLOSE,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
