// TempAcf.cpp : 実装ファイル
//

#include "stdafx.h"
#include "comres.h"
#include "Common.h"
#include "FileIO.h"
#include "TempAcf.h"
#include "Help\CommonHelp.h"

// CTempAcf ダイアログ

CTempAcf::CTempAcf(CWnd* pParent /*=NULL*/)
	: CDialogAF(CTempAcf::IDD, pParent)
{
}

void CTempAcf::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PHI0_STANDARD, m_cPhi0Standard);
	DDX_Control(pDX, IDC_PHI0_UPPER_CHECK, m_cPhi0UpperCheck);
	DDX_Control(pDX, IDC_PHI0_UPPER_LIMIT, m_cPhi0UpperLimit);
	DDX_Control(pDX, IDC_PHI0_LOWER_CHECK, m_cPhi0LowerCheck);
	DDX_Control(pDX, IDC_PHI0_LOWER_LIMIT, m_cPhi0LowerLimit);
	DDX_Control(pDX, IDC_PHI0_WEIGHTING, m_cPhi0Weighting);
	DDX_Control(pDX, IDC_PHI1_STANDARD, m_cPhi1Standard);
	DDX_Control(pDX, IDC_PHI1_UPPER_CHECK, m_cPhi1UpperCheck);
	DDX_Control(pDX, IDC_PHI1_UPPER_LIMIT, m_cPhi1UpperLimit);
	DDX_Control(pDX, IDC_PHI1_LOWER_CHECK, m_cPhi1LowerCheck);
	DDX_Control(pDX, IDC_PHI1_LOWER_LIMIT, m_cPhi1LowerLimit);
	DDX_Control(pDX, IDC_PHI1_WEIGHTING, m_cPhi1Weighting);
	DDX_Control(pDX, IDC_TAUE_STANDARD, m_cTaueStandard);
	DDX_Control(pDX, IDC_TAUE_UPPER_CHECK, m_cTaueUpperCheck);
	DDX_Control(pDX, IDC_TAUE_UPPER_LIMIT, m_cTaueUpperLimit);
	DDX_Control(pDX, IDC_TAUE_LOWER_CHECK, m_cTaueLowerCheck);
	DDX_Control(pDX, IDC_TAUE_LOWER_LIMIT, m_cTaueLowerLimit);
	DDX_Control(pDX, IDC_TAUE_WEIGHTING, m_cTaueWeighting);
	DDX_Control(pDX, IDC_TAU1_STANDARD, m_cTau1Standard);
	DDX_Control(pDX, IDC_TAU1_UPPER_CHECK, m_cTau1UpperCheck);
	DDX_Control(pDX, IDC_TAU1_UPPER_LIMIT, m_cTau1UpperLimit);
	DDX_Control(pDX, IDC_TAU1_LOWER_CHECK, m_cTau1LowerCheck);
	DDX_Control(pDX, IDC_TAU1_LOWER_LIMIT, m_cTau1LowerLimit);
	DDX_Control(pDX, IDC_TAU1_WEIGHTING, m_cTau1Weighting);
}

BEGIN_MESSAGE_MAP(CTempAcf, CDialogAF)
	ON_BN_CLICKED(IDC_SET_WEIGHTING, OnSetWeighting)
	ON_BN_CLICKED(IDC_PHI0_LOWER_CHECK, OnPhi0LowerCheck)
	ON_BN_CLICKED(IDC_PHI0_UPPER_CHECK, OnPhi0UpperCheck)
	ON_BN_CLICKED(IDC_PHI1_LOWER_CHECK, OnPhi1LowerCheck)
	ON_BN_CLICKED(IDC_PHI1_UPPER_CHECK, OnPhi1UpperCheck)
	ON_BN_CLICKED(IDC_TAU1_LOWER_CHECK, OnTau1LowerCheck)
	ON_BN_CLICKED(IDC_TAU1_UPPER_CHECK, OnTau1UpperCheck)
	ON_BN_CLICKED(IDC_TAUE_LOWER_CHECK, OnTaueLowerCheck)
	ON_BN_CLICKED(IDC_TAUE_UPPER_CHECK, OnTaueUpperCheck)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

// CTempAcf メッセージ ハンドラ

BOOL CTempAcf::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	ShowWeightings();

	OnPhi0LowerCheck();
	OnPhi0UpperCheck();
	OnTaueLowerCheck();
	OnTaueUpperCheck();
	OnTau1LowerCheck();
	OnTau1UpperCheck();
	OnPhi1LowerCheck();
	OnPhi1UpperCheck();

	return TRUE;
}

