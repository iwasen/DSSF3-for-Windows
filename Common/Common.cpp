#include "stdafx.h"
#include "common.h"
#include "comres.h"
#ifdef USE_ASIO
#include "AsioManager.h"
#endif
#ifdef USE_WASAPI
#include "WasapiManager.h"
#endif
#include "DssData.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <atlbase.h>
#include <mmsystem.h>

constexpr char g_sSystemName[] = "Diagnostic System for Sound Fields";
constexpr char g_sRegistryName[] = "Analyzing System for Sound Fields Ver5";
CDllUxTheme g_dllUxTheme;

static HDDEDATA EXPENTRY DdeCallback(WORD wType, WORD wFmt, HCONV hConv, HSZ hszTopic, HSZ hszItem, HDDEDATA hData, DWORD lData1, DWORD lData2);

#if defined(_PRJ_RTA) || defined(_PRJ_NMS)
static struct SAMPLING_RATE {
	DWORD dwSamplingRate;
	DWORD dwFormat;
	DWORD dwAvail;
} tSamplingRate[] = {
	{192000, 0},
	{ 96000, WAVE_FORMAT_96M16 | WAVE_FORMAT_96S16},
	{ 88200, 0},
	{ 64000, 0},
	{ 48000, WAVE_FORMAT_48M16 | WAVE_FORMAT_48S16},
	{ 44100, WAVE_FORMAT_44M16 | WAVE_FORMAT_44S16},
	{ 32000, 0},
	{ 22050, WAVE_FORMAT_2M16 | WAVE_FORMAT_2S16},
	{ 16000, 0},
	{ 11025, WAVE_FORMAT_1M16 | WAVE_FORMAT_1S16},
	{  8000, 0}
};

static BOOL bCheckSamplingRateIn;
static BOOL bCheckSamplingRateOut;

static void CheckSamplingRateIn(INT_PTR nDevice);
static void CheckSamplingRateOut(INT_PTR nDevice);
#endif	// _PRJ_RTA,_PRJ_NMS

#if defined(_PRJ_RTA) || defined(_PRJ_SAS)
static BOOL FindMMLib();
static BOOL FindMMLibSub(const CString sDir, CString &sFindPath, int nLevel = 0);
#endif	// _PRJ_RTA,_PRJ_SAS

#if defined(_PRJ_RTA) || defined(_PRJ_NMS)
void ResetSamplingRate()
{
	bCheckSamplingRateIn = FALSE;
	bCheckSamplingRateOut = FALSE;

	for (int i = 0; i < ARRAY_SIZE(tSamplingRate); i++)
		tSamplingRate[i].dwAvail = 0;
}

void SetSamplingRateList(CComboBox &combo, int nInOut, INT_PTR nDeviceIn, INT_PTR nDeviceOut, int &nDefault)
{
	CString strBuf;
	int nIndex = -1;
	int nSelect = -1;

	if (nInOut & WAVE_IN) {
		if (!bCheckSamplingRateIn) {
			CheckSamplingRateIn(nDeviceIn);
			bCheckSamplingRateIn = TRUE;
		}
	}

	if (nInOut & WAVE_OUT) {
		if (!bCheckSamplingRateOut) {
			CheckSamplingRateOut(nDeviceOut);
			bCheckSamplingRateOut = TRUE;
		}
	}

	combo.ResetContent();

	for (int i = 0; i < ARRAY_SIZE(tSamplingRate); i++) {
		if ((tSamplingRate[i].dwAvail & nInOut) == (DWORD)nInOut) {
			strBuf.Format("%.3f kHz", (double)tSamplingRate[i].dwSamplingRate / 1000);
			nIndex = combo.AddString(strBuf);
			combo.SetItemData(nIndex, tSamplingRate[i].dwSamplingRate);

			if (tSamplingRate[i].dwSamplingRate == (DWORD)nDefault)
				nSelect = nIndex;
		}
	}

	if (nIndex != -1) {
		if (nSelect == -1) {
			nSelect = nIndex;
			nDefault = (int)combo.GetItemData(nIndex);
		}

		combo.SetCurSel(nSelect);
	}
}

