#pragma once

#include "AcfWnd.h"
#include "MyCtrl.h"
#include "SetData.h"
#include "DialogEx.h"
#include "ToolBarEx.h"

class CGraphACF : public CDialogExt
{
public:
	CGraphACF(CWnd* pParent = NULL);   // 標準のコンストラクタ

	HWAVEDATA m_hWaveData;
	AcfCondition *m_pAcfCondition;
	IAcfCondition *m_pIAcfCondition;
	NmsMicCal *m_pNmsMicCal;
	AcfFactor *m_pAcfFactor;
	double m_fRate;
	int m_nStep;
	double m_fStartTime;
	double m_fDispTime;

	void ReDraw(int nStep);

protected:
	enum { IDD = IDD_GRAPH_ACF };

	CMyEdit	m_cZoomH;
	CScrollBar	m_cScrollH;
	CAcfWnd	m_cGraphAcf;
	BOOL	m_bLog;
	CToolBarEx m_tbZoomH;
	double *m_pAcfData;
	double *m_pAcfDataLog;
	int m_nData;
	double m_fTotalTime;
	int m_nScrollPos;
	int m_nScrollSize;
	BOOL m_bReDraw;

	void CalcGraphWindow();
	void DispGraphWindow();
	static void CALLBACK GraphCallBack(WPARAM code, LPARAM data, double param1, double param2);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	afx_msg void OnZoomFullH();
	afx_msg void OnZoomInH();
	afx_msg void OnZoomOutH();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnLog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
