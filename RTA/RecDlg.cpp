// RecDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "RtaDlg.h"
#include "RecDlg.h"
#include "WaveInEx.h"
#include "WaveOutEx.h"
#include "WaveFile.h"
#include "AutoRec.h"
#include "Help\ContextHelp.h"
#include <shlwapi.h>

#define REC_MODE_NONE	0
#define REC_MODE_IN		1
#define REC_MODE_OUT	2

#define PLAY_MODE_NONE	0
#define PLAY_MODE_OUT	2

#define STATUS_STOP		0
#define STATUS_RECORD	1
#define STATUS_PLAY		2

#define SCROLL_RANGE 16384
#define MIN_DISP_TIME	0.001
#define MAX_ZOOM_TIME		512

#define INIT_TIME	10

// CRecDlg ダイアログ

CRecDlg::CRecDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CRecDlg::IDD, pParent)
{
	m_bMove = FALSE;
	m_pWaveBuf = NULL;
	m_nRecMode = REC_MODE_NONE;
	m_nPlayMode = PLAY_MODE_NONE;
	m_fDataTime = 0;
	m_bDispSelectArea = FALSE;
	m_fSelectStart = -1;
	m_fSelectEnd = -1;
	m_dwRecCount = 0;
	m_nRecChannel = 1;
	m_nPlayChannel = 1;
	m_fMaxData = 0;
	m_iPriority = g_oSetData.Rec.nPriority;
	m_dwAllocSize = 0;
	m_nCurrentStatus = 0;
	m_nSamplingRate = 48000;
	m_nBitsPerSample = 0;

	AllocBuf();

	Create(IDD, g_oSetData.Rec.nPosLeft, g_oSetData.Rec.nPosTop, 0, pParent);
}

CRecDlg::~CRecDlg()
{
	FreeBuf();
}

void CRecDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRAPH_WAVE, m_cRecWnd);
	DDX_Control(pDX, IDC_ZOOM_H, m_cZoomH);
	DDX_Control(pDX, IDC_RECORD, m_cRecord);
	DDX_Control(pDX, IDC_PLAY, m_cPlay);
	DDX_Control(pDX, IDC_SCROLL_H, m_cGraphScroll);
	DDX_Control(pDX, IDC_GRAPH_NORM, m_cGraphNorm);
	DDX_Control(pDX, IDC_FILE_SAVE, m_cFileSave);
	DDX_Control(pDX, IDC_SELECT_START, m_cSelectStart);
	DDX_Control(pDX, IDC_SELECT_END, m_cSelectEnd);
	DDX_Control(pDX, IDC_SELECT_TIME, m_cSelectTime);
	DDX_Control(pDX, IDC_RECORD_TIME, m_cRecordTime);
	DDX_Control(pDX, IDC_CUT, m_cCut);
	DDX_Control(pDX, IDC_SELECT_ALL, m_cSelectAll);
	DDX_Radio(pDX, IDC_PRIORITY_INPUT, m_iPriority);
}

BEGIN_MESSAGE_MAP(CRecDlg, CBaseDlg)
	ON_BN_CLICKED(IDC_RECORD, OnBnClickedRecord)
	ON_BN_CLICKED(IDC_PLAY, OnBnClickedPlay)
	ON_WM_MOVE()
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_COMMAND(IDM_ZOOM_IN_H, OnZoomInH)
	ON_COMMAND(IDM_ZOOM_OUT_H, OnZoomOutH)
	ON_COMMAND(IDM_ZOOM_FULL_H, OnZoomFullH)
	ON_BN_CLICKED(IDC_FILE_LOAD, OnBnClickedFileLoad)
	ON_BN_CLICKED(IDC_FILE_SAVE, OnBnClickedFileSave)
	ON_BN_CLICKED(IDC_SELECT_ALL, OnBnClickedSelectAll)
	ON_BN_CLICKED(IDC_CUT, OnBnClickedCut)
	ON_EN_CHANGE(IDC_SELECT_START, OnEnChangeSelectStart)
	ON_EN_CHANGE(IDC_SELECT_END, OnEnChangeSelectEnd)
	ON_BN_CLICKED(IDC_GRAPH_NORM, OnBnClickedGraphNorm)
	ON_BN_CLICKED(IDC_PRIORITY_INPUT, OnBnClickedPriorityInput)
	ON_BN_CLICKED(IDC_PRIORITY_OUTPUT, OnBnClickedPriorityOutput)
	ON_WM_HELPINFO()
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

