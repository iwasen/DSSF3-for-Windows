#pragma once

#include "mtfwnd.h"
#include "MyCtrl.h"
#include "SetData.h"

class CGraphMTF : public CDialogAF
{
public:
	CGraphMTF(CWnd* pParent = NULL);   // 標準コンストラクタ

	class CImpFactor *m_pParent;
	DbImpulseRec *m_pDbImpulseRec;
	DbAcParamResult *m_pDbAcParamResult;
	double *m_pIRLeft;
	double *m_pIRRight;
	double m_fRate;
	int m_nData;

	void ReDraw();

protected:
	enum { IDD = IDD_GRAPH_MTF };

	double *m_pMtfLeft;
	double *m_pMtfRight;
	BOOL m_bReDraw;
	CMtfWnd m_cMtfLeft;
	CMtfWnd m_cMtfRight;
	CMyEdit m_cSTIL;
	CMyEdit m_cSTIR;
	CMyEdit m_cSTIML;
	CMyEdit m_cSTIMR;
	CMyEdit m_cSTIFL;
	CMyEdit m_cSTIFR;
	CMyEdit m_cRASTIL;
	CMyEdit m_cRASTIR;

	void DispGraphWindow();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
