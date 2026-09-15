#pragma once

#include "pcscl.h"
#include "mmdb.h"

class CScreenCopyBtn : public CButton
{
public:
	CScreenCopyBtn();
	virtual ~CScreenCopyBtn();

	BOOL m_bNoProcess;

	HDIB CreateDIB(CWnd *pWnd);

protected:
	static CString m_TempFile;

	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnClicked();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};
