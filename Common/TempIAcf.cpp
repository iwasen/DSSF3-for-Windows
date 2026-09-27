// TempIAcf.cpp : 実装ファイル
//

#include "stdafx.h"
#include "comres.h"
#include "Common.h"
#include "FileIO.h"
#include "TempIAcf.h"
#include "Help\CommonHelp.h"

// CTempIAcf ダイアログ

CTempIAcf::CTempIAcf(CWnd* pParent /*=NULL*/)
	: CDialogAF(CTempIAcf::IDD, pParent)
{
}

void CTempIAcf::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IACC_STANDARD, m_cIACCStandard);
	DDX_Control(pDX, IDC_IACC_UPPER_CHECK, m_cIACCUpperCheck);
	DDX_Control(pDX, IDC_IACC_UPPER_LIMIT, m_cIACCUpperLimit);
	DDX_Control(pDX, IDC_IACC_LOWER_CHECK, m_cIACCLowerCheck);
	DDX_Control(pDX, IDC_IACC_LOWER_LIMIT, m_cIACCLowerLimit);
	DDX_Control(pDX, IDC_IACC_WEIGHTING, m_cIACCWeighting);
	DDX_Control(pDX, IDC_TIACC_STANDARD, m_cTIACCStandard);
	DDX_Control(pDX, IDC_TIACC_UPPER_CHECK, m_cTIACCUpperCheck);
	DDX_Control(pDX, IDC_TIACC_UPPER_LIMIT, m_cTIACCUpperLimit);
	DDX_Control(pDX, IDC_TIACC_LOWER_CHECK, m_cTIACCLowerCheck);
	DDX_Control(pDX, IDC_TIACC_LOWER_LIMIT, m_cTIACCLowerLimit);
	DDX_Control(pDX, IDC_TIACC_WEIGHTING, m_cTIACCWeighting);
	DDX_Control(pDX, IDC_WIACC_STANDARD, m_cWIACCStandard);
	DDX_Control(pDX, IDC_WIACC_UPPER_CHECK, m_cWIACCUpperCheck);
	DDX_Control(pDX, IDC_WIACC_UPPER_LIMIT, m_cWIACCUpperLimit);
	DDX_Control(pDX, IDC_WIACC_LOWER_CHECK, m_cWIACCLowerCheck);
	DDX_Control(pDX, IDC_WIACC_LOWER_LIMIT, m_cWIACCLowerLimit);
	DDX_Control(pDX, IDC_WIACC_WEIGHTING, m_cWIACCWeighting);
}

BEGIN_MESSAGE_MAP(CTempIAcf, CDialogAF)
	ON_BN_CLICKED(IDC_SET_WEIGHTING, OnSetWeighting)
	ON_BN_CLICKED(IDC_IACC_LOWER_CHECK, OnIACCLowerCheck)
	ON_BN_CLICKED(IDC_IACC_UPPER_CHECK, OnIACCUpperCheck)
	ON_BN_CLICKED(IDC_TIACC_LOWER_CHECK, OnTIACCLowerCheck)
	ON_BN_CLICKED(IDC_TIACC_UPPER_CHECK, OnTIACCUpperCheck)
	ON_BN_CLICKED(IDC_WIACC_LOWER_CHECK, OnWIACCLowerCheck)
	ON_BN_CLICKED(IDC_WIACC_UPPER_CHECK, OnWIACCUpperCheck)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

// CTempIAcf メッセージ ハンドラ

BOOL CTempIAcf::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	ShowWeightings();

	OnIACCLowerCheck();
	OnIACCUpperCheck();
	OnWIACCLowerCheck();
	OnWIACCUpperCheck();
	OnTIACCLowerCheck();
	OnTIACCUpperCheck();

	return TRUE;
}

void CTempIAcf::OnSetWeighting()
{
	NsWeightData nsWeightData;

	ReadNsWeightData(&nsWeightData);
	nsWeightData.fIACC = (float)m_cIACCWeighting;
	nsWeightData.fWIACC = (float)m_cWIACCWeighting;
	nsWeightData.fTIACC = (float)m_cTIACCWeighting;
	WriteNsWeightData(&nsWeightData);
}

void CTempIAcf::OnIACCLowerCheck()
{
	m_cIACCLowerLimit.EnableWindow(m_cIACCLowerCheck);
}

void CTempIAcf::OnIACCUpperCheck()
{
	m_cIACCUpperLimit.EnableWindow(m_cIACCUpperCheck);
}

void CTempIAcf::OnTIACCLowerCheck()
{
	m_cTIACCLowerLimit.EnableWindow(m_cTIACCLowerCheck);
}

void CTempIAcf::OnTIACCUpperCheck()
{
	m_cTIACCUpperLimit.EnableWindow(m_cTIACCUpperCheck);
}

void CTempIAcf::OnWIACCLowerCheck()
{
	m_cWIACCLowerLimit.EnableWindow(m_cWIACCLowerCheck);
}

void CTempIAcf::OnWIACCUpperCheck()
{
	m_cWIACCUpperLimit.EnableWindow(m_cWIACCUpperCheck);
}

void CTempIAcf::ShowWeightings()
{
	NsWeightData nsWeightData;

	ReadNsWeightData(&nsWeightData);
	m_cIACCWeighting.Format("%.3g", nsWeightData.fIACC);
	m_cWIACCWeighting.Format("%.3g", nsWeightData.fWIACC);
	m_cTIACCWeighting.Format("%.3g", nsWeightData.fTIACC);
}

