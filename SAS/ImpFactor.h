#pragma once

#include "DialogML.h"
#include "FileIO.h"
#include "MSeq.h"
#include "TSP.h"
#include "GraphIR.h"
#include "GraphEcho.h"
#include "GraphIACC.h"
#include "GraphFreq.h"
#include "GraphParam.h"
#include "GraphPref.h"
#include "GraphMTF.h"
#include "ScreenCopyBtn.h"
#include "PlaybackBtn.h"
#include "Param1.h"
#include "Param2.h"

class CImpFactor : public CDialogML
{
public:
	CImpFactor(CWnd* pParent = NULL);   // 標準のコンストラクタ
	~CImpFactor();

	long m_nFolderID;
	long m_nImpulseID;
	class CSasView *m_pView;
	int m_nParamItem;

	static void CloseAll();
	void ChangeData();

protected:
	enum { IDD = IDD_IMP_FACTOR };

	CPlaybackBtn	m_cPlayback;
	CScreenCopyBtn	m_cScreenCopy;
	CButton m_cSelectData;
	CButton	m_cNextData;
	CButton	m_cPrevData;
	CButton m_cCsvOutput;
	CListCtrl	m_cParameterList;
	CMyTab	m_cGraph;
	int		m_iBit;
	CString	m_sComment;
	int		m_iMeasureNum;
	CString	m_sMeasureTime;
	int		m_iSampling;
	CString	m_sTime;
	CString	m_sTitle;
	CString	m_sChannel;
	CString	m_sFolderTitle;
	CString	m_sFolderComment;
	CString	m_sDate;
	CString	m_sName;
	CString	m_sPlace;
	CString	m_sScale;
	CMyTab m_cCalcCondition;
	CGraphIR *m_pGraphIR;
	CGraphEcho *m_pGraphEcho;
	CGraphIACC *m_pGraphIACC;
	CGraphFreq *m_pGraphFreq;
	CGraphParam *m_pGraphParam;
	CGraphPref *m_pGraphPref;
	CGraphMTF *m_pGraphMTF;
	DbImpulseRec m_dbImpulseRec;
	DbAcParamRec m_dbAcParamRec;
	CIRMethod *m_pMethod;
	int m_nData;
	int m_nWaveData;
	double *m_pWaveLeft;
	double *m_pWaveRight;
	double *m_pIRLeft;
	double *m_pIRRight;
	int m_nCurrentDataNo;
	int m_nSortItem;
	int m_nSortDir;
	HWAVEDATA m_hWaveData;
	BOOL m_bChangeData;
	double m_fScale;
	double m_fRate;
	double m_fAdjustSPL;
	BOOL m_bCalculated;
	int m_nCurrentPage;
	static CObList m_WindowList;
	int m_nMethod;
	CString m_sCurrentFreq;
	double m_fStartTimeIR;
	double m_fDispTimeIR;
	double m_fStartTimeEcho;
	double m_fDispTimeEcho;
	CParam1 m_Param1;
	CParam2 m_Param2;
	int m_nParam;
	int m_nParamPrev;
	int m_nScrollX[2];
	int m_nScrollY;

	BOOL SetFolderInfo();
	BOOL ReadData();
	void DispGraph();
	void InitDispAcParamList();
	void InitAcParamList();
	void DispAcParamList();
	void DispAcParam1List();
	void DispAcParam2List();
	void DispCalcParam();
	void DeleteGraphWindow();
	void FreeBuffers();
	void CalcIR(int freq, int freqBand);
	static int CALLBACK CompareItems1(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK CompareItems2(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	HDIB ScreenCopy();
	HDIB GraphCopy();
	void ReDrawAll();
	void EnablePrevNext();
	void SaveDataCheck();
	void SaveStartDisp();
	BOOL CalcMeasurementIR();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnItemchangedParamList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPrevData();
	afx_msg void OnNextData();
	afx_msg void OnColumnclickParamList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeGraph(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelectData();
	afx_msg void OnPlayback();
	afx_msg void OnScreenCopy();
	afx_msg void OnTcnSelchangeCalcCondition(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCsvOutput();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
