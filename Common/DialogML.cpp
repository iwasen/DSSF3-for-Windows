// DialogML.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "DialogML.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogML ダイアログ

CDialogML::CDialogML(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
{
	m_nIDTemplate = nIDTemplate;
	m_pParent = pParent;
}

void CDialogML::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDialogML, CDialogAF)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogML メッセージ ハンドラ

BOOL CDialogML::Create(CWnd *pParentWnd)
{
	if (pParentWnd == NULL)
		pParentWnd = m_pParent;

	return CDialogAF::Create(m_nIDTemplate, pParentWnd);
}

void CDialogML::OnCancel()
{
}

void CDialogML::OnOK()
{
}

void CDialogML::PostNcDestroy()
{
	delete this;
}

int CDialogML::MessageBox(UINT nIDPrompt, UINT nType)
{
	CString msg;

	msg.LoadString(nIDPrompt);
	return CWnd::MessageBox(msg, NULL, nType);
}
