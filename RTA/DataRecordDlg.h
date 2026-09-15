#pragma once

// 通知コード
#define DR_CLOSE	1	// ウィンドウクローズ
#define DR_RECORD	2	// データ記録

// 記録タイミングモード
#define TIMING_CYCLE	0	// 定周期
#define TIMING_MANUAL	1	// 手動

// CDataRecordDlg ダイアログ

class CDataRecordDlg : public CDialogAF
{
public:
	CDataRecordDlg(CWnd* pParent = NULL);   // 標準コンストラクタ

	void InitDataList(int nOctBand, const CString *pFreqScale);
	void RecordData(int sec, FFTDATA *pFftData);
	void DeleteRecordData();

protected:
	enum { IDD = IDD_DATA_RECORD };

	CListCtrl m_cDataList;
	CButton m_cTimingCycle;
	CButton m_cTimingManual;
	CMyEdit m_cTimingInterval;
	CWnd *m_pParentWnd;
	CButton m_cTimingRecord;
	CMyButton m_cDeleteAtStart;
	CMyButton m_cAlwaysNewData;
	CButton m_cFileSave;
	CSpinButtonCtrl m_cTimingSpin;
	int m_nOctBand;
	int m_nSortItem;
	int m_nSortDir;

	void EnableControls();
	CString FormatData(double fData);
	static int CALLBACK CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedFileSave();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOK();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedTimingCycle();
	afx_msg void OnBnClickedTimingManual();
	afx_msg void OnEnChangeTimingInterval();
	afx_msg void OnBnClickedTimingRecord();
	afx_msg void OnBnClickedDeleteData();
	afx_msg void OnBnClickedDeleteAtStart();
	afx_msg void OnBnClickedAlwaysNewData();
	afx_msg void OnLvnColumnclickDataList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