void CTempAcf::OnSetWeighting()
{
	NsWeightData nsWeightData;

	ReadNsWeightData(&nsWeightData);
	nsWeightData.fPhi0 = (float)m_cPhi0Weighting;
	nsWeightData.fTaue = (float)m_cTaueWeighting;
	nsWeightData.fTau1 = (float)m_cTau1Weighting;
	nsWeightData.fPhi1 = (float)m_cPhi1Weighting;
	WriteNsWeightData(&nsWeightData);
}

void CTempAcf::OnPhi0LowerCheck()
{
	m_cPhi0LowerLimit.EnableWindow(m_cPhi0LowerCheck);
}

void CTempAcf::OnPhi0UpperCheck()
{
	m_cPhi0UpperLimit.EnableWindow(m_cPhi0UpperCheck);
}

void CTempAcf::OnPhi1LowerCheck()
{
	m_cPhi1LowerLimit.EnableWindow(m_cPhi1LowerCheck);
}

void CTempAcf::OnPhi1UpperCheck()
{
	m_cPhi1UpperLimit.EnableWindow(m_cPhi1UpperCheck);
}

void CTempAcf::OnTau1LowerCheck()
{
	m_cTau1LowerLimit.EnableWindow(m_cTau1LowerCheck);
}

void CTempAcf::OnTau1UpperCheck()
{
	m_cTau1UpperLimit.EnableWindow(m_cTau1UpperCheck);
}

void CTempAcf::OnTaueLowerCheck()
{
	m_cTaueLowerLimit.EnableWindow(m_cTaueLowerCheck);
}

void CTempAcf::OnTaueUpperCheck()
{
	m_cTaueUpperLimit.EnableWindow(m_cTaueUpperCheck);
}

void CTempAcf::ShowWeightings()
{
	NsWeightData nsWeightData;

	ReadNsWeightData(&nsWeightData);
	m_cPhi0Weighting.Format("%.3g", nsWeightData.fPhi0);
	m_cTaueWeighting.Format("%.3g", nsWeightData.fTaue);
	m_cTau1Weighting.Format("%.3g", nsWeightData.fTau1);
	m_cPhi1Weighting.Format("%.3g", nsWeightData.fPhi1);
}

void CTempAcf::SetData(const NsTmpData &nsTmpData)
{
	m_cPhi0Standard.Format("%.3g", nsTmpData.fPhi0Standard);
	m_cPhi0UpperLimit.Format("%.3g", nsTmpData.fPhi0UpperLimit);
	m_cPhi0LowerLimit.Format("%.3g", nsTmpData.fPhi0LowerLimit);
	m_cPhi0UpperCheck = nsTmpData.bPhi0UpperCheck;
	m_cPhi0LowerCheck = nsTmpData.bPhi0LowerCheck;
	OnPhi0LowerCheck();
	OnPhi0UpperCheck();

	m_cTaueStandard.Format("%.3g", nsTmpData.fTaueStandard * 1000);
	m_cTaueUpperLimit.Format("%.3g", nsTmpData.fTaueUpperLimit * 1000);
	m_cTaueLowerLimit.Format("%.3g", nsTmpData.fTaueLowerLimit * 1000);
	m_cTaueUpperCheck = nsTmpData.bTaueUpperCheck;
	m_cTaueLowerCheck = nsTmpData.bTaueLowerCheck;
	OnTaueLowerCheck();
	OnTaueUpperCheck();

	m_cTau1Standard.Format("%.3g", nsTmpData.fTau1Standard * 1000);
	m_cTau1UpperLimit.Format("%.3g", nsTmpData.fTau1UpperLimit * 1000);
	m_cTau1LowerLimit.Format("%.3g", nsTmpData.fTau1LowerLimit * 1000);
	m_cTau1UpperCheck = nsTmpData.bTau1UpperCheck;
	m_cTau1LowerCheck = nsTmpData.bTau1LowerCheck;
	OnTau1LowerCheck();
	OnTau1UpperCheck();

	m_cPhi1Standard.Format("%.3g", nsTmpData.fPhi1Standard);
	m_cPhi1UpperLimit.Format("%.3g", nsTmpData.fPhi1UpperLimit);
	m_cPhi1LowerLimit.Format("%.3g", nsTmpData.fPhi1LowerLimit);
	m_cPhi1UpperCheck = nsTmpData.bPhi1UpperCheck;
	m_cPhi1LowerCheck = nsTmpData.bPhi1LowerCheck;
	OnPhi1LowerCheck();
	OnPhi1UpperCheck();
}

