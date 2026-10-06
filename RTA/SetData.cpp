#include "stdafx.h"
#include "Rta.h"
#include "Directory.h"

#define FILENAME	"RTA.dat"

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
		Init();
	} else {
		if (Sg.fWaveFreq == 0)
			Sg.fWaveFreq = 200;

		if (Sg.fSweepTime == 0)
			Sg.fSweepTime = 10;

		if (Sg.nDLevel > 100)
			Sg.nDLevel = 100;

		if (Fft.nTimeRange == 0)
			Fft.nTimeRange = 5;
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

void CSetData::Init()
{
	memset(this, 0, sizeof(CSetData));

	Sg.nSamplingRate = 48000;
	Sg.nLLevel = 20;
	Sg.nRLevel = 20;
	Sg.nDLevel = 80;
	Sg.bRendou = TRUE;
	Sg.fFreqL = 1000.0;
	Sg.fFreqR = 1000.0;
	Sg.bRendouF = TRUE;
	Sg.fStartFreq = 100.0;
	Sg.fEndFreq = 10000.0;
	Sg.fSweepSpeed = 0.5;
	Sg.nPulseNum = 1;
	Sg.fPulseCycle = 10;
	Sg.fPresetFreq[0] = 10;
	Sg.fPresetFreq[1] = 100;
	Sg.fPresetFreq[2] = 1000;
	Sg.fPresetFreq[3] = 10000;
	Sg.fReferencePitch = 440;
	Sg.nScale = 9;
	Sg.nOctave = 4;
	for (int i = 0; i < N_SG_COMPONENT; i++)
		Sg.fCompFreq[i] = i + 1;
	Sg.nCompLevel[0] = 50;
	Sg.fSynthFreq = 1000;
	Sg.fWaveFreq = 200;
	Sg.fSweepTime = 10;

	Fft.nChannel = 1;
	Fft.nSamplingRate = 48000;
	Fft.nFftMinFreq = 20;
	Fft.nFftMaxFreq = 20000;
	Fft.bFftAutoFreq = TRUE;
	Fft.nFftMinLevel = -140;
	Fft.nFftMaxLevel = 0;
	Fft.bFftAutoLevel = TRUE;
	Fft.nFftWindowFunc = WF_HANNING;
	Fft.nFftSize = 4096;
	Fft.nPeakHoldTime = 5;
	Fft.nTimeDir = 0;
	Fft.nTimeDataNum = 20;
	Fft.nMicCalID = -1;
	Fft.nCrfZoom = 1;
	Fft.nTimeRes = 2;
	Fft.nSmoothing = 125;
	Fft.bRlSplit = 1;
	Fft.nOctaveBand = 1;
	Fft.nTimeRange = 5;

	Os.nChannel = 1;
	Os.nSamplingRate = 48000;
	Os.bLevelAuto = TRUE;
	Os.nSweep = 9;
	Os.bTrigAuto = TRUE;
	Os.bTrigDisp = TRUE;
	Os.bTrigPos = TRUE;
	Os.bTrigFree = TRUE;
	Os.nOverlay = 1;
	Os.bRendou = TRUE;
	Os.bZeroLevel = TRUE;
	Os.nPosL = 20;
	Os.nPosR = -20;

	Fre.nChannel = 1;
	Fre.nSamplingRate = 48000;
	Fre.nMinLevel = -50;
	Fre.nMaxLevel = 0;
	Fre.nSweepFreqStart = 20;
	Fre.nSweepFreqEnd = 20000;
	Fre.nSweepTime = 20;
	Fre.nSweepLevel = -10;
	Fre.nSpotFreqStart = 20;
	Fre.nSpotFreqEnd = 20000;
	Fre.nSpotPoint= 20;
	Fre.nSpotLevel = -10;
	Fre.nNoiseFreqStart = 20;
	Fre.nNoiseFreqEnd = 20000;
	Fre.nNoiseResolution = 1;
	Fre.nNoiseAveraging = 3;
	Fre.nNoiseLevel = -10;
	Fre.nMicCalID = -1;

	Dst.nChannel = 1;
	Dst.nSamplingRate = 48000;
	Dst.nMaxHarmonics = 10;
	Dst.nGuardCnt = 4;
	Dst.nScaleMin = -100;
	Dst.nScaleMax = -20;
	Dst.nManualFreq = 1000;
	Dst.nManualLevel = -10;
	Dst.nFreqStart = 20;
	Dst.nFreqEnd = 10000;
	Dst.nFreqPoint = 20;
	Dst.nFreqLevel = -10;
	Dst.nLevelStart = -40;
	Dst.nLevelEnd = 0;
	Dst.nLevelPoint = 20;
	Dst.nLevelFreq = 1000;
	Dst.bFreqMarker = TRUE;
	Dst.bLevelMarker = TRUE;

	Imp.nSamplingRate = 48000;
	Imp.nTime = 4;
	Imp.nChannel = 1;
	Imp.nMeasureNum = 1;
	Imp.bAutoLevel = TRUE;
	Imp.bAutoRetry = TRUE;
	Imp.nMethod = 1;

	Acf.nSamplingRate = 48000;
	Acf.fIntegrationTime = 2;
	Acf.fCalcCycle = 0.5;
	Acf.fMaxTau = 100;
	Acf.nTimeDataNum = 17;
	Acf.nMeasureTime = 10;
	Acf.nFilter = 0;
	Acf.nScaleLevel = -40;
	Acf.fRealtimeIntegrationTime = 2.0;
	Acf.fRealtimeRunningStep = 0.5;
	Acf.nRealtimeFreqWeighting = 0;
	Acf.fRealtimeMaxDelayTime = 0.2;
	Acf.fRealtimeTaueInterval = 5;
	Acf.fRealtimeTaueEndLevel = -5;
	Acf.fRealtimeTaueEndTime = 50;
	Acf.nRealtimeMinLevel = -70;
	Acf.nRealtimeMaxLevel = -10;
	Acf.fRealtimeTaueMinTime = 0;
	Acf.fRealtimeTaueMaxTime = 100;
	Acf.nRealtimeTaueScale = 1;

	Flt.nOrder = 4;
	Flt.fRipple1 = 1;
	Flt.fRipple2 = 40;
	Flt.fCutoff1 = 100;
	Flt.fCutoff2 = 1000;

	Dtr.bDeleteAtStart = TRUE;
	Dtr.bAlwaysNewData = TRUE;

	Tim.nPosLeft = 300;
	Tim.nPosTop = 200;
	Tim.nFontSize = 90;
	Tim.nFormat = 2;
	Tim.nTransparency = 0;
}

#define INI_FILE	".\\RTA.INI"
#define SECTION_FONT_SIZE	"FontSize"
#define SECTION_DEVICE_BIT	"Device"
#define SECTION_AUTO_REC	"AutoRec"

CSetData2::CSetData2()
{
	CString sDataDirectory;
	CString sPathName;
	char buf[256];

	GetDataDirectory(sDataDirectory);
	sPathName.Format("%s\\%s", (LPCSTR)sDataDirectory, INI_FILE);

	nFontSizeSg = nFontSizeSgOrg = GetPrivateProfileInt(SECTION_FONT_SIZE, "SG", 0, sPathName);
	nFontSizeFft = nFontSizeFftOrg = GetPrivateProfileInt(SECTION_FONT_SIZE, "FFT", 0, sPathName);
	nFontSizeOs = nFontSizeOsOrg = GetPrivateProfileInt(SECTION_FONT_SIZE, "OS", 0, sPathName);
	nFontSizeFre = nFontSizeFreOrg = GetPrivateProfileInt(SECTION_FONT_SIZE, "FRE", 0, sPathName);
	nFontSizeDst = nFontSizeDstOrg = GetPrivateProfileInt(SECTION_FONT_SIZE, "DST", 0, sPathName);
	nFontSizeImp = nFontSizeImpOrg = GetPrivateProfileInt(SECTION_FONT_SIZE, "IMP", 0, sPathName);
	nFontSizeAcf = nFontSizeAcfOrg = GetPrivateProfileInt(SECTION_FONT_SIZE, "ACF", 0, sPathName);

	bInput16BitOnly = bInput16BitOnlyOrg = GetPrivateProfileInt(SECTION_DEVICE_BIT, "Input16bitOnly", 0, sPathName);
	bOutput16BitOnly = bOutput16BitOnlyOrg = GetPrivateProfileInt(SECTION_DEVICE_BIT, "Output16bitOnly", 0, sPathName);

	GetPrivateProfileString(SECTION_AUTO_REC, "Folder", "", buf, sizeof(buf), sPathName);
	sAutoRecFolder = sAutoRecFolderOrg = buf;
	bAutoRecInput = bAutoRecInputOrg = GetPrivateProfileInt(SECTION_AUTO_REC, "Input", 0, sPathName);
	bAutoRecOutput = bAutoRecOutputOrg = GetPrivateProfileInt(SECTION_AUTO_REC, "Output", 0, sPathName);
}

CSetData2::~CSetData2()
{
	CString sDataDirectory;
	CString sPathName;

	GetDataDirectory(sDataDirectory);
	sPathName.Format("%s\\%s", (LPCSTR)sDataDirectory, INI_FILE);

	if (nFontSizeSg != nFontSizeSgOrg)
		WritePrivateProfileInt(SECTION_FONT_SIZE, "SG", nFontSizeSg, sPathName);

	if (nFontSizeFft != nFontSizeFftOrg)
		WritePrivateProfileInt(SECTION_FONT_SIZE, "FFT", nFontSizeFft, sPathName);

	if (nFontSizeOs != nFontSizeOsOrg)
		WritePrivateProfileInt(SECTION_FONT_SIZE, "OS", nFontSizeOs, sPathName);

	if (nFontSizeFre != nFontSizeFreOrg)
		WritePrivateProfileInt(SECTION_FONT_SIZE, "FRE", nFontSizeFre, sPathName);

	if (nFontSizeDst != nFontSizeDstOrg)
		WritePrivateProfileInt(SECTION_FONT_SIZE, "DST", nFontSizeDst, sPathName);

	if (nFontSizeImp != nFontSizeImpOrg)
		WritePrivateProfileInt(SECTION_FONT_SIZE, "IMP", nFontSizeImp, sPathName);

	if (nFontSizeAcf != nFontSizeAcfOrg)
		WritePrivateProfileInt(SECTION_FONT_SIZE, "ACF", nFontSizeAcf, sPathName);

	if (bInput16BitOnly != bInput16BitOnlyOrg)
		WritePrivateProfileInt(SECTION_DEVICE_BIT, "Input16bitOnly", bInput16BitOnly, sPathName);

	if (bOutput16BitOnly != bOutput16BitOnlyOrg)
		WritePrivateProfileInt(SECTION_DEVICE_BIT, "Output16bitOnly", bOutput16BitOnly, sPathName);

	if (sAutoRecFolder != sAutoRecFolderOrg)
		WritePrivateProfileString(SECTION_AUTO_REC, "Folder", sAutoRecFolder, sPathName);

	if (bAutoRecInput != bAutoRecInputOrg)
		WritePrivateProfileInt(SECTION_AUTO_REC, "Input", bAutoRecInput, sPathName);

	if (bAutoRecOutput != bAutoRecOutputOrg)
		WritePrivateProfileInt(SECTION_AUTO_REC, "Output", bAutoRecOutput, sPathName);
}

void CSetData2::WritePrivateProfileInt(LPCSTR lpAppName, LPCSTR lpKeyName, int nData, LPCSTR lpFileName)
{
	CString str;

	str.Format("%d", nData);
	WritePrivateProfileString(lpAppName, lpKeyName, str, lpFileName);
}
