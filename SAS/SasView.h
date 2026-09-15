#pragma once

#include "listvwex.h"
#include "MyCtrl.h"

class CSasView : public CListView
{
public:
	BOOL CheckSelect(long impulseID);
	long GetNextItem(long impulseID);
	long GetPrevItem(long impulseID);

protected: // シリアライズ機能のみから作成します。
	CSasView();
	DECLARE_DYNCREATE(CSasView)

	int m_nSortItem;
	int m_nSortDir;
	CMyEdit m_cEditTitle;

	CSasDoc* GetDocument();
	void SetHeader(int nHeader);
	void DrawDB();
	void DrawFolder();
	void DrawIR();
	void DrawACF();
	void DrawNMS();
	int GetSelectedItem();
	void DispIR(int nItem);
	void DispACF(int nItem);
	void DispNMS(int nItem);
	static int CALLBACK CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	void DeleteItem(long nDataID);

	DECLARE_MESSAGE_MAP()
	virtual void OnDraw(CDC* pDC);  // このビューを描画する際にオーバーライドされます。
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate(); // 構築後の最初の１度だけ呼び出されます。
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnParamDisp();
	afx_msg void OnUpdateParamDisp(CCmdUI* pCmdUI);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
	afx_msg void OnFileDelete();
	afx_msg void OnUpdateFileDelete(CCmdUI *pCmdUI);
	afx_msg void OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnFileChangeTitle();
	afx_msg void OnUpdateFileChangeTitle(CCmdUI *pCmdUI);
	afx_msg void OnLvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnFileExport();
	afx_msg void OnUpdateFileExport(CCmdUI *pCmdUI);
	afx_msg void OnFileImport();
	afx_msg void OnUpdateFileImport(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // ChasView.cpp ファイルがデバッグ環境の時使用されます。
inline CSasDoc* CSasView::GetDocument()
   { return (CSasDoc*)m_pDocument; }
#endif
