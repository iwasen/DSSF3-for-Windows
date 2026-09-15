#pragma once

#include "DialogML.h"
#include "GraphFactor.h"
#include "GraphACF.h"
#include "GraphIACF.h"
#include "GraphWave.h"
#include "GraphSpec.h"
#include "ScreenCopyBtn.h"
#include "PlaybackBtn.h"

class CAcfFactor : public CDialogML
{
public:
	CAcfFactor(CWnd* pParent = NULL);   // 標準のコンストラクタ
	~CAcfFactor();

	long m_nFolderID;
	long m_nAcfID;
	class CSasView *m_pView;

	static void CloseAll();

protected:
	enum { IDD = IDD_ACF_FACTOR };

	CPlaybackBtn 	m_cPlayback;
	CScreenCopyBtn	m_cScreenCopy;
	CMyEdit	m_cCalcCycle;
	CButton m_cSelectData;
	CButton	m_cPrevData;
	CButton	m_cNextData;
	CButton m_cCsvOutput;
	CMyEdit	m_cTaueInterval;
	CMyEdit	m_cTaueEndTime;
	CMyEdit	m_cTaueEndLevel;
	CMyEdit	m_cPhi0RefLevel;
	CMyEdit	m_cPhi0RefData;
	CMyEdit	m_cIntegrationTime;
	CListCtrl	m_cFactorList;
	CMyTab	m_cGraph;
	CString	m_sFolderTitle;
	CString	m_sName;
	CString	m_sPlace;
	CString	m_sFolderComment;
	CString	m_sScale;
	CString	m_sDate;
	int		m_iSampling;
	CString	m_sMeasureTime;
	CString	m_sTitle;
	CString	m_sComment;
	CString	m_sTime;
	CString	m_sFreqFilter;
	CString	m_sChannel;
	int m_iBitsPerSample;
	int m_nColumn;
	double m_fScale;
	double m_fRate;
	DbAcfRec m_dbAcfRec;
	DbAcfFactorRec m_dbAcfFactorRec;
	BOOL m_bChangeData;
	BOOL m_bCalculated;
	double m_fAdjustPhi0;
	AcfConditionData m_AcfConditionData;
	AcfFactorData *m_pAcfFactorData;
	AcfFactor *m_pAcfFactor;
	IAcfFactor *m_pIAcfFactor;
	HWAVEDATA m_hWaveData;
	CGraphFactor *m_pGraphPhi0;
	CGraphFactor *m_pGraphTauE;
	CGraphFactor *m_pGraphTau1;
	CGraphFactor *m_pGraphPhi1;
	CGraphFactor *m_pGraphSPL;
	CGraphFactor *m_pGraphIACC;
	CGraphFactor *m_pGraphTIACC;
	CGraphFactor *m_pGraphWIACC;
	CGraphACF *m_pGraphACF;
	CGraphIACF *m_pGraphIACF;
	CGraphWave *m_pGraphWave;
	CGraphSpec *m_pGraphSpec;
	int m_nCurrentPage;
	int m_nSortItem;
	int m_nSortDir;
	static CObList m_WindowList;
	int m_nCurrentDataNo;
	int m_nListKind;
	TaunPhinData *m_pTaunPhinData;
	double m_fStartTimePhi0;
	double m_fDispTimePhi0;
	double m_fStartValuePhi0;
	double m_fDispValuePhi0;
	double m_fStartTimeTauE;
	double m_fDispTimeTauE;
	double m_fStartValueTauE;
	double m_fDispValueTauE;
	double m_fStartTimeTau1;
	double m_fDispTimeTau1;
	double m_fStartValueTau1;
	double m_fDispValueTau1;
	double m_fStartTimePhi1;
	double m_fDispTimePhi1;
	double m_fStartValuePhi1;
	double m_fDispValuePhi1;
	double m_fStartTimeIACC;
	double m_fDispTimeIACC;
	double m_fStartValueIACC;
	double m_fDispValueIACC;
	double m_fStartTimeTIACC;
	double m_fDispTimeTIACC;
	double m_fStartValueTIACC;
	double m_fDispValueTIACC;
	double m_fStartTimeWIACC;
	double m_fDispTimeWIACC;
	double m_fStartValueWIACC;
	double m_fDispValueWIACC;
	double m_fStartTimeACF;
	double m_fDispTimeACF;
	double m_fStartTimeWave;
	double m_fDispTimeWave;
	double m_fStartValueWave;
	double m_fDispValueWave;
	double m_fCurrentMarkerTime;
	DbAcfMarkerRec *m_pAcfMarkerRec;
	int m_nAcfMarkerRec;
	int m_nSelectedMarkerID;

	BOOL SetFolderInfo();
	BOOL ReadData();
	void DispGraph();
	int GetListKind();
	void InitAcfFactorList();
	void DispAcfFactorList(int nSel);
	void DispAcfFactorList1();
	void DispAcfFactorList2();
	void DispAcfFactorList3();
	void ReDispAcfFactorList();
	void DispCalcParam();
	void DeleteGraphWindow();
	void FreeBuffers();
	void EnablePrevNext();
	static int CALLBACK CompareItems1(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK CompareItems2(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK CompareItems3(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	void RedrawGraph();
	HDIB ScreenCopy();
	HDIB GraphCopy();
	CString GetDataText();
	void SaveStartDisp();
	void SelectData(double fTime);
	BOOL CalcMeasurementAcf();
	static BOOL AbortCheck(int nPercent);
	void ReadAcfMarker();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnSelectData();
	afx_msg void OnPrevData();
	afx_msg void OnNextData();
	afx_msg void OnColumnclickFactorList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeGraph(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedFactorList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPlayback();
	afx_msg void OnScreenCopy();
	afx_msg LRESULT OnSelectData(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedCsvOutput();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg LRESULT OnRightButton(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
	afx_msg void OnAddAcfMarker();
	afx_msg LRESULT OnSelectMarker(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEditAcfMarker();
	afx_msg void OnDeleteAcfMarker();
	afx_msg void OnPlaybackAcfMarker();
};
