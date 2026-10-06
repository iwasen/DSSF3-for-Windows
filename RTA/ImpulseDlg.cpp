// ImpulseDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "RtaDlg.h"
#include "FileDlg.h"
#include "ImpulseDlg.h"
#include "IfaSet.h"
#include "IfaSave.h"
#include "IRAssistDlg.h"
#include "WaveInEx.h"
#include "WaveOutEx.h"
#include "Regist.h"
#include "Help\ContextHelp.h"

#define IDT_RETRY			5000
#define SCROLL_RANGE		16384
#define MAX_ZOOM_TIME		4096
#define CHECK_DATA_NUM		4096
#define INIT_LEVEL_AUTO		4
#define INIT_LEVEL_MANUAL	2

/////////////////////////////////////////////////////////////////////////////
// CImpulseDlg ダイアログ

CImpulseDlg::CImpulseDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CImpulseDlg::IDD, pParent)
{
	m_iChannel = -1;
	m_iWaveOut = -1;
	m_iFileFormat = -1;
	m_iMethod = -1;

	m_pWaveOutData= NULL;
	m_pLeftInputData = NULL;
	m_pRightInputData = NULL;
	m_pLeftImpulseData = NULL;
	m_pRightImpulseData = NULL;
	m_pCheckBuf = NULL;
	m_pIFilter = NULL;
	m_nIFilterID= -1;
	m_nScrollSize = -1;
	m_nScrollPos = -1;
	m_bInitialized = FALSE;
	m_pAssistDlg = NULL;
	m_pWaveIn = CWaveInEx::GetInstance();
	m_pWaveOut = CWaveOutEx::GetInstance();
	m_bValidData = FALSE;
	m_nWindowKind = WINDOW_IMP;

	Create(IDD, g_oSetData.Imp.nPosLeft, g_oSetData.Imp.nPosTop, g_oSetData2.nFontSizeImp, pParent);
}

CImpulseDlg::~CImpulseDlg()
{
	m_pWaveIn->ReleaseInstance();
	m_pWaveOut->ReleaseInstance();
}

void CImpulseDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILE_LOAD, m_cFileLoad);
	DDX_Control(pDX, IDC_MSEQ_METHOD, m_cMSeqMethod);
	DDX_Control(pDX, IDC_TSP_METHOD, m_cTSPMethod);
	DDX_Control(pDX, IDC_AUTO_RETRY, m_cAutoRetry);
	DDX_Control(pDX, IDC_ZOOM_H, m_cZoomH);
	DDX_Control(pDX, IDC_IFA_SET, m_cIfaSet);
	DDX_Control(pDX, IDC_CHANNEL_MONO, m_cChannelMono);
	DDX_Control(pDX, IDC_CHANNEL_STEREO, m_cChannelStereo);
	DDX_Control(pDX, IDC_ASSIST, m_cAssist);
	DDX_Control(pDX, IDC_PLAYBACK, m_cPlayback);
	DDX_Control(pDX, IDC_LAUNCH_SA, m_cLaunchSA);
	DDX_Control(pDX, IDC_AUTO_LEVEL, m_cAutoLevel);
	DDX_Control(pDX, IDC_MESSAGE, m_cMessage);
	DDX_Control(pDX, IDC_MEASURE_NUM, m_cMeasureNum);
	DDX_Control(pDX, IDC_IFA_MESSAGE, m_cIfaMessage);
	DDX_Control(pDX, IDC_IFA_CANCEL, m_cIfaCancel);
	DDX_Control(pDX, IDC_IFA_SAVE, m_cIfaSave);
	DDX_Control(pDX, IDC_WAVE_OUT, m_cWaveOut);
	DDX_Control(pDX, IDC_MR_TIME, m_cMrTime);
	DDX_Control(pDX, IDC_MR_SAMPLING, m_cMrSampling);
	DDX_Control(pDX, IDC_MR_MTIME, m_cMrMTime);
	DDX_Control(pDX, IDC_MR_MNUM, m_cMrMNum);
	DDX_Control(pDX, IDC_MR_CHANNEL, m_cMrChannel);
	DDX_Control(pDX, IDC_MR_BIT, m_cMrBit);
	DDX_Control(pDX, IDC_FILE_SAVE, m_cFileSave);
	DDX_Control(pDX, IDC_MEASURENUMSPIN, m_cImpMeasureNumSpin);
	DDX_Control(pDX, IDC_BTN_STOP, m_cBtnStop);
	DDX_Control(pDX, IDC_MEASURE_TIME, m_cMeasureTime);
	DDX_Control(pDX, IDC_IMPALSE_R, m_cImpulseR);
	DDX_Control(pDX, IDC_IMPALSE_L, m_cImpulseL);
	DDX_Control(pDX, IDC_SCROLL_H, m_cGraphScroll);
	DDX_Control(pDX, IDC_SAMPLING_RATE, m_cSamplingRate);
	DDX_Control(pDX, IDC_BTN_START, m_cBtnStart);
	DDX_Control(pDX, IDCANCEL, m_cExit);
	DDX_Radio(pDX, IDC_CHANNEL_MONO, m_iChannel);
	DDX_Radio(pDX, IDC_WAVE_IR, m_iWaveOut);
	DDX_Radio(pDX, IDC_FILE_WAVE, m_iFileFormat);
	DDX_Radio(pDX, IDC_MSEQ_METHOD, m_iMethod);
	DDX_Control(pDX, IDC_EDIT_OFFSET_TIME, m_cEditOffsetTime);
}

BEGIN_MESSAGE_MAP(CImpulseDlg, CBaseDlg)
	ON_BN_CLICKED(IDC_BTN_START, OnStart)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_SAMPLING_RATE, OnSelchangeSamplingRate)
	ON_CBN_SELCHANGE(IDC_MEASURE_TIME, OnSelchangeMeasureTime)
	ON_BN_CLICKED(IDC_CHANNEL_MONO, OnChannelMono)
	ON_BN_CLICKED(IDC_CHANNEL_STEREO, OnChannelStereo)
	ON_WM_HSCROLL()
	ON_COMMAND(IDM_ZOOM_IN_H, OnZoomInH)
	ON_COMMAND(IDM_ZOOM_OUT_H, OnZoomOutH)
	ON_COMMAND(IDM_ZOOM_FULL_H, OnZoomFullH)
	ON_BN_CLICKED(IDC_BTN_STOP, OnStop)
	ON_BN_CLICKED(IDC_FILE_SAVE, OnFileSave)
	ON_BN_CLICKED(IDC_FILE_LOAD, OnFileLoad)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_WAVE_OUT, OnWaveOut)
	ON_BN_CLICKED(IDC_IFA_SET, OnIfaSet)
	ON_BN_CLICKED(IDC_IFA_CANCEL, OnIfaCancel)
	ON_BN_CLICKED(IDC_IFA_SAVE, OnIfaSave)
	ON_WM_MOVE()
	ON_EN_CHANGE(IDC_MEASURE_NUM, OnChangeMeasureNum)
	ON_BN_CLICKED(IDC_FILE_TEXT, OnFileText)
	ON_BN_CLICKED(IDC_FILE_WAVE, OnFileWave)
	ON_BN_CLICKED(IDC_WAVE_IR, OnWaveIr)
	ON_BN_CLICKED(IDC_WAVE_IN_MSEQ, OnWaveInMseq)
	ON_BN_CLICKED(IDC_WAVE_OUT_MSRQ, OnWaveOutMsrq)
	ON_BN_CLICKED(IDC_ASSIST, OnAssist)
	ON_BN_CLICKED(IDC_AUTO_LEVEL, OnAutoLevel)
	ON_BN_CLICKED(IDC_PLAYBACK, OnPlayback)
	ON_BN_CLICKED(IDC_AUTO_RETRY, OnAutoRetry)
	ON_BN_CLICKED(IDC_MSEQ_METHOD, OnMSeqMethod)
	ON_BN_CLICKED(IDC_TSP_METHOD, OnTspMethod)
	ON_WM_HELPINFO()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_LAUNCH_SA, &CImpulseDlg::OnBnClickedLaunchSa)
	ON_EN_CHANGE(IDC_EDIT_OFFSET_TIME, &CImpulseDlg::OnEnChangeEditOffsetTime)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImpulseDlg メッセージ ハンドラ