// CRecDlg メッセージ ハンドラ

BOOL CRecDlg::OnInitDialog()
{
	CBaseDlg::OnInitDialog();

	CreateStatusBar();
	SetCurrentStatus(STATUS_STOP);

	if (m_tbZoomH.Create(this, WS_CHILD | WS_VISIBLE | CBRS_SIZE_FIXED | CBRS_ALIGN_LEFT | CBRS_TOOLTIPS, ID_ZOOM_H) &&
		m_tbZoomH.LoadToolBar(IDR_ZOOM_H)) {
		WINDOWPLACEMENT wp;
		m_cZoomH.GetWindowPlacement(&wp);

		CSize sizeToolBar;
		sizeToolBar = m_tbZoomH.CalcFixedLayout(FALSE, FALSE);
		m_tbZoomH.MoveWindow(12, wp.rcNormalPosition.top - 8, sizeToolBar.cx, sizeToolBar.cy);
	}

	m_bMove = TRUE;

	m_cRecWnd.Initialize(90, GraphCallBack, (LPARAM)this, FALSE);

	InitDispTime();
	DispWaveGraph();

	m_cSelectStart.SetValidChar(VC_NUM | VC_POINT);
	m_cSelectEnd.SetValidChar(VC_NUM | VC_POINT);

	DragAcceptFiles();

	return TRUE;
}

void CRecDlg::OnMove(int x, int y)
{
	CBaseDlg::OnMove(x, y);

	if (m_bMove) {
		CRect rect;
		GetWindowRect(rect);
		g_oSetData.Rec.nPosLeft = rect.left;
		g_oSetData.Rec.nPosTop = rect.top;
	}
}

void CALLBACK CRecDlg::GraphCallBack(WPARAM code, LPARAM data, double param1, double param2)
{
	CRecDlg *pWnd = (CRecDlg *)data;

	switch (code) {
	case CC_SCROLL:
		pWnd->m_fStartTime -= param1 * pWnd->m_fDispTime;
		pWnd->DispWaveGraph();
		break;
	case CC_SELECT_SET:
		pWnd->DispSelectArea(param1, param2);
		break;
	}
}

void CRecDlg::OnBnClickedRecord()
{
	if (m_cPlay)
		PlayModeOff();

	if (m_cRecord)
		RecModeOff();
	else
		RecModeOn();
}

void CRecDlg::OnBnClickedPlay()
{
	if (m_cRecord)
		RecModeOff();

	if (m_cPlay)
		PlayModeOff();
	else
		PlayModeOn();
}

void CRecDlg::RecModeOn()
{
	m_cRecord = TRUE;

	if (CWaveInEx::SetRecMode(this, 0) != NULL)
		InitRecMode(REC_MODE_IN);

	if (CWaveOutEx::SetRecMode(this) != NULL)
		InitRecMode(REC_MODE_OUT);
}

void CRecDlg::RecModeOff()
{
	m_cRecord = FALSE;
	CWaveInEx::SetRecMode(NULL, 0);
	CWaveOutEx::SetRecMode(NULL);

	InitRecMode(REC_MODE_NONE);

	SetCurrentStatus(STATUS_STOP);
}

void CRecDlg::PlayModeOn()
{
	m_cPlay = TRUE;

	if (CWaveInEx::SetRecMode(this, 1) != NULL)
		InitPlayMode(PLAY_MODE_OUT);
}

void CRecDlg::PlayModeOff()
{
	m_cPlay = FALSE;
	CWaveInEx::SetRecMode(NULL, 1);

	InitPlayMode(PLAY_MODE_NONE);

	SetCurrentStatus(STATUS_STOP);
}

