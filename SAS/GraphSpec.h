#pragma once

#include "FreqWnd.h"
#include "FileIO.h"

class CGraphSpec : public CDialogAF
{
public:
	CGraphSpec(CWnd* pParent = NULL);   // 標準のコンストラクタ

	HANDLE m_hWaveData;
	AcfCondition *m_pAcfCondition;
	double m_fRate;
	int m_nStep;

	void ReDraw(int nSel);

protected:
	enum { IDD = IDD_GRAPH_FREQ };

	CFreqWnd m_cGraphRight;
	CFreqWnd m_cGraphLeft;
	double *m_pWaveLeft;
	double *m_pWaveRight;
	double *m_pSpecLeft;
	double *m_pSpecRight;
	double *m_pWindowFunc;
	int m_nChannels;
	int m_nData;
	int m_nFftSize;
	BOOL m_bReDraw;

	void DispGraphWindow(BOOL bCalc);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnCancel() {};
	virtual void OnOK() {};
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnMean();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
