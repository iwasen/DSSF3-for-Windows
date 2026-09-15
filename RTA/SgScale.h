#pragma once

#include "Keyboard.h"
#include "MyCtrl.h"

class CSgScale : public CDialogAF
{
public:
	CSgScale(CWnd* pParent = NULL);   // 標準コンストラクタ

protected:
	enum { IDD = IDD_SG_SCALE };

	int m_iScale;
	CMyButton m_cPlayMode;
	CKeyboard m_cKeyboard;
	CComboBox m_cWaveForm;
	CMyEdit m_cReferencePitch;
	CMyEdit m_cFreq;
	int m_iOctave;
	CSpinButtonCtrl m_cReferencePitchSpin;

	void SetScale(int nScale);
	void SetOctave(int nOctave);
	void DispFreq();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnKeyboard(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedScaleC();
	afx_msg void OnBnClickedScaleCs();
	afx_msg void OnBnClickedScaleD();
	afx_msg void OnBnClickedScaleDs();
	afx_msg void OnBnClickedScaleE();
	afx_msg void OnBnClickedScaleF();
	afx_msg void OnBnClickedScaleFs();
	afx_msg void OnBnClickedScaleG();
	afx_msg void OnBnClickedScaleGs();
	afx_msg void OnBnClickedScaleA();
	afx_msg void OnBnClickedScaleAs();
	afx_msg void OnBnClickedScaleB();
	afx_msg void OnBnClickedOctave0();
	afx_msg void OnBnClickedOctave1();
	afx_msg void OnBnClickedOctave2();
	afx_msg void OnBnClickedOctave3();
	afx_msg void OnBnClickedOctave4();
	afx_msg void OnBnClickedOctave5();
	afx_msg void OnBnClickedOctave6();
	afx_msg void OnBnClickedOctave7();
	afx_msg void OnBnClickedOctave8();
	afx_msg void OnCbnSelchangeWaveForm();
	afx_msg void OnEnChangeReferencePitch();
	afx_msg void OnBnClickedPlayMode();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