void CRecDlg::InitRecMode(int nRecMode)
{
	CWaveInEx *pWaveIn;
	CWaveOutEx *pWaveOut;

	switch (nRecMode) {
	case REC_MODE_IN:
		if (m_nRecMode == REC_MODE_OUT && g_oSetData.Rec.nPriority == 1)
			return;

		if ((pWaveIn = CWaveInEx::GetRecWave()) == NULL)
			return;

		m_dwRecCount = 0;
		m_nSamplingRate = pWaveIn->GetSamplesPerSec();
		m_nRecChannel = pWaveIn->GetChannels();
		m_nBitsPerSample = pWaveIn->GetBitsPerSample();
		m_cRecWnd.SetSelectArea(0, 0);
		m_cGraphNorm.EnableWindow(FALSE);
		break;
	case REC_MODE_OUT:
		if (m_nRecMode == REC_MODE_IN && g_oSetData.Rec.nPriority == 0)
			return;

		if ((pWaveOut = CWaveOutEx::GetRecWave()) == NULL)
			return;

		m_dwRecCount = 0;
		m_nSamplingRate = pWaveOut->GetSamplesPerSec();
		m_nRecChannel = pWaveOut->GetChannels();
		m_nBitsPerSample = pWaveOut->GetBitsPerSample();
		m_cRecWnd.SetSelectArea(0, 0);
		m_cGraphNorm.EnableWindow(FALSE);
		break;
	case REC_MODE_NONE:
		if (m_dwRecCount != 0)
			m_fTotalTime = (double)(m_dwRecCount / m_nRecChannel) / m_nSamplingRate;
		else
			m_fTotalTime = INIT_TIME;
		m_fDispTime = m_fTotalTime;
		m_fMaxData = 0;
		DispWaveGraph();
		if (m_dwRecCount != 0)
			m_cRecWnd.SetSelectArea(0, m_fTotalTime);
		m_cGraphNorm.EnableWindow(TRUE);
		break;
	}

	m_fDataTime = (double)(m_dwRecCount / m_nRecChannel) / m_nSamplingRate;

	DispRecInfo();
	EnableButton();

	m_nRecMode = nRecMode;
}

void CRecDlg::InitPlayMode(int nPlayMode)
{
	CWaveInEx *pWaveIn;

	if (nPlayMode == PLAY_MODE_OUT) {
		if ((pWaveIn = CWaveInEx::GetRecWave()) != NULL) {
			m_nPlayChannel = pWaveIn->GetChannels();

			m_fPlayTime = m_fSelectStart;
			m_dwPlayCount = min((DWORD)(m_fSelectStart * m_nSamplingRate * m_nRecChannel) / m_nRecChannel * m_nRecChannel, m_dwRecCount);
			m_dwPlayEndCount = min((DWORD)(m_fSelectEnd * m_nSamplingRate * m_nRecChannel) / m_nRecChannel * m_nRecChannel, m_dwRecCount);

		}
	}

	m_nPlayMode = nPlayMode;
}

void CRecDlg::EnableButton()
{
	BOOL bEnable = (m_dwRecCount != 0);
	m_cPlay.EnableWindow(bEnable);
	m_cFileSave.EnableWindow(bEnable);
	m_cCut.EnableWindow(bEnable);
	m_cSelectAll.EnableWindow(bEnable);
}

int CRecDlg::OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify)
{
	LONG rc = 0;
	int nData;

	switch (nCode) {
	case WAVEIN_OPEN:
		if (m_cRecord)
			InitRecMode(REC_MODE_IN);
		break;
	case WAVEIN_CLOSE:
		if (m_nRecMode == REC_MODE_IN)
			RecModeOff();
		SetCurrentStatus(STATUS_STOP);
		break;
	case WAVEIN_DATA:
		if (m_nRecMode == REC_MODE_IN) {
			int dwRecCount = m_dwRecCount;
			nData = RecordData(pWaveNotify);
			DispCurrentWave(dwRecCount, nData);
			SetCurrentStatus(STATUS_RECORD);
		}
		break;
	case WAVEIN_QUERY_RATE:
		rc = m_nSamplingRate;
		break;
	case WAVEIN_QUERY_BITS:
		rc = m_nBitsPerSample;
		break;
	case WAVEOUT_OPEN:
		if (m_cRecord)
			InitRecMode(REC_MODE_OUT);
		else if (m_cPlay)
			InitPlayMode(PLAY_MODE_OUT);
		break;
	case WAVEOUT_CLOSE:
		if (m_nPlayMode == PLAY_MODE_OUT) {
			CWaveInEx::CloseRecWave();
			m_cRecWnd.SetReplayPosition(-1);
		} else {
			if (m_nRecMode == REC_MODE_OUT)
				RecModeOff();
		}
		SetCurrentStatus(STATUS_STOP);
		break;
	case WAVEOUT_DATA:
	case WAVEOUT_CLOSE_WAIT:
		if (m_nPlayMode == PLAY_MODE_OUT) {
			BOOL bValid = TRUE;

			if (pWaveNotify->nFlags & WHDR_DONE) {
				//bValid = CWaveInEx::NotifyWave(WAVEIN_DATA, pWaveNotify);
				m_fPlayTime += (double)pWaveNotify->nSamplesRecorded / m_nSamplingRate;
				m_cRecWnd.SetReplayPosition(m_fPlayTime);
			}

			if (bValid) {
				rc = PlayData(pWaveNotify);
				pWaveNotify->nSamplesRecorded = rc;
				SetCurrentStatus(STATUS_PLAY);
			} else
				CWaveInEx::CloseRecWave();
		} else {
			if (m_nRecMode == REC_MODE_OUT) {
				if (pWaveNotify->nFlags & WHDR_DONE) {
					pWaveNotify->nSamplesRecorded = pWaveNotify->nSamplesNum;
					int nRecCount = m_dwRecCount;
					nData = RecordData(pWaveNotify);
					DispCurrentWave(nRecCount, nData);
					SetCurrentStatus(STATUS_RECORD);
				}
			}
		}
		break;
	}

	return rc;
}

