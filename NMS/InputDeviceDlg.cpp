// InputDeviceDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Nms.h"
#include "InputDeviceDlg.h"
#include "mixer.h"
#include "WaveIn.h"
#include "WaveOut.h"
#include "PeakLevelWnd.h"
#include "SetData.h"
#include "Help\ContextHelp.h"

#define NFREQ_RATE	44100
#define NFREQ_MIN	20
#define NFREQ_MAX	(NFREQ_RATE / 2)
#define NFFT_SIZE	4096

#define WAVEBUF_SIZE	16384
#define WAVEBUF_NUM		4
#define SAMPLING_RATE	44100

#define MIN_LEVEL_BAR	600
#define MAX_LEVEL_BAR	1400

#define MIN_SENS		60
#define MAX_SENS		140

static CWaveIn *g_pWaveIn;
static CWaveOut *g_pWaveOut;

/////////////////////////////////////////////////////////////////////////////
// CInputDeviceDlg ダイアログ

CInputDeviceDlg::CInputDeviceDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(CInputDeviceDlg::IDD, pParent)
{
	m_fFreq = 0.0;
	m_fLevel = 0.0;
	m_bSignalOut = FALSE;

	m_nSortItem = -1;
	m_pFreqData = NULL;
	m_fAngle = 0;
	m_pFilterTbl = NULL;
	memset(m_fMeanBuf, 0, sizeof(m_fMeanBuf));
	m_nMeanBufPtr = 0;
	m_fInputSens = 100;
	m_nSelectInput = 0;

	g_pWaveIn = new CWaveIn;
	g_pWaveOut = new CWaveOut;
}

CInputDeviceDlg::~CInputDeviceDlg()
{
	if (m_pFilterTbl != NULL)
		delete [] m_pFilterTbl;

	if (g_pPeakLevelWnd != NULL)
		g_pPeakLevelWnd->m_cPeakLevel.Clear();

	delete g_pWaveIn;
	delete g_pWaveOut;
}

void CInputDeviceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHANGE, m_cChange);
	DDX_Control(pDX, IDC_SETTING, m_cSetting);
	DDX_Control(pDX, IDC_SEPARATOR, m_cSeparator);
	DDX_Control(pDX, IDC_REGIST, m_cRegist);
	DDX_Control(pDX, IDC_MICCAL_LIST, m_cMicCalList);
	DDX_Control(pDX, IDC_LEVEL_BAR, m_cLevelBar);
	DDX_Control(pDX, IDC_INPUT_LEVEL, m_cInputLevel);
	DDX_Control(pDX, IDC_FREQ_RESPONSE, m_cFreqResponse);
	DDX_Control(pDX, IDC_FREQ_LIST, m_cFreqList);
	DDX_Control(pDX, IDC_FILTER, m_cFilter);
	DDX_Control(pDX, IDC_DELETE, m_cDelete);
	DDX_Control(pDX, IDC_INPUT_VOLUME, m_cInputVolume);
	DDX_Control(pDX, IDC_INPUT_SELECTOR, m_cInputSelector);
	DDX_Control(pDX, IDC_INPUT_DEVICE, m_cInputDevice);
	DDX_Text(pDX, IDC_COMMENT, m_sComment);
	DDX_Text(pDX, IDC_FREQ, m_fFreq);
	DDX_Text(pDX, IDC_LEVEL, m_fLevel);
	DDX_Text(pDX, IDC_MICAMPINFO, m_sMicAmpInfo);
	DDX_Text(pDX, IDC_MICINFO, m_sMicInfo);
	DDX_Check(pDX, IDC_SIGNAL_OUT, m_bSignalOut);
}

