// MicCal2.cpp : 実装ファイル
//

#include "stdafx.h"
#include "RTA.h"
#include "RtaDlg.h"
#include "MicCal2.h"

#define NFREQ_RATE	44100
#define NFREQ_MIN	20
#define NFREQ_MAX	(NFREQ_RATE / 2)
#define NFFT_SIZE	4096

#define MIN_LEVEL_BAR	600
#define MAX_LEVEL_BAR	1400

#define MIN_SENS		60
#define MAX_SENS		140

// CMicCal2 ダイアログ

CMicCal2::CMicCal2(CWnd* pParent /*=NULL*/)
	: CDialogAF(CMicCal2::IDD, pParent)
{
	m_fFreq = 0.0;
	m_fLevel = 0.0;
	m_bUpdate = FALSE;

	m_pFreqData = NULL;
	m_pFilterTbl = NULL;
	memset(m_fMeanBuf, 0, sizeof(m_fMeanBuf));
	m_nMeanBufPtr = 0;
	m_fInputSens = 100;
}

CMicCal2::~CMicCal2()
{
	if (m_pFilterTbl != NULL)
		delete [] m_pFilterTbl;
}

void CMicCal2::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILTER, m_cFilter);
	DDX_Control(pDX, IDC_LEVEL_BAR, m_cLevelBar);
	DDX_Control(pDX, IDC_INPUT_LEVEL, m_cInputLevel);
	DDX_Control(pDX, IDC_FREQ_RESPONSE, m_cFreqResponse);
	DDX_Control(pDX, IDC_FREQ_LIST, m_cFreqList);
	DDX_Text(pDX, IDC_COMMENT, m_sComment);
	DDX_Text(pDX, IDC_FREQ, m_fFreq);
	DDX_Text(pDX, IDC_LEVEL, m_fLevel);
	DDX_Text(pDX, IDC_MICINFO, m_sMicInfo);
	DDX_Text(pDX, IDC_MICAMPINFO, m_sMicAmpInfo);
}

BEGIN_MESSAGE_MAP(CMicCal2, CDialogAF)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FREQ_LIST, OnItemchangedFreqList)
	ON_BN_CLICKED(IDC_FREQ_REGIST, OnFreqRegist)
	ON_BN_CLICKED(IDC_FREQ_DELETE, OnFreqDelete)
	ON_EN_CHANGE(IDC_MICINFO, OnChangeMicinfo)
	ON_EN_CHANGE(IDC_MICAMPINFO, OnChangeMicampinfo)
	ON_NOTIFY(LVN_INSERTITEM, IDC_FREQ_LIST, OnInsertitemFreqList)
	ON_NOTIFY(LVN_DELETEITEM, IDC_FREQ_LIST, OnDeleteitemFreqList)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_FILTER, OnSelchangeFilter)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_COMMENT, OnEnChangeComment)
END_MESSAGE_MAP()

// CMicCal2 メッセージ ハンドラ

BOOL CMicCal2::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	m_cFreqList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	InitFreqList();

	m_cLevelBar.SetScrollRange(MIN_LEVEL_BAR, MAX_LEVEL_BAR, FALSE);
	m_cLevelBar.SetScrollPos((int)(m_fInputSens * 10), FALSE);

	CString str;
	LOGFONT font{
		.lfHeight = 140,
		.lfCharSet = DEFAULT_CHARSET
	};
	str.LoadString(IDS_DEFAULTFONT);
	strcpy_s(font.lfFaceName, str);

	m_Font.CreatePointFontIndirect(&font);
	m_cInputLevel.SetFont(&m_Font);

	SetFilterNameList(m_cFilter);
	m_cFilter.SetCurSel(0);

	m_pFreqData = new double[NFFT_SIZE];
	m_pFilterTbl = new double[WAVEBUF_SAMPLESPERBUFFER];

	m_cFreqResponse.Initialize(80, NULL, 0, RGB(255, 0, 0), 0);
	m_cFreqResponse.EnableWindow(TRUE);
	DispFreqWindow();

	m_bUpdate = FALSE;

	return TRUE;
}

void CMicCal2::OnDestroy()
{
	CDialogAF::OnDestroy();

	if (m_pFreqData != NULL) {
		delete [] m_pFreqData;
		m_pFreqData = NULL;
	}
}

