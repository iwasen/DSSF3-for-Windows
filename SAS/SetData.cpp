#include "stdafx.h"
#include "Sas.h"
#include "SetData.h"
#include "Directory.h"

#define FILENAME "SAS.dat"

CSetData setData;

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
		MainWindow.width2 = 150;

		CalcParam.nStartFreq = 1;
		CalcParam.nEndFreq = 10;
		CalcParam.nSplRefData = SPL_MAXREF;
		CalcParam.fSplRefLevel = 0;
		CalcParam.fDT1MinTime = 10.0;
		CalcParam.fTsubEnd = -20.0;
		CalcParam.bTsubAuto = TRUE;
		CalcParam.fTsubNoise = 0.5;
		CalcParam.fWIACCLevel = 0.1;
		CalcParam.fPrefSPL = 0;
		CalcParam.fPrefTauE = 100.0;
		CalcParam.fTCustom1 = -5;
		CalcParam.fTCustom2 = -20;
		CalcParam.fCCustom = 40;

		AcfCondition.fIntegrationTime = 2.0;
		AcfCondition.fRunningStep = 0.1;
		AcfCondition.nFreqWeighting = 1;
		AcfCondition.fMaxDelayTime = 0.2;
		AcfCondition.nPhi0RefData = SPL_MAXREF;
		AcfCondition.fPhi0RefLevel = 0;
		AcfCondition.fTaueInterval = 5;
		AcfCondition.fTaueEndLevel = -5;
		AcfCondition.fTaueEndTime = 50;

		NmsCondition.AcfCondition.fIntegrationTime = 2.0;
		NmsCondition.AcfCondition.fRunningStep = 0.1;
		NmsCondition.AcfCondition.nFreqWeighting = 1;
		NmsCondition.AcfCondition.fMaxDelayTime = 0.2;
		NmsCondition.AcfCondition.nPhi0RefData = SPL_ABSOLUTE;
		NmsCondition.AcfCondition.fPhi0RefLevel = 0;
		NmsCondition.AcfCondition.fTaueInterval = 1;
		NmsCondition.AcfCondition.fTaueEndLevel = -10;
		NmsCondition.AcfCondition.fTaueEndTime = 50;
		NmsCondition.IAcfCondition.fIACCWLevel = 0.1;
		NmsCondition.IdentCondition.nBeforeStep = 1;
		NmsCondition.IdentCondition.nAfterStep = 1;

		ImpSC.nColumns = 2;
		ImpSC.nColSpace = 10;
		ImpSC.nRowSpace = 10;

		AcfSC.nColumns = 2;
		AcfSC.nColSpace = 10;
		AcfSC.nRowSpace = 10;

		NmsSC.nColumns = 2;
		NmsSC.nColSpace = 10;
		NmsSC.nRowSpace = 10;
	}
}

CSetData::~CSetData()
{
	CFile setDataFile;
	CString sDataDirectory;
	CString sPathName;

	GetDataDirectory(sDataDirectory);
	sPathName.Format("%s\\%s", (LPCSTR)sDataDirectory, FILENAME);

	if (setDataFile.Open(sPathName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
		setDataFile.Write((char *)this, sizeof(CSetData));
}