BOOL CImpulseDlg::OnInitDialog()
{
	CBaseDlg::OnInitDialog();

	CreateStatusBar();
	SetStatusBar(IDS_STATUS_STOP);

	if (m_tbZoomH.Create(this, WS_CHILD | WS_VISIBLE | CBRS_SIZE_FIXED | CBRS_ALIGN_LEFT | CBRS_TOOLTIPS, ID_ZOOM_H) &&
		m_tbZoomH.LoadToolBar(IDR_ZOOM_H)) {
		WINDOWPLACEMENT wp;
		m_cZoomH.GetWindowPlacement(&wp);

		CSize sizeToolBar;
		sizeToolBar = m_tbZoomH.CalcFixedLayout(FALSE, FALSE);
		m_tbZoomH.MoveWindow(10, wp.rcNormalPosition.top - 8, sizeToolBar.cx, sizeToolBar.cy);
	}

	CString str;
	str.LoadString(IDS_LEFTCHANNEL);
	m_cImpulseL.Initialize(90, str, 1, COLOR_LEFT, GraphCallBack, (LPARAM)this);
	str.LoadString(IDS_RIGHTCHANNEL);
	m_cImpulseR.Initialize(90, str, 1, COLOR_RIGHT, GraphCallBack, (LPARAM)this);

	CRect rectImpulseL;
	CRect rectImpulseR;
	CRect rectControl;
	m_cImpulseL.GetWindowRect(rectImpulseL);
	m_cImpulseR.GetWindowRect(rectImpulseR);
	CWnd *pWnd = GetTopWindow();
	while (pWnd != NULL) {
		switch (pWnd->GetDlgCtrlID()) {
		case IDC_IMPALSE_L:
			SetCtlPosition(pWnd, 0, 0, 1, 0.5);
			break;
		case IDC_IMPALSE_R:
			SetCtlPosition(pWnd, 0, 0.5, 1, 1);
			break;
		case IDC_SCROLL_H:
			SetCtlPosition(pWnd, 0, 0.5, 1, 0.5);
			break;
		default:
			pWnd->GetWindowRect(rectControl);
			if (rectControl.left > rectImpulseL.right)
				SetCtlPosition(pWnd, 1, 0, 1, 0);
			else if (rectControl.top < rectImpulseR.top)
				SetCtlPosition(pWnd, 0, 0.5, 0, 0.5);
			else
				SetCtlPosition(pWnd, 0, 1, 0, 1);
			break;
		}
		pWnd = pWnd->GetNextWindow();
	}
	SetCtlPosition(&m_cImpulseL.m_wndRemark, 1, 0, 1, 0);
	SetCtlPosition(&m_cImpulseR.m_wndRemark, 1, 0, 1, 0);

	EnableImpWindow();

	SetSamplingRateList(m_cSamplingRate, WAVE_IN | WAVE_OUT, g_nWaveInDevice, g_nWaveOutDevice, g_oSetData.Imp.nSamplingRate);

	m_iChannel = g_oSetData.Imp.nChannel;
	m_cMeasureNum = g_oSetData.Imp.nMeasureNum;
	m_iWaveOut = 0;
	m_cIfaMessage.LoadString(IDS_NONE_HOSEI);

	m_iWaveOut = g_oSetData.Imp.nFileType;
	m_iFileFormat = g_oSetData.Imp.nFileFormat;

	m_cAutoLevel = g_oSetData.Imp.bAutoLevel;
	m_cAutoRetry = g_oSetData.Imp.bAutoRetry;

	m_iMethod = g_oSetData.Imp.nMethod;

	m_cEditOffsetTime = g_oSetData.Imp.fOffsetTime;
	m_cEditOffsetTime.SetValidChar(VC_NUM | VC_MINUS | VC_POINT);

	UpdateData(FALSE);

	m_cImpMeasureNumSpin.SetRange(1, 100);

	SetTimeList();

	m_fTotalTime = double(1 << (g_oSetData.Imp.nTime + STAGE_OFFSET)) / g_oSetData.Imp.nSamplingRate;
	InitDispTime();
	DispImpWindow();

	StartStopButton(FALSE);

	m_bInitialized = TRUE;

	return TRUE;
}

void CImpulseDlg::OnStart()
{
	StartMeasuring();
}

void CImpulseDlg::StartMeasuring()
{
	CString time;

	SYSTEMTIME st;
	GetLocalTime(&st);
	time.Format("%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay,
					st.wHour, st.wMinute, st.wSecond);

	UpdateData(TRUE);
	SetMeasuringParameters(g_oSetData.Imp.nSamplingRate, TOPMSEQPARAM + g_oSetData.Imp.nTime, g_oSetData.Imp.nMeasureNum, g_oSetData.Imp.nChannel + 1, 16, time, g_oSetData.Imp.nMethod);

	if (m_pIFilter != NULL && m_nIfaSampling != m_nRate) {
		::AfxMessageBox(IDS_MSG_SAMPLINGRATE);
		return;
	}

	m_nWaveBufNum = min(1024, m_nData);

	m_pCheckBuf = new double[m_nData];
	m_nCheckCount = 0;
	m_bImpulseData = FALSE;

	m_nRetryCounter = 0;
	m_nMultiplier = g_oSetData.Imp.bAutoLevel ? INIT_LEVEL_AUTO : INIT_LEVEL_MANUAL;

	if (Start())
		StartStopButton(TRUE);
	else {
		FreeBuffers();
		StartStopButton(FALSE);
	}
}

void CImpulseDlg::SetMeasuringParameters(int nRate, int nStage, int nMeasureNum, int nChannel, int nBit, const CString &currentTime, int nMethod, int nData)
{
	FreeBuffers();

	m_sTime = currentTime;
	m_nMeasureNum = nMeasureNum;

	m_nStage = nStage;
	m_nData = (nData == 0) ? 1 << m_nStage : nData;

	m_nChannel = nChannel;
	m_nBitsPerSample = nBit;
	m_nRate = nRate;
	m_nMethod = nMethod;

	m_pLeftInputData = new double[m_nData];
	m_pLeftImpulseData = new double[m_nData];
	if (m_nChannel == 2) {
		m_pRightInputData = new double[m_nData];
		m_pRightImpulseData = new double[m_nData];
	}

	m_pWaveOutData = new double[(INT_PTR)1 << m_nStage];

	if (m_nMethod == METHOD_MSEQ)
		m_pMethod = &m_oMSeq;
	else
		m_pMethod = &m_oTSP;

	m_pMethod->InitMethod(nStage);
	m_pMethod->GenerateSequence(m_pWaveOutData, 1);

	if (m_pIFilter != NULL)
		InverseConvolution(m_pWaveOutData, m_nData, m_pIFilter, m_nIFilter);
}

