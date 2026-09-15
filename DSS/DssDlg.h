// DssDlg.h : ヘッダー ファイル
//

#pragma once

#include "Folder.h"
#include "Initialize.h"
#include "Mmlib.h"

// CDssDlg ダイアログ
class CDssDlg : public CPropertySheet
{
// コンストラクション
public:
	CDssDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// 実装
protected:
	HICON m_hIcon;
	CFolder m_cFolder;
	CInitialize m_cInitialize;
	CMmlib m_cMmlib;

	// 生成された、メッセージ割り当て関数
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
};