void CTempIAcf::SetData(const NsTmpData &nsTmpData)
{
	m_cIACCStandard.Format("%.3g", nsTmpData.fIACCStandard);
	m_cIACCUpperLimit.Format("%.3g", nsTmpData.fIACCUpperLimit);
	m_cIACCLowerLimit.Format("%.3g", nsTmpData.fIACCLowerLimit);
	m_cIACCUpperCheck = nsTmpData.bIACCUpperCheck;
	m_cIACCLowerCheck = nsTmpData.bIACCLowerCheck;
	OnIACCLowerCheck();
	OnIACCUpperCheck();

	m_cWIACCStandard.Format("%.3g", nsTmpData.fWIACCStandard * 1000);
	m_cWIACCUpperLimit.Format("%.3g", nsTmpData.fWIACCUpperLimit * 1000);
	m_cWIACCLowerLimit.Format("%.3g", nsTmpData.fWIACCLowerLimit * 1000);
	m_cWIACCUpperCheck = nsTmpData.bWIACCUpperCheck;
	m_cWIACCLowerCheck = nsTmpData.bWIACCLowerCheck;
	OnWIACCLowerCheck();
	OnWIACCUpperCheck();

	m_cTIACCStandard.Format("%.3g", nsTmpData.fTIACCStandard * 1000);
	m_cTIACCUpperLimit.Format("%.3g", nsTmpData.fTIACCUpperLimit * 1000);
	m_cTIACCLowerLimit.Format("%.3g", nsTmpData.fTIACCLowerLimit * 1000);
	m_cTIACCUpperCheck = nsTmpData.bTIACCUpperCheck;
	m_cTIACCLowerCheck = nsTmpData.bTIACCLowerCheck;
	OnTIACCLowerCheck();
	OnTIACCUpperCheck();
}

void CTempIAcf::GetData(NsTmpData &nsTmpData)
{
	nsTmpData.fIACCStandard = (float)m_cIACCStandard;
	nsTmpData.fIACCUpperLimit = (float)m_cIACCUpperLimit;
	nsTmpData.fIACCLowerLimit = (float)m_cIACCLowerLimit;
	nsTmpData.bIACCUpperCheck = m_cIACCUpperCheck;
	nsTmpData.bIACCLowerCheck = m_cIACCLowerCheck;
	nsTmpData.fIACCSum2 = 0;

	nsTmpData.fWIACCStandard = (float)m_cWIACCStandard / 1000;
	nsTmpData.fWIACCUpperLimit = (float)m_cWIACCUpperLimit / 1000;
	nsTmpData.fWIACCLowerLimit = (float)m_cWIACCLowerLimit / 1000;
	nsTmpData.bWIACCUpperCheck = m_cWIACCUpperCheck;
	nsTmpData.bWIACCLowerCheck = m_cWIACCLowerCheck;
	nsTmpData.fWIACCSum2 = 0;

	nsTmpData.fTIACCStandard = (float)m_cTIACCStandard / 1000;
	nsTmpData.fTIACCUpperLimit = (float)m_cTIACCUpperLimit / 1000;
	nsTmpData.fTIACCLowerLimit = (float)m_cTIACCLowerLimit / 1000;
	nsTmpData.bTIACCUpperCheck = m_cTIACCUpperCheck;
	nsTmpData.bTIACCLowerCheck = m_cTIACCLowerCheck;
	nsTmpData.fTIACCSum2 = 0;
}

BOOL CTempIAcf::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_IACC_STANDARD, IDH_TEMP_IACF_IACC_STANDARD,
		IDC_IACC_UPPER_CHECK, IDH_TEMP_IACF_IACC_UPPER_CHECK,
		IDC_IACC_UPPER_LIMIT, IDH_TEMP_IACF_IACC_UPPER_LIMIT,
		IDC_IACC_LOWER_CHECK, IDH_TEMP_IACF_IACC_LOWER_CHECK,
		IDC_IACC_LOWER_LIMIT, IDH_TEMP_IACF_IACC_LOWER_LIMIT,
		IDC_WIACC_STANDARD, IDH_TEMP_IACF_WIACC_STANDARD,
		IDC_WIACC_UPPER_CHECK, IDH_TEMP_IACF_WIACC_UPPER_CHECK,
		IDC_WIACC_UPPER_LIMIT, IDH_TEMP_IACF_WIACC_UPPER_LIMIT,
		IDC_WIACC_LOWER_CHECK, IDH_TEMP_IACF_WIACC_LOWER_CHECK,
		IDC_WIACC_LOWER_LIMIT, IDH_TEMP_IACF_WIACC_LOWER_LIMIT,
		IDC_TIACC_STANDARD, IDH_TEMP_IACF_TIACC_STANDARD,
		IDC_TIACC_UPPER_CHECK, IDH_TEMP_IACF_TIACC_UPPER_CHECK,
		IDC_TIACC_UPPER_LIMIT, IDH_TEMP_IACF_TIACC_UPPER_LIMIT,
		IDC_TIACC_LOWER_CHECK, IDH_TEMP_IACF_TIACC_LOWER_CHECK,
		IDC_TIACC_LOWER_LIMIT, IDH_TEMP_IACF_TIACC_LOWER_LIMIT,
		IDC_IACC_WEIGHTING, IDH_TEMP_IACF_IACC_WEIGHTING,
		IDC_WIACC_WEIGHTING, IDH_TEMP_IACF_WIACC_WEIGHTING,
		IDC_TIACC_WEIGHTING, IDH_TEMP_IACF_TIACC_WEIGHTING,
		IDC_SET_WEIGHTING, IDH_TEMP_IACF_SET_WEIGHTING,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
