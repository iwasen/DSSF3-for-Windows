#pragma once

#include "FFT.h"
#include "FileIO.h"
#include "MyCtrl.h"
#include "CsvFile.h"
#include "AverageBuf.h"

#define MAX_3DD	32
#define MAX_OCT	(MAX_OCT3*8)

#define VM_SINGLE	0
#define VM_OVERLAY	1
#define VM_SPLIT	2

#define CRF_RANGE	10

#define CH_MONO			0
#define CH_STEREO		1
#define CH_LEFT_ONLY	2
#define CH_RIGHT_ONLY	3
#define CH_DIFFERENCE	4

struct FFTDATA
{
	double *m_pPowerSpecBuf;
	CAverageBuf m_oPowerSpecBuf;
	double m_fFftAllPower;
	POINT *m_pPwsPoint;
	int m_nPwsPointCount;
	int m_nPwsDispFreq;
	int m_nPwsFreqPos;
	int m_nPwsLevelPos;
	double *m_pPwsPeakLevel;
	POINT *m_pPwsPeakPoint;
	int m_nPwsPeakCount;
	double m_fOctData[MAX_OCT];
	BOOL m_bOctData;
	double m_fOctPeakLevel[MAX_OCT];
	int m_nOctDispBand;
	POINT *m_p3ddPoint[MAX_3DD];
	int m_n3ddPointCount[MAX_3DD];
	int m_n3ddCurrentPtr;
	double *m_pCrfBuf;
	POINT *m_pCrfPoint;
	int m_nCrfPointCount;
	int m_nCrfDispTime;
	int m_nCrfTimePos;
	int m_nCrfLevelPos;
	CBitmap *m_pSpgBmp;
	double *m_pCepBuf;
	POINT *m_pCepPoint;
	int m_nCepPointCount;
	int m_nCepDispTime;
	int m_nCepTimePos;
	int m_nCepLevelPos;
	double *m_pCepPeakLevel;
	POINT *m_pCepPeakPoint;
	int m_nCepPeakCount;
	double *m_pSpectrogramRingBuf;
};

struct FFTWINDOW
{
	BOOL m_bMasterWindow;
	CWnd *m_pWnd;
	int m_nMode;
	CBitmap *m_pBitmap;
	CBitmap *m_pBitmap2;
	int m_nWidth, m_nHeight;
	int m_nFrameLeft, m_nFrameTop, m_nFrameRight, m_nFrameBottom;
	int m_nFrameWidth, m_nFrameHeight;
	int m_nScaleWidth, m_nScaleHeight;
	int m_nScaleWidth2, m_nScaleHeight2;
	CSize m_sizeClient;
	CSize m_sizeMargin;
	int m_nStatusBarHeight;
};

class CFftWnd : public CWnd
{
public:
	CFftWnd();
	virtual ~CFftWnd();

	class CFftDlg *m_pFftDlg;
	DbMicCalRec m_oMicCalDataL;
	DbMicCalRec m_oMicCalDataR;
	BOOL m_bPeakReset;
	double m_fFreqStep;
	double m_fTimeStep2;
	int m_nOctBand;
	CString m_aFreqScale[MAX_OCT];
	FFTDATA m_aFftData[2];
	CPtrList m_oFftWindow;

