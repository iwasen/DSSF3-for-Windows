// MicCalDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "MicCalDlg.h"
#include "Help\ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CMicCalDlg ダイアログ

CMicCalDlg::CMicCalDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(CMicCalDlg::IDD, pParent)
{
	m_nSortItem = -1;
	m_bSetting = FALSE;
	m_pMicCalL = NULL;
	m_pMicCalR = NULL;
	m_bSignalOut = FALSE;
	m_fAngle = 0;
	m_pWaveIn = CWaveInEx::GetInstance();
	m_pWaveOut = CWaveOutEx::GetInstance();
}

CMicCalDlg::~CMicCalDlg()
{
	m_pWaveIn->ReleaseInstance();
	m_pWaveOut->ReleaseInstance();
}

void CMicCalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SEPARATOR, m_cSeparator);
	DDX_Control(pDX, IDC_SETTING, m_cSetting);
	DDX_Control(pDX, IDC_REGIST, m_cRegist);
	DDX_Control(pDX, IDC_DELETE, m_cDelete);
	DDX_Control(pDX, IDC_CHANGE, m_cChange);
	DDX_Control(pDX, IDC_MICCAL_LIST, m_cMicCalList);
	DDX_Control(pDX, IDC_COMMON_RL, m_cCommonRL);
	DDX_Control(pDX, IDC_SEPARATE_RL, m_cSeparateRL);
	DDX_Control(pDX, IDC_MICCAL_TAB, m_cMicCalTab);
	DDX_Check(pDX, IDC_SIGNAL_OUT, m_bSignalOut);
}

BEGIN_MESSAGE_MAP(CMicCalDlg, CDialogAF)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_MICCAL_LIST, OnItemchangedMiccalList)
	ON_BN_CLICKED(IDC_REGIST, OnRegist)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_SETTING, OnSetting)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_MICCAL_LIST, OnColumnclickMiccalList)
	ON_BN_CLICKED(IDC_READ, OnRead)
	ON_NOTIFY(NM_DBLCLK, IDC_MICCAL_LIST, OnDblclkMiccalList)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_BN_CLICKED(IDC_COMMON_RL, OnBnClickedCommonRl)
	ON_BN_CLICKED(IDC_SEPARATE_RL, OnBnClickedSeparateRl)
	ON_BN_CLICKED(IDC_SIGNAL_OUT, OnSignalOut)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMicCalDlg メッセージ ハンドラ

BOOL CMicCalDlg::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	m_cMicCalList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	InitMicCalList();

	SetMicCalList();

	m_cCommonRL.SetCheck(1);

	LV_FINDINFO lvFindInfo{
		.flags = LVFI_PARAM,
		.lParam = m_MicCalID
	};
	int index;
	if ((index = m_cMicCalList.FindItem(&lvFindInfo)) != -1)
		m_cMicCalList.SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);

	CRect rectDlg;
	GetWindowRect(rectDlg);
	m_WindowSize.cx = rectDlg.Width();
	m_WindowSize.cy = rectDlg.Height();

	CRect rectSeparator;
	m_cSeparator.GetWindowRect(rectSeparator);

	SetWindowPos(NULL, 0, 0, m_WindowSize.cx, rectSeparator.top - rectDlg.top, SWP_NOZORDER | SWP_NOMOVE);

	return TRUE;
}

void CMicCalDlg::OnDestroy()
{
	DeleteTabCtrl();

	m_pWaveOut->Close();
	m_pWaveIn->Close();

	CDialogAF::OnDestroy();
}

void CMicCalDlg::InitMicCalList()
{
	struct LISTCOLUMN {
		UINT textID;
		int width;
	};
	static const LISTCOLUMN tColumn[] = {
		{IDS_MICINFO, 150},
		{IDS_MICAMPINFO, 150},
		{IDS_COMMENT, 150}
	};
	CString str;

	for (int i = 0; i < ARRAY_SIZE(tColumn); i++) {
		str.LoadString(tColumn[i].textID);
		LV_COLUMN lvc{
			.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT,
			.fmt = LVCFMT_LEFT,
			.cx = CDpi::AdjustDpi(tColumn[i].width, 91),
			.pszText = (LPTSTR)(LPCSTR)str
		};
		m_cMicCalList.InsertColumn(i, &lvc);
	}
}

