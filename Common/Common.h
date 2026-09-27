#pragma once

#include "pcscl.h"
#include "mmdb.h"
#include "DllUxTheme.h"
#include <ddeml.h>

// ビルドバージョン
//#define BUILD_DEV
#define BUILD_REL

// ユーザ定義ウィンドウメッセージ
#define	WM_WAVE_IN			(WM_APP + 0)
#define	WM_WAVE_OUT			(WM_APP + 1)
#define WM_SG_CHANGE		(WM_APP + 2)
#define WM_TAB_NOTIFY		(WM_APP + 3)
#define WM_SELECT_DATA		(WM_APP + 4)
#define WM_CHECK_LICENSE1	(WM_APP + 5)
#define WM_CHECK_LICENSE2	(WM_APP + 6)
#define WM_REGIST_LICENSE	(WM_APP + 7)
#define WM_KEYBOARD			(WM_APP + 8)
#define WM_TAB_DRAG			(WM_APP + 9)
#define WM_FREQ_LEVEL_LEFT	(WM_APP + 10)
#define WM_FREQ_LEVEL_RIGHT	(WM_APP + 11)
#define WM_TAB_DBLCLICK		(WM_APP + 12)
#define WM_ASIO_NOTIFY_IN	(WM_APP + 13)
#define WM_ASIO_NOTIFY_OUT	(WM_APP + 14)
#define WM_CHANGE_STATUS	(WM_APP + 15)
#define	WM_DATA_RECORD		(WM_APP + 16)
#define	WM_TAB_INIT_DIALOG	(WM_APP + 17)
#define	WM_CLOSE_ACF_REALTIME_DIALOG	(WM_APP + 18)
#define	WM_DISP_ACF_REALTIME	(WM_APP + 19)
#define	WM_MYCTRL_RBUTTONDOWN	(WM_APP + 20)
#define WM_CHANGE_FONT_SIZE	(WM_APP + 21)
#define WM_AUTO_REC_FILE	(WM_APP + 22)
#define WM_RIGHT_BUTTON		(WM_APP + 23)
#define WM_MARKER_CURSOR	(WM_APP + 24)
#define WM_SELECT_MARKER	(WM_APP + 25)
#define WM_GRAPH_DRAG		(WM_APP + 26)
#define WM_RESIZE			(WM_APP + 27)
#define WM_CLOSE_GRAPH		(WM_APP + 28)
#define WM_WASAPI_NOTIFY_IN	(WM_APP + 29)
#define WM_WASAPI_NOTIFY_OUT	(WM_APP + 30)
#define WM_NOTIFY_DEVICE_CHANGE	(WM_APP + 31)
#define WM_ASIO_CLOSE_WAVEIN	(WM_APP + 32)
#define WM_ASIO_CLOSE_WAVEOUT	(WM_APP + 33)

// 窓関数
#define WF_RECTANGLAR	0
#define WF_TRIANGLAR	1
#define WF_HAMMING		2
#define WF_HANNING		3
#define WF_BLACKMAN		4
#define WF_BLACKMANHARRIS	5
#define WF_FLATTOP		6
#define WF_NUM			7

// CImpWnd クラスコールバックコード
#define CC_SELECT_SET	0
#define CC_SELECT_START	1
#define CC_SELECT_END	2
#define CC_SELECT_MOVE	3
#define CC_SCROLL		4
#define CC_DT1_START	5
#define CC_DT1_END		6
#define CC_DT1_MOVE		7
#define CC_DBLCLK		8
#define CC_RBUTTONDOWN	9

#define SPL_MAXREF		-1
#define SPL_ABSOLUTE	-2

#define FREQ_ALL		0
#define FREQ_AFILTER	-1

// インパルス応答測定方法
#define METHOD_MSEQ			0
#define METHOD_TSP			1

#define STAGE_OFFSET		10
#define STAGE_NUM			10

#define SA_IR_TITLE			"Impulse Response (RA)"
#define SA_ACF_TITLE		"ACF (RA)"

// 対数変換
#define dB10(x)	(log10((double)(x)) * 10)
#define dB20(x)	(log10((double)(x)) * 20)

// べき乗
#define PowDouble(v) ((double)((unsigned long)1 << (v - 1)))

// Wave入力／出力
#define WAVE_IN		0x01
#define WAVE_OUT	0x02

