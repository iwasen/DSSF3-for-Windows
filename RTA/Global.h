#pragma once

#include "SetData.h"
#include "WaveForm.h"
#include "WaveInEx.h"
#include "WaveOutEx.h"

#define MIN_FREQ 10.0
#define MAX_FREQ 20000.0

#define MIN_PHASE -180
#define MAX_PHASE 180

#define WINDOW_SG		0x0001
#define WINDOW_FFT		0x0002
#define WINDOW_OS		0x0004
#define WINDOW_IMP		0x0008
#define WINDOW_ACF		0x0010
#define WINDOW_DST		0x0020
#define WINDOW_REC		0x0040
#define WINDOW_FLT		0x0080
#define WINDOW_TIME		0x0100
#define WINDOW_FRE		0x0200
#define WINDOW_ALL		0xffff

#define SG_MODE_TONE	0
#define SG_MODE_NOISE	1
#define SG_MODE_SWEEP	2
#define SG_MODE_PULSE	3
#define SG_MODE_SYNTH	4
#define SG_MODE_SCALE	5
#define SG_MODE_WAVE	6

#define FFT_MODE_PWS	0
#define FFT_MODE_OCT	1
#define FFT_MODE_3DD	2
#define FFT_MODE_CRF	3
#define FFT_MODE_PHS	4
#define FFT_MODE_SPG	5
#define FFT_MODE_CRS	6
#define FFT_MODE_COH	7
#define FFT_MODE_CEP	8
#define FFT_MODE_NUM	9

#define WAVE_SIN		0
#define WAVE_TRIANGLE	1
#define WAVE_SQUARE		2
#define WAVE_SAW		3
#define WAVE_USER		4

#define NOISE_WHITE		0
#define NOISE_PINK		1
#define NOISE_BROWN		2

#define NOISE_MODE_MONO		0
#define NOISE_MODE_STEREO	1
#define NOISE_MODE_INVERSE	2

#define CH_MONO			0
#define CH_STEREO		1
#define CH_LEFT_ONLY	2
#define CH_RIGHT_ONLY	3
#define CH_DIFFERENCE	4

#define TN_CHANGE_SAMPLING_RATE	0

#define N_WAVE_FORM	4

#define COLOR_LEFT			RGB(0, 128, 255)
#define COLOR_RIGHT			RGB(255, 64, 192)
#define COLOR_LEFT_PEAK		RGB(0, 0, 255)
#define COLOR_RIGHT_PEAK	RGB(255, 0, 0)
#define COLOR_LEFT_LINE		RGB(160, 192, 255)
#define COLOR_RIGHT_LINE	RGB(255, 160, 192)
#define COLOR_LEFT_ZERO		RGB(0x33, 0x33, 0xee)
#define COLOR_RIGHT_ZERO	RGB(0xee, 0x33, 0x33)

#define COLOR_TEXT			RGB(0, 0, 0)
#define COLOR_BKGRND		::GetSysColor(COLOR_3DFACE)
#define COLOR_GRAPH			RGB(255, 255, 255)
#define COLOR_LIGHTGRAY		RGB(192, 192, 192)
#define COLOR_GRAY			RGB(128, 128, 128)
#define COLOR_BLACK			RGB(0, 0, 0)
#define COLOR_RED			RGB(255, 0, 0)
#define COLOR_GREEN			RGB(0, 192, 0)
#define COLOR_BLUE			RGB(0, 0, 255)
#define COLOR_LIGHTGREEN	RGB(160, 255, 160)

#define MAX_OCT1		12
#define MAX_OCT3		40
#define OCT_BAND_NUM	5

#define MICCAL_RIGHT(n)	(-(int)(n) - 1)

// ñ¢ìoò^éûÇÃìÆçÏêßå¿éûä‘
#define NOREG_LIMIT_TIME	10

extern CSetData g_oSetData;
extern CSetData2 g_oSetData2;
extern INT_PTR g_nWaveInDevice;
extern INT_PTR g_nWaveOutDevice;
extern HICON g_hIcon;
extern const LPCTSTR g_pFreqScale1[];
extern const LPCTSTR g_pFreqScale3[];
extern const int g_nOctBandTbl[];
extern CWaveForm g_oWaveForm;
extern const UINT g_tFftModeId[];
extern int g_nFilterUpdateCounter;
extern const int g_tFontSize[];

extern void SetWindowFuncList(CComboBox &combo);
extern void SetWaveFormList(CComboBox &combo, int nSel);
extern void StartWaveInOut(CWaveInEx *pWaveIn, CWaveOutEx *pWaveOut, BOOL bSetVolume);
extern int GetLinearScaleStep(int nMaxValue, int nMinvalue, int nScaleSize);
extern double GetLinearScaleStep(double fRange, int nPixel, int *pStep = NULL);
extern int GetLogScaleStep(int nValue);
extern double GetLogScaleStep(double fValue);
extern int GetScaleStartValue(int nMinvalue, int nStep);
extern double GetScaleStartValue(double fMinValue, double fStep);
extern CString GetDirName(const CString &filePath);
extern CString GetFileName(CString &filePath);
extern CString &GetString(UINT nID);
extern BOOL SelectListBox(CComboBox &ctl, DWORD_PTR dwData);
extern BOOL SelectListBox(CListBox &ctl, DWORD_PTR dwData);
extern CString GetNumberString(int nNumber);
extern CString GetNumberString(double fNumber);
extern BOOL CreateShortCut(LPCSTR lpszShortcutFile, LPCSTR lpszDescription, LPCSTR lpszTargetFile, LPCSTR pszParameter);
extern void CheckFileName(CString &fileName);
extern void GetAxisName(int nMode, CString &sNameX, CString &sNameY);
extern void GetAxisUnit(int nMode, CString &sUnitX, CString &sUnitY);
extern LPCSTR GetRunningStatus(int nStringId, int nInputBits, int nOutputBits, int nRecordBits);
extern double GetDither(int nBitsPerSample);
extern void SetBackupList(CListCtrl &cListBackup, CStringArray &oBackupList);
extern CString Hankaku2Zenkaku(LPCSTR str);
