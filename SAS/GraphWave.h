#pragma once

#include "WaveWnd.h"
#include "MyCtrl.h"
#include "SetData.h"
#include "DialogEx.h"
#include "ToolBarEx.h"

class CGraphWave : public CDialogExt
{
public:
	CGraphWave(CWnd* pParent = NULL);   // 標準のコンストラクタ
	~CGraphWave();

	HANDLE m_hWaveData;
	double m_fStartTime;
	double m_fDispTime;
	double m_fStartValue;
	double m_fDispValue;
	AcfCondition *m_pAcfCondition;
	DbAcfMarkerRec **m_pAcfMarkerRec;
	int *m_nAcfMarkerRec;

	void ReDraw(int nSel);

protected:
	enum { IDD = IDD_GRAPH_FACTOR };

	CWaveWnd	m_cGraphLeft;
	CWaveWnd	m_cGraphRight;
	CScrollBar	m_cScrollH;
	CScrollBar	m_cScrollV;
	CMyEdit	m_cZoomH;
	CMyEdit	m_cZoomV;
	CToolBarEx m_tbZoomH;
	CToolBarEx m_tbZoomV;
	BOOL m_bReDraw;
	double m_fTotalTime;
	double m_fTotalValue;
	double m_fMinValue;
	int m_nHScrollPos;
	int m_nHScrollSize;
	int m_nVScrollPos;
	int m_nVScrollSize;
	int m_nZoomMode;
	int m_nSel;
	CWaveWnd	*m_pZoomGraph;
	double m_fZoomStartTime;
	double m_fZoomDispTime;
	double m_fZoomMinValue;
	double m_fZoomMaxValue;
	double *m_pTempBuf;
	int m_nData;
	int m_nChannels;
	int m_nZoomChannel;

	void DispGraphWindow();
	void SetHScroll();
	void SetVScroll();
	static void CALLBACK GraphCallBack(WPARAM code, LPARAM data, double param1, double param2);
	void DispZoomGraph(CWaveWnd *pWnd);
	void CloseZoomWnd();
	void SetZoomArea();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnZoomFullH();
	afx_msg void OnZoomInH();
	afx_msg void OnZoomOutH();
	afx_msg void OnZoomFullV();
	afx_msg void OnZoomInV();
	afx_msg void OnZoomOutV();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg LRESULT OnMarkerCursor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSelectMarker(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGraphDrag(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnResize(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCloseGraph(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
};
