// SetData.cpp: CSetData クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Nms.h"
#include "SetData.h"
#include "Directory.h"

#define FILENAME "NMS.dat"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CSetData::CSetData()
{
	CFile oSetDataFile;
	BOOL bRead = FALSE;
	CFileStatus st;
	CString sDataDirectory;
	CString sPathName;

	GetDataDirectory(sDataDirectory);
	sPathName.Format("%s\\%s", (LPCSTR)sDataDirectory, FILENAME);

	if (oSetDataFile.Open(sPathName, CFile::modeRead | CFile::shareDenyNone)) {
		oSetDataFile.GetStatus(st);
		if (st.m_size == sizeof(CSetData)) {
			oSetDataFile.Read(this, sizeof(CSetData));
			bRead = TRUE;
		}
	}

	if (!bRead) {
		Measurement.nSamplingRate = 48000;
		Measurement.fSamplingLevel = 20;
		Measurement.bRelativeLevel = TRUE;
		Measurement.fTimeConstant = 0.15;
		Measurement.nSamplingTiming = 0;
		Measurement.fSamplingTime1 = 2;
		Measurement.fSamplingTime2 = 2;
		Measurement.AcfCondition.fIntegrationTime = 2;
		Measurement.AcfCondition.fRunningStep = 0.1;
		Measurement.AcfCondition.nFreqWeighting = 1;
		Measurement.AcfCondition.fMaxDelayTime = 0.2;
		Measurement.AcfCondition.nPhi0RefData = SPL_ABSOLUTE;
		Measurement.AcfCondition.nPhi0RefData = 0;
		Measurement.AcfCondition.fTaueInterval = 1;
		Measurement.AcfCondition.fTaueEndLevel = -10;
		Measurement.AcfCondition.fTaueEndTime = 50;
		Measurement.IAcfCondition.fIACCWLevel = 0.1;
		Measurement.IdentCondition.nBeforeStep = 1;
		Measurement.IdentCondition.nAfterStep = 1;
		Measurement.nMicCalID = -1;

		CTime tCurrent = CTime::GetCurrentTime();
		CTime tDate(tCurrent.GetYear(), tCurrent.GetMonth(), tCurrent.GetDay(), 0, 0, 0);
		g_oSetData.Measurement.tStartDateTime = tDate;
		g_oSetData.Measurement.tEndDateTime = tDate;

		ManualWindow.fSamplingTime = 5;

		Calc.bAutoCalc = TRUE;
	} else {
		if (Measurement.nSamplingRate < 8000)
			Measurement.nSamplingRate = 44100;
	}
}

CSetData::~CSetData()
{
	CFile g_SetDataFile;
	CString sDataDirectory;
	CString sPathName;

	GetDataDirectory(sDataDirectory);
	sPathName.Format("%s\\%s", (LPCSTR)sDataDirectory, FILENAME);

	if (g_SetDataFile.Open(sPathName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
		g_SetDataFile.Write((char *)this, sizeof(CSetData));
}