// ズームボタンID
#define ID_ZOOM_H			30000
#define ID_ZOOM_H_FULL		30001
#define ID_ZOOM_H_IN		30002
#define ID_ZOOM_H_OUT		30003
#define ID_ZOOM_V			30010
#define ID_ZOOM_V_FULL		30011
#define ID_ZOOM_V_IN		30012
#define ID_ZOOM_V_OUT		30013

#define MEMCLEAR(ptr,size)	memset(ptr,0,sizeof(*(ptr))*size)
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define PRODUCT_CODE	"DSSF3"

// ライセンスチェック
//#define GET_CHECK_DATA GetCheckData1()
#define GET_CHECK_DATA \
		((GetCheckData1() + 19580414) ^ 0x3f81f9ca)
#define CHECK_LICENSE0(_app_id, _product_version) \
		(CheckLicense1(g_sSystemName, g_hIcon, m_pRegistryName, (_app_id << 8), TRUE, PRODUCT_CODE, _product_version) == m_nCheckData ? TRUE : FALSE)
#define CHECK_LICENSE1(_app_id) \
		::AfxGetMainWnd()->SendMessage(WM_CHECK_LICENSE1, (WPARAM)(_app_id << 8))
#define CHECK_LICENSE2(_app_id) \
		::AfxGetMainWnd()->SendMessage(WM_CHECK_LICENSE2, (WPARAM)(_app_id << 8))
#define REGIST_LICENSE() \
		AfxGetMainWnd()->SendMessage(WM_REGIST_LICENSE)

struct SGraphDrag {
	double fStartX;
	double fStartY;
	double fEndX;
	double fEndY;
};

typedef void (CALLBACK *PFNGRAPH)(WPARAM, LPARAM, double, double);

extern const char g_sSystemName[];
extern const char g_sRegistryName[];
extern CDllUxTheme g_dllUxTheme;

#if defined(_PRJ_RTA) || defined(_PRJ_NMS)
extern void ResetSamplingRate();
extern void SetSamplingRateList(CComboBox &combo, int nInOut, INT_PTR nDeviceIn, INT_PTR nDeviceOut, int &nDefault);
#endif	// _PRJ_RTA,_PRJ_NMS
#if defined(_PRJ_RTA)
extern int GetSamplingRateIndex(int nSamplingRate);
#endif	// _PRJ_RTA
extern void SetFilterNameList(CComboBox &combo);
extern double GetCeilValue(double fValue);
extern int MessageBoxID(CWnd *pWnd, UINT nIDPrompt, UINT nType);
extern BOOL ExecMMLib(CWnd *pWnd);
extern BOOL CheckMMLib();
extern BOOL SendMMLib(CWnd *pWnd, LPCTSTR pFolderTitle, LPCTSTR pFolderComment, LPCTSTR pDataTitle, LPCTSTR pDataComment, LPCTSTR pText, HANDLE hDIB);
extern void LoadBimapStretch(CBitmap &bitmapDst, UINT nID, int nWidth, int nHeight);
extern int ReadWaveData(HWAVEDATA hWaveData, double *pLeftData, double *pRightData, int nData, int nOffset = 0);
extern HWAVEDATA MakeWaveDataHandle(int nSamplesPerSec, int nBitsPerSample, int nChannel, int nDataNum, const double *pLeftData, const double *pRightData, BOOL bNormalize = FALSE);
extern void OpenURL(UINT nID);
extern void OpenURL(LPCTSTR pUrl);
extern DLGTEMPLATE *GetDialogResource(UINT nID, int nFontSize);
extern void MakeWindowFunc(int type, int size, double *data, double fMul = 1);
extern void ScreenShot(HWND hWnd);
extern void DispContextHelp(const HELPINFO* pHelpInfo, const UINT *pIDs);
extern int ConvertToInt(double fData, int nBits);
extern void CopyWaveFromDouble(void *pWaveData, const double *pLeftData, const double *pRightData, int nDataNum, int nBits, double fMul = 1.0);
extern void CopyWaveToDouble(const void *pWaveData, double *pLeftData, double *pRightData, int nDataNum, int nBits);
extern double GetMaxData(const double *pData, int nData, int nAlign = 1);
extern double GetMaxData(const double *pLeftData, const double *pRightData, int nData);
extern CString GetExeDir();
extern void SetFileType(LPCTSTR ext, LPCTSTR docname, LPCTSTR doctype, LPCTSTR exepath) ;
