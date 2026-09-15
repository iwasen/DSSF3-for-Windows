#pragma once

#include "WaveEditWnd.h"
#include "WaveForm.h"
#include "MyCtrl.h"
#include "SgDlg.h"
#include "ToolBarEx.h"

#define EDIT_MODE_ADD		0
#define EDIT_MODE_SEL		1

class CWaveEditManual : public CDialogAF
{
public:
	CWaveEditManual(CWnd* pParent = NULL);   // 標準コンストラクタ

	WaveFormManual m_oWaveForm;
	CSgDlg *m_pSgDlg;
	CMyButton m_cBtnTest;

	void MouseDown(float x, float y);
	void MouseMove(float x, float y);
	void MouseAdd(float x, float y);
	void MouseDelete();
	int GetSelPoint();
	int GetEditMode();

protected:
	enum { IDD = IDD_WAVE_EDIT_MANUAL };

	double m_fStartX;
	double m_fDispX;
	double m_fStartY;
	double m_fDispY;
	double m_fTotalX;
	double m_fTotalY;
	double m_fMinY;
	int m_nHScrollPos;
	int m_nHScrollSize;
	int m_nVScrollPos;
	int m_nVScrollSize;
	CToolBarEx m_tbZoomH;
	CToolBarEx m_tbZoomV;
	CWaveEditWnd m_cWaveEditWnd;
	CMyEdit m_cPosX;
	CMyEdit m_cPosY;
	CMyEdit m_cZoomH;
	CMyEdit m_cZoomV;
	CMyButton m_cSmoothing;
	void SetHScroll();
	void SetVScroll();
	CScrollBar m_cScrollV;
	CScrollBar m_cScrollH;
	int m_iEditMode;
	int m_nSelPoint;

	void InitData();
	void AddVertex(float x, float y);
	void DeleteVertex(int n);
	void SelectVertex(float x, float y);
	BOOL MoveVertex(float x, float y);
	void DispPosition();
	void DispGraphWindow(BOOL bDispPosition = TRUE);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnZoomFullH();
	afx_msg void OnZoomInH();
	afx_msg void OnZoomOutH();
	afx_msg void OnZoomFullV();
	afx_msg void OnZoomInV();
	afx_msg void OnZoomOutV();
	afx_msg void OnBnClickedVertexDelete();
	afx_msg void OnBnClickedAllClear();
	afx_msg void OnEnChangePosX();
	afx_msg void OnEnChangePosY();
	afx_msg void OnBnClickedSmoothing();
	afx_msg void OnBnClickedTest();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