int CRecDlg::RecordData(LPWAVENOTIFY pWaveNotify)
{
	DWORD nData = pWaveNotify->nSamplesRecorded * m_nRecChannel;
	double *pData = pWaveNotify->pSamplesData;
	DWORD i;

	VirtualAlloc(&m_pWaveBuf[m_dwRecCount], (SIZE_T)nData * sizeof(double), MEM_COMMIT, PAGE_READWRITE);

	for (i = 0; i < nData && m_dwRecCount < m_dwAllocSize; i++) {
		m_pWaveBuf[m_dwRecCount] = *pData++;

		m_dwRecCount++;
	}

	return i;
}

int CRecDlg::PlayData(LPWAVENOTIFY pWaveNotify)
{
	int nData = pWaveNotify->nSamplesNum;
	double *pData = pWaveNotify->pSamplesData;
	int i;
	double fLeftData, fRightData;

	for (i = 0; i < nData && m_dwPlayCount < m_dwPlayEndCount; i++) {
		fLeftData = m_pWaveBuf[m_dwPlayCount++];
		fRightData = (m_nRecChannel == 1) ? fLeftData : m_pWaveBuf[m_dwPlayCount++];

		*pData++ = fLeftData;
		if (m_nPlayChannel == 2)
			*pData++ = fRightData;
	}

	return i;
}

void CRecDlg::AllocBuf()
{
	MEMORYSTATUSEX msex{
		.dwLength = sizeof(msex)
	};
	::GlobalMemoryStatusEx(&msex);

	DWORDLONG dwMem = msex.ullAvailPhys;
	dwMem /= sizeof(double) * 2;
	dwMem *= 2;
	dwMem = min(dwMem, (DWORDLONG)0x80000000);

	m_dwAllocSize = (DWORD)dwMem;
	while (true) {
		m_pWaveBuf = (double *)VirtualAlloc(NULL, (SIZE_T)m_dwAllocSize * sizeof(double), MEM_RESERVE, PAGE_READWRITE);
		if (m_pWaveBuf != NULL)
			break;

		m_dwAllocSize /= 2;
	}
}

void CRecDlg::FreeBuf()
{
	if (m_pWaveBuf != NULL) {
		VirtualFree(m_pWaveBuf, 0, MEM_RELEASE);
		m_pWaveBuf = NULL;
	}
}

void CRecDlg::SetCurrentStatus(int nCurrentStatus)
{
	int nStringId;

	switch (nCurrentStatus) {
	case STATUS_STOP:
		if (m_dwRecCount == 0)
			nStringId = IDS_NOT_RECORDED;
		else
			nStringId = IDS_RECORDED;
		break;
	case STATUS_RECORD:
		if (m_nCurrentStatus == STATUS_RECORD)
			return;
		nStringId = IDS_RECORDING;
		break;
	case STATUS_PLAY:
		if (m_nCurrentStatus == STATUS_PLAY)
			return;
		nStringId = IDS_PLAYING;
		break;
	default:
		return;
	}

	SetStatusBar(GetRunningStatus(nStringId, 0, 0,m_nBitsPerSample));
	m_nCurrentStatus = nCurrentStatus;
}

void CRecDlg::OnDestroy()
{
	RecModeOff();
	PlayModeOff();

	CBaseDlg::OnDestroy();
}

void CRecDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/)
{
	switch (nSBCode) {
	case SB_LEFT:
		m_fStartTime = 0;
		break;
	case SB_LINELEFT:
		m_fStartTime -= m_fDispTime / 32;
		break;
	case SB_LINERIGHT:
		m_fStartTime += m_fDispTime / 32;
		break;
	case SB_PAGELEFT:
		m_fStartTime -= m_fDispTime;
		break;
	case SB_PAGERIGHT:
		m_fStartTime += m_fDispTime;
		break;
	case SB_RIGHT:
		m_fStartTime = m_fTotalTime - m_fDispTime;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_fStartTime = m_fTotalTime * nPos / SCROLL_RANGE;
		break;
	}

	DispWaveGraph();
}

void CRecDlg::DispWaveGraph()
{
	if (m_fDispTime == 0)
		m_fDispTime = m_fTotalTime;
	else if (m_fDispTime < MIN_DISP_TIME)
		m_fDispTime = MIN_DISP_TIME;
	else if (m_fDispTime > m_fTotalTime)
		m_fDispTime = m_fTotalTime;

	if (m_fStartTime < 0)
		m_fStartTime = 0;
	else if (m_fStartTime > m_fTotalTime - m_fDispTime)
		m_fStartTime = m_fTotalTime - m_fDispTime;

	int nScrollSize = (int)(m_fDispTime / m_fTotalTime * SCROLL_RANGE);
	int nScrollPos = (int)(m_fStartTime / m_fTotalTime * SCROLL_RANGE);

	if (nScrollSize != m_nScrollSize || nScrollPos != m_nScrollPos) {
		m_nScrollSize = nScrollSize;
		m_nScrollPos = nScrollPos;

		SCROLLINFO si{
			.cbSize = sizeof(SCROLLINFO),
			.fMask = SIF_ALL,
			.nMin = 0,
			.nMax = SCROLL_RANGE,
			.nPage = (UINT)nScrollSize,
			.nPos = nScrollPos
		};
		m_cGraphScroll.SetScrollInfo(&si, TRUE);
	}

	CToolBarCtrl &tc = m_tbZoomH.GetToolBarCtrl();
	if (m_fDispTime == m_fTotalTime / MAX_ZOOM_TIME) {
		tc.EnableButton(IDM_ZOOM_FULL_H, TRUE);
		tc.EnableButton(IDM_ZOOM_IN_H, FALSE);
		tc.EnableButton(IDM_ZOOM_OUT_H, TRUE);
	} else if (m_fDispTime == m_fTotalTime) {
		tc.EnableButton(IDM_ZOOM_FULL_H, FALSE);
		tc.EnableButton(IDM_ZOOM_IN_H, TRUE);
		tc.EnableButton(IDM_ZOOM_OUT_H, FALSE);
	} else {
		tc.EnableButton(IDM_ZOOM_FULL_H, TRUE);
		tc.EnableButton(IDM_ZOOM_IN_H, TRUE);
		tc.EnableButton(IDM_ZOOM_OUT_H, TRUE);
	}

	double fMaxData;
	if (m_cGraphNorm) {
		if (m_fMaxData == 0)
			m_fMaxData = GetMaxData(m_pWaveBuf, m_dwRecCount);
		fMaxData = m_fMaxData;
	} else
		fMaxData = 1.0;

	m_cRecWnd.DispGraph(m_fTotalTime, m_fStartTime, m_fDispTime, m_pWaveBuf, m_dwRecCount, m_nRecChannel, 0, fMaxData);

	m_cZoomH = int(m_fTotalTime / m_fDispTime);
}

void CRecDlg::InitDispTime()
{
	m_fTotalTime = INIT_TIME;
	m_fDispTime = m_fTotalTime;
	m_fStartTime = 0;
}

void CRecDlg::OnZoomFullH()
{
	m_fDispTime = m_fTotalTime;
	DispWaveGraph();
}

void CRecDlg::OnZoomInH()
{
	m_fDispTime /= 2;
	DispWaveGraph();
}

void CRecDlg::OnZoomOutH()
{
	m_fDispTime *= 2;
	DispWaveGraph();
}

void CRecDlg::OnBnClickedFileLoad()
{
	CFileDialog fileDlg(TRUE, "wav", NULL,
			OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
			"Wave File (*.wav *.wavdssf)|*.wav;*.wavdssf|All Files (*.*)|*.*||", this, 0);

	if (fileDlg.DoModal() == IDOK)
		LoadFile(fileDlg.GetPathName());
}

