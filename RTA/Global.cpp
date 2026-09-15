#include "stdafx.h"
#include "Rta.h"
#include "WaveInEx.h"
#include "WaveOutEx.h"
#include "Mrnd.h"
#include "DssData.h"
#include <shlobj.h>

CSetData g_oSetData;
CSetData2 g_oSetData2;
const char *g_tWindowFunc[WF_NUM] = {
	"Rectangular", "Triangular", "Hamming", "Hanning", "Blackman", "Blackman-Harris", "Flat-Top"
};
INT_PTR g_nWaveInDevice;
INT_PTR g_nWaveOutDevice;
HICON g_hIcon;
CWaveForm g_oWaveForm;
int g_nFilterUpdateCounter;

const char *g_pFreqScale1[MAX_OCT1 + 1] = {
	"ALL",
	"16", "31.5", "63",
	"125", "250", "500",
	"1k", "2k", "4k",
	"8k", "16k", ""
};

const char *g_pFreqScale3[MAX_OCT3 + 1] = {
	"ALL",
	"16", "20", "25", "31.5", "40", "50", "63", "80", "100", "125",
	"160", "200", "250", "315", "400", "500", "630", "800", "1k", "1.25k",
	"1.6k", "2k", "2.5k", "3.15k", "4k", "5k", "6.3k", "8k", "10k", "12.5k",
	"16k", "20k", "25k", "31.5k", "40k", "50k", "63k", "8k", "100k", ""
};

const int g_nOctBandTbl[OCT_BAND_NUM] = {
	1, 3, 6, 12, 24
};

static const UINT g_tWaveForm[N_WAVE_FORM] = {
	IDS_SINUSOIDAL, IDS_TRIANGULAR, IDS_SQUARE, IDS_SAWTOOTH
};

const UINT g_tFftModeId[FFT_MODE_NUM] = {
	IDS_SPECTRUM, IDS_OCTAVE, IDS_WATERFALL, IDS_CORRELATION, IDS_PHASE2, IDS_SPECTROGRAM, IDS_CROSS_SPECTRUM, IDS_COHERENCE, IDS_CEPSTRUM
};

const int g_tFontSize[] = {
#ifdef _LANG_JPN
	9, 10, 11, 12, 14, 16, 18, 0
#endif
#ifdef _LANG_ENG
	8, 10, 12, 14, 18, 0
#endif
};

void SetWindowFuncList(CComboBox &combo)
{
	for (int i = 0; i < WF_NUM; i++)
		combo.AddString(g_tWindowFunc[i]);
}

void SetWaveFormList(CComboBox &combo, int nSel)
{
	int i;
	CString str, str2;

	for (i = 0; i < N_WAVE_FORM; i++) {
		str.LoadString(g_tWaveForm[i]);
		combo.AddString(str);
	}

	str.LoadString(IDS_WAVEFORM);
	for (i = 0; i < MAX_WAVEFORM; i++) {
		str2.Format("%s:%d", (LPCSTR)str, i + 1);
		combo.AddString(str2);
	}

	combo.SetCurSel(nSel);
}

void StartWaveInOut(CWaveInEx *pWaveIn, CWaveOutEx *pWaveOut, BOOL bSetVolume)
{
	if (bSetVolume) {
		pWaveOut->SetVolume(0xffff, 0xffff);
		Sleep(200);			// ボリュームが安定するまでのウェイト
	}

	if (g_nWaveOutDevice >= 0) {
		pWaveOut->Pause();
		pWaveOut->Start();
		pWaveOut->Restart();
		pWaveIn->Start();
	} else {
		pWaveIn->Start();
		pWaveOut->Start();
	}
}

int GetLinearScaleStep(int nMaxValue, int nMinvalue, int nScaleSize)
{
	int step = (nMaxValue - nMinvalue) / (nScaleSize / CDpi::AdjustDpi(30));
	if (step == 0)
		step = 1;

	int scale = (int)log10((double)step);
	int num = (int)(step / pow(10.0, scale));

	if (num < 2)
		num = 1;
	else if (num < 5)
		num = 2;
	else
		num = 5;

	return (int)(num * pow(10.0, scale));
}

int GetLogScaleStep(int nValue)
{
	return (int)(pow(10.0, (double)(int)log10((double)nValue)) + 0.5);
}

double GetLogScaleStep(double fValue)
{
	return pow(10, floor(log10(fValue) + 1e-3));
}

double GetLinearScaleStep(double fRange, int nPixel, int *pStep)
{
	int nStep;

	if (fRange <= 0)
		return 1;

	double t1 = fRange / nPixel;
	double fStep = pow(10.0, floor(log10(t1)));
	double t2 = t1 / fStep;
	if (t2 < 2) {
		fStep *= 0.5;
		nStep = 2;
	} else if (t2 < 5)
		nStep = 2;
	else
		nStep = 5;

	if (pStep != NULL)
		*pStep = nStep;

	return fStep;
}

int GetScaleStartValue(int nMinValue, int nStep)
{
	if (nMinValue > 0)
		nMinValue += nStep - 1;

	return nMinValue / nStep * nStep;
}

