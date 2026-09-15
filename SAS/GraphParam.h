#pragma once

#include "ParamWnd.h"

class CGraphParam : public CDialogAF
{
public:
	CGraphParam(CWnd* pParent = NULL);   // 標準のコンストラクタ

	DbImpulseRec *m_pDbImpulseRec;
	DbAcParamRec *m_pDbAcParamRec;
	double m_fAdjustSPL;
	class CImpFactor *m_pParent;

	void ReDraw();

protected:
	enum { IDD = IDD_GRAPH_PARAM };

	CParamWnd	m_cGraphParam;
	int		m_iParamItem;
	CButton	m_cParamIACC;
	CButton	m_cParamTIACC;
	CButton	m_cParamWIACC;
	int *m_pDataFreq;
	double *m_pDataAll;
	double *m_pDataLeft;
	double *m_pDataRight;
	BOOL m_bReDraw;

	void DispParamWindow();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnCancel() {};
	virtual void OnOK() {};
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnParamItem();
	afx_msg void OnDestroy();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