void CRecDlg::LoadFile(LPCTSTR pPathName)
{
	char *pExt = ::PathFindExtension(pPathName);

	if (_stricmp(pExt, ".wav") == 0)
		LoadWavFile(pPathName);
	else if (_stricmp(pExt, ".wavdssf") == 0)
		LoadWavDssfFile(pPathName);
}

void CRecDlg::LoadWavFile(LPCTSTR pPathName)
{
	CWaveFile waveFile;
	PCMWAVEFORMAT waveFormat;
	DWORD dwDataSize;

	if (!waveFile.Open(pPathName, &waveFormat, &dwDataSize))
		return;

	CWaveInEx::CloseRecWave();

	m_dwRecCount = min(dwDataSize / (waveFormat.wBitsPerSample / 8), m_dwAllocSize);
	while (m_dwRecCount != 0) {
		if (VirtualAlloc(m_pWaveBuf, (SIZE_T)m_dwRecCount * sizeof(double), MEM_COMMIT, PAGE_READWRITE) != NULL) {
			BYTE *pDummy = NULL;
			try {
				pDummy = new BYTE[m_dwRecCount * (waveFormat.wBitsPerSample / 8)];
			} catch (...) {
			}
			if (pDummy != NULL) {
				delete [] pDummy;
				break;
			}
		}
		m_dwRecCount /= 2;
	}

	waveFile.Read(m_pWaveBuf, m_dwRecCount);

	m_nSamplingRate = waveFormat.wf.nSamplesPerSec;
	m_nRecChannel = waveFormat.wf.nChannels;
	m_nBitsPerSample = waveFormat.wBitsPerSample;

	m_fDataTime = (double)(m_dwRecCount / m_nRecChannel) / m_nSamplingRate;
	m_fTotalTime = m_fDataTime;
	m_fDispTime = m_fTotalTime;
	m_fStartTime = 0;
	m_fMaxData = 0;

	DispWaveGraph();
	m_cRecWnd.SetSelectArea(0, m_fTotalTime);
	DispRecInfo();

	EnableButton();

	SetCurrentStatus(STATUS_STOP);

	if ((BOOL)m_cPlay == FALSE)
		OnBnClickedPlay();
}

void CRecDlg::LoadWavDssfFile(LPCTSTR pPathName)
{
	LoadWavFile(pPathName);

	CFile file;
	if (file.Open(pPathName, CFile::modeRead | CFile::shareDenyNone)) {
		int nSizeWav;
		int nSizeAutoRecInfo;
		int nSizeSetData;
		AutoRecInfo oAutoRecInfo{};

		file.Seek(4, CFile::begin);
		file.Read(&nSizeWav, 4);
		file.Seek(nSizeWav, CFile::current);
		file.Read(&nSizeAutoRecInfo, sizeof(int));
		file.Read(&oAutoRecInfo, nSizeAutoRecInfo);
		file.Read(&nSizeSetData, sizeof(int));

		switch (oAutoRecInfo.nWindow) {
		case WINDOW_SG:
			file.Read(&g_oSetData.Sg, nSizeSetData);
			break;
		case WINDOW_FFT:
			file.Read(&g_oSetData.Fft, nSizeSetData);
			break;
		case WINDOW_OS:
			file.Read(&g_oSetData.Os, nSizeSetData);
			break;
		case WINDOW_IMP:
			file.Read(&g_oSetData.Imp, nSizeSetData);
			break;
		case WINDOW_ACF:
			file.Read(&g_oSetData.Acf, nSizeSetData);
			break;
		case WINDOW_DST:
			file.Read(&g_oSetData.Dst, nSizeSetData);
			break;
		case WINDOW_FRE:
			file.Read(&g_oSetData.Fre, nSizeSetData);
			break;
		}

		CRtaApp *pRtaApp = (CRtaApp *)::AfxGetApp();
		CRtaDlg *pRtaDlg = pRtaApp->m_pWndRta;
		pRtaDlg->CloseRaWindow(WINDOW_ALL & ~WINDOW_REC);
		pRtaDlg->OpenRaWindow(oAutoRecInfo.nWindow);

		file.Close();
	}
}