void CMicCal2::InitFreqList()
{
	struct LISTCOLUMN {
		UINT textID;
		int width;
	};
	static const LISTCOLUMN tColumn[] = {
		{IDS_FREQUENCY, 55},
		{IDS_LEVEL, 55}
	};
	CString str;

	for (int i = 0; i < ARRAY_SIZE(tColumn); i++) {
		str.LoadString(tColumn[i].textID);
		LV_COLUMN lvc{
			.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT,
			.fmt = LVCFMT_LEFT,
			.cx = CDpi::AdjustDpi(tColumn[i].width, 92),
			.pszText = (LPSTR)(LPCSTR)str
		};
		m_cFreqList.InsertColumn(i, &lvc);
	}

	m_cFreqList.InsertItem(0, "10");
	m_cFreqList.SetItemText(0, 1, "0");
	m_cFreqList.SetItemData(0, (DWORD)10);
	m_cFreqList.InsertItem(1, "100");
	m_cFreqList.SetItemText(1, 1, "0");
	m_cFreqList.SetItemData(1, (DWORD)100);
	m_cFreqList.InsertItem(2, "1000");
	m_cFreqList.SetItemText(2, 1, "0");
	m_cFreqList.SetItemData(2, (DWORD)1000);
	m_cFreqList.InsertItem(3, "10000");
	m_cFreqList.SetItemText(3, 1, "0");
	m_cFreqList.SetItemData(3, (DWORD)10000);
}

void CMicCal2::ReadMicCalData(int nMicCalID)
{
	CDbMicCal dbMicCal;
	if (!dbMicCal.Open())
		return;

	if (!dbMicCal.SearchRecID(nMicCalID))
		return;

	DbMicCalRec dbMicCalRec;
	if (!dbMicCal.ReadRecID(nMicCalID, &dbMicCalRec))
		return;

	m_sMicInfo = dbMicCalRec.sMicInfo;
	m_sMicAmpInfo = dbMicCalRec.sMicAmpInfo;
	m_sComment = dbMicCalRec.sComment;
	m_fInputSens = -dbMicCalRec.fInputSens;
	UpdateData(FALSE);

	m_cLevelBar.SetScrollPos((int)(m_fInputSens * 10), TRUE);

	CString str;
	m_cFreqList.DeleteAllItems();
	for (int i = 0; i < dbMicCalRec.nFreqData; i++) {
		str.Format("%g", dbMicCalRec.aFreq[i].freq);
		m_cFreqList.InsertItem(i, str);
		str.Format("%g", dbMicCalRec.aFreq[i].level);
		m_cFreqList.SetItemText(i, 1, str);
		m_cFreqList.SetItemData(i, (DWORD)dbMicCalRec.aFreq[i].freq);
	}
	m_cFreqList.SortItems(CompareFreq, (LPARAM)this);

	if (m_pFreqData != NULL)
		DispFreqWindow();

	CRtaApp *pWinApp = (CRtaApp *)::AfxGetApp();
	pWinApp->m_pWndRta->SetInputDevice(dbMicCalRec.nInputSel, dbMicCalRec.nInputVol);

	m_bUpdate = FALSE;
}

int CMicCal2::RegistMicCalData(int nMicCalID)
{
	UpdateData(TRUE);

	CDbMicCal dbMicCal;
	if (!dbMicCal.Open())
		return -1;

	DbMicCalRec dbMicCalRec;
	if (nMicCalID < 0)
		dbMicCalRec.nMicCalID = nMicCalID;
	else {
		if (!dbMicCal.GetNewID(&dbMicCalRec.nMicCalID))
			return -1;
	}

	GetMicCalData(&dbMicCalRec);

	dbMicCal.StoreRec(&dbMicCalRec);
	m_bUpdate = FALSE;

	return dbMicCalRec.nMicCalID;
}

void CMicCal2::ChangeMicCalData(int nMicCalID)
{
	UpdateData(TRUE);

	CDbMicCal dbMicCal;
	if (!dbMicCal.Open())
		return;

	DbMicCalRec dbMicCalRec;
	dbMicCalRec.nMicCalID = nMicCalID;
	GetMicCalData(&dbMicCalRec);

	dbMicCal.UpdateRec(&dbMicCalRec);
	m_bUpdate = FALSE;
}

