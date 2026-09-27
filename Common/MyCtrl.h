#pragma once

#include <afxtempl.h>

#define VC_NUM		0x01
#define VC_MINUS	0x02
#define VC_POINT	0x04

#define NOTIFY_RBUTTONDOWN	1

/////////////////////////////////////////////////////////////////////////////
// CMyButton ウィンドウ

class CMyButton : public CButton
{
public:
	BOOL operator=(BOOL param);
	operator BOOL();

protected:
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CMyEdit ウィンドウ

class CMyEdit : public CEdit
{
public:
	CMyEdit();

	int operator=(int param);
	long operator=(long nParam);
	float operator=(float param);
	double operator=(double param);
	CString &operator=(CString &param);
	LPCTSTR operator=(LPCTSTR param);
	CMyEdit &operator=(CMyEdit &param);
	int operator==(LPCTSTR param);
	int operator==(int param);
	int operator!=(int param);
	operator int();
	operator float();
	operator double();
	operator CString();
	void Format(LPCTSTR lpszFormat, ...);
	void Blank();
	void LoadString(UINT nStringID);
	void FormatMessage(UINT nStringID, ...);
	void SetTextColor(COLORREF color);
	void SetValidChar(UINT nValidChar);
	void SetMaxLength(int nMaxLength);
	void SetNotifyWnd(HWND hNotifyWnd);
	BOOL IsEmpty();

protected:
	CString m_str;
	BOOL m_bSetTextColor;
	CBrush m_brush;
	COLORREF m_nTextColor;
	CString m_sValidStr;
	HWND m_hNotifyWnd;
	int m_nMaxLength;

	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////
// CMyTab ウィンドウ

class CMyTab : public CTabCtrl
{
public:
	CMyTab();

	int AddPage(CDialog *pDialog, UINT id, LPCTSTR title, BOOL bCreate = FALSE, BOOL bSort = FALSE);
	int AddPage(CDialog *pDialog, UINT id, UINT titleID, BOOL bCreate = FALSE, BOOL bSort = FALSE);
	void DeleteAllPages();
	void DeletePage(int n);
	void SetPage(int n);
	void NotifyChildDlg(WPARAM nCode);
	CDialog *GetCurrentDlg();
	void SetFontSize(int nFontSize);
	void SetDrag(int n, BOOL bDrag);
	void SetGrayed(int n, BOOL bGray);
	void GetCurrentTitle(CString &sTitle);

protected:
	struct TabData {
		CDialog *pDialog;
		UINT id;
		BOOL bDrag;
	};

	int m_nFontSize;
	CArray < TabData, TabData& > m_TabData;
	int m_nClickedTab;
	CPoint m_pointClick;

	void DisplayChildDlg(int n);
	void CreateChildDlg(int n);

	DECLARE_MESSAGE_MAP()
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////
// CTicString

class CTicString : public CObject
{
public:
	int x;
	CString text;
};

/////////////////////////////////////////////////////////////////////////////
// CMySliderString

class CMySliderString : public CWnd
{
public:
	virtual ~CMySliderString();

	void AddTicString(int x, LPCTSTR pText);
	void RemoveAll();

protected:
	CObList m_StringList;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
};

/////////////////////////////////////////////////////////////////////////////
// CMySliderCtrl

class CMySliderCtrl : public CSliderCtrl
{
public:
	void AddTicString(int nTic, LPCTSTR pStr);
	void ClearTics(BOOL bRedraw = FALSE);

protected:
	CMySliderString m_MySliderString;

	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
};

class CMyToolBar : public CToolBar
{
public:
	void DrawGripper(CDC* pDC, const CRect& rect);
	void EraseNonClient();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoPaint(CDC* pDC);
	afx_msg void OnNcPaint();
};