void CRecDlg::OnBnClickedFileSave()
{
	CFileDialog fileDlg(FALSE, "wav", "",
			OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
			"16bit Wave File (*.wav)|*.wav|24bit Wave File (*.wav)|*.wav||", this, 0);

	if (fileDlg.DoModal() == IDCANCEL)
		return;

	int nBitsPerSample = fileDlg.m_pOFN->nFilterIndex == 1 ? 16 : 24;
	int nBytesPerSample = (nBitsPerSample / 8) * m_nRecChannel;

	PCMWAVEFORMAT waveFormat{
		.wf{
			.wFormatTag = WAVE_FORMAT_PCM,
			.nChannels = (WORD)m_nRecChannel,
			.nSamplesPerSec = (DWORD)m_nSamplingRate,
			.nAvgBytesPerSec = (DWORD)(m_nSamplingRate * nBytesPerSample),
			.nBlockAlign = (WORD)nBytesPerSample,
		},
		.wBitsPerSample = (WORD)nBitsPerSample
	};

	// Filter処理
	CRtaApp *pWinApp = (CRtaApp *)::AfxGetApp();
	if (pWinApp->m_pWndRta->m_pWndFilter != NULL) {
		if (g_oSetData.Flt.nInputOutput == 1) {
			CDigitalFilter cFilterLeft, cFilterRight;
			cFilterLeft.InitIIR((EDFShape)g_oSetData.Flt.nShape, (EDFBand)g_oSetData.Flt.nBand, g_oSetData.Flt.nOrder, m_nSamplingRate, g_oSetData.Flt.fCutoff1, g_oSetData.Flt.fCutoff2, g_oSetData.Flt.fRipple1);
			cFilterRight.InitIIR((EDFShape)g_oSetData.Flt.nShape, (EDFBand)g_oSetData.Flt.nBand, g_oSetData.Flt.nOrder, m_nSamplingRate, g_oSetData.Flt.fCutoff1, g_oSetData.Flt.fCutoff2, g_oSetData.Flt.fRipple1);

			int nData = m_dwRecCount / m_nRecChannel;
			double *pData = m_pWaveBuf;
			for (int i = 0; i < nData; i++) {
				*pData++ = cFilterLeft.ExecIIR(*pData);
				if (m_nRecChannel == 2)
					*pData++ = cFilterRight.ExecIIR(*pData);
			}
		}
	}

	CWaveFile waveFile;

	waveFile.Create(fileDlg.GetPathName(), &waveFormat);
	waveFile.Write(m_pWaveBuf, m_dwRecCount);
}

void CRecDlg::DispSelectArea(double fSelectStart, double fSelectEnd)
{
	m_bDispSelectArea = TRUE;

	if (m_fSelectStart != fSelectStart) {
		m_fSelectStart = fSelectStart;
		m_cSelectStart.Format("%.2f", fSelectStart);
	}

	if (m_fSelectEnd != fSelectEnd) {
		m_fSelectEnd = fSelectEnd;
		m_cSelectEnd.Format("%.2f", fSelectEnd);
	}

	m_cSelectTime.Format("%.2f", fSelectEnd - fSelectStart);

	m_bDispSelectArea = FALSE;
}

void CRecDlg::OnBnClickedSelectAll()
{
	m_cRecWnd.SetSelectArea(0, m_fDataTime);
}

void CRecDlg::OnBnClickedCut()
{
	if (m_pWaveBuf == NULL)
		return;

	int nSelectStart = min((DWORD)(m_fSelectStart * m_nSamplingRate), m_dwRecCount / m_nRecChannel);
	int nSelectEnd = min((DWORD)(m_fSelectEnd * m_nSamplingRate), m_dwRecCount / m_nRecChannel);
	int nSample = (nSelectEnd - nSelectStart) * m_nRecChannel;

	if (nSample <= 0)
		return;

	memcpy(m_pWaveBuf, m_pWaveBuf + nSelectStart * m_nRecChannel, nSample * sizeof(double));

	m_dwRecCount = nSample;

	m_fDataTime = (double)(m_dwRecCount / m_nRecChannel) / m_nSamplingRate;
	m_fTotalTime = m_fDataTime;
	m_fDispTime = m_fTotalTime;
	m_fStartTime = 0;
	m_fMaxData = 0;

	DispWaveGraph();
	m_cRecWnd.SetSelectArea(0, m_fDataTime);
	DispRecInfo();
}