void CMicCal2::OnItemchangedFreqList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (pNMListView->uNewState & LVIS_SELECTED) {
		m_fFreq = atof(m_cFreqList.GetItemText(pNMListView->iItem, 0));
		m_fLevel = atof(m_cFreqList.GetItemText(pNMListView->iItem, 1));
		UpdateData(FALSE);
	}
	*pResult = 0;
}

void CMicCal2::OnFreqRegist()
{
	int index;

	UpdateData(TRUE);

	if (m_fFreq <= 0) {
		::AfxMessageBox(IDS_MSG_UNDER0FREQ);
		return;
	}

	CString str;
	str.Format("%g", m_fFreq);

	LV_FINDINFO findInfo{
		.flags = LVFI_STRING,
		.psz = str
	};
	if ((index = m_cFreqList.FindItem(&findInfo)) == -1) {
		if (m_cFreqList.GetItemCount() == MICCAL_NFREQ) {
			str.FormatMessage(IDS_MSG_INPUTNDATA, MICCAL_NFREQ);
			::AfxMessageBox(str);
			return;
		}
		m_cFreqList.InsertItem(0, str);
		index = 0;
	}
	str.Format("%g", m_fLevel);
	m_cFreqList.SetItemText(index, 1, str);
	m_cFreqList.SetItemData(index, (DWORD)m_fFreq);

	m_cFreqList.SortItems(CompareFreq, (LPARAM)this);

	DispFreqWindow();

	m_bUpdate = TRUE;
}

void CMicCal2::OnFreqDelete()
{
	int index;

	UpdateData(TRUE);

	CString str;
	str.Format("%g", m_fFreq);

	LV_FINDINFO findInfo{
		.flags = LVFI_STRING,
		.psz = str
	};
	if ((index = m_cFreqList.FindItem(&findInfo)) != -1) {
		m_cFreqList.DeleteItem(index);
	}

	DispFreqWindow();

	m_bUpdate = TRUE;
}

void CMicCal2::DispFreqWindow()
{
	MakeFreqData();
	m_cFreqResponse.DispGraph(m_pFreqData, NFFT_SIZE, NFREQ_RATE, NFREQ_MIN, NFREQ_MAX, -20, 20, NULL, FALSE);
}

int CALLBACK CMicCal2::CompareFreq(LPARAM lParam1, LPARAM lParam2, LPARAM /*lParamSort*/)
{
	if (lParam1 > lParam2)
		return 1;
	else if (lParam1 < lParam2)
		return -1;
	else
		return 0;
}

void CMicCal2::MakeFreqData()
{
	FilterData micCalData[MICCAL_NFREQ];

	const int nItem = m_cFreqList.GetItemCount();
	for (int i = 0; i < nItem; i++) {
		micCalData[i].fFreq = atof(m_cFreqList.GetItemText(i, 0));
		micCalData[i].fLevel = atof(m_cFreqList.GetItemText(i, 1));
	}

	MakeFilterTbl3(m_pFreqData, NFFT_SIZE, NFREQ_RATE, micCalData, nItem, 10);

	SetFilter(micCalData, nItem);
}

void CMicCal2::GetMicCalData(DbMicCalRec *pDbMicCalRec)
{
	pDbMicCalRec->sMicInfo = m_sMicInfo;
	pDbMicCalRec->sMicAmpInfo = m_sMicAmpInfo;
	pDbMicCalRec->sComment = m_sComment;
	pDbMicCalRec->fInputSens = -m_fInputSens;

	CRtaApp *pWinApp = (CRtaApp *)::AfxGetApp();
	int nInputDevice, nInputVolume;
	pWinApp->m_pWndRta->GetInputDevice(&nInputDevice, &nInputVolume);
	pDbMicCalRec->nInputSel = nInputDevice;
	pDbMicCalRec->nInputVol = nInputVolume;

	pDbMicCalRec->nFreqData = GetFreqData((FilterData *)pDbMicCalRec->aFreq);
}

int CMicCal2::GetFreqData(FilterData *pFilterData)
{
	int i;

	const int nFreqData = m_cFreqList.GetItemCount();
	for (i = 0; i < nFreqData; i++) {
		pFilterData->fFreq = atof(m_cFreqList.GetItemText(i, 0));
		pFilterData->fLevel = atof(m_cFreqList.GetItemText(i, 1));
		pFilterData++;
	}

	for ( ; i < MICCAL_NFREQ; i++) {
		pFilterData->fFreq = 0;
		pFilterData->fLevel = 0;
		pFilterData++;
	}

	return nFreqData;
}

