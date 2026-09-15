#pragma once

#include "EchoWnd.h"
#include "ImpWnd.h"
#include "FileIO.h"
#include "MyCtrl.h"
#include "DialogEx.h"
#include "ToolBarEx.h"

class CGraphEcho : public CDialogExt
{
public:
	CGraphEcho(CWnd* pParent = NULL);   // 標準のコンストラクタ

	class CImpFactor *m_pParent;
	HWAVEDATA m_hWaveData;
	DbImpulseRec *m_pDbImpulseRec;
	DbAcParamRec *m_pDbAcParamRec;
	double *m_pIRLeft;
	double *m_pIRRight;
	double m_fRate;
	int m_nData;
	double m_fStartTime;
	double m_fDispTime;

	void ReDraw();

protected:
	enum { IDD = IDD_GRAPH_ECHO };

	CMyEdit	m_cZoomH;
	CButton	m_cRegress;
	CEchoWnd	m_cGraphEE;
	CImpWnd	m_cGraphIR;
	CButton	m_cLeftRight;
	CButton	m_cLeft;
	CButton	m_cRight;
	CEchoWnd	m_cGraphEcho;
	CScrollBar	m_cScrollH;
	int		m_iLeftRight;
	CToolBarEx m_tbZoomH;
	double *m_pEchoTime;
	double *m_pEchoEnergy;
	double *m_pEchoData;
	double *m_pEchoData2;
	double m_fTotalTime;
	int m_nScrollPos;
	int m_nScrollSize;
	BOOL m_bReDraw;
	double m_fT0;
	double m_fT1;
	double m_fDev;
	double m_fTsubNoise;
	double m_fT20Reg0, m_fT20Reg1;
	double m_fT30Reg0, m_fT30Reg1;
	double m_fEDTReg0, m_fEDTReg1;
	double m_fEndTime;

	void CalcGraphWindow();
	void DispGraphWindow();
	static void CALLBACK GraphCallBack(WPARAM code, LPARAM data, double param1, double param2);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnCancel() {};
	virtual void OnOK() {};
	afx_msg void OnZoomFullH();
	afx_msg void OnZoomInH();
	afx_msg void OnZoomOutH();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnLeftRight();
	afx_msg void OnLeft();
	afx_msg void OnRight();
	afx_msg void OnRegress();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};

/////////////////////////////////////////////////////////////////////////////
// CChangeTsubDlg ダイアログ

class CChangeTsubDlg : public CDialogAF
{
public:
	CChangeTsubDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

	CEdit	m_cTsubEnd;
	double	m_dTsubEnd;
	BOOL	m_bTsubAuto;
	double	m_dTsubNoise;

protected:
	enum { IDD = IDD_CHANGE_TSUB };

	void EnableControls();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	afx_msg void OnTsubAuto();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