void CRecDlg::OnEnChangeSelectStart()
{
	if (m_bDispSelectArea)
		return;

	m_fSelectStart = m_cSelectStart;

	if (m_fSelectStart < 0)
		m_fSelectStart = 0;

	if (m_fSelectStart > m_fSelectEnd - MIN_DISP_TIME)
		m_fSelectStart = m_fSelectEnd - MIN_DISP_TIME;

	m_cRecWnd.SetSelectArea(m_fSelectStart, m_fSelectEnd);
}

void CRecDlg::OnEnChangeSelectEnd()
{
	if (m_bDispSelectArea)
		return;

	m_fSelectEnd = m_cSelectEnd;

	if (m_fSelectEnd > m_fDataTime)
		m_fSelectEnd = m_fDataTime;

	if (m_fSelectEnd < m_fSelectStart + MIN_DISP_TIME)
		m_fSelectEnd = m_fSelectStart + MIN_DISP_TIME;

	m_cRecWnd.SetSelectArea(m_fSelectStart, m_fSelectEnd);
}

void CRecDlg::OnBnClickedGraphNorm()
{
	DispWaveGraph();
}

void CRecDlg::DispRecInfo()
{
	int nDataTime = (int)m_fDataTime;

	m_cRecordTime.Format("%d:%02d:%02d", nDataTime / 3600, nDataTime / 60 % 60, nDataTime % 60);
}

void CRecDlg::DispCurrentWave(int nRecCount, int nData)
{
	int nSpan = INIT_TIME * m_nSamplingRate * m_nRecChannel;
	int nCount = nRecCount / nSpan;
	int nOffset = nRecCount % nSpan;

	if (nOffset < nData)
		m_cRecWnd.DispGraph((nCount + 1) * INIT_TIME, nCount * INIT_TIME, INIT_TIME, NULL, 0, m_nRecChannel, 0, 0);

	m_cRecWnd.DispGraph2(m_pWaveBuf + nSpan * nCount, nSpan, m_nRecChannel, 0, nOffset, nData);

	m_fDataTime = (double)(m_dwRecCount / m_nRecChannel) / m_nSamplingRate;
	DispRecInfo();
}

void CRecDlg::OnBnClickedPriorityInput()
{
	g_oSetData.Rec.nPriority = 0;

	if (m_nRecMode == REC_MODE_OUT)
		InitRecMode(REC_MODE_IN);
}

void CRecDlg::OnBnClickedPriorityOutput()
{
	g_oSetData.Rec.nPriority = 1;

	if (m_nRecMode == REC_MODE_IN)
		InitRecMode(REC_MODE_OUT);
}

void CRecDlg::OnDropFiles(HDROP hDropInfo)
{
	char fullPathName[_MAX_PATH + 1];

	DragQueryFile(hDropInfo, 0, fullPathName, _MAX_PATH + 1);

	LoadFile(fullPathName);

	CBaseDlg::OnDropFiles(hDropInfo);
}

BOOL CRecDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_GRAPH_WAVE, IDH_REC_GRAPH_WAVE,
		IDC_ZOOM_H, IDH_REC_ZOOM_H,
		IDC_SCROLL_H, IDH_REC_SCROLL_H,
		IDC_GRAPH_NORM, IDH_REC_GRAPH_NORM,
		IDC_RECORD_TIME, IDH_REC_RECORD_TIME,
		IDC_SELECT_ALL, IDH_REC_SELECT_ALL,
		IDC_CUT, IDH_REC_CUT,
		IDC_SELECT_START, IDH_REC_SELECT_START,
		IDC_SELECT_END, IDH_REC_SELECT_END,
		IDC_SELECT_TIME, IDH_REC_SELECT_TIME,
		IDC_RECORD, IDH_REC_RECORD,
		IDC_PLAY, IDH_REC_PLAY,
		IDC_PRIORITY_INPUT, IDH_REC_PRIORITY_INPUT,
		IDC_PRIORITY_OUTPUT, IDH_REC_PRIORITY_OUTPUT,
		IDC_FILE_SAVE, IDH_REC_FILE_SAVE,
		IDC_FILE_LOAD, IDH_REC_FILE_LOAD,
		IDCANCEL, IDH_REC_CLOSE,
		ID_ZOOM_H_FULL, IDH_REC_ZOOM_H_FULL,
		ID_ZOOM_H_IN, IDH_REC_ZOOM_H_IN,
		ID_ZOOM_H_OUT, IDH_REC_ZOOM_H_OUT,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