void CMicCal2::OnChangeMicinfo()
{
	UpdateData(TRUE);
	m_bUpdate = TRUE;
}

void CMicCal2::OnChangeMicampinfo()
{
	UpdateData(TRUE);
	m_bUpdate = TRUE;
}

void CMicCal2::OnEnChangeComment()
{
	UpdateData(TRUE);
	m_bUpdate = TRUE;
}

void CMicCal2::OnInsertitemFreqList(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	m_bUpdate = TRUE;

	*pResult = 0;
}

void CMicCal2::OnDeleteitemFreqList(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	m_bUpdate = TRUE;

	*pResult = 0;
}

void CMicCal2::WaveInData(double *pData)
{
	int i, j;
	double xt, yt;
	double *pFftBuf = new double[WAVEBUF_SAMPLESPERBUFFER];

	for (i = 0; i < WAVEBUF_SAMPLESPERBUFFER; i++) {
		pFftBuf[i] = *pData++;
		pData++;
	}

	m_oRFFT.fft(WAVEBUF_SAMPLESPERBUFFER, pFftBuf);

	double fPower = 0;
	for (i = 1; i < WAVEBUF_SAMPLESPERBUFFER / 2; i++) {
		j = i * 2;

		xt = pFftBuf[j] * m_pFilterTbl[i];
		yt = pFftBuf[j + 1] * m_pFilterTbl[i];

		fPower += xt * xt + yt * yt;
	}

	delete [] pFftBuf;

	m_fMeanBuf[m_nMeanBufPtr] = fPower * 4;
	if (++m_nMeanBufPtr >= N_MEAN)
		m_nMeanBufPtr = 0;

	fPower = 0;
	for (i = 0; i < N_MEAN; i++)
		fPower += m_fMeanBuf[i];

	fPower /= N_MEAN;

	if (fPower > 0)
		m_cInputLevel.Format("%.1lf", dB10(fPower) + m_fInputSens);
}

void CMicCal2::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (nSBCode) {
	case SB_LEFT:
		m_fInputSens = MIN_SENS;
		break;
	case SB_LINELEFT:
		if (m_fInputSens > MIN_SENS)
			m_fInputSens -= 0.1;
		break;
	case SB_LINERIGHT:
		if (m_fInputSens < MAX_SENS)
			m_fInputSens += 0.1;
		break;
	case SB_PAGELEFT:
		m_fInputSens -= 1;
		if (m_fInputSens < MIN_SENS)
			m_fInputSens = MIN_SENS;
		break;
	case SB_PAGERIGHT:
		m_fInputSens += 1;
		if (m_fInputSens > MAX_SENS)
			m_fInputSens = MAX_SENS;
		break;
	case SB_RIGHT:
		m_fInputSens = MAX_SENS;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_fInputSens = nPos / 10.0;
		break;
	}

	pScrollBar->SetScrollPos((int)(m_fInputSens * 10));
}

void CMicCal2::SetFilter(const FilterData *pFilterData, int nFilterData)
{
	int i;

	MakeFilterTbl3(m_pFilterTbl, WAVEBUF_SAMPLESPERBUFFER, SAMPLING_RATE, pFilterData, nFilterData, 20);
	m_pFilterTbl[0] = 0;
	for (i = 1; i < WAVEBUF_SAMPLESPERBUFFER; i++)
		m_pFilterTbl[i] = 1 / (m_pFilterTbl[i] * WAVEBUF_SAMPLESPERBUFFER);

	const int nFilter = m_cFilter.GetCurSel();
	if (nFilter != 0) {
		double *pFilter = new double[WAVEBUF_SAMPLESPERBUFFER];
		MakeFilterTbl2(pFilter, WAVEBUF_SAMPLESPERBUFFER, SAMPLING_RATE, nFilter, 20);
		for (i = 0; i < WAVEBUF_SAMPLESPERBUFFER; i++)
			m_pFilterTbl[i] *= pFilter[i];
		delete [] pFilter;
	}
}

void CMicCal2::OnSelchangeFilter()
{
	MakeFreqData();
}