BOOL CImpulseDlg::Start()
{
	m_nOutPtr = 0;
	m_nOutCounter = 0;
	m_bOutEnd = FALSE;
	m_nInPtr = 0;
	m_nInCounter = 0;
	m_bInEnd = FALSE;

	if (!m_pWaveOut->Open(g_nWaveOutDevice, this, 1, m_nRate, m_nWaveBufNum, 4, TRUE, g_oSetData.Imp.bAutoLevel))
		return FALSE;

	if (!m_pWaveIn->Open(g_nWaveInDevice, this, m_nChannel, m_nRate, m_nWaveBufNum, 4, FALSE)) {
		m_pWaveOut->Close();
		::AfxMessageBox(IDS_MSG_SOUNDINOUT);
		return FALSE;
	}

	m_nBitsPerSample = m_pWaveIn->GetBitsPerSample();

	if (!(g_oSetData.Imp.bAutoRetry || g_oSetData.Imp.bAutoLevel) || m_nCheckCount % 2 == 1) {
		memset(m_pLeftInputData, 0, m_nData * sizeof(double));
		if (m_pRightInputData != NULL)
			memset(m_pRightInputData, 0, m_nData * sizeof(double));
	}

	StartWaveInOut(m_pWaveIn, m_pWaveOut, g_oSetData.Imp.bAutoLevel);

	if (m_nRetryCounter == 0)
		m_cMessage.LoadString(IDS_MEASURING);
	else
		m_cMessage.FormatMessage(IDS_RETRY, m_nRetryCounter);

	return TRUE;
}

void CImpulseDlg::OnStop()
{
	CancelMeasuring();

	InitDispTime();
	DispImpWindow();
}

void CImpulseDlg::CancelMeasuring()
{
	Stop();

	m_pWaveOut->Close();
	m_pWaveIn->Close();

	FreeBuffers();
}

void CImpulseDlg::Stop()
{
	m_cMessage.Blank();
	StartStopButton(FALSE);
}

void CImpulseDlg::FreeBuffers()
{
	if (m_pWaveOutData != NULL) {
		delete [] m_pWaveOutData;
		m_pWaveOutData = NULL;
	}

	if (m_pLeftInputData != NULL) {
		delete [] m_pLeftInputData;
		m_pLeftInputData = NULL;
	}

	if (m_pRightInputData != NULL) {
		delete [] m_pRightInputData;
		m_pRightInputData = NULL;
	}

	if (m_pLeftImpulseData != NULL) {
		delete [] m_pLeftImpulseData;
		m_pLeftImpulseData = NULL;
	}

	if (m_pRightImpulseData != NULL) {
		delete [] m_pRightImpulseData;
		m_pRightImpulseData = NULL;
	}

	if (m_pCheckBuf != NULL) {
		delete [] m_pCheckBuf;
		m_pCheckBuf = NULL;
	}

	ValidData(FALSE);
}

int CImpulseDlg::OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify)
{
	double *pData;
	int i;
	int nEndCount;

	switch (nCode) {
	case WAVEOUT_OPEN:
//		pWaveNotify->nWindow = WINDOW_IMP;
		break;
	case WAVEOUT_CLOSE:
		m_bOutEnd = TRUE;
		if (m_bInEnd)
			EndProcess();
		break;
	case WAVEOUT_DATA:
		if (!(g_oSetData.Imp.bAutoRetry || g_oSetData.Imp.bAutoLevel) || m_nCheckCount % 2 == 1) {
			if (m_nOutCounter >= m_nMeasureNum + 1)
				return 0;
		} else {
			if (m_nOutCounter >= 1)
				return 0;
		}

		pData = pWaveNotify->pSamplesData;
		for (i = 0; i < m_nWaveBufNum; i++) {
			*pData++ = (m_pWaveOutData[m_nOutPtr] / m_nMultiplier);
			if (++m_nOutPtr >= m_nData) {
				m_nOutPtr = 0;
				m_nOutCounter++;
				break;
			}
		}
		return m_nWaveBufNum;
	case WAVEIN_OPEN:
		SetStatusBar(GetRunningStatus(IDS_STATUS_RUNNING, m_pWaveIn->GetBitsPerSample(), m_pWaveOut->GetBitsPerSample(), 0));
//		pWaveNotify->nWindow = WINDOW_IMP;
		break;
	case WAVEIN_CLOSE:
		m_bInEnd = TRUE;
		if (m_bOutEnd)
			EndProcess();
		SetStatusBar(IDS_STATUS_STOP);
		break;
	case WAVEIN_DATA:
		if (!(g_oSetData.Imp.bAutoRetry || g_oSetData.Imp.bAutoLevel) || m_nCheckCount % 2 == 1) {
			if (m_nInCounter >= 1) {
				pData = pWaveNotify->pSamplesData;
				if (m_nChannel == 1) {
					for (i = 0; i < m_nWaveBufNum; i++, m_nInPtr++)
						m_pLeftInputData[m_nInPtr] += *pData++ / m_nMeasureNum;
				} else {
					for (i = 0; i < m_nWaveBufNum; i++, m_nInPtr++) {
						m_pLeftInputData[m_nInPtr] += *pData++ / m_nMeasureNum;
						m_pRightInputData[m_nInPtr] += *pData++ / m_nMeasureNum;
					}
				}
			} else
				m_nInPtr += m_nWaveBufNum;

			nEndCount = m_nMeasureNum + 1;
		} else {
			pData = pWaveNotify->pSamplesData;
			if (m_nChannel == 1) {
				for (i = 0; i < m_nWaveBufNum; i++, m_nInPtr++)
					m_pCheckBuf[m_nInPtr] = *pData++;
			} else {
				for (i = 0; i < m_nWaveBufNum; i++, m_nInPtr++) {
					m_pCheckBuf[m_nInPtr] = *pData++;
					pData++;
				}
			}

			nEndCount = 1;
		}

		if (m_nInPtr >= m_nData) {
			m_nInPtr = 0;
			m_nInCounter++;
		}

		if (m_nInCounter >= nEndCount)
			return FALSE;

		return TRUE;
	}

	return 0;
}

