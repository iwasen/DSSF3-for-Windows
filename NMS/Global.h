#pragma once

#include "fft.h"
#include "SetData.h"

extern CSetData g_oSetData;
extern HICON g_hIcon;
extern CString g_sTempDir;
extern int g_nTempCounter;
extern class CSplWnd *g_pSplWnd;
extern class CPeakLevelWnd *g_pPeakLevelWnd;
extern class CNmsFactor *g_pNmsFactor;
extern class CManualMode *g_pManualWnd;

extern void SetInputDevice(int nInputDevice, int nInputSelector, int nInputVolume);
