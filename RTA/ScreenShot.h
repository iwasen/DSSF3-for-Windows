#pragma once

class CScreenShot
{
public:
	virtual ~CScreenShot();

	void ShowScreenShot(LPCSTR pTitle, HBITMAP hBmp, BOOL bCopy);
	void CloseScreenShot(class CScreenShotWnd *pWnd);
	static void OutputImageFile(CWnd *pWnd, CBitmap &bitmap);

protected:
	CObList m_ItemList;
	POSITION m_Position;
};

class CScreenShotWnd : public CWnd
{
public:
	void ShowBitmap(LPCSTR pTitle, HBITMAP hBmp, CScreenShot *pScreenShot);

protected:
	CBitmap m_bitmap;
	int m_nWidth;
	int m_nHeight;
	CScreenShot *m_pScreenShot;

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPopupFileSave();
};
