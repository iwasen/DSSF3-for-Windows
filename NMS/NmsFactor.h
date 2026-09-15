#pragma once

#include "MyCtrl.h"
#include "GraphFactor.h"
#include "NmsItem.h"
#include "DialogML.h"
#include "PlaybackBtn.h"
#include "mmdb.h"

class CNmsFactor : public CDialogML
{
public:
	CNmsFactor(CWnd* pParent = NULL);   // 標準のコンストラクタ
	~CNmsFactor();

	static void OpenFactorWnd(const CNmsItem *pNmsItem);
	static void CloseFactorWnd();

protected:
	enum { IDD = IDD_NMS_FACTOR };

	CMyTab	m_cGraph;
	CListCtrl	m_cFactorList;
	CPlaybackBtn m_cPlayBackBtn;

	const NmsConditionData *m_pNmsConditionData;
	NmsFactorData *m_pNmsFactorData;
	int m_nNmsFactorData;
	AcfFactor *m_pAcfFactor;
	IAcfFactor *m_pIAcfFactor;
	CGraphFactor *m_pGraphPhi0;
	CGraphFactor *m_pGraphTauE;
	CGraphFactor *m_pGraphTau1;
	CGraphFactor *m_pGraphPhi1;
	CGraphFactor *m_pGraphSPL;
	CGraphFactor *m_pGraphIACC;
	CGraphFactor *m_pGraphTIACC;
	CGraphFactor *m_pGraphWIACC;
	int m_nCurrentPage;
	int m_nSortItem;
	int m_nSortDir;
	int m_nIndex;
	int m_nCurrentDataNo;
	HWAVEDATA m_hWaveData;

	void ShowFactor(const CNmsItem *pNmsItem);
	void DispGraph();
	void InitNmsFactorList();
	void DispNmsFactorList();
	void DeleteGraphWindow();
	void FreeBuffers();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeGraph(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedFactorList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPlayBack();
	afx_msg LRESULT OnSelectData(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