void CMicCalDlg::SetMicCalList()
{
	CDbMicCal dbMicCal;
	if (!dbMicCal.Open())
		return;

	m_cMicCalList.DeleteAllItems();

	int nItem = 0;
	DbMicCalRec dbMicCalRec;
	while (dbMicCal.ReadRecNext(&dbMicCalRec)) {
		if (dbMicCalRec.nMicCalID >= 0) {
			m_cMicCalList.InsertItem(nItem, dbMicCalRec.sMicInfo);
			m_cMicCalList.SetItemText(nItem, 1, dbMicCalRec.sMicAmpInfo);
			m_cMicCalList.SetItemText(nItem, 2, dbMicCalRec.sComment);
			m_cMicCalList.SetItemData(nItem, dbMicCalRec.nMicCalID);
			nItem++;
		}
	}
}

void CMicCalDlg::OnItemchangedMiccalList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;

	if (pNMListView->uNewState & LVIS_SELECTED) {
		CDbMicCal dbMicCal;

		if (!dbMicCal.Open())
			return;

		if (dbMicCal.SearchRecID(MICCAL_RIGHT(pNMListView->lParam))) {
			m_cCommonRL.SetCheck(0);
			m_cSeparateRL.SetCheck(1);
		} else {
			m_cCommonRL.SetCheck(1);
			m_cSeparateRL.SetCheck(0);
		}

		SetTabCtrl();
	}
}

