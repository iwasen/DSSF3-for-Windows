#include "StdAfx.h"
#include "AutoRec.h"
#include "Common.h"
#include "Global.h"

CAutoRec::CAutoRec(void)
{
	m_bRecording = FALSE;
}

CAutoRec::~CAutoRec(void)
{
	StopRecord();
}

void CAutoRec::StartRecord(int nSampleRate, int nChannels, int nWindow, BOOL bOut)
{
	if (nWindow == 0 || g_oSetData2.sAutoRecFolder.IsEmpty())
		return;

	if (m_bRecording)
		StopRecord();

	m_oSetData = g_oSetData;

	m_oAutoRecInfo.nSampleRate = nSampleRate;
	m_oAutoRecInfo.nChannels = nChannels;
	m_oAutoRecInfo.oStartTime = CTime::GetCurrentTime();
	m_oAutoRecInfo.nRecTime = 0;
	m_oAutoRecInfo.nWindow = nWindow;
	m_oAutoRecInfo.bOut = bOut;

	m_sFileName.Format("%s\\%s-%s-%s.wavdssf", (LPCSTR)g_oSetData2.sAutoRecFolder, (LPCSTR)m_oAutoRecInfo.oStartTime.Format("%Y%m%d-%H%M%S"), (LPCSTR)GetWindowName(), bOut ? "O" : "I");

	int nBitsPerSample = 16;
	int nBytesPerSample = (nBitsPerSample / 8) * nChannels;

	PCMWAVEFORMAT waveFormat{
		.wf{
			.wFormatTag = WAVE_FORMAT_PCM,
			.nChannels = (WORD)nChannels,
			.nSamplesPerSec = (DWORD)nSampleRate,
			.nAvgBytesPerSec = (DWORD)(nSampleRate * nBytesPerSample),
			.nBlockAlign = (WORD)nBytesPerSample
		},
		.wBitsPerSample = 16
	};
	if (m_oWaveFile.Create(m_sFileName, &waveFormat))
		m_bRecording = TRUE;
}

void CAutoRec::StopRecord()
{
	if (m_bRecording) {
		m_bRecording = FALSE;

		m_oWaveFile.Close();

		m_oAutoRecInfo.nRecTime = (int)(CTime::GetCurrentTime() - m_oAutoRecInfo.oStartTime).GetTotalSeconds();

		CFile file;
		if (file.Open(m_sFileName, CFile::modeWrite | CFile::shareDenyWrite)) {
			file.SeekToEnd();

			WriteInfoData(file);

			file.Close();
		}
	}
}

void CAutoRec::WriteData(double *pData, int nData)
{
	if (m_bRecording)
		m_oWaveFile.Write(pData, nData * m_oAutoRecInfo.nChannels);
}

void CAutoRec::WriteInfoData(CFile &file)
{
	int nSizeAutoRecInfo = sizeof(AutoRecInfo);
	file.Write(&nSizeAutoRecInfo, sizeof(int));
	file.Write(&m_oAutoRecInfo, sizeof(m_oAutoRecInfo));

	switch (m_oAutoRecInfo.nWindow) {
	case WINDOW_SG:
		WriteSetData(file, &g_oSetData.Sg, sizeof(g_oSetData.Sg));
		break;
	case WINDOW_FFT:
		WriteSetData(file, &g_oSetData.Fft, sizeof(g_oSetData.Fft));
		break;
	case WINDOW_OS:
		WriteSetData(file, &g_oSetData.Os, sizeof(g_oSetData.Os));
		break;
	case WINDOW_IMP:
		WriteSetData(file, &g_oSetData.Imp, sizeof(g_oSetData.Imp));
		break;
	case WINDOW_ACF:
		WriteSetData(file, &g_oSetData.Acf, sizeof(g_oSetData.Acf));
		break;
	case WINDOW_DST:
		WriteSetData(file, &g_oSetData.Dst, sizeof(g_oSetData.Dst));
		break;
	case WINDOW_FRE:
		WriteSetData(file, &g_oSetData.Fre, sizeof(g_oSetData.Fre));
		break;
	}
}

void CAutoRec::WriteSetData(CFile &file, void *pData, int nSize)
{
	file.Write(&nSize, sizeof(int));
	file.Write(pData, nSize);
}

CString CAutoRec::GetWindowName()
{
	switch (m_oAutoRecInfo.nWindow) {
	case WINDOW_SG:
		return "SG";
	case WINDOW_FFT:
		return "FFT";
	case WINDOW_OS:
		return "OSC";
	case WINDOW_IMP:
		return "IMP";
	case WINDOW_ACF:
		return "ACF";
	case WINDOW_DST:
		return "THD";
	case WINDOW_FRE:
		return "FRQ";
	}

	return "";
}
