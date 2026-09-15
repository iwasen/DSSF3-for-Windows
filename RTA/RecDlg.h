#pragma once

#include "BaseDlg.h"
#include "RecWnd.h"
#include "MyCtrl.h"
#include "WaveNotify.h"
#include <mmsystem.h>

class CRecDlg : public CBaseDlg, public IWaveNotify
{
public:
	CRecDlg(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CRecDlg();

	void LoadWavDssfFile(LPCTSTR pPathName);

protected:
	enum { IDD = IDD_REC };

	CToolBar m_tbZoomH;
	BOOL m_bMove;
	double *m_pWaveBuf;
	DWORD m_dwRecCount;
	DWORD m_dwPlayCount;
	DWORD m_dwPlayEndCount;
	int m_nRecMode;
	int m_nPlayMode;
	int m_nSamplingRate;
	int m_nRecChannel;
	int m_nPlayChannel;
	double m_fTotalTime, m_fStartTime, m_fDispTime;
	double m_fDataTime;
	int m_nScrollSize;
	int m_nScrollPos;
	double m_fMaxData;
	BOOL m_bDispSelectArea;
	double m_fSelectStart;
	double m_fSelectEnd;
	double m_fPlayTime;
	int m_iPriority;
	CRecWnd m_cRecWnd;
	CMyEdit m_cZoomH;
	CMyButton m_cRecord;
	CMyButton m_cPlay;
	CScrollBar m_cGraphScroll;
	CMyButton m_cGraphNorm;
	CMyButton m_cFileSave;
	CMyEdit m_cSelectStart;
	CMyEdit m_cSelectEnd;
	CMyEdit m_cSelectTime;
	CMyEdit m_cRecordTime;
	CButton m_cCut;
	CButton m_cSelectAll;
	DWORD m_dwAllocSize;
	int m_nBitsPerSample;
	int m_nCurrentStatus;

	static void CALLBACK GraphCallBack(WPARAM code, LPARAM data, double param1, double param2);
	void RecModeOn();
	void RecModeOff();
	void PlayModeOn();
	void PlayModeOff();
	void EnableButton();
	void AllocBuf();
	void FreeBuf();
	void SetCurrentStatus(int nCurrentStatus);
	void InitRecMode(int nRecMode);
	void InitPlayMode(int nPlayMode);
	int RecordData(LPWAVENOTIFY pWaveNotify);
	int PlayData(LPWAVENOTIFY pWaveNotify);
	void DispWaveGraph();
	void InitDispTime();
	void DispSelectArea(double fSelectStart, double fSelectEnd);
	void DispRecInfo();
	void DispCurrentWave(int nRecCount, int nData);
	int OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify);
	void LoadFile(LPCTSTR pPathName);
	void LoadWavFile(LPCTSTR pPathName);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRecord();
	afx_msg void OnBnClickedPlay();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnZoomFullH();
	afx_msg void OnZoomInH();
	afx_msg void OnZoomOutH();
	afx_msg void OnBnClickedFileLoad();
	afx_msg void OnBnClickedFileSave();
	afx_msg void OnBnClickedSelectAll();
	afx_msg void OnBnClickedCut();
	afx_msg void OnEnChangeSelectStart();
	afx_msg void OnEnChangeSelectEnd();
	afx_msg void OnBnClickedGraphNorm();
	afx_msg void OnBnClickedPriorityInput();
	afx_msg void OnBnClickedPriorityOutput();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnDropFiles(HDROP hDropInfo);
};