void CMicCalDlg::OnRegist()
{
	if (::AfxMessageBox(IDS_MSG_SAVEMICCAL, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
		return;

	if (m_pMicCalL->m_sMicInfo.IsEmpty()) {
		::AfxMessageBox(IDS_MSG_INPUTMICINFO);
		return;
	}

	int nMicCalID = m_pMicCalL->RegistMicCalData();
	if (m_pMicCalR != NULL && nMicCalID >= 0)
		m_pMicCalR->RegistMicCalData(MICCAL_RIGHT(nMicCalID));

	SetMicCalList();
	SetTabCtrl();
}

void CMicCalDlg::OnChange()
{
	int index;

	if ((index = GetCurSel()) == -1) {
		::AfxMessageBox(IDS_MSG_SELECTMICCAL);
		return;
	}

	if (::AfxMessageBox(IDS_MSG_CHANGEMICCAL, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
		return;

	if (m_pMicCalL->m_sMicInfo.IsEmpty()) {
		::AfxMessageBox(IDS_MSG_INPUTMICINFO);
		return;
	}

	int nMicCalID = (int)m_cMicCalList.GetItemData(index);

	m_pMicCalL->ChangeMicCalData(nMicCalID);
	if (m_pMicCalR != NULL)
		m_pMicCalR->ChangeMicCalData(MICCAL_RIGHT(nMicCalID));
	else {
		CDbMicCal dbMicCal;

		if (dbMicCal.Open())
			dbMicCal.DeleteID(MICCAL_RIGHT(nMicCalID));
	}

	SetMicCalList();
	SetCurSel(nMicCalID);
}

void CMicCalDlg::OnDelete()
{
	int index;

	if ((index = GetCurSel()) == -1) {
		::AfxMessageBox(IDS_MSG_SELECTMICCAL);
		return;
	}

	if (::AfxMessageBox(IDS_MSG_DELTEMICCAL, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
		return;

	CDbMicCal dbMicCal;
	if (!dbMicCal.Open())
		return;

	int nMicCalID = (int)m_cMicCalList.GetItemData(index);

	dbMicCal.DeleteID(nMicCalID);
	dbMicCal.DeleteID(MICCAL_RIGHT(nMicCalID));

	SetMicCalList();
}

void CMicCalDlg::OnSetting()
{
	m_cSetting.ShowWindow(SW_HIDE);
	m_cRegist.ShowWindow(SW_SHOW);
	m_cChange.ShowWindow(SW_SHOW);
	m_cDelete.ShowWindow(SW_SHOW);

	SetWindowPos(NULL, 0, 0, m_WindowSize.cx, m_WindowSize.cy, SWP_NOZORDER | SWP_NOMOVE);

	if (m_pWaveIn->Open(g_nWaveInDevice, this, 2, SAMPLING_RATE, WAVEBUF_SAMPLESPERBUFFER, WAVEBUF_NUM, TRUE))
		m_pWaveIn->Start();

	m_bSetting = TRUE;

	SetTabCtrl();
}

int CMicCalDlg::GetCurSel()
{
	int itemCount = m_cMicCalList.GetItemCount();

	for (int i = 0; i < itemCount; i++) {
		if (m_cMicCalList.GetItemState(i, LVIS_SELECTED) != 0)
			return i;
	}
	return -1;
}

void CMicCalDlg::SetCurSel(DWORD lParam)
{
	int index;

	LV_FINDINFO findInfo{
		.flags = LVFI_PARAM,
		.lParam = (LPARAM)lParam
	};
	if ((index = m_cMicCalList.FindItem(&findInfo)) != -1)
		m_cMicCalList.SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
}

void CMicCalDlg::OnColumnclickMiccalList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (m_nSortItem != pNMListView->iSubItem)
		m_nSortDir = 1;
	else
		m_nSortDir *= -1;

	m_nSortItem = pNMListView->iSubItem;
	m_cMicCalList.SortItems(CompareItems, (LPARAM)this);

	*pResult = 0;
}

int CALLBACK CMicCalDlg::CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CMicCalDlg *pDlg = (CMicCalDlg *)lParamSort;
	CListCtrl &dataList = pDlg->m_cMicCalList;

	LV_FINDINFO findInfo1{
		.flags = LVFI_PARAM,
		.lParam = lParam1
	};
	int index1 = dataList.FindItem(&findInfo1);

	LV_FINDINFO findInfo2{
		.flags = LVFI_PARAM,
		.lParam = lParam2
	};
	int index2 = dataList.FindItem(&findInfo2);

	return dataList.GetItemText(index1, pDlg->m_nSortItem).Compare(dataList.GetItemText(index2, pDlg->m_nSortItem)) * pDlg->m_nSortDir;
}

void CMicCalDlg::OnOK()
{
}

void CMicCalDlg::OnRead()
{
	int index;

	if ((index = GetCurSel()) == -1) {
		::AfxMessageBox(IDS_MSG_SELECTMICCAL);
		return;
	}

	SaveCheck();

	m_MicCalID = (long)m_cMicCalList.GetItemData(index);

	EndDialog(IDOK);
}

void CMicCalDlg::OnDblclkMiccalList(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	OnRead();
	*pResult = 0;
}

void CMicCalDlg::OnReset()
{
	m_MicCalID = -1;

	EndDialog(IDOK);
}

void CMicCalDlg::OnSignalOut()
{
	UpdateData(TRUE);

	if (m_bSignalOut) {
		if (m_pWaveOut->Open(g_nWaveOutDevice, this, 2, SAMPLING_RATE, WAVEBUF_SAMPLESPERBUFFER, WAVEBUF_NUM, FALSE))
			m_pWaveOut->Start();
		else {
			::AfxMessageBox(IDS_MSG_SOUNDINOUT);
			m_bSignalOut = FALSE;
			UpdateData(FALSE);
		}
	} else
		m_pWaveOut->Close();
}

int CMicCalDlg::OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify)
{
	switch (nCode) {
	case WAVEOUT_DATA:
		WaveOutData(pWaveNotify);
		return WAVEBUF_SAMPLESPERBUFFER;
	case WAVEIN_DATA:
		if (m_pMicCalL != NULL)
			m_pMicCalL->WaveInData(pWaveNotify->pSamplesData);
		if (m_pMicCalR != NULL)
			m_pMicCalR->WaveInData(pWaveNotify->pSamplesData + 1);
		return TRUE;
	}

	return 0;
}

void CMicCalDlg::WaveOutData(LPWAVENOTIFY pWaveNotify)
{
	double fAngleStep = 1000.0 / SAMPLING_RATE;
	double *pWaveBuf = pWaveNotify->pSamplesData;
	double s;

	for (int i = 0; i < WAVEBUF_SAMPLESPERBUFFER; i++) {
		s = sin(2 * M_PI * m_fAngle);
		*pWaveBuf++ = s;
		*pWaveBuf++ = s;
		m_fAngle += fAngleStep;
		while (m_fAngle >= 1.0)
			m_fAngle -= 1.0;
	}
}

void CMicCalDlg::SaveCheck()
{
	int index;

	if ((index = GetCurSel()) == -1)
		return;

	int nMicCalID = (int)m_cMicCalList.GetItemData(index);

	if ((m_pMicCalL != NULL && m_pMicCalL->m_bUpdate) || (m_pMicCalR != NULL && m_pMicCalR->m_bUpdate)) {
		if (::AfxMessageBox(IDS_MSG_CHANGEDATA, MB_OKCANCEL | MB_ICONQUESTION) == IDOK) {
			if (m_pMicCalL != NULL)
				m_pMicCalL->ChangeMicCalData(nMicCalID);
			if (m_pMicCalR != NULL)
				m_pMicCalR->ChangeMicCalData(MICCAL_RIGHT(nMicCalID));
		}
	}
}

void CMicCalDlg::SetTabCtrl()
{
	if (m_bSetting) {
		int index;
		int nMicCalID;

		if ((index = GetCurSel()) != -1)
			nMicCalID = (int)m_cMicCalList.GetItemData(index);
		else
			nMicCalID = -1;

		DeleteTabCtrl();

		if (m_cCommonRL) {
			m_pMicCalL = new CMicCal2;

			m_cMicCalTab.AddPage(m_pMicCalL, IDD_MICCAL2, IDS_RIGHT_AND_LEFT, TRUE);

			if (nMicCalID != -1)
				m_pMicCalL->ReadMicCalData(nMicCalID);
		} else {
			m_pMicCalL = new CMicCal2;
			m_pMicCalR = new CMicCal2;

			m_cMicCalTab.AddPage(m_pMicCalL, IDD_MICCAL2, IDS_LEFTCHANNEL, TRUE);
			m_cMicCalTab.AddPage(m_pMicCalR, IDD_MICCAL2, IDS_RIGHTCHANNEL, TRUE);

			if (nMicCalID != -1) {
				m_pMicCalL->ReadMicCalData(nMicCalID);
				m_pMicCalR->ReadMicCalData(MICCAL_RIGHT(nMicCalID));
			}
		}
		m_cMicCalTab.SetPage(0);
	}
}

void CMicCalDlg::DeleteTabCtrl()
{
	m_cMicCalTab.DeleteAllPages();

	if (m_pMicCalL != NULL) {
		delete m_pMicCalL;
		m_pMicCalL = NULL;
	}

	if (m_pMicCalR != NULL) {
		delete m_pMicCalR;
		m_pMicCalR = NULL;
	}
}

void CMicCalDlg::OnBnClickedCommonRl()
{
	SetTabCtrl();
}

void CMicCalDlg::OnBnClickedSeparateRl()
{
	SetTabCtrl();
}

BOOL CMicCalDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_MICCAL_LIST, IDH_MICCAL_LIST,
		IDC_READ, IDH_MICCAL_READ,
		IDC_RESET, IDH_MICCAL_RESET,
		IDCANCEL, IDH_MICCAL_CANCEL,
		IDC_REGIST, IDH_MICCAL_REGIST,
		IDC_CHANGE, IDH_MICCAL_CHANGE,
		IDC_SETTING, IDH_MICCAL_SETTING,
		IDC_DELETE, IDH_MICCAL_DELETE,
		IDC_COMMON_RL, IDH_MICCAL_COMMON_RL,
		IDC_SEPARATE_RL, IDH_MICCAL_SEPARATE_RL,
		IDC_SIGNAL_OUT, IDH_MICCAL_SIGNAL_OUT,
		IDC_MICINFO, IDH_MICCAL_MICINFO,
		IDC_MICAMPINFO, IDH_MICCAL_MICAMPINFO,
		IDC_COMMENT, IDH_MICCAL_COMMENT,
		IDC_INPUT_LEVEL, IDH_MICCAL_INPUT_LEVEL,
		IDC_FILTER, IDH_MICCAL_FILTER,
		IDC_LEVEL_BAR, IDH_MICCAL_LEVEL_BAR,
		IDC_FREQ_RESPONSE, IDH_MICCAL_FREQ_RESPONSE,
		IDC_FREQ_LIST, IDH_MICCAL_FREQ_LIST,
		IDC_FREQ, IDH_MICCAL_FREQ,
		IDC_LEVEL, IDH_MICCAL_LEVEL,
		IDC_FREQ_REGIST, IDH_MICCAL_FREQ_REGIST,
		IDC_FREQ_DELETE, IDH_MICCAL_FREQ_DELETE,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
