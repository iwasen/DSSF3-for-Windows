#pragma once

#include "SetData.h"
#include <mmsystem.h>
#include "WaveFile.h"

struct AutoRecInfo {
	CTime oStartTime;
	int nRecTime;
	int nSampleRate;
	int nChannels;
	int nWindow;
	BOOL bOut;
};

class CAutoRec
{
public:
	CAutoRec(void);
	~CAutoRec(void);

	void StartRecord(int nSampleRate, int nChannels, int nWindow, BOOL bOut);
	void StopRecord();
	void WriteData(double *pData, int nData);

private:
	AutoRecInfo m_oAutoRecInfo;
	BOOL m_bRecording;
	CString m_sFileName;
	CWaveFile m_oWaveFile;
	CSetData m_oSetData;

	void WriteInfoData(CFile &file);
	void WriteSetData(CFile &file, void *pData, int nSize);
	CString GetWindowName();
};
