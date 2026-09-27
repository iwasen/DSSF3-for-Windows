#include "stdafx.h"
#include "RTA.h"
#include "WaveOutEx.h"
#include "AsioManager.h"
#include "WasapiManager.h"

CWaveOutEx *CWaveOutEx::s_pWaveOutEx;
IWaveNotify *CWaveOutEx::s_pRecWnd;

CWaveOutEx::CWaveOutEx()
{
	m_pWaveNotify = NULL;
	m_pMonitorWnd = NULL;
	m_pRecWnd = NULL;
}

void CWaveOutEx::Init()
{
	s_pWaveOutEx = new CWaveOutEx;
}

void CWaveOutEx::Exit()
{
	delete s_pWaveOutEx;
}

CWaveOutEx *CWaveOutEx::GetInstance()
{
	return s_pWaveOutEx;
}

void CWaveOutEx::ReleaseInstance()
{
}

void CWaveOutEx::SetMonitor(IWaveNotify *pMonitorWnd)
{
	s_pWaveOutEx->m_pMonitorWnd = pMonitorWnd;
}

CWaveOutEx *CWaveOutEx::SetRecMode(IWaveNotify *pRecWnd)
{
	s_pRecWnd = pRecWnd;

	if (s_pWaveOutEx->SetRecMode2(pRecWnd))
		return s_pWaveOutEx;

	return NULL;
}

BOOL CWaveOutEx::SetRecMode2(IWaveNotify *pWnd)
{
	m_pRecWnd = pWnd;

	if (m_nWaveDevice >= 0)
		return m_oWaveOut.m_hWave != NULL;
	else if (m_nWaveDevice <= -100)
		return g_cWasapiManager.IsOpenOut();
	else
		return g_cAsioManager.m_bOutOpen;
}

CWaveOutEx *CWaveOutEx::GetRecWave()
{
	return s_pWaveOutEx;
}

void CWaveOutEx::CloseAll()
{
	s_pWaveOutEx->Close();
}

BOOL CWaveOutEx::Open(INT_PTR nWaveDevice, IWaveNotify *pWaveNotify, int nChannels, int nSamplesPerSec, int nSamplesPerBuffer, int nBufferNum, BOOL bErrMsg, BOOL bVolumeSet)
{
	m_nWaveDevice = nWaveDevice;
	m_pWaveNotify = pWaveNotify;
	m_nChannels = nChannels;
	m_nSamplesPerSec = nSamplesPerSec;

	if (m_nWaveDevice >= 0)
		return m_oWaveOut.Open(nWaveDevice, this, nChannels, nSamplesPerSec, nSamplesPerBuffer, nBufferNum, bErrMsg, bVolumeSet);
	else if (m_nWaveDevice <= -100)
		return g_cWasapiManager.OpenOut(-(nWaveDevice + 100), this, nChannels, nSamplesPerSec, nSamplesPerBuffer, bErrMsg);
	else
		return g_cAsioManager.OpenOut(-(nWaveDevice + 1), this, nChannels, nSamplesPerSec, nSamplesPerBuffer, bErrMsg);
}

void CWaveOutEx::Start()
{
	if (m_nWaveDevice >= 0)
		return m_oWaveOut.Start();
	else if (m_nWaveDevice <= -100)
		return g_cWasapiManager.StartOut();
	else
		return g_cAsioManager.StartOut();
}

void CWaveOutEx::Close()
{
	if (m_nWaveDevice >= 0)
		return m_oWaveOut.Close();
	else if (m_nWaveDevice <= -100)
		return g_cWasapiManager.CloseOut();
	else
		return g_cAsioManager.CloseOut();
}

void CWaveOutEx::Reset()
{
	if (m_nWaveDevice >= 0)
		return m_oWaveOut.Reset();
	else if (m_nWaveDevice <= -100)
		return g_cWasapiManager.ResetOut();
	else
		return g_cAsioManager.ResetOut();
}

void CWaveOutEx::Pause()
{
	if (m_nWaveDevice >= 0)
		return m_oWaveOut.Pause();
	else if (m_nWaveDevice <= -100)
		return g_cWasapiManager.PauseOut();
	else
		return g_cAsioManager.PauseOut();
}

void CWaveOutEx::Restart()
{
	if (m_nWaveDevice >= 0)
		return m_oWaveOut.Restart();
	else if (m_nWaveDevice <= -100)
		return g_cWasapiManager.RestartOut();
	else
		return g_cAsioManager.RestartOut();
}

void CWaveOutEx::SetVolume(UINT left, UINT right)
{
	if (m_nWaveDevice >= 0)
		m_oWaveOut.SetVolume(left, right);
}

int CWaveOutEx::GetSamplesPerSec() const
{
	return m_nSamplesPerSec;
}

int CWaveOutEx::GetChannels() const
{
	return m_nChannels;
}

int CWaveOutEx::GetBitsPerSample()
{
	if (m_nWaveDevice >= 0)
		return m_oWaveOut.GetBitsPerSample();
	else if (m_nWaveDevice <= -100)
		return g_cWasapiManager.GetBitsPerSampleOut();
	else
		return g_cAsioManager.GetBitsPerSampleOut();
}

int CWaveOutEx::OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify)
{
	int nRet = 0;

	if (m_pWaveNotify != NULL)
		nRet = m_pWaveNotify->OnWaveNotify(nCode, pWaveNotify);

	if (g_oSetData.Flt.nInputOutput == 1)
		FilteringWave(pWaveNotify);

	if (m_pRecWnd != NULL)
		m_pRecWnd->OnWaveNotify(nCode, pWaveNotify);

	if (m_pMonitorWnd != NULL)
		m_pMonitorWnd->OnWaveNotify(nCode, pWaveNotify);

	switch (nCode) {
	case WAVEOUT_OPEN:
		if (pWaveNotify != NULL && g_oSetData2.bAutoRecOutput)
			m_oAutoRec.StartRecord(pWaveNotify->nSamplesPerSec, pWaveNotify->nChannels, pWaveNotify->nWindow, TRUE);
		break;
	case WAVEOUT_CLOSE:
		m_oAutoRec.StopRecord();
		break;
	case WAVEOUT_DATA:
		m_oAutoRec.WriteData(pWaveNotify->pSamplesData, nRet);
		break;
	}

	return nRet;
}