BEGIN_MESSAGE_MAP(CInputDeviceDlg, CDialogAF)
	ON_CBN_SELCHANGE(IDC_INPUT_DEVICE, OnSelchangeInputDevice)
	ON_CBN_SELCHANGE(IDC_INPUT_SELECTOR, OnSelchangeInputSelector)
	ON_WM_HSCROLL()
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_MICCAL_LIST, OnItemchangedMiccalList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FREQ_LIST, OnItemchangedFreqList)
	ON_BN_CLICKED(IDC_REGIST, OnRegist)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_FREQ_REGIST, OnFreqRegist)
	ON_BN_CLICKED(IDC_FREQ_DELETE, OnFreqDelete)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_MICCAL_LIST, OnColumnclickMiccalList)
	ON_BN_CLICKED(IDC_READ, OnRead)
	ON_NOTIFY(NM_DBLCLK, IDC_MICCAL_LIST, OnDblclkMiccalList)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_EN_CHANGE(IDC_MICINFO, OnChangeMicinfo)
	ON_EN_CHANGE(IDC_MICAMPINFO, OnChangeMicampinfo)
	ON_NOTIFY(LVN_INSERTITEM, IDC_FREQ_LIST, OnInsertitemFreqList)
	ON_NOTIFY(LVN_DELETEITEM, IDC_FREQ_LIST, OnDeleteitemFreqList)
	ON_BN_CLICKED(IDC_SETTING, OnSetting)
	ON_BN_CLICKED(IDC_SIGNAL_OUT, OnSignalOut)
	ON_CBN_SELCHANGE(IDC_FILTER, OnSelchangeFilter)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputDeviceDlg メッセージ ハンドラ

BOOL CInputDeviceDlg::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	m_cMicCalList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_cFreqList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	InitInputDevice();

	m_cInputVolume.SetRange(0, 100);
	m_cInputVolume.SetTicFreq(10);
	m_cInputVolume.SetPageSize(10);

	SetInputSelect(g_oSetData.InputDevice.nInputDevice);

	InitMicCalList();
	InitFreqList();

	SetMicCalList();

	LV_FINDINFO lvFindInfo;
	lvFindInfo.flags = LVFI_PARAM;
	lvFindInfo.lParam = m_MicCalID;
	int index;
	if ((index = m_cMicCalList.FindItem(&lvFindInfo)) != -1)
		m_cMicCalList.SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);

	m_cLevelBar.SetScrollRange(MIN_LEVEL_BAR, MAX_LEVEL_BAR, FALSE);
	m_cLevelBar.SetScrollPos((int)(m_fInputSens * 10), FALSE);

	CString str;
	LOGFONT font;
	memset(&font, 0, sizeof(font));
#ifdef LANG_JPN
	font.lfHeight = 160;
#else
	font.lfHeight = 140;
#endif
	font.lfCharSet = DEFAULT_CHARSET;
	str.LoadString(IDS_DEFAULTFONT);
	strcpy_s(font.lfFaceName, str);
	m_Font.CreatePointFontIndirect(&font);
	m_cInputLevel.SetFont(&m_Font);

	SetFilterNameList(m_cFilter);
	m_cFilter.SetCurSel(FILTER_A);

	m_bUpdate = FALSE;

	CRect rectDlg;
	GetWindowRect(rectDlg);
	m_WindowSize.cx = rectDlg.Width();
	m_WindowSize.cy = rectDlg.Height();

	CRect rectSeparator;
	m_cSeparator.GetWindowRect(rectSeparator);

	SetWindowPos(NULL, 0, 0, m_WindowSize.cx, rectSeparator.top - rectDlg.top, SWP_NOZORDER | SWP_NOMOVE);

	return TRUE;
}

void CInputDeviceDlg::InitInputDevice()
{
	int nInputDevice;
	WAVEINCAPS waveInCaps;
	int i;

	nInputDevice = waveInGetNumDevs();
	for (i = 0; i < nInputDevice; i++) {
		waveInGetDevCaps(i, &waveInCaps, sizeof(waveInCaps));
		m_cInputDevice.AddString(waveInCaps.szPname);
	}
	if (g_oSetData.InputDevice.nInputDevice >= nInputDevice)
		g_oSetData.InputDevice.nInputDevice = 0;
	m_cInputDevice.SetCurSel(g_oSetData.InputDevice.nInputDevice);
}

