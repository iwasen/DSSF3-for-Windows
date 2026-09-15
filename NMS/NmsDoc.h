#pragma once

#include "Measurement.h"

#define VIEWHINT_ADDITEM	1
#define VIEWHINT_UPDATEITEM	2
#define VIEWHINT_CLEARITEM	3

#define MEASUREMENT_STOP	0
#define MEASUREMENT_AUTO	1
#define MEASUREMENT_MANUAL	2

class CNmsDoc : public CDocument
{
public:
	void AddNmsItem(int nIndex, double fDataTime, CTime *pTime);
	void ChangeStatus(int nIndex, int nStatus);
	BOOL SetCondition(int nCalcCounter);
	BOOL CalcFactor(int nCalcCounter);
	void StopManualMode();

	CListCtrl *m_pListCtrl;
	CMeasurement m_oMeasurement;

protected: // シリアライズ機能のみから作成します。
	CNmsDoc();
	virtual ~CNmsDoc();
	DECLARE_DYNCREATE(CNmsDoc)

	int m_nMeasurementMode;
	CObList m_cItemList;
	static BOOL m_bAbortCalc;

	virtual BOOL OnNewDocument();
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);


	void SetMicCalData();
	void DeleteAllItems();
	static BOOL AbortCheck(int nPercent);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnMeasureSet();
	afx_msg void OnUpdateMeasureSet(CCmdUI* pCmdUI);
	afx_msg void OnMeasureStart();
	afx_msg void OnUpdateMeasureStart(CCmdUI* pCmdUI);
	afx_msg void OnMeasureStop();
	afx_msg void OnUpdateMeasureStop(CCmdUI* pCmdUI);
	afx_msg void OnInputLevel();
	afx_msg void OnUpdateInputLevel(CCmdUI* pCmdUI);
	afx_msg void OnInputDevice();
	afx_msg void OnUpdateInputDevice(CCmdUI* pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnPeakLevel();
	afx_msg void OnUpdatePeakLevel(CCmdUI* pCmdUI);
	afx_msg void OnClearData();
	afx_msg void OnUpdateClearData(CCmdUI* pCmdUI);
	afx_msg void OnTemplate();
	afx_msg void OnUpdateTemplate(CCmdUI* pCmdUI);
	afx_msg void OnRecalc();
	afx_msg void OnUpdateRecalc(CCmdUI* pCmdUI);
	afx_msg void OnAutoLearning();
	afx_msg void OnUpdateAutoLearning(CCmdUI* pCmdUI);
	afx_msg void OnManualLearning();
	afx_msg void OnUpdateManualLearning(CCmdUI* pCmdUI);
	afx_msg void OnMeasureManual();
	afx_msg void OnUpdateMeasureManual(CCmdUI* pCmdUI);
	afx_msg void OnAutoCalc();
	afx_msg void OnUpdateAutoCalc(CCmdUI *pCmdUI);
	afx_msg void OnReadWave();
};
