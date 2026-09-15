#pragma once

#include "ImpWnd.h"
#include "MyCtrl.h"
#include "FileIO.h"
#include "mmdb.h"
#include "DialogEx.h"
#include "ToolBarEx.h"

class CGraphIR : public CDialogExt
{
public:
	CGraphIR(CWnd* pParent = NULL);   // 標準のコンストラクタ

	class CImpFactor *m_pParent;
	DbImpulseRec *m_pDbImpulseRec;
	DbAcParamRec *m_pDbAcParamRec;
	double *m_pIRLeft;
	double *m_pIRRight;
	HWAVEDATA m_hWaveData;
	double m_fRate;
	int m_nData;
	double m_fStartTime;
	double m_fDispTime;

	void ReDraw();

protected:
	enum { IDD = IDD_GRAPH_IR };

	CMyEdit	m_cZoomH;
	CMyEdit	m_cRightT1;
	CMyEdit	m_cRightT0;
	CMyEdit	m_cLeftT1;
	CMyEdit	m_cLeftT0;
	CButton	m_cChange;
	CScrollBar	m_cScrollH;
	CImpWnd	m_cGraphRight;
	CImpWnd	m_cGraphLeft;
	CToolBarEx m_tbZoomH;
	double m_fTotalTime;
	int m_nScrollPos;
	int m_nScrollSize;
	BOOL m_bReDraw;

	void DispGraphWindow();
	static void CALLBACK LeftGraphCallBack(WPARAM code, LPARAM data, double param1, double param2);
	static void CALLBACK RightGraphCallBack(WPARAM code, LPARAM data, double param1, double param2);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnCancel() {};
	virtual void OnOK() {};
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnZoomFullH();
	afx_msg void OnZoomInH();
	afx_msg void OnZoomOutH();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnChange();
	afx_msg void OnChangeLeftT0();
	afx_msg void OnChangeLeftT1();
	afx_msg void OnChangeRightT0();
	afx_msg void OnChangeRightT1();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