	void Redraw();
	void InitFFT();
	void InitBuf();
	void ChangeDispMode(FFTWINDOW *pFftWindow);
	void GetWaveData(double *pData);
	void DispData(const FFTWINDOW *pFftWindow);
	void DispData(CWnd *pWnd);
	void SetBitmap(CWnd *pWnd);
	HBITMAP GetBitmap();
	FFTWINDOW *AddFftWindow(int nMode, CWnd *pWnd, BOOL bMasterWindow = FALSE);
	FFTWINDOW *GetFftWindow(CWnd *pWnd);
	FFTWINDOW *GetFftWindow(int nMode);
	void RemoveFftWindow(CWnd *pWnd);
	void CreateFftWindow(int nMode, RECT rectWindow);
	void CloseFftWindow(CWnd *pWnd, int nMode);
	void EraseBkgnd(CDC* pDC, CWnd *pWnd);
	void Paint(CWnd *pWnd);
	void ResetDispFreqAll();
	void SaveFftWindow();
	void SetDispFreq(const FFTWINDOW *pFftWindow, CPoint point);
	void ResetDispFreq(const FFTWINDOW *pFftWindow);
	void CsvOutput(LPCTSTR pFileName);
	void CsvInput(LPCTSTR pFileName);
	void RedrawSpg(const FFTWINDOW *pFftWindow);

protected:
	double m_fTimeStep;
	int m_nViewMode;
	int m_nCrfRange;
	CDC m_dcMem;
	CDC m_dcMem2;
	CDC m_dcMem3;
	CPen m_penLeft, m_penLeftPeak, m_penLeftLine;
	CPen m_penRight, m_penRightPeak, m_penRightLine;
	CPen m_penGreen, m_penLightGreen;
	CPen m_penPhase;
	CPen m_penCross;
	CPen m_penCrossPeak;
	CPen m_penCoherence;
	CPen m_penBlack, m_penGray, m_penLightGray;
	CPen m_penMidGray;
	CBrush m_brushLeftHold;
	CBrush m_brushRightHold;
	CBrush m_brushBlack;
	CBrush m_brushGraph;
	CBitmap m_bitmapLeft, m_bitmapPeakLeft;
	CBitmap m_bitmapRight, m_bitmapPeakRight;
	CFont m_Font, m_Font2;
	BOOL m_bLch;
	BOOL m_bRch;
	BOOL m_bClose;
	char m_bOctScaleFlag[MAX_OCT + 1];
	int m_dx, m_dy;
	int m_nMaxFreq;
	int m_nMinFreq;
	int m_nMaxLevel;
	int m_nMinLevel;
	int m_nFftSize;
	int m_nBarWidth;
	int m_nChannel;
	int m_nSamplingRate;
	int *m_p3ddCheckTbl;
	double *m_pWindowFunc;
	double *m_pFftBufL;
	double *m_pFftBufR;
	double *m_pFftBufM;
	double m_fOctTbl[MAX_OCT + 1];
	double *m_pFilterTblL;
	double *m_pFilterTblR;
	double *m_pPhaseBuf;
	double *m_pSpgLevel;
	double *m_pCoherenceBuf;
	double *m_pCrossBufRe;
	double *m_pCrossBufIm;
	double m_fLogMaxFreq;
	double m_fLogMinFreq;
	double m_fOctBarStep;
	double m_fLeftOffset, m_fRightOffset;
	double m_fSpgTimeStep;
	double *m_pLogFreqTbl;
	POINT *m_p3ddWork;
	CAverageBuf m_oCrossBufRe;
	CAverageBuf m_oCrossBufIm;
	CRFFT m_oFFT;
	BOOL m_bSmoothingReset;
	double m_fSmoothing1;
	double m_fSmoothing2;
	POINT *m_pCrsPoint;
	int m_nCrsPointCount;
	int m_nCrsDispFreq;
	int m_nCrsFreqPos;
	int m_nCrsLevelPos;
	double *m_pCrsPeakLevel;
	POINT *m_pCrsPeakPoint;
	int m_nCrsPeakCount;
	double *m_pCrsBuf;
	double m_fCrossAllPower;
	POINT *m_pPhsPoint;
	int m_nPhsPointCount;
	POINT *m_pCohPoint;
	int m_nCohPointCount;
	int m_nPhsDispFreq;
	int m_nPhsFreqPos;
	int m_nPhsLevelPos;
	int m_nCohDispFreq;
	int m_nCohFreqPos;
	int m_nCohLevelPos;
	HTHEME m_hTheme;
	BOOL m_bGetData;
	int m_nSpectrogramRingBufSize;
	int m_nSpectrogramRingBufNum;
	int m_nSpectrogramRingBufPtr;
	BOOL m_bSpectrogramRingBufRound;
	BOOL m_bValidFftData;

	void PaintSub(CDC &dc, const FFTWINDOW *pFftWindow);
	void PaintData(int nChannel, const FFTWINDOW *pFftWindow);
	void SetBitmapAll();
	void SetBitmap(FFTWINDOW *pFftWindow);
	void CalcSpectrum(const double *pFftBufL, const double *pFftBufR);
	void CalcPowerSpectrum(FFTDATA *pFftData, const double *pBuf, const double *pFilterTbl);
	void CalcCrossSpectrum(const double *pBufL, const double *pBufR);
	void FreeBuffer();
	void FreeBuffer2(const FFTWINDOW *pFftWindow);
	void SetViewMode(const FFTWINDOW *pFftWindow);
	void DrawNote(LPCTSTR pText, int nTop, int nRight);
	int DrawNoteSub(LPCTSTR pText, int x, int y, int nColor);
	void DeleteFftWindow(FFTWINDOW *pFftWindow);
	void RemoveFftWindowAll();
	void SetMinMax(const FFTWINDOW *pFftWindow);

