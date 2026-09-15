// Param2.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "Param2.h"

// CParam2 ダイアログ

CParam2::CParam2(CWnd* pParent /*=NULL*/)
	: CDialogAF(CParam2::IDD, pParent)
{
	m_iFreqBand = -1;
	m_bAFilter = FALSE;
}

void CParam2::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);

	DDX_Radio(pDX, IDC_1OCT, m_iFreqBand);
	DDX_Text(pDX, IDC_START_FREQ, m_sStartFreq);
	DDX_Text(pDX, IDC_END_FREQ, m_sEndFreq);
	DDX_Check(pDX, IDC_AFILTER, m_bAFilter);
	DDX_Control(pDX, IDC_G_REFLEVEL, m_cGRefData);
	DDX_Control(pDX, IDC_T_CUSTOM1, m_cTCustom1);
	DDX_Control(pDX, IDC_T_CUSTOM2, m_cTCustom2);
	DDX_Control(pDX, IDC_C_CUSTOM, m_cCCustom);
}

BEGIN_MESSAGE_MAP(CParam2, CDialogAF)
END_MESSAGE_MAP()

// CParam2 メッセージ ハンドラ
