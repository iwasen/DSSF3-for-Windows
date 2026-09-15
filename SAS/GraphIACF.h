#pragma once

#include "IACCWnd.h"
#include "SetData.h"

class CGraphIACF : public CDialogAF
{
public:
	CGraphIACF(CWnd* pParent = NULL);   // 標準のコンストラクタ

	double m_fRate;
	HWAVEDATA m_hWaveData;
	AcfCondition *m_pAcfCondition;
	IAcfCondition *m_pIAcfCondition;
	NmsMicCal *m_pNmsMicCal;
	IAcfFactor *m_pIAcfFactor;
	int m_nStep;

	void ReDraw(int nStep);

protected:
	enum { IDD = IDD_GRAPH_IACF };

	CIACCWnd	m_cGraphIACF;
	int m_nData;
	double *m_pIAcfData;
	BOOL m_bReDraw;

	void CalcGraphWindow();
	void DispGraphWindow();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
