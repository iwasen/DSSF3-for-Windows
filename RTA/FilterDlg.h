#pragma once

#include "BaseDlg.h"
#include "MyCtrl.h"
#include "FilterWnd.h"

// CFilterDlg ダイアログ

class CFilterDlg : public CBaseDlg
{
public:
	CFilterDlg(CWnd* pParent = NULL);   // 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_FILTER };

protected:
	CComboBox m_cComboFilterBand;
	CComboBox m_ComboFilterOrder;
	CComboBox m_cComboFilterShape;
	CMyEdit m_cEditFilterRipple1;
	CMyEdit m_cEditFilterCutoff1;
	CMyEdit m_cEditFilterCutoff2;
	CMyButton m_cRadioFilterInput;
	CMyButton m_cRadioFilterOutput;
	CFilterWnd m_cStaticFilterCurve;

	BOOL m_bInit;
	CDigitalFilter m_cDigitalFilter;
	double m_fSamplingFreq;

	void SetFilter();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnCbnSelchangeComboFilterBand();
	afx_msg void OnCbnSelchangeComboFilterShape();
	afx_msg void OnCbnSelchangeComboFilterOrder();
	afx_msg void OnEnChangeEditFilterCutoff1();
	afx_msg void OnEnChangeEditFilterCutoff2();
	afx_msg void OnEnChangeEditFilterRipple1();
	afx_msg void OnBnClickedRadioFilterInput();
	afx_msg void OnBnClickedRadioFilterOutput();
};