void CInputDeviceDlg::SetInputSelect(int nInputDevice)
{
	MIXERLINE mxl;
	MIXERCONTROL mxc;
	MIXERCONTROLDETAILS_BOOLEAN mxcdb[MAX_CONTROL_ID];
	MIXERCONTROLDETAILS_LISTTEXT mxcdl[MAX_CONTROL_ID];
	DWORD i, dst, src;

	m_cInputSelector.ResetContent();

	MixerInitialize(nInputDevice, 0);

	// 録音マスターの LineID を取得
	memset(&mxl, 0, sizeof(mxl));
	mxl.cbStruct = sizeof(mxl);
	mxl.dwComponentType	= MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
	if (mixerGetLineInfo((HMIXEROBJ)dwMixerInDevice, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
		return;

	// 録音入力セレクタ
	if (MixerGetControlsByType(dwMixerInDevice, mxl.dwLineID, MIXERCONTROL_CONTROLTYPE_MUX, mxc) == MMSYSERR_NOERROR ||
			MixerGetControlsByType(dwMixerInDevice, mxl.dwLineID, MIXERCONTROL_CONTROLTYPE_MIXER, mxc) == MMSYSERR_NOERROR) {
		m_cSrcItems = 0;
		m_dwDstControlID = mxc.dwControlID;
		m_cMultipleItems = mxc.cMultipleItems;
		if (m_cMultipleItems > MAX_CONTROL_ID)
			m_cMultipleItems = MAX_CONTROL_ID;

		// セレクタリスト取得
		if (MixerGetControlList(dwMixerInDevice, m_dwDstControlID, m_cMultipleItems, mxcdl, sizeof(MIXERCONTROLDETAILS_LISTTEXT)) != MMSYSERR_NOERROR)
			return;

		// セレクト状態取得
		if (MixerGetControlValue(dwMixerInDevice, m_dwDstControlID, m_cMultipleItems, mxcdb, sizeof(MIXERCONTROLDETAILS_BOOLEAN)) != MMSYSERR_NOERROR)
			return;

		// 入力機器のボリュームの ControlID 取得
		for (i = 0; i < m_cMultipleItems; i++) {
			m_dwSrcRecVolID[i] = MixerGetControlID(dwMixerInDevice, mxcdl[i].dwParam1, MIXERCONTROL_CONTROLTYPE_VOLUME);
			m_dwSrcRecMuteID[i] = (DWORD)-1;

			m_cInputSelector.AddString(mxcdl[i].szName);

			if (mxcdb[i].fValue != 0)
				m_nSelectInput = i;
		}
		m_nInputDevice = m_cMultipleItems;
	} else {
		m_cMultipleItems = 0;
		m_cSrcItems = mxl.cConnections;
		if (m_cSrcItems > MAX_CONTROL_ID)
			m_cSrcItems = MAX_CONTROL_ID;
		dst = mxl.dwDestination;
		m_nSelectInput = 0;
		for (src = 0; src < m_cSrcItems; src++) {
			// 入力機器の LineID 取得
			memset(&mxl, 0, sizeof(mxl));
			mxl.cbStruct = sizeof(mxl);
			mxl.dwDestination = dst;
			mxl.dwSource = src;
			if (mixerGetLineInfo((HMIXEROBJ)dwMixerInDevice, &mxl, MIXER_GETLINEINFOF_SOURCE) != MMSYSERR_NOERROR)
				return;

			// 入力機器のボリュームの ControlID 取得
			m_dwSrcRecVolID[src] = MixerGetControlID(dwMixerInDevice, mxl.dwLineID, MIXERCONTROL_CONTROLTYPE_VOLUME);
			m_dwSrcRecMuteID[src] = MixerGetControlID(dwMixerInDevice, mxl.dwLineID, MIXERCONTROL_CONTROLTYPE_MUTE);

			// 入力機器のミュート状態取得
			if (m_dwSrcRecMuteID[src] != -1) {
				if (MixerGetBoolControl(dwMixerInDevice, m_dwSrcRecMuteID[src]) == 0)
					m_nSelectInput = src;
			}

			m_cInputSelector.AddString(mxl.szName);
		}
		m_nInputDevice = m_cSrcItems;
	}

	m_cInputSelector.SetCurSel(m_nSelectInput);
	m_cInputVolume.SetPos(GetInputVolume(m_dwSrcRecVolID[m_nSelectInput]) * 100 / 65535);
	OnSelchangeInputSelector();
}

DWORD CInputDeviceDlg::GetInputVolume(DWORD dwControlID)
{
	return MixerGetUnsignedControl(dwMixerInDevice, dwControlID);
}

void CInputDeviceDlg::SetInputVolume(DWORD dwControlID, DWORD dwLevel)
{
	MixerSetUnsignedControl(dwMixerInDevice, dwControlID, dwLevel);
}

void CInputDeviceDlg::GetInputDevice(int *pInputDevice, int *pInputVolume)
{
	*pInputDevice = m_nSelectInput;
	*pInputVolume = GetInputVolume(m_dwSrcRecVolID[m_nSelectInput]);
}

void CInputDeviceDlg::SetInputDevice(int nInputDevice, int nInputVolume)
{
	if (nInputDevice >= m_nInputDevice)
		return;

	m_cInputSelector.SetCurSel(nInputDevice);
	OnSelchangeInputSelector();

	SetInputVolume(m_dwSrcRecVolID[m_nSelectInput], nInputVolume);
	m_cInputVolume.SetPos(GetInputVolume(m_dwSrcRecVolID[m_nSelectInput]) * 100 / 65535);
}

void CInputDeviceDlg::OnSelchangeInputDevice()
{
	g_oSetData.InputDevice.nInputDevice = m_cInputDevice.GetCurSel();
	SetInputSelect(g_oSetData.InputDevice.nInputDevice);
}

void CInputDeviceDlg::OnSelchangeInputSelector()
{
	int i;
	MIXERCONTROLDETAILS_BOOLEAN mxcdb[MAX_CONTROL_ID];

	m_nSelectInput = m_cInputSelector.GetCurSel();

	for (i = 0; i < (int)m_cSrcItems; i++) {
		if (m_dwSrcRecMuteID[i] != -1)
			MixerSetBoolControl(dwMixerInDevice, m_dwSrcRecMuteID[i], (i == m_nSelectInput) ? 0 : 1);
	}

	if (m_cMultipleItems != 0) {
		for (i = 0; i < (int)m_cMultipleItems; i++)
			mxcdb[i].fValue = (i == m_nSelectInput) ? 1 : 0;

		MixerSetControlValue(dwMixerInDevice, m_dwDstControlID, m_cMultipleItems, mxcdb, sizeof(MIXERCONTROLDETAILS_BOOLEAN));
	}

	m_cInputVolume.SetPos(GetInputVolume(m_dwSrcRecVolID[m_nSelectInput]) * 100 / 65535);
}

void CInputDeviceDlg::OnOK()
{
}

void CInputDeviceDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (pScrollBar->GetDlgCtrlID()) {
	case IDC_INPUT_VOLUME:
		SetInputVolume(m_dwSrcRecVolID[m_nSelectInput], ((CSliderCtrl *)pScrollBar)->GetPos() * 65535 / 100);
		break;
	case IDC_LEVEL_BAR:
		ScrollLevelBar(nSBCode, nPos, pScrollBar);
		break;
	}
}

void CInputDeviceDlg::ScrollLevelBar(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

void CInputDeviceDlg::OnDestroy()
{
	CDialogAF::OnDestroy();

	g_pWaveOut->Close();
	g_pWaveIn->Close();

	if (m_pFreqData != NULL) {
		delete [] m_pFreqData;
		m_pFreqData = NULL;
	}
}

void CInputDeviceDlg::InitMicCalList()
{
	struct LISTCOLUMN {
		UINT textID;
		int width;
	};
	static const LISTCOLUMN tColumn[] = {
		{IDS_MICINFO, 147},
		{IDS_MICAMPINFO, 147},
		{IDS_COMMENT, 147}
	};
	int i;
	CString str;
	LV_COLUMN lvc;

	for (i = 0; i < sizeof(tColumn) / sizeof(LISTCOLUMN); i++) {
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		lvc.fmt = LVCFMT_LEFT;
		str.LoadString(tColumn[i].textID);
		lvc.pszText = (LPTSTR)(LPCSTR)str;
		lvc.cx = CDpi::AdjustDpi(tColumn[i].width, 90);
		m_cMicCalList.InsertColumn(i, &lvc);
	}
}

void CInputDeviceDlg::InitFreqList()
{
	struct LISTCOLUMN {
		UINT textID;
		int width;
	};
	static const LISTCOLUMN tColumn[] = {
		{IDS_FREQUENCY, 55},
		{IDS_LEVEL, 55}
	};
	int i;
	CString str;
	LV_COLUMN lvc;

	for (i = 0; i < sizeof(tColumn) / sizeof(LISTCOLUMN); i++) {
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		lvc.fmt = LVCFMT_LEFT;
		str.LoadString(tColumn[i].textID);
		lvc.pszText = (LPTSTR)(LPCSTR)str;
		lvc.cx = CDpi::AdjustDpi(tColumn[i].width, 90);
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

void CInputDeviceDlg::SetMicCalList()
{
	CDbMicCal dbMicCal;
	DbMicCalRec dbMicCalRec;
	int nItem = 0;

	if (!dbMicCal.Open())
		return;

	m_cMicCalList.DeleteAllItems();

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

void CInputDeviceDlg::OnItemchangedMiccalList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;

	if (pNMListView->uNewState & LVIS_SELECTED) {
		CDbMicCal dbMicCal;
		DbMicCalRec dbMicCalRec;
		CString str;
		int i;

		if (!dbMicCal.Open())
			return;

		if (!dbMicCal.ReadRecID((long)pNMListView->lParam, &dbMicCalRec))
			return;

		m_sMicInfo = dbMicCalRec.sMicInfo;
		m_sMicAmpInfo = dbMicCalRec.sMicAmpInfo;
		m_sComment = dbMicCalRec.sComment;
		m_fInputSens = -dbMicCalRec.fInputSens;
		UpdateData(FALSE);

		m_cLevelBar.SetScrollPos((int)(m_fInputSens * 10), TRUE);

		m_cFreqList.DeleteAllItems();
		for (i = 0; i < dbMicCalRec.nFreqData; i++) {
			str.Format("%g", dbMicCalRec.aFreq[i].freq);
			m_cFreqList.InsertItem(i, str);
			str.Format("%g", dbMicCalRec.aFreq[i].level);
			m_cFreqList.SetItemText(i, 1, str);
			m_cFreqList.SetItemData(i, (DWORD)dbMicCalRec.aFreq[i].freq);
		}
		m_cFreqList.SortItems(CompareFreq, (LPARAM)this);

		if (m_pFreqData != NULL)
			DispFreqWindow();

		SetInputDevice(dbMicCalRec.nInputSel, dbMicCalRec.nInputVol);

		m_bUpdate = FALSE;
	}
}

void CInputDeviceDlg::OnItemchangedFreqList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (pNMListView->uNewState & LVIS_SELECTED) {
		m_fFreq = atof(m_cFreqList.GetItemText(pNMListView->iItem, 0));
		m_fLevel = atof(m_cFreqList.GetItemText(pNMListView->iItem, 1));
		UpdateData(FALSE);
	}
	*pResult = 0;
}

void CInputDeviceDlg::OnRegist()
{
	CDbMicCal dbMicCal;
	DbMicCalRec dbMicCalRec;

	if (::AfxMessageBox(IDS_MSG_SAVEMICCAL, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
		return;

	UpdateData(TRUE);

	if (m_sMicInfo.IsEmpty()) {
		::AfxMessageBox(IDS_MSG_INPUTMICINFO);
		return;
	}

	if (!dbMicCal.Open())
		return;

	if (!dbMicCal.GetNewID(&dbMicCalRec.nMicCalID))
		return;

	GetMicCalData(&dbMicCalRec);

	dbMicCal.StoreRec(&dbMicCalRec);
	m_bUpdate = FALSE;

	SetMicCalList();
	SetCurSel(dbMicCalRec.nMicCalID);
}

void CInputDeviceDlg::OnChange()
{
	CDbMicCal dbMicCal;
	DbMicCalRec dbMicCalRec;
	int index;

	if ((index = GetCurSel()) == -1) {
		::AfxMessageBox(IDS_MSG_SELECTMICCAL);
		return;
	}

	if (::AfxMessageBox(IDS_MSG_CHANGEMICCAL, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
		return;

	UpdateData(TRUE);

	if (m_sMicInfo.IsEmpty()) {
		::AfxMessageBox(IDS_MSG_INPUTMICINFO);
		return;
	}

	if (!dbMicCal.Open())
		return;

	dbMicCalRec.nMicCalID = (long)m_cMicCalList.GetItemData(index);
	GetMicCalData(&dbMicCalRec);

	dbMicCal.UpdateRec(&dbMicCalRec);
	m_bUpdate = FALSE;

	SetMicCalList();
	SetCurSel(dbMicCalRec.nMicCalID);
}

void CInputDeviceDlg::OnDelete()
{
	CDbMicCal dbMicCal;
	int index;

	if ((index = GetCurSel()) == -1) {
		::AfxMessageBox(IDS_MSG_SELECTMICCAL);
		return;
	}

	if (::AfxMessageBox(IDS_MSG_DELTEMICCAL, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
		return;

	if (!dbMicCal.Open())
		return;

	dbMicCal.DeleteID((long)m_cMicCalList.GetItemData(index));
	m_bUpdate = FALSE;

	SetMicCalList();

	m_cFreqList.DeleteAllItems();
}

void CInputDeviceDlg::OnFreqRegist()
{
	LV_FINDINFO findInfo;
	CString str;
	int index;

	UpdateData(TRUE);

	if (m_fFreq <= 0) {
		::AfxMessageBox(IDS_MSG_UNDER0FREQ);
		return;
	}

	str.Format("%g", m_fFreq);

	findInfo.flags = LVFI_STRING;
	findInfo.psz = str;
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
}

void CInputDeviceDlg::OnFreqDelete()
{
	LV_FINDINFO findInfo;
	CString str;
	int index;

	UpdateData(TRUE);

	str.Format("%g", m_fFreq);

	findInfo.flags = LVFI_STRING;
	findInfo.psz = str;
	if ((index = m_cFreqList.FindItem(&findInfo)) != -1) {
		m_cFreqList.DeleteItem(index);
	}

	DispFreqWindow();
}

int CInputDeviceDlg::GetCurSel()
{
	int itemCount = m_cMicCalList.GetItemCount();
	int i;

	for (i = 0; i < itemCount; i++) {
		if (m_cMicCalList.GetItemState(i, LVIS_SELECTED) != 0)
			return i;
	}
	return -1;
}

void CInputDeviceDlg::SetCurSel(DWORD lParam)
{
	LV_FINDINFO findInfo;
	int index;

	findInfo.flags = LVFI_PARAM;
	findInfo.lParam = lParam;
	if ((index = m_cMicCalList.FindItem(&findInfo)) != -1)
		m_cMicCalList.SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
}

void CInputDeviceDlg::DispFreqWindow()
{
	MakeFreqData();
	m_cFreqResponse.DispGraph(m_pFreqData, NFFT_SIZE, NFREQ_RATE, NFREQ_MIN, NFREQ_MAX, -20, 20, NULL, FALSE);
}

void CInputDeviceDlg::OnColumnclickMiccalList(NMHDR* pNMHDR, LRESULT* pResult)
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

int CALLBACK CInputDeviceDlg::CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CInputDeviceDlg *pDlg = (CInputDeviceDlg *)lParamSort;
	CListCtrl &dataList = pDlg->m_cMicCalList;
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

int CALLBACK CInputDeviceDlg::CompareFreq(LPARAM lParam1, LPARAM lParam2, LPARAM /*lParamSort*/)
{
	if (lParam1 > lParam2)
		return 1;
	else if (lParam1 < lParam2)
		return -1;
	else
		return 0;
}

void CInputDeviceDlg::MakeFreqData()
{
	FilterData micCalData[MICCAL_NFREQ];
	int i;

	int nItem = m_cFreqList.GetItemCount();
	for (i = 0; i < nItem; i++) {
		micCalData[i].fFreq = atof(m_cFreqList.GetItemText(i, 0));
		micCalData[i].fLevel = atof(m_cFreqList.GetItemText(i, 1));
	}

	MakeFilterTbl3(m_pFreqData, NFFT_SIZE, NFREQ_RATE, micCalData, nItem, 10);

	SetFilter(micCalData, nItem);
}

void CInputDeviceDlg::OnRead()
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

void CInputDeviceDlg::OnDblclkMiccalList(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	OnRead();

	*pResult = 0;
}

void CInputDeviceDlg::GetMicCalData(DbMicCalRec *pDbMicCalRec)
{
	pDbMicCalRec->sMicInfo = m_sMicInfo;
	pDbMicCalRec->sMicAmpInfo = m_sMicAmpInfo;
	pDbMicCalRec->sComment = m_sComment;
	pDbMicCalRec->fInputSens = -m_fInputSens;
	pDbMicCalRec->nInputSel = m_nSelectInput;
	pDbMicCalRec->nInputVol = GetInputVolume(m_dwSrcRecVolID[m_nSelectInput]);

	pDbMicCalRec->nFreqData = GetFreqData((FilterData *)pDbMicCalRec->aFreq);
}

int CInputDeviceDlg::GetFreqData(FilterData *pFilterData)
{
	int nFreqData;
	int i;

	nFreqData = m_cFreqList.GetItemCount();
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

void CInputDeviceDlg::OnReset()
{
	m_MicCalID = -1;

	EndDialog(IDOK);
}

void CInputDeviceDlg::SaveCheck()
{
	int index;

	if ((index = GetCurSel()) == -1)
		return;

	if (m_bUpdate) {
		if (::AfxMessageBox(IDS_MSG_CHANGEDATA, MB_OKCANCEL | MB_ICONQUESTION) == IDOK) {
			CDbMicCal dbMicCal;
			DbMicCalRec dbMicCalRec;

			UpdateData(TRUE);
			dbMicCalRec.nMicCalID = (long)m_cMicCalList.GetItemData(index);
			GetMicCalData(&dbMicCalRec);

			if (!dbMicCal.Open())
				return;

			dbMicCal.UpdateRec(&dbMicCalRec);
			m_bUpdate = FALSE;
		}
	}
}

void CInputDeviceDlg::OnChangeMicinfo()
{
	m_bUpdate = TRUE;
}

void CInputDeviceDlg::OnChangeMicampinfo()
{
	m_bUpdate = TRUE;
}

void CInputDeviceDlg::OnInsertitemFreqList(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	m_bUpdate = TRUE;

	*pResult = 0;
}

void CInputDeviceDlg::OnDeleteitemFreqList(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	m_bUpdate = TRUE;

	*pResult = 0;
}

void CInputDeviceDlg::OnSetting()
{
	m_cSetting.ShowWindow(SW_HIDE);
	m_cRegist.ShowWindow(SW_SHOW);
	m_cChange.ShowWindow(SW_SHOW);
	m_cDelete.ShowWindow(SW_SHOW);

	SetWindowPos(NULL, 0, 0, m_WindowSize.cx, m_WindowSize.cy, SWP_NOZORDER | SWP_NOMOVE);

	if (g_pWaveIn->Open(g_oSetData.InputDevice.nInputDevice, this, 1, SAMPLING_RATE, WAVEBUF_SIZE, WAVEBUF_NUM, TRUE)) {
		m_pFreqData = new double[NFFT_SIZE];
		m_pFilterTbl = new double[WAVEBUF_SIZE];

		m_cFreqResponse.Initialize(80, NULL, 0, RGB(255, 0, 0), 0);
		m_cFreqResponse.EnableWindow(TRUE);
		DispFreqWindow();

		g_pWaveIn->Start();
	}
}

void CInputDeviceDlg::OnSignalOut()
{
	UpdateData(TRUE);

	if (m_bSignalOut) {
		if (g_pWaveOut->Open(g_oSetData.InputDevice.nInputDevice, this, 1, SAMPLING_RATE, WAVEBUF_SIZE, WAVEBUF_NUM, FALSE))
			g_pWaveOut->Start();
		else {
			::AfxMessageBox(IDS_MSG_SOUNDINOUT);
			m_bSignalOut = FALSE;
			UpdateData(FALSE);
		}
	} else
		g_pWaveOut->Close();
}

int CInputDeviceDlg::OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify)
{
	switch (nCode) {
	case WAVEOUT_DATA:
		WaveOutData(pWaveNotify);
		return WAVEBUF_SIZE;
	case WAVEIN_DATA:
		WaveInData(pWaveNotify);
		return TRUE;
	}

	return 0;
}

void CInputDeviceDlg::WaveOutData(LPWAVENOTIFY pWaveNotify)
{
	double fAngleStep = 1000.0 * M_PI * 2 / SAMPLING_RATE;
	double *pWaveBuf = pWaveNotify->pSamplesData;
	int i;

	for (i = 0; i < WAVEBUF_SIZE; i++) {
		*pWaveBuf++ = sin(m_fAngle);
		m_fAngle += fAngleStep;
		while (m_fAngle >= M_PI * 2)
			m_fAngle -= M_PI * 2;
	}
}

void CInputDeviceDlg::WaveInData(LPWAVENOTIFY pWaveNotify)
{
	int i, j;
	double xt, yt;
	double fPower;
	double fftBuf[WAVEBUF_SIZE];

	memcpy(fftBuf, pWaveNotify->pSamplesData, WAVEBUF_SIZE * sizeof(double));

	m_oRFFT.fft(WAVEBUF_SIZE, fftBuf);

	fPower = 0;
	for (i = 1; i < WAVEBUF_SIZE / 2; i++) {
		j = i * 2;

		xt = fftBuf[j] * m_pFilterTbl[i];
		yt = fftBuf[j + 1] * m_pFilterTbl[i];

		fPower += xt * xt + yt * yt;
	}

	m_fMeanBuf[m_nMeanBufPtr] = fPower * 4;
	if (++m_nMeanBufPtr >= N_MEAN)
		m_nMeanBufPtr = 0;

	fPower = 0;
	for (i = 0; i < N_MEAN; i++)
		fPower += m_fMeanBuf[i];

	fPower /= N_MEAN;

	m_cInputLevel.Format("%.1lf", dB10(fPower) + m_fInputSens);

	if (g_pPeakLevelWnd != NULL)
		g_pPeakLevelWnd->m_cPeakLevel.WaveData(pWaveNotify);
}

void CInputDeviceDlg::SetFilter(FilterData *pFilterData, int nFilterData)
{
	int i;

	MakeFilterTbl3(m_pFilterTbl, WAVEBUF_SIZE, SAMPLING_RATE, pFilterData, nFilterData, 20);
	m_pFilterTbl[0] = 0;
	for (i = 1; i < WAVEBUF_SIZE; i++)
		m_pFilterTbl[i] = 1 / (m_pFilterTbl[i] * WAVEBUF_SIZE);

	int nFilter = m_cFilter.GetCurSel();
	if (nFilter != 0) {
		double *pFilter = new double[WAVEBUF_SIZE];
		MakeFilterTbl2(pFilter, WAVEBUF_SIZE, SAMPLING_RATE, nFilter, 20);
		for (i = 0; i < WAVEBUF_SIZE; i++)
			m_pFilterTbl[i] *= pFilter[i];
		delete [] pFilter;
	}
}

void CInputDeviceDlg::OnSelchangeFilter()
{
	MakeFreqData();
}

BOOL CInputDeviceDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_READ, IDH_INPUT_DEVICE_READ,
		IDC_RESET, IDH_INPUT_DEVICE_RESET,
		IDCANCEL, IDH_INPUT_DEVICE_CANCEL,
		IDC_REGIST, IDH_INPUT_DEVICE_REGIST,
		IDC_CHANGE, IDH_INPUT_DEVICE_CHANGE,
		IDC_DELETE, IDH_INPUT_DEVICE_DELETE,
		IDC_SETTING, IDH_INPUT_DEVICE_SETTING,
		IDC_INPUT_DEVICE, IDH_INPUT_DEVICE_INPUT_DEVICE,
		IDC_INPUT_SELECTOR, IDH_INPUT_DEVICE_INPUT_SELECTOR,
		IDC_INPUT_VOLUME, IDH_INPUT_DEVICE_INPUT_VOLUME,
		IDC_MICCAL_LIST, IDH_INPUT_DEVICE_MICCAL_LIST,
		IDC_FREQ_RESPONSE, IDH_INPUT_DEVICE_FREQ_RESPONSE,
		IDC_FREQ_LIST, IDH_INPUT_DEVICE_FREQ_LIST,
		IDC_FREQ, IDH_INPUT_DEVICE_FREQ,
		IDC_LEVEL, IDH_INPUT_DEVICE_LEVEL,
		IDC_FREQ_REGIST, IDH_INPUT_DEVICE_FREQ_REGIST,
		IDC_FREQ_DELETE, IDH_INPUT_DEVICE_FREQ_DELETE,
		IDC_MICINFO, IDH_INPUT_DEVICE_MICINFO,
		IDC_MICAMPINFO, IDH_INPUT_DEVICE_MICAMPINFO,
		IDC_COMMENT, IDH_INPUT_DEVICE_COMMENT,
		IDC_INPUT_LEVEL, IDH_INPUT_DEVICE_INPUT_LEVEL,
		IDC_FILTER, IDH_INPUT_DEVICE_FILTER,
		IDC_SIGNAL_OUT, IDH_INPUT_DEVICE_SIGNAL_OUT,
		IDC_LEVEL_BAR, IDH_INPUT_DEVICE_LEVEL_BAR,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