void CheckSamplingRateIn(INT_PTR nDevice)
{
	int i;

	if (nDevice >= 0) {
		// WAVE
		WAVEINCAPS wic;
		DWORD dwAvail;

		waveInGetDevCaps(nDevice, &wic, sizeof(wic));

		for (i = 0; i < ARRAY_SIZE(tSamplingRate); i++) {
			dwAvail = 0;

			if (tSamplingRate[i].dwFormat) {
				if (tSamplingRate[i].dwFormat & wic.dwFormats)
					dwAvail = WAVE_IN;
			}

			if (dwAvail == 0) {
				const int nChannels = 2;
				const int nSamplesPerSec = tSamplingRate[i].dwSamplingRate;
				const int nBitsPerSample = 16;

				WAVEFORMATEX waveFormat{
					.wFormatTag = WAVE_FORMAT_PCM,
					.nChannels = (WORD)nChannels,
					.nSamplesPerSec = (DWORD)nSamplesPerSec,
					.nAvgBytesPerSec = (DWORD)(nSamplesPerSec * nChannels * (nBitsPerSample / 8)),
					.nBlockAlign = (WORD)(nChannels * (nBitsPerSample / 8)),
					.wBitsPerSample = (WORD)nBitsPerSample,
					.cbSize = sizeof(waveFormat)
				};
				if (waveInOpen(NULL, (UINT)nDevice, &waveFormat, 0, 0, WAVE_FORMAT_QUERY) == MMSYSERR_NOERROR)
					dwAvail = WAVE_IN;
			}

			tSamplingRate[i].dwAvail |= dwAvail;
		}
#ifdef USE_WASAPI
	} if (nDevice <= -100) {
		// WASAPI
		for (i = 0; i < ARRAY_SIZE(tSamplingRate); i++) {
			if (g_cWasapiManager.CanSamplingRateIn(-(nDevice + 100), tSamplingRate[i].dwSamplingRate))
				tSamplingRate[i].dwAvail |= WAVE_IN;
		}
#endif	//USE_WASAPI
#ifdef USE_ASIO
	} else {
		// ASIO
		for (i = 0; i < ARRAY_SIZE(tSamplingRate); i++) {
			if (g_cAsioManager.CanSamplingRate(-(nDevice + 1), tSamplingRate[i].dwSamplingRate))
				tSamplingRate[i].dwAvail |= WAVE_IN;
		}
#endif	//USE_ASIO
	}
}

void CheckSamplingRateOut(INT_PTR nDevice)
{
	int i;

	if (nDevice >= 0) {
		// WAVE
		WAVEOUTCAPS woc;
		DWORD dwAvail;

		waveOutGetDevCaps(nDevice, &woc, sizeof(woc));

		for (i = 0; i < ARRAY_SIZE(tSamplingRate); i++) {
			dwAvail = 0;

			if (tSamplingRate[i].dwFormat) {
				if (tSamplingRate[i].dwFormat & woc.dwFormats)
					dwAvail = WAVE_OUT;
			}

			if (dwAvail == 0) {
				const int nChannels = 2;
				const int nSamplesPerSec = tSamplingRate[i].dwSamplingRate;
				const int nBitsPerSample = 16;

				WAVEFORMATEX waveFormat{
					.wFormatTag = WAVE_FORMAT_PCM,
					.nChannels = (WORD)nChannels,
					.nSamplesPerSec = (DWORD)nSamplesPerSec,
					.nAvgBytesPerSec = (DWORD)(nSamplesPerSec * nChannels * (nBitsPerSample / 8)),
					.nBlockAlign = (WORD)(nChannels * (nBitsPerSample / 8)),
					.wBitsPerSample = (WORD)nBitsPerSample,
					.cbSize = sizeof(waveFormat)
				};
				if (waveOutOpen(NULL, (UINT)nDevice, &waveFormat, 0, 0, WAVE_FORMAT_QUERY) == MMSYSERR_NOERROR)
					dwAvail = WAVE_OUT;
			}

			tSamplingRate[i].dwAvail |= dwAvail;
		}
#ifdef USE_WASAPI
	} if (nDevice <= -100) {
		// WASAPI
		for (i = 0; i < ARRAY_SIZE(tSamplingRate); i++) {
			if (g_cWasapiManager.CanSamplingRateOut(-(nDevice + 100), tSamplingRate[i].dwSamplingRate))
				tSamplingRate[i].dwAvail |= WAVE_OUT;
		}
#endif	//USE_WASAPI
#ifdef USE_ASIO
	} else {
		// ASIO
		for (i = 0; i < ARRAY_SIZE(tSamplingRate); i++) {
			if (g_cAsioManager.CanSamplingRate(-(nDevice + 1), tSamplingRate[i].dwSamplingRate))
				tSamplingRate[i].dwAvail |= WAVE_OUT;
		}
#endif	//USE_ASIO
	}
}
#endif	// _PRJ_RTA,_PRJ_NMS