void CImpulseDlg::EndProcess()
{
	if (!m_bStart)
		return;

	if (g_oSetData.Imp.bAutoLevel) {
		if (m_nCheckCount == 0) {
			if (LevelCheck(m_pCheckBuf))
				m_nCheckCount++;

			SetTimer(IDT_RETRY, 1000 * m_nData / m_nRate, NULL);
			return;
		}
	}

	if (g_oSetData.Imp.bAutoRetry) {
		const int nCheckDataNum = min(m_nData, CHECK_DATA_NUM);
		const double *pCheck1 = m_pLeftInputData + (m_nData - nCheckDataNum);
		const double *pCheck2 = m_pCheckBuf + (m_nData - nCheckDataNum);

		if (m_nCheckCount == 0 || TimingCheck(pCheck1, pCheck2, nCheckDataNum) > 5) {
			m_nRetryCounter += m_nCheckCount % 2;
			m_nCheckCount++;
			SetTimer(IDT_RETRY, 1000 * m_nData / m_nRate, NULL);
			return;
		}
	}

	const int nTimeOffset = (int)(g_oSetData.Imp.fOffsetTime / 1000 * m_nRate);
	//memcpy(m_pLeftImpulseData, m_pLeftInputData, m_nData * sizeof(double));
	ShiftCopy(m_pLeftImpulseData, m_pLeftInputData, m_nData, nTimeOffset);
	m_pMethod->CalcImpulse(m_pLeftImpulseData);
	if (m_nChannel == 2) {
		//memcpy(m_pRightImpulseData, m_pRightInputData, m_nData * sizeof(double));
		ShiftCopy(m_pRightImpulseData, m_pRightInputData, m_nData, nTimeOffset);
		m_pMethod->CalcImpulse(m_pRightImpulseData);
	}

	Stop();

	if (m_pAssistDlg == NULL) {
		m_fTotalTime = double(m_nData) / m_nRate;
		DispImpWindow();

		DispMeasureParam();

		ValidData(TRUE);
	} else
		m_pAssistDlg->EndIR();
}

void CImpulseDlg::ShiftCopy(double *pTo, double *pFrom, int nData, int nShift)
{
	int nCopy;

	if (nShift < 0) {
		nShift = -nShift % nData;
		nCopy = nData - nShift;
		memcpy(pTo, pFrom + nShift, nCopy * sizeof(double));
		memcpy(pTo + nCopy, pFrom, (nData - nCopy) * sizeof(double));
	} else {
		nShift = nShift % nData;
		nCopy = nData - nShift;
		memcpy(pTo + nShift, pFrom, nCopy * sizeof(double));
		memcpy(pTo, pFrom + nCopy, (nData - nCopy) * sizeof(double));
	}
}

int CImpulseDlg::TimingCheck(const double *pData1, const double *pData2, int nCheckDataNum)
{
	int i, j;
	double xt1, yt1, xt2, yt2;

	double *pBuf1 = new double[nCheckDataNum];
	double *pBuf2 = new double[nCheckDataNum];

	memcpy(pBuf1, pData1, nCheckDataNum * sizeof(double));
	memcpy(pBuf2, pData2, nCheckDataNum * sizeof(double));

	m_oRFFT.fft(nCheckDataNum, pBuf1);
	m_oRFFT.fft(nCheckDataNum, pBuf2);

	pBuf1[0] = 0;
	pBuf1[1] = 0;

	for (i = 1; i < nCheckDataNum / 2; i++) {
		j = i * 2;

		xt1 = pBuf1[j];
		yt1 = pBuf1[j + 1];

		xt2 = pBuf2[j];
		yt2 = pBuf2[j + 1];

		pBuf1[j] = xt1 * xt2 + yt1 * yt2;
		pBuf1[j + 1] = xt2 * yt1 - xt1 * yt2;
	}

	m_oRFFT.ifft(nCheckDataNum, pBuf1);

	int index = 0;
	double max = 0;
	for (i = 0; i < nCheckDataNum; i++) {
		if (pBuf1[i] > max) {
			max = pBuf1[i];
			index = i;
		}
	}
	if (index > nCheckDataNum / 2)
		index = nCheckDataNum - index;

	delete [] pBuf1;
	delete [] pBuf2;

	return index;
}

BOOL CImpulseDlg::LevelCheck(const double *pData)
{
	double fMaxData = 0;
	for (int i = 0; i < m_nData; i++) {
		const double fAbsData = fabs(*pData++);
		if (fAbsData > fMaxData)
			fMaxData = fAbsData;
	}

	if (fMaxData >= 1.0) {
		m_nMultiplier *= 2;
		return FALSE;
	}

	if (fMaxData < 1 / 4) {
		if (m_nMultiplier != 1) {
			m_nMultiplier /= 2;
			return FALSE;
		}
	}

	return TRUE;
}

void CImpulseDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent) {
	case IDT_RETRY:
		KillTimer(nIDEvent);
		if (m_bStart)
			Start();
		break;
	}

	CBaseDlg::OnTimer(nIDEvent);
}

void CImpulseDlg::OnSelchangeSamplingRate()
{
	int nIndex;

	if ((nIndex = m_cSamplingRate.GetCurSel()) != CB_ERR) {
		g_oSetData.Imp.nSamplingRate = (int)m_cSamplingRate.GetItemData(nIndex);
		SetTimeList();
		InitDispTime();
		DispImpWindow();
	}
}

void CImpulseDlg::OnSelchangeMeasureTime()
{
	g_oSetData.Imp.nTime = m_cMeasureTime.GetCurSel();
	InitDispTime();
	DispImpWindow();
}

void CImpulseDlg::SetTimeList()
{
	CString strBuf;

	m_cMeasureTime.ResetContent();
	const double sps = g_oSetData.Imp.nSamplingRate;
	for (int i = 0; i < STAGE_NUM; i++) {
		strBuf.Format("%.3f s", (1 << (i + STAGE_OFFSET)) / sps);
		m_cMeasureTime.AddString(strBuf);
	}
	m_cMeasureTime.SetCurSel(g_oSetData.Imp.nTime);
}

void CImpulseDlg::OnChannelMono()
{
	g_oSetData.Imp.nChannel = CH_MONO;
	EnableImpWindow();
	DispImpWindow();
}

void CImpulseDlg::OnChannelStereo()
{
	g_oSetData.Imp.nChannel = CH_STEREO;
	EnableImpWindow();
	DispImpWindow();
}

void CImpulseDlg::EnableImpWindow()
{
	if (g_oSetData.Imp.nChannel == CH_MONO) {
		m_cImpulseL.EnableWindow(TRUE);
		m_cImpulseR.EnableWindow(FALSE);
	} else {
		m_cImpulseL.EnableWindow(TRUE);
		m_cImpulseR.EnableWindow(TRUE);
	}
}

void CImpulseDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/)
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

	DispImpWindow();
}

void CImpulseDlg::OnZoomInH()
{
	m_fDispTime /= 2;
	DispImpWindow();
}

void CImpulseDlg::OnZoomOutH()
{
	m_fDispTime *= 2;
	DispImpWindow();
}

void CImpulseDlg::OnZoomFullH()
{
	m_fDispTime = m_fTotalTime;
	DispImpWindow();
}

void CImpulseDlg::InitDispTime()
{
//	m_fTotalTime = double(1 << tMSeqParam[g_oSetData.Imp.nTime].nBit) / g_tSamplingRate[g_oSetData.Imp.nSamplingRate];
	m_fDispTime = m_fTotalTime * g_oSetData.Imp.nScrollSize / SCROLL_RANGE;
	m_fStartTime = m_fTotalTime * g_oSetData.Imp.nScrollPos / SCROLL_RANGE;
}

