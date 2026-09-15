#pragma once

#include "MyCtrl.h"
#include "FreWnd.h"
#include "FFT.h"
#include "Noise.h"
#include "AverageBuf.h"
#include "DialogEx.h"
#include "FreHoldData.h"

class CFreNoise : public CDialogExt
{
public:
	CFreNoise(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CFreNoise();

	int Initialize();
	void SetResolutionList();
	void Redraw();
	BOOL WaveOutData(double *pData);
	BOOL WaveInData(const double *pData);
	BOOL CheckDataExist();
	BOOL CheckDataHold();
	void CsvOutput(LPCTSTR pFileName);
	HBITMAP GetBitmap();
	void AddDataHold(COLORREF colorLeft, COLORREF colorRight);
	void DelDataHold(BOOL bRedraw = TRUE);
	void SaveHoldData(CFile &oFile);
	void LoadHoldData(CFile &oFile);
	void ReadCsv(CCsvFile &oCsvFile);

protected:
	enum { IDD = IDD_FRE_NOISE };

	CFreWnd m_cGraph;
	CMyEdit m_cFreqStart;
	CMyEdit m_cFreqEnd;
	CSliderCtrl m_cLevelSlider;
	CMyEdit m_cLevelEdit;
	CComboBox m_cResolution;
	CComboBox m_cAveraging;
	int m_nSamplingRate;
	int m_nChannel;
	double *m_pWaveLeft;
	double *m_pWaveRight;
	CAverageBuf m_oLeftData;
	CAverageBuf m_oRightData;
	double *m_pFreq;
	int m_nFreqStart;
	int m_nFreqEnd;
	int m_nLevel;
	int m_nWaveBufSize;
	int m_nFreqPoint;
	BOOL m_bValidData;
	CRFFT m_cFFT;
	CNoise m_cNoise;
	BOOL m_bInitialized;
	double *m_pFilterTblL;
	double *m_pFilterTblR;
	int m_nInStartDelay;
	CList<CFreHoldData, CFreHoldData&> m_oHoldDataList;

	void AllocAverageBuf();
	void InitLevelSlider();
	void SetLevel(int nLevel);
	void SetAveragingList();
	void FreeBuffers();
	void CalcFreqResponse(double *pWave, double *pData, const double *m_pFilterTbl);
	void MakeFilterTbl();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeFreqStart();
	afx_msg void OnEnChangeFreqEnd();
	afx_msg void OnEnChangeLevelEdit();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCbnSelchangeResolution();
	afx_msg void OnCbnSelchangeAveraging();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg LRESULT OnTabInitDialog(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