#if defined(_PRJ_RTA)
int GetSamplingRateIndex(int nSamplingRate)
{
	for (int i = 0; i < ARRAY_SIZE(tSamplingRate); i++) {
		if (nSamplingRate - 500 < (int)tSamplingRate[i].dwSamplingRate && nSamplingRate + 500 > (int)tSamplingRate[i].dwSamplingRate) {
			return i;
		}
	}

	return -1;
}
#endif	// _PRJ_RTA

void SetFilterNameList(CComboBox &combo)
{
	static constexpr int tFilterName[] = {
	//	"Flat", "Aì¡ê´", "Bì¡ê´", "Cì¡ê´"
		IDS_FLAT, IDS_AWEIGHTING, IDS_BWEIGHTING, IDS_CWEIGHTING
	};
	CString str;

	for (int i = 0; i < 4; i++) {
		if (str.LoadString(tFilterName[i]))
			combo.AddString(str);
	}
}

double GetCeilValue(double fValue)
{
	double a;
	double x;

	if (fValue != 0) {
		a = fabs(fValue);
		x = pow(10.0, floor(log10(a)));
		x = ceil(a / x) * x;
		if (fValue < 0)
			x = -x;
	} else
		x = 0;

	return x;
}

int MessageBoxID(CWnd *pWnd, UINT nIDPrompt, UINT nType)
{
	CString msg;

	msg.LoadString(nIDPrompt);
	if (pWnd != NULL)
		return pWnd->MessageBox(msg, NULL, nType);
	else
		return ::AfxMessageBox(msg, nType);
}

