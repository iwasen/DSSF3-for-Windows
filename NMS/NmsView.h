#pragma once

#include "listvwex.h"
#include "NmsItem.h"

class CNmsView : public CListViewEx
{
protected: // シリアライズ機能のみから作成します。
	DECLARE_DYNCREATE(CNmsView)

	CNmsDoc* GetDocument();

	void SetListHeader();
	void AddItem(const CNmsItem *pNmsItem);
	void UpdateItem(const CNmsItem *pNmsItem);
	void SetNmsItem(const CNmsItem *pNmsItem);
	CNmsItem *GetSelectedItem();

	DECLARE_MESSAGE_MAP()
	virtual void OnInitialUpdate(); // 構築後の最初の１度だけ呼び出されます。
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnDraw(CDC* pDC);  // このビューを描画する際にオーバーライドされます。
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnViewFactor();
	afx_msg void OnUpdateViewFactor(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnChangeNoiseSrc();
	afx_msg void OnUpdateChangeNoiseSrc(CCmdUI* pCmdUI);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
};

#ifndef _DEBUG  // NmsView.cpp ファイルがデバッグ環境の時使用されます。
inline CNmsDoc* CNmsView::GetDocument()
   { return (CNmsDoc*)m_pDocument; }
#endif
