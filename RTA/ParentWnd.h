#pragma once

class CParentWnd : public CWnd
{
public:
	CParentWnd();

protected:
	ULONG m_nCheckData;

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
	afx_msg LRESULT OnGetCheckLicense1(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetCheckLicense2(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRegistLicense(WPARAM wParam, LPARAM lParam);
};