double GetScaleStartValue(double fMinValue, double fStep)
{
	return floor(fMinValue / fStep + 1e-3) * fStep;
}

CString GetDirName(const CString &filePath)
{
	int n = filePath.ReverseFind('\\');
	if (n < 0)
		return filePath;
	else
		return filePath.Left(n);
}

CString GetFileName(CString &filePath)
{
	int n = filePath.ReverseFind('\\');
	if (n < 0)
		return filePath;
	else
		return filePath.Mid(n + 1);
}

CString &GetString(UINT nID)
{
	static CString str;

	str.LoadString(nID);
	return str;
}

BOOL SelectListBox(CComboBox &ctl, DWORD_PTR dwData)
{
	int n = ctl.GetCount();

	for (int i = 0; i < n; i++) {
		if (ctl.GetItemData(i) == dwData) {
			ctl.SetCurSel(i);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL SelectListBox(CListBox &ctl, DWORD_PTR dwData)
{
	int n = ctl.GetCount();

	for (int i = 0; i < n; i++) {
		if (ctl.GetItemData(i) == dwData) {
			ctl.SetCurSel(i);
			return TRUE;
		}
	}

	return FALSE;
}

CString GetNumberString(int nNumber)
{
	CString str;

	if (nNumber < 1000)
		str.Format("%d", nNumber);
	else if (nNumber < 1000000)
		str.Format("%dk", nNumber / 1000);
	else
		str.Format("%dM", nNumber / 1000000);

	return str;
}

CString GetNumberString(double fNumber)
{
	CString str1, str2;

	str1.Format("%g", fNumber);

	if (fNumber < 1000)
		str2.Format("%g", fNumber);
	else if (fNumber < 1000000)
		str2.Format("%gk", fNumber / 1000);
	else
		str2.Format("%gM", fNumber / 1000000);

	if (str1.GetLength() < str2.GetLength())
		return str1;
	else
		return str2;
}

BOOL CreateShortCut(
		LPCTSTR pszShortcutFile,	// 作成する[*.lnk]のファイル名
		LPCTSTR pszDescription,		// ディスクリプション名
		LPCTSTR pszTargetFile,		// リンク元のファイルのフルパス名
		LPCTSTR pszParameter)		// 起動パラメータ
{
	BOOL bRet = FALSE;
	HRESULT  hResult;
	IShellLink *psl;
	int len, indx;
	CString sTemp,sWorkDir;
	char chPath[MAX_PATH];
	LPITEMIDLIST pidl;
	CString sShortcutPath;

	//　デスクトップフォルダ
	::SHGetSpecialFolderLocation(::AfxGetMainWnd()->GetSafeHwnd(), CSIDL_DESKTOPDIRECTORY, &pidl);
	::SHGetPathFromIDList(pidl, chPath);
	sShortcutPath.Format("%s\\%s.lnk", chPath, pszShortcutFile);

	//  COMの初期化
	CoInitialize( NULL );

	//  IShellLink インターフェースの取得
	hResult = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&psl);
	if (SUCCEEDED(hResult)) {
		IPersistFile*  ppf;

		// IPersistFileインターフェースの取得
		hResult = psl->QueryInterface(IID_IPersistFile, (void **)&ppf);
		if (SUCCEEDED(hResult)) {
			WCHAR  wszShortcutFile[MAX_PATH]; // ユニコード用のバッファ

			sTemp = (CString)pszTargetFile;
			len  = sTemp.GetLength();
			indx = sTemp.ReverseFind('\\');
			sWorkDir = sTemp.Mid(0, indx);
			hResult = psl->SetWorkingDirectory(sWorkDir); // 作業dir

			// リンク元のファイルのパスをセットする
			hResult = psl->SetPath( pszTargetFile );
			if (SUCCEEDED(hResult)) {
				// 起動パラメータをセット
				psl->SetArguments(pszParameter);

				// ショートカットファイルのディスクリプションをセット
				hResult = psl->SetDescription(pszDescription);
				if (SUCCEEDED(hResult)) {
					MultiByteToWideChar(CP_ACP, 0, sShortcutPath, -1, wszShortcutFile, MAX_PATH);
					hResult = ppf->Save(wszShortcutFile, TRUE);
					if (SUCCEEDED(hResult))
						bRet = TRUE;
				}
			}
			ppf->Release();
		}
		psl->Release();
	}

	CoUninitialize();
	return bRet;
}

void CheckFileName(CString &fileName)
{
	int n;

	while ((n = fileName.FindOneOf(":<>\\/*?| ")) != -1)
		fileName.SetAt(n, '_');
}

void GetAxisName(int nMode, CString &sNameX, CString &sNameY)
{
	switch (nMode) {
	case FFT_MODE_PWS:
	case FFT_MODE_OCT:
	case FFT_MODE_3DD:
	case FFT_MODE_SPG:
		sNameX.LoadString(IDS_FREQUENCY);
		sNameY.LoadString(IDS_SPL);
		break;
	case FFT_MODE_CRF:
		sNameX.LoadString(IDS_TAU);
		sNameY.LoadString(IDS_CORRELATION);
		break;
	case FFT_MODE_PHS:
		sNameX.LoadString(IDS_FREQUENCY);
		sNameY.LoadString(IDS_PHASE2);
		break;
	case FFT_MODE_CRS:
		sNameX.LoadString(IDS_FREQUENCY);
		sNameY.LoadString(IDS_LEVEL);
		break;
	case FFT_MODE_COH:
		sNameX.LoadString(IDS_FREQUENCY);
		sNameY.LoadString(IDS_COHERENCE2);
		break;
	case FFT_MODE_CEP:
		sNameX.LoadString(IDS_TIME);
		sNameY.LoadString(IDS_LEVEL);
		break;
	}
}

void GetAxisUnit(int nMode, CString &sUnitX, CString &sUnitY)
{
	switch (nMode) {
	case FFT_MODE_PWS:
	case FFT_MODE_OCT:
	case FFT_MODE_3DD:
	case FFT_MODE_SPG:
		sUnitX = "Hz";
		sUnitY = "dB";
		break;
	case FFT_MODE_CRF:
		sUnitX = "ms";
		sUnitY.Empty();
		break;
	case FFT_MODE_PHS:
		sUnitX = "Hz";
		sUnitY = "deg";
		break;
	case FFT_MODE_CRS:
		sUnitX = "Hz";
		sUnitY = "dB";
		break;
	case FFT_MODE_COH:
		sUnitX = "Hz";
		sUnitY.Empty();
		break;
	case FFT_MODE_CEP:
		sUnitX = "ms";
		sUnitY = "dB";
		break;
	}
}

LPCTSTR GetRunningStatus(int nStringId, int nInputBits, int nOutputBits, int nRecordBits)
{
	static CString sStatus;
	CString sRunning;
	CString sInput;
	CString sOutput;
	CString sRecord;
	CString sTemp;

	sRunning.LoadString(nStringId);

	if (nInputBits > 0) {
		sTemp.LoadString(IDS_INPUT);
		sInput.Format("%s:%dbit", (LPCSTR)sTemp, nInputBits);
	}

	if (nOutputBits > 0) {
		sTemp.LoadString(IDS_OUTPUT);
		sOutput.Format("%s:%dbit", (LPCSTR)sTemp, nOutputBits);
	}

	if (nRecordBits > 0)
		sRecord.Format("%dbit", nRecordBits);

	if (sInput.GetLength() != 0 && sOutput.GetLength() != 0)
		sStatus.Format("%s (%s / %s)", (LPCSTR)sRunning, (LPCSTR)sInput, (LPCSTR)sOutput);
	else if (sInput.GetLength() != 0)
		sStatus.Format("%s (%s)", (LPCSTR)sRunning, (LPCSTR)sInput);
	else if (sOutput.GetLength() != 0)
		sStatus.Format("%s (%s)", (LPCSTR)sRunning, (LPCSTR)sOutput);
	else if (sRecord.GetLength() != 0)
		sStatus.Format("%s (%s)", (LPCSTR)sRunning, (LPCSTR)sRecord);
	else
		sStatus = sRunning;

	return sStatus;
}

double GetDither(int nBitsPerSample)
{
	if (nBitsPerSample != 0)
		return (rnd() - 0.5) / PowDouble(nBitsPerSample);
	else
		return 0;
}

void SetBackupList(CListCtrl &cListBackup, CStringArray &oBackupList)
{
	cListBackup.DeleteAllItems();
	oBackupList.RemoveAll();

	CString sFind;
	sFind.Format("%s\\*", g_DssData.m_sBackupFolder);
	CFileFind find;
	CString sIniFileName;
	BOOL bFind = find.FindFile(sFind);
	while (bFind) {
		bFind = find.FindNextFile();

		if (find.GetFileName() != "." && find.GetFileName() != "..") {
			char time[32];
			char comment[1024];
			CFileStatus status;
			CFile::GetStatus(find.GetFilePath(), status);
			if (status.m_attribute & CFile::directory) {
				sIniFileName.Format("%s\\backup.ini", (LPCSTR)find.GetFilePath());
				if (::GetPrivateProfileString("backup", "time", "", time, sizeof(time), sIniFileName)) {
					::GetPrivateProfileString("backup", "comment", "", comment, sizeof(comment), sIniFileName);

					LVITEM lvitem;
					memset(&lvitem, 0, sizeof(lvitem));
					lvitem.mask = LVIF_TEXT | LVIF_PARAM;
					lvitem.iItem = 0;
					lvitem.pszText = time;
					lvitem.lParam = oBackupList.Add(find.GetFilePath());
					lvitem.iItem = cListBackup.InsertItem(&lvitem);

					lvitem.mask = LVIF_TEXT;
					lvitem.iSubItem = 1;
					lvitem.pszText = comment;
					cListBackup.SetItem(&lvitem);
				}
			}
		}
	}
}

CString Hankaku2Zenkaku(const char *str)
{
	char ret_str[1024];

	LCMapString(GetUserDefaultLCID(), LCMAP_FULLWIDTH, str, -1, ret_str, sizeof(ret_str));
	return ret_str;
}