void CTempAcf::GetData(NsTmpData &nsTmpData)
{
	nsTmpData.fPhi0Standard = (float)m_cPhi0Standard;
	nsTmpData.fPhi0UpperLimit = (float)m_cPhi0UpperLimit;
	nsTmpData.fPhi0LowerLimit = (float)m_cPhi0LowerLimit;
	nsTmpData.bPhi0UpperCheck = m_cPhi0UpperCheck;
	nsTmpData.bPhi0LowerCheck = m_cPhi0LowerCheck;
	nsTmpData.fPhi0Sum2 = 0;

	nsTmpData.fTaueStandard = (float)m_cTaueStandard / 1000;
	nsTmpData.fTaueUpperLimit = (float)m_cTaueUpperLimit / 1000;
	nsTmpData.fTaueLowerLimit = (float)m_cTaueLowerLimit / 1000;
	nsTmpData.bTaueUpperCheck = m_cTaueUpperCheck;
	nsTmpData.bTaueLowerCheck = m_cTaueLowerCheck;
	nsTmpData.fTaueSum2 = 0;

	nsTmpData.fTau1Standard = (float)m_cTau1Standard / 1000;
	nsTmpData.fTau1UpperLimit = (float)m_cTau1UpperLimit / 1000;
	nsTmpData.fTau1LowerLimit = (float)m_cTau1LowerLimit / 1000;
	nsTmpData.bTau1UpperCheck = m_cTau1UpperCheck;
	nsTmpData.bTau1LowerCheck = m_cTau1LowerCheck;
	nsTmpData.fTau1Sum2 = 0;

	nsTmpData.fPhi1Standard = (float)m_cPhi1Standard;
	nsTmpData.fPhi1UpperLimit = (float)m_cPhi1UpperLimit;
	nsTmpData.fPhi1LowerLimit = (float)m_cPhi1LowerLimit;
	nsTmpData.bPhi1UpperCheck = m_cPhi1UpperCheck;
	nsTmpData.bPhi1LowerCheck = m_cPhi1LowerCheck;
	nsTmpData.fPhi1Sum2 = 0;
}

BOOL CTempAcf::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDC_PHI0_STANDARD, IDH_TEMP_ACF_PHI0_STANDARD,
		IDC_PHI0_UPPER_CHECK, IDH_TEMP_ACF_PHI0_UPPER_CHECK,
		IDC_PHI0_UPPER_LIMIT, IDH_TEMP_ACF_PHI0_UPPER_LIMIT,
		IDC_PHI0_LOWER_CHECK, IDH_TEMP_ACF_PHI0_LOWER_CHECK,
		IDC_PHI0_LOWER_LIMIT, IDH_TEMP_ACF_PHI0_LOWER_LIMIT,
		IDC_TAUE_STANDARD, IDH_TEMP_ACF_TAUE_STANDARD,
		IDC_TAUE_UPPER_CHECK, IDH_TEMP_ACF_TAUE_UPPER_CHECK,
		IDC_TAUE_UPPER_LIMIT, IDH_TEMP_ACF_TAUE_UPPER_LIMIT,
		IDC_TAUE_LOWER_CHECK, IDH_TEMP_ACF_TAUE_LOWER_CHECK,
		IDC_TAUE_LOWER_LIMIT, IDH_TEMP_ACF_TAUE_LOWER_LIMIT,
		IDC_PHI1_STANDARD, IDH_TEMP_ACF_PHI1_STANDARD,
		IDC_PHI1_UPPER_CHECK, IDH_TEMP_ACF_PHI1_UPPER_CHECK,
		IDC_PHI1_UPPER_LIMIT, IDH_TEMP_ACF_PHI1_UPPER_LIMIT,
		IDC_PHI1_LOWER_CHECK, IDH_TEMP_ACF_PHI1_LOWER_CHECK,
		IDC_PHI1_LOWER_LIMIT, IDH_TEMP_ACF_PHI1_LOWER_LIMIT,
		IDC_TAU1_STANDARD, IDH_TEMP_ACF_TAU1_STANDARD,
		IDC_TAU1_UPPER_CHECK, IDH_TEMP_ACF_TAU1_UPPER_CHECK,
		IDC_TAU1_UPPER_LIMIT, IDH_TEMP_ACF_TAU1_UPPER_LIMIT,
		IDC_TAU1_LOWER_CHECK, IDH_TEMP_ACF_TAU1_LOWER_CHECK,
		IDC_TAU1_LOWER_LIMIT, IDH_TEMP_ACF_TAU1_LOWER_LIMIT,
		IDC_PHI0_WEIGHTING, IDH_TEMP_ACF_PHI0_WEIGHTING,
		IDC_TAUE_WEIGHTING, IDH_TEMP_ACF_TAUE_WEIGHTING,
		IDC_PHI1_WEIGHTING, IDH_TEMP_ACF_PHI1_WEIGHTING,
		IDC_TAU1_WEIGHTING, IDH_TEMP_ACF_TAU1_WEIGHTING,
		IDC_SET_WEIGHTING, IDH_TEMP_ACF_SET_WEIGHTING,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