	void SetBitmapPws(FFTWINDOW *pFftWindow);
	void DrawScalePws(const FFTWINDOW *pFftWindow, int nLeft, int nTop, int nRight, int nBottom, BOOL bAxisX, LPCTSTR pText);
	void GetWaveDataPws(const FFTWINDOW *pFftWindow);
	void CalcPws(const FFTWINDOW *pFftWindow, FFTDATA *pFftWork);
	int CalcPwsSub(const FFTWINDOW *pFftWindow, const double *pBuf, POINT *pPoint);
	void DispPwsInfo(const FFTWINDOW *pFftWindow);
	void DispPwsInfoSub(const FFTWINDOW *pFftWindow, FFTDATA *pFftData, int nMessageId);
	void PaintPws(const FFTWINDOW *pFftWindow, int nChannel);
	void PaintPwsSub(const FFTWINDOW *pFftWindow, FFTDATA *pFftData, CPen &penData, CPen &penPeak);
	void SetDispFreqPws(const FFTWINDOW *pFftWindow, CPoint point);
	void ResetDispFreqPws(const FFTWINDOW *pFftWindow);
	void CsvOutputPws(CCsvFile &oCsvFile);
	void CsvInputPws(CCsvFile &oCsvFile);

	void SetBitmapOct(FFTWINDOW *pFftWindow);
	void DrawScaleOct(const FFTWINDOW *pFftWindow, int nLeft, int nTop, int nRight, int nBottom, BOOL bAxisX, TEXTMETRIC &tm, LPCTSTR pText);
	void GetWaveDataOct(const FFTWINDOW *pFftWindow);
	void CalcOct(FFTDATA *pFftData);
	void DispOctInfo(const FFTWINDOW *pFftWindow);
	void DispOctInfoSub(const FFTWINDOW *pFftWindow, FFTDATA *pFftData, int nMessageId);
	void PaintOct(const FFTWINDOW *pFftWindow, int nChannel);
	void SetDispFreqOct(const FFTWINDOW *pFftWindow, CPoint point);
	void ResetDispFreqOct(const FFTWINDOW *pFftWindow);
	void CsvOutputOct(CCsvFile &oCsvFile);
	void CsvInputOct(CCsvFile &oCsvFile);

	void SetBitmap3dd(FFTWINDOW *pFftWindow);
	void DrawScale3dd(const FFTWINDOW *pFftWindow, int nLeft, int nTop, int nRight, int nBottom, BOOL bAxisX, LPCTSTR pText);
	void GetWaveData3dd(const FFTWINDOW *pFftWindow);
	void Calc3dd(const FFTWINDOW *pFftWindow, FFTDATA *pFftData);
	void Disp3ddInfo(const FFTWINDOW *pFftWindow);
	void Disp3ddInfoSub(const FFTWINDOW *pFftWindow, FFTDATA *pFftData, int nMessageId);
	void Paint3dd(const FFTWINDOW *pFftWindow, int nChannel);
	void Paint3ddSub(const FFTWINDOW *pFftWindow, FFTDATA *pFftData);

	void SetBitmapCrf(FFTWINDOW *pFftWindow);
	void DrawScaleCrf(const FFTWINDOW *pFftWindow, int nLeft, int nTop, int nRight, int nBottom, BOOL bAxisX, LPCTSTR pText);
	void SetBitmapCrfSub(int nLeft, int nTop, int nRight, int nBottom, BOOL bAxisX, double step, int zero, int width, int dir, int ns);
	void GetWaveDataCrf(const FFTWINDOW *pFftWindow);
	void CalcAutoCorrelation(const double *pFftBuf, double *pCrfBuf);
	void CalcLateralCorrelation(double *pFftBufRe, double *pFftBufIm);
	void CalcCrf(const FFTWINDOW *pFftWindow, FFTDATA *pFftData, int nOffset, int nDataSize);
	void DispCrfInfo(const FFTWINDOW *pFftWindow);
	void DispCrfInfoSub(const FFTWINDOW *pFftWindow, FFTDATA *pFftData, int nMessageId);
	void PaintCrf(const FFTWINDOW *pFftWindow, int nChannel);
	void PaintCrfSub(const FFTWINDOW *pFftWindow, FFTDATA *pFftData, int nOffest, CPen &penData, CPen &penLine, CPen &penPeak);
	void SetDispFreqCrf(const FFTWINDOW *pFftWindow, CPoint point);
	void ResetDispFreqCrf(const FFTWINDOW *pFftWindow);
	void CsvOutputCrf(CCsvFile &oCsvFile);
	void CsvInputCrf(CCsvFile &oCsvFile);

	void SetBitmapPhs(FFTWINDOW *pFftWindow);
	void DrawScalePhs(const FFTWINDOW *pFftWindow, int nLeft, int nTop, int nRight, int nBottom, BOOL bAxisX);
	void GetWaveDataPhs(const FFTWINDOW *pFftWindow);
	void CalcPhs(const FFTWINDOW *pFftWindow);
	int CalcPhsSub(const FFTWINDOW *pFftWindow, double *pPhaseBuf, POINT *pPoint);
	void DispPhsInfo(const FFTWINDOW *pFftWindow);
	void PaintPhs(const FFTWINDOW *pFftWindow, int nChannel);
	void SetDispFreqPhs(const FFTWINDOW *pFftWindow, CPoint point);
	void ResetDispFreqPhs(const FFTWINDOW *pFftWindow);
	void CsvOutputPhs(CCsvFile &oCsvFile);
	void CsvInputPhs(CCsvFile &oCsvFile);