#if defined(_PRJ_RTA) || defined(_PRJ_SAS)
BOOL ExecMMLib(CWnd *pWnd)
{
	if (g_DssData.m_sMmlibPath[0] == '\0') {
		if (!FindMMLib()) {
			MessageBoxID(pWnd, IDS_ERR_NO_MMLIB_PATH, MB_OK | MB_ICONEXCLAMATION);
			return FALSE;
		}
	}

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi{};
	CString sCommandLine;
	sCommandLine.Format("%s /C", g_DssData.m_sMmlibPath);
	if (!::CreateProcess(NULL, (LPSTR)(LPCSTR)sCommandLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi)) {
		MessageBoxID(pWnd, IDS_ERR_EXEC_MMLIB, MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	::CloseHandle(pi.hThread);
	::CloseHandle(pi.hProcess);

	for (int i = 0; i < 10; i++) {
		::Sleep(500);

		if (CheckMMLib())
			return TRUE;
	}

	MessageBoxID(pWnd, IDS_ERR_EXEC_MMLIB, MB_OK |MB_ICONEXCLAMATION);

	return FALSE;
}

BOOL FindMMLib()
{
	CString sProgramFiles;
	CString sProgramFilesX86;
	CString sMMLib;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, NULL, sProgramFiles.GetBuffer(MAX_PATH)))) {
		sProgramFiles.ReleaseBuffer();
		if (FindMMLibSub(sProgramFiles, sMMLib)) {
			strcpy_s(g_DssData.m_sMmlibPath, sMMLib);
			g_DssData.SaveData();
			return TRUE;
		}
	}

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROGRAM_FILESX86, NULL, NULL, sProgramFilesX86.GetBuffer(MAX_PATH)))) {
		sProgramFilesX86.ReleaseBuffer();
		if (sProgramFilesX86 != sProgramFiles) {
			if (FindMMLibSub(sProgramFilesX86, sMMLib)) {
				strcpy_s(g_DssData.m_sMmlibPath, sMMLib);
				g_DssData.SaveData();
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL FindMMLibSub(const CString sDir, CString &sFindPath, int nLevel)
{
	CFileFind oFileFind;

	CString sSearchFile = sDir + "\\*";

	if (!oFileFind.FindFile(sSearchFile))
		return FALSE;

	BOOL bFndEndJug = TRUE;
	while (bFndEndJug) {
		bFndEndJug = oFileFind.FindNextFile();

		if (oFileFind.IsDots())
			continue;

		if (oFileFind.IsDirectory()) {
			if (nLevel != 0 || oFileFind.GetFileName().CompareNoCase("Multimedia Library") == 0 || oFileFind.GetFileName().CompareNoCase("YMEC") == 0)
				if (FindMMLibSub(oFileFind.GetFilePath(), sFindPath, nLevel + 1))
					return TRUE;
		} else {
			if (oFileFind.GetFileName().CompareNoCase("mmlib.exe") == 0) {
				sFindPath = oFileFind.GetFilePath();
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CheckMMLib()
{
	DWORD idInst = 0L;
	if (DdeInitialize(&idInst,
				(PFNCALLBACK)MakeProcInstance(
				(FARPROC)DdeCallback, hInstance),
				APPCMD_CLIENTONLY, 0) != 0) {
			return FALSE;
	}

	HSZ hService = DdeCreateStringHandle(idInst, "MMLIB", 0);
	HSZ hTopic = DdeCreateStringHandle(idInst, "data", 0);

	HCONV hConv = DdeConnect(idInst, hService, hTopic, NULL);
	if (hConv == NULL) {
		hConv = DdeConnect(idInst, hService, hTopic, NULL);
		if (hConv == NULL) {
			DdeFreeStringHandle(idInst, hService);
			DdeFreeStringHandle(idInst, hTopic);
			DdeUninitialize(idInst);
			idInst = 0;
			return FALSE;
		}
	}

	DdeDisconnect(hConv);

	DdeFreeStringHandle(idInst, hService);
	DdeFreeStringHandle(idInst, hTopic);

	DdeUninitialize(idInst);

	return TRUE;
}

BOOL SendMMLib(CWnd *pWnd, LPCSTR pFolderTitle, LPCSTR pFolderComment, LPCSTR pDataTitle, LPCSTR pDataComment, LPCSTR pText, HANDLE hDIB)
{
	UINT	err;

#pragma warning(push)
#pragma warning(disable : 4200)
	struct MMLIBDATA {
		struct {
			char	title[64];
			char	comment[64];
		} folder;
		struct {
			char	title[64];
			char	comment[64];
		} data;
		char	text[65536];
		DWORD	size;
		char	dib[0];
	} *pMmlibData;
#pragma warning(pop)

	DWORD idInst = 0L;
	if ((err = ::DdeInitialize(&idInst,
				(PFNCALLBACK)MakeProcInstance(
				(FARPROC)DdeCallback, hInstance),
				APPCMD_CLIENTONLY, 0)) != 0) {
			MessageBoxID(pWnd, IDS_ERR_DDE_INITIALIZE, MB_OK);
			return FALSE;
	}

	HSZ	hService = ::DdeCreateStringHandle(idInst, "MMLIB", 0);
	HSZ	hTopic = ::DdeCreateStringHandle(idInst, "data", 0);

	HCONV hConv = ::DdeConnect(idInst, hService, hTopic, NULL);
	if (hConv == NULL) {
		hConv = ::DdeConnect(idInst, hService, hTopic, NULL);
		if (hConv == NULL) {
			::DdeFreeStringHandle(idInst, hService);
			::DdeFreeStringHandle(idInst, hTopic);
			::DdeUninitialize(idInst);
			idInst = 0;
			MessageBoxID(pWnd, IDS_ERR_MMLIB, MB_OK);
			return FALSE;
		}
	}

	const DWORD nDibSize = (DWORD)::GlobalSize(hDIB);
	const DWORD nDataSize = sizeof(MMLIBDATA) + nDibSize;
	pMmlibData = (MMLIBDATA *)new char[nDataSize];

	strcpy_s(pMmlibData->folder.title, pFolderTitle);
	strcpy_s(pMmlibData->folder.comment, pFolderComment);
	strcpy_s(pMmlibData->data.title, pDataTitle);
	strcpy_s(pMmlibData->data.comment, pDataComment);
	strcpy_s(pMmlibData->text, pText);
	pMmlibData->size = nDibSize;

	LPBYTE pDib = (LPBYTE)::GlobalLock(hDIB);
	memcpy(pMmlibData->dib, pDib, nDibSize);
	::GlobalUnlock(hDIB);

	const UINT uFmt = ::RegisterClipboardFormat("MMLIB_DATA");
	HDDEDATA hMmlibData = ::DdeCreateDataHandle(idInst, (LPBYTE)pMmlibData, nDataSize, 0, 0, uFmt, 0);

	HDDEDATA hData = ::DdeClientTransaction((LPBYTE)hMmlibData, (DWORD)-1, hConv, NULL, uFmt, XTYP_EXECUTE, 10000, NULL);
	if (hData != NULL)
		::DdeFreeDataHandle(hData);

	::DdeDisconnect(hConv);

	delete [] pMmlibData;

	::DdeFreeStringHandle(idInst, hService);
	::DdeFreeStringHandle(idInst, hTopic);

	::DdeUninitialize(idInst);

	return TRUE;
}
#endif	//_PRJ_RTA,_PRJ_SAS

HDDEDATA EXPENTRY DdeCallback(WORD /*wType*/, WORD /*wFmt*/, HCONV /*hConv*/, HSZ /*hszTopic*/, HSZ /*hszItem*/, HDDEDATA /*hData*/, DWORD /*lData1*/, DWORD /*lData2*/)
{
	return(0);
}

void LoadBimapStretch(CBitmap &bitmapDst, UINT nID, int nWidth, int nHeight)
{
	CBitmap bitmapSrc;
	CWnd *pWnd = CWnd::GetDesktopWindow();
	CDC *pDC = pWnd->GetDC();
	CDC dcSrc, dcDst;
	dcSrc.CreateCompatibleDC(pDC);
	dcDst.CreateCompatibleDC(pDC);

	bitmapSrc.LoadBitmap(nID);
	bitmapDst.DeleteObject();
	bitmapDst.CreateCompatibleBitmap(pDC, nWidth, nHeight);

	CBitmap *pObjSrc = dcSrc.SelectObject(&bitmapSrc);
	CBitmap *pObjDst = dcDst.SelectObject(&bitmapDst);

	BITMAP bm;
	bitmapSrc.GetBitmap(&bm);

	dcDst.StretchBlt(0, 0, nWidth, nHeight, &dcSrc, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

	dcSrc.SelectObject(pObjSrc);
	dcDst.SelectObject(pObjDst);

	pWnd->ReleaseDC(pDC);
}

int ReadWaveData(HWAVEDATA hWaveData, double *pLeftData, double *pRightData, int nData, int nOffset)
{
	if (pLeftData == NULL)
		return 0;

	if (PWAVEDATA pWaveData = (PWAVEDATA)::GlobalLock(hWaveData); pWaveData != NULL) {
		const int nBytesPerSample = pWaveData->nChannels * (pWaveData->nBitsPerSample / 8);
		const int nWaveSize = pWaveData->dataSize / nBytesPerSample;
		const int nReadSize = min(nWaveSize - nOffset, nData);

		CopyWaveToDouble((BYTE *)pWaveData->waveData + nOffset * nBytesPerSample, pLeftData, pWaveData->nChannels == 2 ? pRightData : NULL, nReadSize, pWaveData->nBitsPerSample);

		::GlobalUnlock(hWaveData);

		if (nReadSize < nData) {
			const int nClear = nData - nReadSize;

			memset(pLeftData + nReadSize, 0, sizeof(double) * nClear);
			if (pRightData != NULL)
				memset(pRightData + nReadSize, 0, sizeof(double) * nClear);
		}

		return nReadSize;
	} else
		return 0;
}

HWAVEDATA MakeWaveDataHandle(int nSamplesPerSec, int nBitsPerSample, int nChannel, int nDataNum, const double *pLeftData, const double *pRightData, BOOL bNormalize)
{
	const int nBytes = nDataNum * (nBitsPerSample / 8) * (pRightData == NULL ? 1 : nChannel);
	HWAVEDATA hWaveData = ::GlobalAlloc(GHND, sizeof(WAVEDATA) + nBytes);
	if (PWAVEDATA pWaveData = (PWAVEDATA)::GlobalLock(hWaveData); pWaveData!= NULL) {
		pWaveData->nChannels = (WORD)nChannel;
		pWaveData->nBitsPerSample = (WORD)nBitsPerSample;
		pWaveData->nSamplesPerSec = nSamplesPerSec;
		pWaveData->dataSize = nBytes;

		double fMul;
		if (bNormalize) {
			const double fMaxData = GetMaxData(pLeftData, pRightData, nDataNum);
			fMul = fMaxData == 0 ? 0 : 1 / fMaxData;
		} else
			fMul = 1.0;

		CopyWaveFromDouble(pWaveData->waveData, pLeftData, pRightData, nDataNum, nBitsPerSample, fMul);

		::GlobalUnlock(hWaveData);
	}

	return hWaveData;
}

void OpenURL(UINT nID)
{
	CString str;

	str.LoadString(nID);
	::ShellExecute(NULL, "open", str, NULL, NULL, SW_SHOWNORMAL);
}

void OpenURL(LPCSTR pUrl)
{
	::ShellExecute(NULL, "open", pUrl, NULL, NULL, SW_SHOWNORMAL);
}

typedef struct {
  WORD      dlgVer;
  WORD      signature;
  DWORD     helpID;
  DWORD     exStyle;
  DWORD     style;
  WORD      cDlgItems;
  short     x;
  short     y;
  short     cx;
  short     cy;
  short menu;
  short windowClass;
  short     title[1];
} DLGTEMPLATEEX;

DLGTEMPLATE *GetDialogResource(UINT nID, int nFontSize)
{
	DLGTEMPLATEEX* pDlg = NULL;
	HRSRC hResFind = ::FindResource(NULL, MAKEINTRESOURCE(nID), RT_DIALOG);
	if (hResFind) {
		HGLOBAL hResData = ::LoadResource(NULL, hResFind);
		if (hResData) {
			pDlg = (DLGTEMPLATEEX*)LockResource(hResData);

			if (nFontSize != 0) {
				int i;
				for (i = 0; pDlg->title[i] != 0; i++)
					;

				if (pDlg->title[i + 1] != nFontSize)
					pDlg->title[i + 1] = (short)nFontSize;
			}
		}
	}
	return (DLGTEMPLATE *)pDlg;
}

#if defined(_PRJ_RTA) || defined(_PRJ_SAS)
void MakeWindowFunc(int type, int size, double *data, double fMul)
{
	double m = fMul / size;
	double fTemp;

	switch (type) {
	case WF_RECTANGLAR:
		break;
	case WF_TRIANGLAR:
		m *= sqrt(3.0);
		break;
	case WF_HAMMING:
		m /= sqrt(0.3974);
		break;
	case WF_HANNING:
		m /= sqrt(0.375);
		break;
	case WF_BLACKMAN:
		m /= sqrt(0.3046);
		break;
	case WF_BLACKMANHARRIS:
		m /= sqrt(0.257963355);
		break;
	case WF_FLATTOP:
		m /= sqrt(3.770284);
		break;
	}

	const int size1 = size - 1;

	for (int i = 0; i < size; i++) {
		switch (type) {
		case WF_RECTANGLAR:
			*data++ = m;
			break;
		case WF_TRIANGLAR:
			*data++ = (double)(1.0 - abs(size / 2 - i) / (double)(size / 2)) * m;
			break;
		case WF_HAMMING:
			*data++ = (double)(0.54 - 0.46 * cos(2 * M_PI * i / (size1))) * m;
			break;
		case WF_HANNING:
			*data++ = (double)(0.5 - 0.5 * cos(2 * M_PI * i / (size1))) * m;
			break;
		case WF_BLACKMAN:
			fTemp = 2 * M_PI * i / size1;
			*data++ = (double)(0.42 - 0.5 * cos(fTemp) + 0.08 * cos(2 * fTemp)) * m;
			break;
		case WF_BLACKMANHARRIS:
			fTemp = 2 * M_PI * i / size1;
			*data++ = (double)(0.35875 - 0.48829 * cos(fTemp) + 0.14128 * cos(2 * fTemp) - 0.01168 * cos(3 * fTemp)) * m;
			break;
		case WF_FLATTOP:
			fTemp = 2 * M_PI * i / size1;
			*data++ = (double)(1 - 1.93 * cos(fTemp) + 1.29 * cos(2 * fTemp) - 0.388 * cos(3 * fTemp) + 0.032 * cos(4 * fTemp)) * m;
			break;
		}
	}
}
#endif	//_PRJ_RTA,_PRJ_SAS

void ScreenShot(HWND hWnd)
{
	HWND hWndTop;
	hWnd = CWnd::GetSafeOwner_(hWnd, &hWndTop);
	if (hWnd == NULL)
		return;

	CWnd *pWnd = CWnd::FromHandle(hWnd);
	if (pWnd == NULL)
		return;

	CRect rect;
	pWnd->GetWindowRect(&rect);
	const int nWidth = rect.Width();
	const int nHeight = rect.Height();

	CDC *pDC = pWnd->GetWindowDC();
	CDC DCMem;
	DCMem.CreateCompatibleDC(pDC);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, nWidth, nHeight);

	CGdiObject *pOrg = DCMem.SelectObject(&bmp);
	DCMem.BitBlt(0, 0, nWidth, nHeight, pDC, 0, 0, SRCCOPY);
	DCMem.SelectObject(pOrg);
	pWnd->ReleaseDC(pDC);

	HDIB hDIB;
	MMPictureGetBitmap(&hDIB, bmp);

	CString filter, title;
	filter = "Image File (*.png)|*.png||";
	CFileDialog dlg(FALSE, "png", NULL, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, filter, pWnd, 0);

	title.LoadString(IDS_SAVE_IMAGE_FILE);
	dlg.m_ofn.lpstrTitle = title;
	if (dlg.DoModal() == IDOK)
		MMPictureWriteFile(hDIB, (LPSTR)(LPCSTR)dlg.GetPathName());

	::GlobalFree(hDIB);
}

void DispContextHelp(const HELPINFO* pHelpInfo, const UINT *pIDs)
{
	UINT nHelpId = 0;
	UINT nCtrlId = 0;

	if (pHelpInfo->iCtrlId == ID_ZOOM_H || pHelpInfo->iCtrlId == ID_ZOOM_V) {
		int nIndex;
		POINT pt = pHelpInfo->MousePos;
		::ScreenToClient((HWND)pHelpInfo->hItemHandle, &pt);
		nIndex = (int)::SendMessage((HWND)pHelpInfo->hItemHandle, TB_HITTEST, 0, (LPARAM)&pt);
		if (nIndex >= 0)
			nCtrlId = pHelpInfo->iCtrlId + nIndex + 1;
	} else
		nCtrlId = pHelpInfo->iCtrlId;

	while (*pIDs) {
		if (*pIDs == nCtrlId) {
			nHelpId = pIDs[1];
			break;
		}

		pIDs += 2;
	}

	if (nHelpId != 0) {
		CString sHelpFile;

		HH_POPUP hp{
			.cbStruct = sizeof(HH_POPUP),
			.idString = nHelpId,
			.pt{
				.x = pHelpInfo->MousePos.x,
				.y = pHelpInfo->MousePos.y
			},
			.clrForeground = RGB(0, 0, 0),
			.clrBackground = (COLORREF)-1,
			.rcMargins{
				.left = -1,
				.top = -1,
				.right = -1,
				.bottom = -1
			},
#ifdef _LANG_JPN
			.pszFont = "ÉÅÉCÉäÉI,9"
#endif
		};
		sHelpFile.LoadString(9000/*IDS_HELP_FILE*/);
		if (!sHelpFile.IsEmpty()) {
			CString sHelpPath;
			sHelpPath.Format("%s\\%s::/ContextHelp.txt", (LPCSTR)GetExeDir(), (LPCSTR)sHelpFile);
			::HtmlHelp((HWND)pHelpInfo->hItemHandle, sHelpPath, HH_DISPLAY_TEXT_POPUP, (DWORD_PTR)&hp);
		}
	}
}

int ConvertToInt(double fData, int nBits)
{
	const double fMax = PowDouble(nBits);
	fData *= fMax - 1;

	if (fData > fMax - 1)
		fData = fMax - 1;
	else if (fData < -fMax)
		fData = -fMax;

	return (int)floor(fData + 0.5);
}

void CopyWaveFromDouble(void *pWaveData, const double *pLeftData, const double *pRightData, int nDataNum, int nBits, double fMul)
{
	BYTE *bp;
	short *sp;
	float *fp;
	int i;
	union {
		int i;
		BYTE b[4];
	} ib;

	switch (nBits) {
	case 16:
		sp = (short *)pWaveData;
		for (i = 0; i < nDataNum; i++) {
			*sp++ = (short)ConvertToInt(*pLeftData++ * fMul, 16);
			if (pRightData != NULL)
				*sp++ = (short)ConvertToInt(*pRightData++ * fMul, 16);
		}
		break;
	case 24:
		bp = (BYTE *)pWaveData;
		for (i = 0; i < nDataNum; i++) {
			ib.i = ConvertToInt(*pLeftData++ * fMul, 24);
			*bp++ = ib.b[0];
			*bp++ = ib.b[1];
			*bp++ = ib.b[2];

			if (pRightData != NULL) {
				ib.i = ConvertToInt(*pRightData++ * fMul, 24);
				*bp++ = ib.b[0];
				*bp++ = ib.b[1];
				*bp++ = ib.b[2];
			}
		}
		break;
	case 32:
		fp = (float *)pWaveData;
		for (i = 0; i < nDataNum; i++) {
			*fp++ = (float)(*pLeftData++ * fMul);
			if (pRightData != NULL)
				*fp++ = (float)(*pRightData++ * fMul);
		}
		break;
	}
}

void CopyWaveToDouble(const void *pWaveData, double *pLeftData, double *pRightData, int nDataNum, int nBits)
{
	BYTE *bp;
	short *sp;
	float *fp;
	int i;
	union {
		int i;
		BYTE b[4];
	} ib;

	switch (nBits) {
	case 8:
		bp = (BYTE *)pWaveData;
		for (i = 0; i < nDataNum; i++) {
			*pLeftData++ = (char)(*bp++ - 128) / PowDouble(8);
			if (pRightData != NULL)
				*pRightData++ = (char)(*bp++ - 128) / PowDouble(8);
		}
		break;
	case 16:
		sp = (short *)pWaveData;
		for (i = 0; i < nDataNum; i++) {
			*pLeftData++ = *sp++ / PowDouble(16);
			if (pRightData != NULL)
				*pRightData++ = *sp++ / PowDouble(16);
		}
		break;
	case 24:
		bp = (BYTE *)pWaveData;
		ib.b[0] = 0;
		for (i = 0; i < nDataNum; i++) {
			ib.b[1] = *bp++;
			ib.b[2] = *bp++;
			ib.b[3] = *bp++;
			*pLeftData++ = ib.i / PowDouble(32);

			if (pRightData != NULL) {
				ib.b[1] = *bp++;
				ib.b[2] = *bp++;
				ib.b[3] = *bp++;
				*pRightData++ = ib.i / PowDouble(32);
			}
		}
		break;
	case 32:
		fp = (float *)pWaveData;

		double fAdj = 1;
		for (i = 0; i < nDataNum; i++) {
			if (fp[i] > 1.0) {
				fAdj = 1.0 / PowDouble(16);
				break;
			}
		}

		for (i = 0; i < nDataNum; i++) {
			*pLeftData++ = *fp++ * fAdj;
			if (pRightData != NULL)
				*pRightData++ = *fp++ * fAdj;
		}
		break;
	}
}

double GetMaxData(const double *pData, int nData, int nAlign)
{
	double fMaxData = 0;

	if (pData != NULL) {
		for (int i = 0; i < nData; i++) {
			const double fAbs = fabs(*pData);
			pData += nAlign;

			if (fAbs > fMaxData)
				fMaxData = fAbs;
		}
	}

	return fMaxData;
}

double GetMaxData(const double *pLeftData, const double *pRightData, int nData)
{
	const double fLeftMaxData = GetMaxData(pLeftData, nData, 1);
	const double fRightMaxData = GetMaxData(pRightData, nData, 1);

	return max(fLeftMaxData, fRightMaxData);
}

CString GetExeDir()
{
	char szExePath[MAX_PATH];

	::GetModuleFileName(NULL, szExePath, sizeof(szExePath));
	::PathRemoveFileSpec(szExePath);
	return szExePath;
}

void SetFileType(LPCSTR ext, LPCSTR docname, LPCSTR doctype, LPCSTR exepath)
{
	CRegKey reg;

	if (reg.SetValue(HKEY_CLASSES_ROOT, ext, docname) != ERROR_SUCCESS)
		return;

	CString dname = docname;
	reg.SetValue(HKEY_CLASSES_ROOT, dname, doctype);
	reg.SetValue(HKEY_CLASSES_ROOT, dname + "\\shell", "open");
	reg.SetValue(HKEY_CLASSES_ROOT, dname + "\\shell\\open\\command", exepath);

	::SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
}
