#pragma once

// CToolBarEx

class CToolBarEx : public CToolBar
{
public:
	CToolBarEx();

protected:
	BOOL m_bInEraseBG;

	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

