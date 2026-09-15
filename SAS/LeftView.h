#pragma once

#include "MyCtrl.h"

class CSasDoc;

class CLeftView : public CTreeView
{
protected: // シリアライズ機能のみから作成します。
	DECLARE_DYNCREATE(CLeftView)

	CSasDoc* GetDocument();

	void SetTreeItem();
	BOOL SelectItem(HTREEITEM hItem, DWORD dwItemData);
	void DeleteItem(long nItemID);
	BOOL DeleteItemSub(long nItemID, HTREEITEM hTreeItem);
	BOOL ChangeTitle(long nItemID, HTREEITEM hTreeItem, LPCTSTR pTitle);

protected:
	CMyEdit m_cEditTitle;

	DECLARE_MESSAGE_MAP()
	virtual void OnDraw(CDC* pDC);  // このビューを描画する際にオーバーライドされます。
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // 構築後の最初の１度だけ呼び出されます。
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
	afx_msg void OnFileDelete();
	afx_msg void OnUpdateFileDelete(CCmdUI *pCmdUI);
	afx_msg void OnFileChangeTitle();
	afx_msg void OnUpdateFileChangeTitle(CCmdUI *pCmdUI);
	afx_msg void OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnFileExport();
	afx_msg void OnUpdateFileExport(CCmdUI *pCmdUI);
	afx_msg void OnFileImport();
	afx_msg void OnUpdateFileImport(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // LeftView.cpp ファイルがデバッグ環境の時使用されます。
inline CSasDoc* CLeftView::GetDocument()
   { return (CSasDoc*)m_pDocument; }
#endif