void CImpulseDlg::DispImpWindow()
{
	if (m_fDispTime == 0)
		m_fDispTime = m_fTotalTime;
	else if (m_fDispTime < m_fTotalTime / MAX_ZOOM_TIME)
		m_fDispTime = m_fTotalTime / MAX_ZOOM_TIME;
	else if (m_fDispTime > m_fTotalTime)
		m_fDispTime = m_fTotalTime;

	if (m_fStartTime < 0)
		m_fStartTime = 0;
	else if (m_fStartTime > m_fTotalTime - m_fDispTime)
		m_fStartTime = m_fTotalTime - m_fDispTime;

	g_oSetData.Imp.nScrollSize = (int)(m_fDispTime / m_fTotalTime * SCROLL_RANGE);
	g_oSetData.Imp.nScrollPos = (int)(m_fStartTime / m_fTotalTime * SCROLL_RANGE);

	if (g_oSetData.Imp.nScrollSize != m_nScrollSize || g_oSetData.Imp.nScrollPos != m_nScrollPos) {
		m_nScrollSize = g_oSetData.Imp.nScrollSize;
		m_nScrollPos = g_oSetData.Imp.nScrollPos;

		SCROLLINFO si{
			.cbSize = sizeof(SCROLLINFO),
			.fMask = SIF_ALL,
			.nMin = 0,
			.nMax = SCROLL_RANGE,
			.nPage = (UINT)g_oSetData.Imp.nScrollSize,
			.nPos = g_oSetData.Imp.nScrollPos
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

	BOOL bScroll = m_fDispTime < m_fTotalTime;
	m_cImpulseL.DispImpulse(m_fTotalTime, m_fStartTime, m_fDispTime, m_pLeftImpulseData, m_nData, bScroll);
	m_cImpulseR.DispImpulse(m_fTotalTime, m_fStartTime, m_fDispTime, m_pRightImpulseData, m_nData, bScroll);

	m_cZoomH = int(m_fTotalTime / m_fDispTime);
}

void CImpulseDlg::StartStopButton(BOOL bStart)
{
	if (bStart) {
		m_cBtnStart.EnableWindow(FALSE);
		m_cBtnStop.EnableWindow(TRUE);
		if (GetActiveWindow()->GetSafeHwnd() == GetSafeHwnd())
			m_cBtnStop.SetFocus();
		EnableControl(FALSE);
	} else {
		m_cBtnStart.EnableWindow(TRUE);
		m_cBtnStop.EnableWindow(FALSE);
		if (GetActiveWindow()->GetSafeHwnd() == GetSafeHwnd())
			m_cBtnStart.SetFocus();
		EnableControl(TRUE);
	}

	m_bStart = bStart;
}

void CImpulseDlg::OnFileSave()
{
	if (!m_bValidData)
		return;

	if (!CHECK_LICENSE1(APP_RA))
		return;

	HWAVEDATA hWaveData = MakeWaveDataHandle(m_nRate, m_nBitsPerSample, m_nChannel, m_nData, m_pLeftInputData, m_pRightInputData);

	CSaveIR dlg(this);

	dlg.m_Time = m_sTime;
	dlg.m_hWaveData = hWaveData;
	dlg.m_iSampling = m_nRate;
	dlg.m_nStage = m_nStage;
	dlg.m_iMeasureNum = m_nMeasureNum;
	dlg.m_nChannel = m_nChannel;
	dlg.m_iBit = m_nBitsPerSample;
	dlg.m_bUseFilter = (m_pIFilter != NULL) ? TRUE : FALSE;
	dlg.m_nIFilterID = m_nIFilterID;
	dlg.m_nMultiplier = m_nMultiplier;
	dlg.m_bImpulseData = m_bImpulseData;
	dlg.m_nMethod = m_nMethod;

	dlg.DoModal();

	::GlobalFree(hWaveData);
}

void CImpulseDlg::OnFileLoad()
{
	if (!CHECK_LICENSE1(APP_RA))
		return;

	CLoadIR dlg(this);

	switch (dlg.DoModal()) {
	case IDOK:
		LoadImpulseData(dlg.m_nImpulseID);
		break;
	case IDC_WAVE_FILE:
		LoadWaveFile(dlg.m_sWaveFile);
		break;
	}
}

void CImpulseDlg::LoadImpulseData(long nImpulseID)
{
	CDbImpulse dbImpulse;
	if (!dbImpulse.Open())
		return;

	DbImpulseRec dbImpulseRec;
	dbImpulse.DBChgIdx(dbImpulse.m_nIdxImpulseID);
	if (!dbImpulse.ReadRecID(nImpulseID, &dbImpulseRec))
		return;

	HWAVEDATA hWaveData;
	if (MMWaveReadDB(&hWaveData, dbImpulse.GetFCB(), "WAVEDATA") != 0) {
		::AfxMessageBox(IDS_ERR_READDATA, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	m_bImpulseData = dbImpulseRec.bImpulseData;

	const int nBit = dbImpulseRec.nBit & 0xfc;
	const int nMethod = dbImpulseRec.nBit & 0x03;

	PWAVEDATA pWaveData = (PWAVEDATA)::GlobalLock(hWaveData);
	const int nData = pWaveData->dataSize / pWaveData->nChannels / (pWaveData->nBitsPerSample / 8);
	GlobalUnlock(hWaveData);

	SetMeasuringParameters(dbImpulseRec.nSampling, dbImpulseRec.nMeasTime, dbImpulseRec.nMeasNum, dbImpulseRec.nChannel, nBit, dbImpulseRec.sTime, nMethod, nData);

	ReadWaveData(hWaveData, m_pLeftInputData, m_pRightInputData, m_nData);

	::GlobalFree(hWaveData);

	CIRMethod *pMethod = NULL;
	if (nMethod == METHOD_MSEQ)
		pMethod = &m_oMSeq;
	else if (nMethod == METHOD_TSP)
		pMethod = &m_oTSP;

	memcpy(m_pLeftImpulseData, m_pLeftInputData, m_nData * sizeof(double));
	if (!m_bImpulseData)
		pMethod->CalcImpulse(m_pLeftImpulseData);

	if (m_nChannel == 2) {
		memcpy(m_pRightImpulseData, m_pRightInputData, m_nData * sizeof(double));
		if (!m_bImpulseData)
			pMethod->CalcImpulse(m_pRightImpulseData);
	}

	m_fTotalTime = double(m_nData) / m_nRate;
	DispImpWindow();

	DispMeasureParam();

	ValidData(TRUE);
}

void CImpulseDlg::LoadWaveFile(LPCSTR pFileName)
{
	CFileStatus st;
	if (!CFile::GetStatus(pFileName, st))
		return;

	CString fileTime;
	fileTime.Format("%04d%02d%02d%02d%02d%02d", st.m_mtime.GetYear(), st.m_mtime.GetMonth(), st.m_mtime.GetDay(),
					st.m_mtime.GetHour(), st.m_mtime.GetMinute(), st.m_mtime.GetSecond());

	HWAVEDATA hWaveData;
	if (MMWaveReadFile(&hWaveData, (LPSTR)pFileName) != 0) {
		::AfxMessageBox(IDS_ERR_READWAVEFILE, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	m_bImpulseData = TRUE;

	PWAVEDATA pWaveData = (PWAVEDATA)::GlobalLock(hWaveData);
	m_nBitsPerSample = pWaveData->nBitsPerSample;
	const int nData = pWaveData->dataSize / pWaveData->nChannels / (pWaveData->nBitsPerSample / 8);
	int nStage = 0;
	while (true) {
		if ((nData >> nStage) == 0)
			break;
		nStage++;
	}
	::GlobalUnlock(hWaveData);

	SetMeasuringParameters(pWaveData->nSamplesPerSec, nStage, 1, pWaveData->nChannels, pWaveData->nBitsPerSample, fileTime, g_oSetData.Imp.nMethod, nData);

	ReadWaveData(hWaveData, m_pLeftInputData, m_pRightInputData, m_nData);

	::GlobalFree(hWaveData);

	memcpy(m_pLeftImpulseData, m_pLeftInputData, m_nData * sizeof(double));
	if (m_nChannel == 2)
		memcpy(m_pRightImpulseData, m_pRightInputData, m_nData * sizeof(double));

	m_fTotalTime = double(m_nData) / m_nRate;
	DispImpWindow();

	DispMeasureParam();

	ValidData(TRUE);
}

void CImpulseDlg::OnDestroy()
{
	CBaseDlg::OnDestroy();

	if (m_bStart)
		OnStop();

	FreeBuffers();

	if (m_pIFilter != NULL) {
		delete [] m_pIFilter;
		m_pIFilter = NULL;
	}

	CWnd *pWnd = CWnd::FindWindowEx(HWND_DESKTOP, NULL, NULL, SA_IR_TITLE);
	while (pWnd != NULL) {
		pWnd->PostMessage(WM_CLOSE);
		pWnd = CWnd::FindWindowEx(HWND_DESKTOP, pWnd->m_hWnd, NULL, SA_IR_TITLE);
	}
}

void CImpulseDlg::DispMeasureParam()
{
	m_cMrTime = ::FormatTime(m_sTime);
	m_cMrSampling = m_nRate;
	m_cMrMTime.Format("%.3f", m_nData / (double)m_nRate);
	m_cMrMNum = m_nMeasureNum;
	m_cMrBit= m_nBitsPerSample;
	m_cMrChannel = (m_nChannel == 1) ? "Mono" : "Stereo";
}

void CImpulseDlg::ValidData(BOOL bValid)
{
	m_bValidData = bValid;

	m_cFileSave.EnableWindow(bValid);
	m_cWaveOut.EnableWindow(bValid);
//	m_cAdjust.EnableWindow(bValid);
	m_cIfaSave.EnableWindow(bValid);
	m_cPlayback.EnableWindow(bValid);
	m_cLaunchSA.EnableWindow(bValid);
}

void CImpulseDlg::OnWaveOut()
{
	if (!CHECK_LICENSE1(APP_RA))
		return;

	if (g_oSetData.Imp.nFileFormat == 0)
		WaveFileOut();
	else
		TextFileOut();
}

void CImpulseDlg::WaveFileOut()
{
	if (!m_bValidData)
		return;

	CFileDialog fileDlg(FALSE, "wav", "",
			OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
			"16bit Wave File (*.wav)|*.wav|24bit Wave File (*.wav)|*.wav||", this, 0);

	if (fileDlg.DoModal() == IDCANCEL)
		return;

	const int nBitsPerSample = fileDlg.m_pOFN->nFilterIndex == 1 ? 16 : 24;

	HWAVEDATA hWaveData;

	switch (g_oSetData.Imp.nFileType) {
	case 0:		// インパルス応答
		hWaveData = MakeWaveDataHandle(m_nRate, nBitsPerSample, m_nChannel, m_nData, m_pLeftImpulseData, m_pRightImpulseData, TRUE);
		break;
	case 1:		// 出力信号
		hWaveData = MakeWaveDataHandle(m_nRate, nBitsPerSample, 1, m_nData, m_pWaveOutData, NULL);
		break;
	case 2:		// 入力信号
		hWaveData = MakeWaveDataHandle(m_nRate, nBitsPerSample, m_nChannel, m_nData, m_pLeftInputData, m_pRightInputData);
		break;
	default:
		return;
	}

	if (MMWaveWriteFile(hWaveData, (LPSTR)(LPCSTR)fileDlg.GetPathName()) != 0)
		::AfxMessageBox(IDS_ERR_WRITEWAVEFILE, MB_OK | MB_ICONEXCLAMATION);

	::GlobalFree(hWaveData);
}

void CImpulseDlg::TextFileOut()
{
	if (!m_bValidData)
		return;

	CFileDialog fileDlg(FALSE, "txt", NULL,
			OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
			"Text File (*.txt; *.csv)|*.txt;*.csv|CSV File (*.csv)|*.csv|All Files (*.*)|*.*||", this, 0);

	if (fileDlg.DoModal() == IDCANCEL)
		return;

	double fMul;
	double *pLeftData = NULL, *pRightData = NULL;

	switch (g_oSetData.Imp.nFileType) {
	case 0:		// インパルス応答
		pLeftData = m_pLeftImpulseData;
		pRightData = m_pRightImpulseData;
		fMul = 1 / GetMaxData(pLeftData, pRightData, m_nData);
		break;
	case 1:		// 出力信号
		pLeftData = m_pWaveOutData;
		fMul = 1.0;
		break;
	case 2:		// 入力信号
		pLeftData = m_pLeftInputData;
		pRightData = m_pRightInputData;
		fMul = 1.0;
		break;
	default:
		return;
	}

	FILE *fp;
	if (fopen_s(&fp, fileDlg.GetPathName(), "w") == 0) {
		fprintf(fp, "Sampling Rate,%d\n", m_nRate);
		fprintf(fp, "Number of Channels,%d\n", pRightData != NULL ? 2 : 1);
		fprintf(fp, "Number of Samples,%d\n\n", m_nData);
		for (int i = 0; i < m_nData; i++) {
			fprintf(fp, "%g", pLeftData[i] * fMul);
			if (pRightData != NULL)
				fprintf(fp, ",%g", pRightData[i] * fMul);
			fprintf(fp, "\n");
		}
		fclose(fp);
	} else
		::AfxMessageBox(IDS_ERR_OPENOUTPUTFILE);
}

void CImpulseDlg::InverseConvolution(double *pWaveData, int nWaveData, const double *pImpulseData, int nImpulseData)
{
	int i, j;
	double t;
	double xt1, yt1, xt2, yt2;
	const int nData = min(nWaveData, nImpulseData);
	const int nLoop = nWaveData / 2;

	double *pFilterData = new double[nWaveData];
	memcpy(pFilterData, pImpulseData, nData * sizeof(double));
//#	for (i = 0; i < nData; i++)
//#		pFilterData[i] = pImpulseData[i];
	memset(&pFilterData[nData], 0, (nWaveData - nData) * sizeof(double));
//#	for ( ; i < nWaveData; i++)
//#		pFilterData[i] = 0;

	m_oRFFT.fft(nWaveData, pWaveData);
	m_oRFFT.fft(nWaveData, pFilterData);

	double dmax = 0;
	for (i = 1; i < nLoop; i++) {
		j = i * 2;

		xt2 = pFilterData[j];
		yt2 = pFilterData[j + 1];

		t = sqrt(xt2 * xt2 + yt2 * yt2);
		if (t > dmax)
			dmax = t;
	}

	const double limit = dmax / pow(10.0, (double)m_nMaxAdjLevel / 20);
	pWaveData[0] = pWaveData[1] = 0;
	for (i = 1; i < nLoop; i++) {
		j = i * 2;

		xt1 = pWaveData[j];
		yt1 = pWaveData[j + 1];
		xt2 = pFilterData[j];
		yt2 = pFilterData[j + 1];

		if (m_bPhaseAdj) {
			t = xt2 * xt2 + yt2 * yt2;
			if (sqrt(t) < limit)
				t = sqrt(t) * limit;
			pWaveData[j] = (xt1 * xt2 + yt1 * yt2) / t;
			pWaveData[j + 1] = (xt2 * yt1 - xt1 * yt2) / t;

		} else {
			t = sqrt(xt2 * xt2 + yt2 * yt2);
			if (t < limit)
				t = limit;
			pWaveData[j] = xt1 / t;
			pWaveData[j + 1] = yt1 / t;
		}
	}

	delete [] pFilterData;

	m_oRFFT.ifft(nWaveData, pWaveData);

	double fmax = 0;
	for (i = 0; i < nWaveData; i++) {
		if (fabs(pWaveData[i]) > fmax)
			fmax = fabs(pWaveData[i]);
	}

	for (i = 0; i < nWaveData; i++)
		pWaveData[i] /= fmax;
}

void CImpulseDlg::OnIfaSet()
{
	CIfaSet IfaSetDlg(this);

	IfaSetDlg.m_nIFilterID = m_nIFilterID;
	if (IfaSetDlg.DoModal() == IDOK) {
		if (m_pIFilter != NULL)
			delete [] m_pIFilter;

		m_nIFilterID = IfaSetDlg.m_nIFilterID;
		m_nIFilter = IfaSetDlg.m_nEndPos - IfaSetDlg.m_nStartPos;
		m_nMaxAdjLevel = IfaSetDlg.m_nMaxAdjLevel;
		m_bPhaseAdj = IfaSetDlg.m_bPhaseAdj;
		m_nIfaSampling = IfaSetDlg.m_iSampling;
		m_pIFilter  = new double[m_nIFilter];

		memcpy(m_pIFilter, &IfaSetDlg.m_pIRData[IfaSetDlg.m_nStartPos], m_nIFilter * sizeof(double));
//#		for (int i = 0; i < m_nIFilter; i++)
//#			m_pIFilter[i] = IfaSetDlg.m_pIRData[IfaSetDlg.m_nStartPos + i];

		m_cIfaMessage = IfaSetDlg.m_sTitle;
		m_cIfaCancel.EnableWindow(TRUE);
	}
}

void CImpulseDlg::OnIfaCancel()
{
	if (m_pIFilter != NULL) {
		delete [] m_pIFilter;
		m_pIFilter = NULL;

		m_cIfaMessage.LoadString(IDS_NONE_HOSEI);
		m_cIfaCancel.EnableWindow(FALSE);
	}
}

void CImpulseDlg::OnIfaSave()
{
	if (!m_bValidData)
		return;

	CIfaSave IfaSaveDlg(this);
	IfaSaveDlg.m_pIRData = m_pLeftImpulseData;
	IfaSaveDlg.m_iSampling = m_nRate;
	IfaSaveDlg.m_nStage = m_nStage;
	IfaSaveDlg.DoModal();
}

void CImpulseDlg::OnMove(int x, int y)
{
	CBaseDlg::OnMove(x, y);

	if (m_bInitialized) {
		CRect rect;
		GetWindowRect(rect);
		g_oSetData.Imp.nPosLeft = rect.left;
		g_oSetData.Imp.nPosTop = rect.top;
	}
}

void CImpulseDlg::OnChangeMeasureNum()
{
	if (m_cMeasureNum.m_hWnd != NULL)
		g_oSetData.Imp.nMeasureNum = m_cMeasureNum;
}

void CImpulseDlg::OnFileText()
{
	UpdateData(TRUE);
	g_oSetData.Imp.nFileFormat = m_iFileFormat;
}

void CImpulseDlg::OnFileWave()
{
	UpdateData(TRUE);
	g_oSetData.Imp.nFileFormat = m_iFileFormat;
}

void CImpulseDlg::OnWaveIr()
{
	UpdateData(TRUE);
	g_oSetData.Imp.nFileType = m_iWaveOut;
}

void CImpulseDlg::OnWaveInMseq()
{
	UpdateData(TRUE);
	g_oSetData.Imp.nFileType = m_iWaveOut;
}

void CImpulseDlg::OnWaveOutMsrq()
{
	UpdateData(TRUE);
	g_oSetData.Imp.nFileType = m_iWaveOut;
}

void CALLBACK CImpulseDlg::GraphCallBack(WPARAM code, LPARAM data, double param1, double /*param2*/)
{
	CImpulseDlg *pWnd = (CImpulseDlg *)data;

	if (code == CC_SCROLL) {
		pWnd->m_fStartTime -= param1 * pWnd->m_fDispTime;
		pWnd->DispImpWindow();
	}
}

void CImpulseDlg::OnAssist()
{
	CIRAssistDlg dlg(this);

	m_pAssistDlg = &dlg;

	BOOL bAutoRetrySave = g_oSetData.Imp.bAutoRetry;
	g_oSetData.Imp.bAutoRetry = FALSE;

	if (dlg.DoModal() != -1) {
		if (((CRtaApp *)::AfxGetApp())->m_pWndRta->m_pWndImp != NULL) {
			SetTimeList();
			m_pAssistDlg = NULL;
		}
	}

	g_oSetData.Imp.bAutoRetry = bAutoRetrySave;
}

void CImpulseDlg::OnAutoLevel()
{
	g_oSetData.Imp.bAutoLevel = m_cAutoLevel;
}

void CImpulseDlg::OnAutoRetry()
{
	g_oSetData.Imp.bAutoRetry = m_cAutoRetry;
}

void CImpulseDlg::OnPlayback()
{
	m_cPlayback.Play(m_pLeftImpulseData, m_pRightImpulseData, m_nData, m_nRate);
}

void CImpulseDlg::EnableControl(BOOL bEnable)
{
	m_cFileLoad.EnableWindow(bEnable);
	m_cSamplingRate.EnableWindow(bEnable);
	m_cAssist.EnableWindow(bEnable);
	m_cMSeqMethod.EnableWindow(bEnable);
	m_cTSPMethod.EnableWindow(bEnable);
	m_cAutoLevel.EnableWindow(bEnable);
	m_cAutoRetry.EnableWindow(bEnable);
	m_cMeasureTime.EnableWindow(bEnable);
	m_cMeasureNum.EnableWindow(bEnable);
	m_cChannelMono.EnableWindow(bEnable);
	m_cChannelStereo.EnableWindow(bEnable);
	m_cIfaSet.EnableWindow(bEnable);
	m_cGraphScroll.EnableWindow(bEnable);
	m_cExit.EnableWindow(bEnable);
	m_cEditOffsetTime.EnableWindow(bEnable);
}

void CImpulseDlg::OnMSeqMethod()
{
	g_oSetData.Imp.nMethod = METHOD_MSEQ;
}

void CImpulseDlg::OnTspMethod()
{
	g_oSetData.Imp.nMethod = METHOD_TSP;
}

void CImpulseDlg::ChangeWaveDevice(int /*nInOut*/)
{
	if (m_bStart)
		OnStop();

	SetSamplingRateList(m_cSamplingRate, WAVE_IN | WAVE_OUT, g_nWaveInDevice, g_nWaveOutDevice, g_oSetData.Imp.nSamplingRate);
	OnSelchangeSamplingRate();
}

BOOL CImpulseDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDC_IMPALSE_L, IDH_IMP_IMPALSE_L,
		IDC_IMPALSE_R, IDH_IMP_IMPALSE_R,
		IDC_ZOOM_H, IDH_IMP_ZOOM_H,
		IDC_SCROLL_H, IDH_IMP_SCROLL_H,
		IDC_MR_TIME, IDH_IMP_MR_TIME,
		IDC_MR_SAMPLING, IDH_IMP_MR_SAMPLING,
		IDC_MR_MTIME, IDH_IMP_MR_MTIME,
		IDC_MR_MNUM, IDH_IMP_MR_MNUM,
		IDC_MR_BIT, IDH_IMP_MR_BIT,
		IDC_MR_CHANNEL, IDH_IMP_MR_CHANNEL,
		IDC_BTN_START, IDH_IMP_START,
		IDC_BTN_STOP, IDH_IMP_STOP,
		IDC_FILE_SAVE, IDH_IMP_FILE_SAVE,
		IDC_FILE_LOAD, IDH_IMP_FILE_LOAD,
		IDC_SAMPLING_RATE, IDH_IMP_SAMPLING_RATE,
		IDC_ASSIST, IDH_IMP_ASSIST,
		IDC_MEASURE_TIME, IDH_IMP_MEASURE_TIME,
		IDC_MEASURE_NUM, IDH_IMP_MEASURE_NUM,
		IDC_CHANNEL_MONO, IDH_IMP_CHANNEL_MONO,
		IDC_CHANNEL_STEREO, IDH_IMP_CHANNEL_STEREO,
		IDC_MSEQ_METHOD, IDH_IMP_MSEQ_METHOD,
		IDC_TSP_METHOD, IDH_IMP_TSP_METHOD,
		IDC_AUTO_LEVEL, IDH_IMP_AUTO_LEVEL,
		IDC_AUTO_RETRY, IDH_IMP_AUTO_RETRY,
		IDC_IFA_SET, IDH_IMP_IFA_SET,
		IDC_IFA_CANCEL, IDH_IMP_IFA_CANCEL,
		IDC_IFA_SAVE, IDH_IMP_IFA_SAVE,
		IDC_IFA_MESSAGE, IDH_IMP_IFA_MESSAGE,
		IDC_WAVE_IR, IDH_IMP_WAVE_IR,
		IDC_WAVE_OUT_MSRQ, IDH_IMP_WAVE_OUT_MSRQ,
		IDC_WAVE_IN_MSEQ, IDH_IMP_WAVE_IN_MSEQ,
		IDC_FILE_WAVE, IDH_IMP_FILE_WAVE,
		IDC_FILE_TEXT, IDH_IMP_FILE_TEXT,
		IDC_WAVE_OUT, IDH_IMP_WAVE_OUT,
		IDC_MESSAGE, IDH_IMP_MESSAGE,
		IDC_PLAYBACK, IDH_IMP_PLAYBACK,
		IDCANCEL, IDH_IMP_CLOSE,
		ID_ZOOM_H_FULL, IDH_IMP_ZOOM_H_FULL,
		ID_ZOOM_H_IN, IDH_IMP_ZOOM_H_IN,
		ID_ZOOM_H_OUT, IDH_IMP_ZOOM_H_OUT,
		IDC_LAUNCH_SA, IDH_IMP_LAUNCH_SA,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

void CImpulseDlg::OnSize(UINT nType, int cx, int cy)
{
	if (m_cImpulseL.m_hWnd)
		m_cImpulseL.ModifyStyle(WS_CLIPCHILDREN, 0);
	if (m_cImpulseR.m_hWnd)
		m_cImpulseR.ModifyStyle(WS_CLIPCHILDREN, 0);

	CBaseDlg::OnSize(nType, cx, cy);

	if (m_cImpulseL.m_hWnd)
		m_cImpulseL.ModifyStyle(0, WS_CLIPCHILDREN);
	if (m_cImpulseR.m_hWnd)
		m_cImpulseR.ModifyStyle(0, WS_CLIPCHILDREN);

	if (m_bInitialized) {
		m_cImpulseL.Resize();
		m_cImpulseR.Resize();
		DispImpWindow();
	}
}

void CImpulseDlg::OnBnClickedLaunchSa()
{
	HWAVEDATA hWaveData = MakeWaveDataHandle(m_nRate, m_nBitsPerSample, m_nChannel, m_nData, m_pLeftInputData, m_pRightInputData);
	if (hWaveData != NULL) {
		if (SaveImpulse(hWaveData)) {
			STARTUPINFO si;
			PROCESS_INFORMATION pi;
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			CString sCommand = "SAS /IMP";
			::CreateProcess(NULL, sCommand.GetBuffer(), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
			::CloseHandle(pi.hProcess);
			::CloseHandle(pi.hThread);
		}
	}

	::GlobalFree(hWaveData);
}

BOOL CImpulseDlg::SaveImpulse(HWAVEDATA hWaveData)
{
	CDbImpulse dbImpulse;
	if (!dbImpulse.Open())
		return FALSE;

	CString sTitle;
	sTitle.LoadString(IDS_MEASUREMENT_DATA);

	DbImpulseRec dbImpulseRec;
	dbImpulseRec.nImpulseID = -2;
	dbImpulseRec.nFolderID = -2;
	dbImpulseRec.nSampling = m_nRate;
	dbImpulseRec.nMeasTime = m_nStage;
	dbImpulseRec.nMeasNum = m_nMeasureNum;
	dbImpulseRec.nChannel = m_nChannel;
	dbImpulseRec.nBit = m_nBitsPerSample | m_nMethod;
	dbImpulseRec.sTime = m_sTime;
	dbImpulseRec.sTitle= sTitle;
	dbImpulseRec.sComment.Empty();
	dbImpulseRec.nUseFilter = (m_pIFilter != NULL) ? TRUE : FALSE;
	dbImpulseRec.nIFilterID = m_nIFilterID;
	dbImpulseRec.fTemperature = 0.0;
	dbImpulseRec.fHumidity = 0.0;
	dbImpulseRec.nMultiplier = m_nMultiplier;
	dbImpulseRec.bImpulseData = m_bImpulseData;

	dbImpulse.DBChgIdx(dbImpulse.m_nIdxImpulseID);
	dbImpulse.DBDeleteKey("-2", 2, 1);

	if (!dbImpulse.StoreRec(&dbImpulseRec))
		return FALSE;

	if (MMWaveWriteDB(hWaveData, dbImpulse.GetFCB(), "WAVEDATA", MMTYPE_WAV) != 0)
		return FALSE;

	return TRUE;
}

void CImpulseDlg::OnEnChangeEditOffsetTime()
{
	g_oSetData.Imp.fOffsetTime = m_cEditOffsetTime;
}