	void SetBitmapSpg(FFTWINDOW *pFftWindow);
	void DrawScaleSpg(const FFTWINDOW *pFftWindow, int nLeft, int nTop, int nRight, int nBottom, BOOL bAxisX, LPCTSTR pText);
	void DrawLevelScale(const FFTWINDOW *pFftWindow, int nLeft, int nTop, int nRight, int nBottom);
	COLORREF GetLevelColor(double fLevel);
	void GetWaveDataSpg(const FFTWINDOW *pFftWindow);
	void CalcSpg(const FFTWINDOW *pFftWindow, FFTDATA *pFftData, int nTimeStep);
	void CalcSpgSub(const FFTWINDOW *pFftWindow, const double *pBuf, CBitmap *pBitmap, int nTimeStep);
	void DispSpgInfo(const FFTWINDOW *pFftWindow);
	void DispSpgInfoSub(const FFTWINDOW *pFftWindow, FFTDATA *pFftData, int nMessageId);
	void PaintSpg(const FFTWINDOW *pFftWindow, int nChannel);
	void RedrawSpgSub(const FFTWINDOW *pFftWindow, const double *pBuf, CBitmap *pBitmap, int nTimeStep, double fTime);

	void SetBitmapCrs(FFTWINDOW *pFftWindow);
	void DrawScaleCrs(const FFTWINDOW *pFftWindow, int nLeft, int nTop, int nRight, int nBottom, BOOL bAxisX);
	void GetWaveDataCrs(const FFTWINDOW *pFftWindow);
	void CalcCrs(const FFTWINDOW *pFftWindow);
	int CalcCrsSub(const FFTWINDOW *pFftWindow, const double *pBuf, POINT *pPoint);
	void DispCrsInfo(const FFTWINDOW *pFftWindow);
	void PaintCrs(const FFTWINDOW *pFftWindow, int nChannel);
	void SetDispFreqCrs(const FFTWINDOW *pFftWindow, CPoint point);
	void ResetDispFreqCrs(const FFTWINDOW *pFftWindow);
	void CsvOutputCrs(CCsvFile &oCsvFile);
	void CsvInputCrs(CCsvFile &oCsvFile);

	void SetBitmapCoh(FFTWINDOW *pFftWindow);
	void DrawScaleCoh(const FFTWINDOW *pFftWindow, int nLeft, int nTop, int nRight, int nBottom, BOOL bAxisX);
	void GetWaveDataCoh(const FFTWINDOW *pFftWindow);
	int CalcCoh(const FFTWINDOW *pFftWindow, const double *pBuf, POINT *pPoint);
	void DispCohInfo(const FFTWINDOW *pFftWindow);
	void PaintCoh(const FFTWINDOW *pFftWindow, int nChannel);
	void SetDispFreqCoh(const FFTWINDOW *pFftWindow, CPoint point);
	void ResetDispFreqCoh(const FFTWINDOW *pFftWindow);
	void CsvOutputCoh(CCsvFile &oCsvFile);
	void CsvInputCoh(CCsvFile &oCsvFile);

	void SetBitmapCep(FFTWINDOW *pFftWindow);
	void DrawScaleCep(const FFTWINDOW *pFftWindow, int nLeft, int nTop, int nRight, int nBottom, BOOL bAxisX, LPCTSTR pText);
	void GetWaveDataCep(const FFTWINDOW *pFftWindow);
	void CalcCep(const FFTWINDOW *pFftWindow, FFTDATA *pFftWork);
	int CalcCepSub(const FFTWINDOW *pFftWindow, double *pCepBuf, POINT *pPoint);
	void DispCepInfo(const FFTWINDOW *pFftWindow);
	void DispCepInfoSub(const FFTWINDOW *pFftWindow, FFTDATA *pFftData, int nMessageId);
	void PaintCep(const FFTWINDOW *pFftWindow, int nChannel);
	void PaintCepSub(const FFTWINDOW *pFftWindow, FFTDATA *pFftData, CPen &penData, CPen &penPeak);
	void SetDispFreqCep(const FFTWINDOW *pFftWindow, CPoint point);
	void ResetDispFreqCep(const FFTWINDOW *pFftWindow);
	void CsvOutputCep(CCsvFile &oCsvFile);
	void CsvInputCep(CCsvFile &oCsvFile);
	void GetThemaHandle();

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	LRESULT OnThemeChanged();
};
