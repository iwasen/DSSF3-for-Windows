// AcfFactor.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "AcfFactor.h"
#include "SasView.h"
#include "CalcAcf.h"
#include "AcfScDlg.h"
#include "OutputFile.h"
#include "CalcAcfDlg.h"
#include "ProgressDlg.h"
#include "AcfMarkerDlg.h"
#include "Help\ContextHelp.h"

#define CLM_TIME	0
#define CLM_PHI0	1
#define CLM_TAUE	2
#define CLM_TAU1	3
#define CLM_PHI1	4
//#define CLM_SPL		5
#define CLM_IACC	5
#define CLM_TIACC	6
#define CLM_WIACC	7

/////////////////////////////////////////////////////////////////////////////
// CAcfFactor ダイアログ

CObList CAcfFactor::m_WindowList;

CAcfFactor::CAcfFactor(CWnd* pParent /*=NULL*/)
	: CDialogML(CAcfFactor::IDD, pParent)
{
	m_iSampling = 0;
	m_iBitsPerSample = 0;

	m_bChangeData = FALSE;
	m_pAcfFactorData = NULL;
	m_pAcfFactor = NULL;
	m_pIAcfFactor = NULL;
	m_hWaveData = NULL;
	m_pGraphPhi0 = NULL;
	m_pGraphTauE = NULL;
	m_pGraphTau1 = NULL;
	m_pGraphPhi1 = NULL;
	m_pGraphSPL = NULL;
	m_pGraphIACC = NULL;
	m_pGraphTIACC = NULL;
	m_pGraphWIACC = NULL;
	m_pGraphACF = NULL;
	m_pGraphIACF = NULL;
	m_pGraphWave = NULL;
	m_pGraphSpec = NULL;
	m_nCurrentPage = 0;
	m_nSortItem = 0;
	m_nSortDir = 1;
	m_nCurrentDataNo = -1;
	m_nListKind = 0;
	m_pTaunPhinData = NULL;
	m_fStartTimePhi0 = 0;
	m_fDispTimePhi0 = 0;
	m_fStartValuePhi0 = 0;
	m_fDispValuePhi0 = 0;
	m_fStartTimeTauE = 0;
	m_fDispTimeTauE = 0;
	m_fStartValueTauE = 0;
	m_fDispValueTauE = 0;
	m_fStartTimeTau1 = 0;
	m_fDispTimeTau1 = 0;
	m_fStartValueTau1 = 0;
	m_fDispValueTau1 = 0;
	m_fStartTimePhi1 = 0;
	m_fDispTimePhi1 = 0;
	m_fStartValuePhi1 = 0;
	m_fDispValuePhi1 = 0;
	m_fStartTimeIACC = 0;
	m_fDispTimeIACC = 0;
	m_fStartValueIACC = 0;
	m_fDispValueIACC = 0;
	m_fStartTimeTIACC = 0;
	m_fDispTimeTIACC = 0;
	m_fStartValueTIACC = 0;
	m_fDispValueTIACC = 0;
	m_fStartTimeWIACC = 0;
	m_fDispTimeWIACC = 0;
	m_fStartValueWIACC = 0;
	m_fDispValueWIACC = 0;
	m_fStartTimeACF = 0;
	m_fDispTimeACF = 0;
	m_fStartTimeWave = 0;
	m_fDispTimeWave = 0;
	m_fStartValueWave = 0;
	m_fDispValueWave = 0;
	m_pAcfMarkerRec = NULL;
	m_nSelectedMarkerID = 0;

	m_WindowList.AddTail(this);
}

CAcfFactor::~CAcfFactor()
{
	DeleteGraphWindow();
	FreeBuffers();

	if (m_pAcfMarkerRec != NULL) {
		delete [] m_pAcfMarkerRec;
		m_pAcfMarkerRec = NULL;
	}

	POSITION pos = m_WindowList.Find(this);
	m_WindowList.RemoveAt(pos);
}

void CAcfFactor::DoDataExchange(CDataExchange* pDX)
{
	CDialogML::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLAYBACK, m_cPlayback);
	DDX_Control(pDX, IDC_SCREEN_COPY, m_cScreenCopy);
	DDX_Control(pDX, IDC_CALC_CYCLE, m_cCalcCycle);
	DDX_Control(pDX, IDC_SELECT_DATA, m_cSelectData);
	DDX_Control(pDX, IDC_PREV_DATA, m_cPrevData);
	DDX_Control(pDX, IDC_NEXT_DATA, m_cNextData);
	DDX_Control(pDX, IDC_TAUE_INTERVAL, m_cTaueInterval);
	DDX_Control(pDX, IDC_TAUE_ENDTIME, m_cTaueEndTime);
	DDX_Control(pDX, IDC_TAUE_ENDLEVEL, m_cTaueEndLevel);
	DDX_Control(pDX, IDC_PHI0_REFLEVEL, m_cPhi0RefLevel);
	DDX_Control(pDX, IDC_PHI0_REFDATA, m_cPhi0RefData);
	DDX_Control(pDX, IDC_INTEGRATION_TIME, m_cIntegrationTime);
	DDX_Control(pDX, IDC_FACTOR_LIST, m_cFactorList);
	DDX_Control(pDX, IDC_GRAPH, m_cGraph);
	DDX_Control(pDX, IDC_CSV_OUTPUT, m_cCsvOutput);
	DDX_Text(pDX, IDC_FOLDER_TITLE, m_sFolderTitle);
	DDX_Text(pDX, IDC_NAME, m_sName);
	DDX_Text(pDX, IDC_PLACE, m_sPlace);
	DDX_Text(pDX, IDC_FOLDER_COMMENT, m_sFolderComment);
	DDX_Text(pDX, IDC_SCALE, m_sScale);
	DDX_Text(pDX, IDC_DATE, m_sDate);
	DDX_Text(pDX, IDC_SAMPLING, m_iSampling);
	DDX_Text(pDX, IDC_MEASURE_TIME, m_sMeasureTime);
	DDX_Text(pDX, IDC_TITLE, m_sTitle);
	DDX_Text(pDX, IDC_COMMENT, m_sComment);
	DDX_Text(pDX, IDC_TIME, m_sTime);
	DDX_Text(pDX, IDC_FREQ_FILTER, m_sFreqFilter);
	DDX_Text(pDX, IDC_CHANNEL, m_sChannel);
	DDX_Text(pDX, IDC_BIT, m_iBitsPerSample);
}

BEGIN_MESSAGE_MAP(CAcfFactor, CDialogML)
	ON_BN_CLICKED(IDC_SELECT_DATA, OnSelectData)
	ON_BN_CLICKED(IDC_PREV_DATA, OnPrevData)
	ON_BN_CLICKED(IDC_NEXT_DATA, OnNextData)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_FACTOR_LIST, OnColumnclickFactorList)
	ON_NOTIFY(TCN_SELCHANGE, IDC_GRAPH, OnSelchangeGraph)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FACTOR_LIST, OnItemchangedFactorList)
	ON_BN_CLICKED(IDC_PLAYBACK, OnPlayback)
	ON_BN_CLICKED(IDC_SCREEN_COPY, OnScreenCopy)
	ON_MESSAGE(WM_SELECT_DATA, OnSelectData)
	ON_BN_CLICKED(IDC_CSV_OUTPUT, OnBnClickedCsvOutput)
	ON_WM_HELPINFO()
	ON_MESSAGE(WM_RIGHT_BUTTON, OnRightButton)
	ON_WM_ENTERIDLE()
	ON_COMMAND(IDM_ADD_ACF_MARKER, &CAcfFactor::OnAddAcfMarker)
	ON_MESSAGE(WM_SELECT_MARKER, OnSelectMarker)
	ON_COMMAND(IDM_EDIT_ACF_MARKER, &CAcfFactor::OnEditAcfMarker)
	ON_COMMAND(IDM_DELETE_ACF_MARKER, &CAcfFactor::OnDeleteAcfMarker)
	ON_COMMAND(IDM_PLAYBACK_ACF_MARKER, &CAcfFactor::OnPlaybackAcfMarker)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAcfFactor メッセージ ハンドラ

BOOL CAcfFactor::OnInitDialog()
{
	CDialogML::OnInitDialog();

	m_cFactorList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	SetFolderInfo();

	if (ReadData())
		DispGraph();
	else if (m_nAcfID == -2) {
		ShowWindow(SW_MINIMIZE);
		PostMessage(WM_CLOSE);
	}

	m_cScreenCopy.m_bNoProcess = TRUE;

	return TRUE;
}

BOOL CAcfFactor::SetFolderInfo()
{
	if (m_nFolderID != -2) {
		CDbFolder dbFolder;
		if (!dbFolder.Open())
			return FALSE;

		DbFolderRec dbFolderRec;
		if (!dbFolder.ReadRecID(m_nFolderID, &dbFolderRec))
			return FALSE;

		m_sFolderTitle = dbFolderRec.sTitle;
		m_sName = dbFolderRec.sName;
		m_sPlace = dbFolderRec.sPlace;
		m_sFolderComment = dbFolderRec.sComment;
		m_sDate = FormatTime(dbFolderRec.sDate);
		m_fScale = (dbFolderRec.fScale != 0) ? dbFolderRec.fScale : 1.0;
		m_sScale.Format("1/%g", m_fScale);
	} else
		m_fScale = 1.0;

	UpdateData(FALSE);

	return TRUE;
}

BOOL CAcfFactor::ReadData()
{
	m_cPlayback.Stop();

	m_bChangeData = TRUE;

	FreeBuffers();

	CDbAcf dbAcf;
	if (!dbAcf.Open())
		return FALSE;

	CDbAcfFactor dbAcfFactor;
	if (!dbAcfFactor.Open())
		return FALSE;

	if (!dbAcf.ReadRecID(m_nAcfID, &m_dbAcfRec))
		return FALSE;

	MMWaveReadDB(&m_hWaveData, dbAcf.GetFCB(), "WAVEDATA");

	m_iSampling = m_dbAcfRec.nSampling;
	m_sMeasureTime.Format("%.2f", m_dbAcfRec.fDataTime);
	m_iBitsPerSample = (m_dbAcfRec.nChannel & 0x04) ? 24 : 16;
	m_dbAcfRec.nChannel &= 0x03;
	m_sChannel = (m_dbAcfRec.nChannel == 1) ? "Mono" : "Stereo";
	m_sTitle = m_dbAcfRec.sTitle;
	m_sComment = m_dbAcfRec.sComment;
	m_sTime = FormatTime(m_dbAcfRec.sTime);

	m_fRate = m_dbAcfRec.nSampling / m_fScale;

	if (m_nAcfID != -2) {
		if (dbAcfFactor.ReadRecID(m_nAcfID, &m_dbAcfFactorRec, &m_AcfConditionData, &m_pAcfFactorData, &m_pTaunPhinData)) {
			AcfCondition *pAcfCondition = &m_AcfConditionData.AcfCondition;
			m_fAdjustPhi0 = GetAdjustPhi0(m_nFolderID, pAcfCondition->nPhi0RefData, pAcfCondition->fPhi0RefLevel);
			m_bCalculated = TRUE;
		} else {
			m_dbAcfFactorRec = {};
			m_fAdjustPhi0 = 0;
			m_bCalculated = FALSE;
		}
	} else {
		if (!CalcMeasurementAcf())
			return FALSE;
		m_fAdjustPhi0 = 0;
		m_bCalculated = TRUE;
	}

	if (m_dbAcfRec.nChannel == 1 && m_nSortItem > CLM_PHI1)
		m_nSortItem = 0;

	ReadAcfMarker();

	m_nListKind = GetListKind();
	InitAcfFactorList();
	DispAcfFactorList(0);
	DispCalcParam();

	EnablePrevNext();

	m_cScreenCopy.EnableWindow(m_bCalculated);
	m_cCsvOutput.EnableWindow(m_bCalculated);

	SetWindowText(m_nAcfID != -2 ? (LPCSTR)m_dbAcfRec.sTitle : SA_ACF_TITLE);

	m_bChangeData = FALSE;

	return TRUE;
}

void CAcfFactor::DispGraph()
{
	int i;

	m_cGraph.DeleteAllPages();
	SaveStartDisp();
	DeleteGraphWindow();

	if (!m_bCalculated)
		return;

	if (m_pAcfFactor != NULL)
		delete [] m_pAcfFactor;

	m_pAcfFactor = new AcfFactor[m_dbAcfFactorRec.nAcfFactor];
	for (i = 0; i < m_dbAcfFactorRec.nAcfFactor; i++)
		m_pAcfFactor[i] = m_pAcfFactorData[i].Acf;

	if (m_pIAcfFactor != NULL)
		delete [] m_pIAcfFactor;

	m_pIAcfFactor = new IAcfFactor[m_dbAcfFactorRec.nAcfFactor];
	for (i = 0; i < m_dbAcfFactorRec.nAcfFactor; i++)
		m_pIAcfFactor[i] = m_pAcfFactorData[i].IAcf;

	m_pGraphPhi0 = new CGraphFactor;
	m_pGraphPhi0->m_nFactor = FACTOR_PHI0;
	m_pGraphPhi0->m_pAcfFactor = m_pAcfFactor;
	m_pGraphPhi0->m_nData = m_dbAcfFactorRec.nAcfFactor;
	m_pGraphPhi0->m_fStepTime = m_AcfConditionData.AcfCondition.fRunningStep;
	m_pGraphPhi0->m_fAdjustPhi0 = m_fAdjustPhi0;
	m_pGraphPhi0->m_fStartTime = m_fStartTimePhi0;
	m_pGraphPhi0->m_fDispTime = m_fDispTimePhi0;
	m_pGraphPhi0->m_fStartValue = m_fStartValuePhi0;
	m_pGraphPhi0->m_fDispValue = m_fDispValuePhi0;
	m_pGraphPhi0->m_pAcfMarkerRec = &m_pAcfMarkerRec;
	m_pGraphPhi0->m_nAcfMarkerRec = &m_nAcfMarkerRec;
	m_cGraph.AddPage(m_pGraphPhi0, IDD_GRAPH_FACTOR, IDS_PHI0);

	m_pGraphTauE = new CGraphFactor;
	m_pGraphTauE->m_nFactor = FACTOR_TAUE;
	m_pGraphTauE->m_pAcfFactor = m_pAcfFactor;
	m_pGraphTauE->m_nData = m_dbAcfFactorRec.nAcfFactor;
	m_pGraphTauE->m_fStepTime = m_AcfConditionData.AcfCondition.fRunningStep;
	m_pGraphTauE->m_fAdjustPhi0 = m_fAdjustPhi0;
	m_pGraphTauE->m_fStartTime = m_fStartTimeTauE;
	m_pGraphTauE->m_fDispTime = m_fDispTimeTauE;
	m_pGraphTauE->m_fStartValue = m_fStartValueTauE;
	m_pGraphTauE->m_fDispValue = m_fDispValueTauE;
	m_pGraphTauE->m_pAcfMarkerRec = &m_pAcfMarkerRec;
	m_pGraphTauE->m_nAcfMarkerRec = &m_nAcfMarkerRec;
	m_cGraph.AddPage(m_pGraphTauE, IDD_GRAPH_FACTOR, IDS_TAUE);

	m_pGraphTau1 = new CGraphFactor;
	m_pGraphTau1->m_nFactor = FACTOR_TAU1;
	m_pGraphTau1->m_pAcfFactor = m_pAcfFactor;
	m_pGraphTau1->m_nData = m_dbAcfFactorRec.nAcfFactor;
	m_pGraphTau1->m_fStepTime = m_AcfConditionData.AcfCondition.fRunningStep;
	m_pGraphTau1->m_fAdjustPhi0 = m_fAdjustPhi0;
	m_pGraphTau1->m_fStartTime = m_fStartTimeTau1;
	m_pGraphTau1->m_fDispTime = m_fDispTimeTau1;
	m_pGraphTau1->m_fStartValue = m_fStartValueTau1;
	m_pGraphTau1->m_fDispValue = m_fDispValueTau1;
	m_pGraphTau1->m_pAcfMarkerRec = &m_pAcfMarkerRec;
	m_pGraphTau1->m_nAcfMarkerRec = &m_nAcfMarkerRec;
	m_cGraph.AddPage(m_pGraphTau1, IDD_GRAPH_FACTOR, IDS_TAU1);

	m_pGraphPhi1 = new CGraphFactor;
	m_pGraphPhi1->m_nFactor = FACTOR_PHI1;
	m_pGraphPhi1->m_pAcfFactor = m_pAcfFactor;
	m_pGraphPhi1->m_nData = m_dbAcfFactorRec.nAcfFactor;
	m_pGraphPhi1->m_fStepTime = m_AcfConditionData.AcfCondition.fRunningStep;
	m_pGraphPhi1->m_fAdjustPhi0 = m_fAdjustPhi0;
	m_pGraphPhi1->m_fStartTime = m_fStartTimePhi1;
	m_pGraphPhi1->m_fDispTime = m_fDispTimePhi1;
	m_pGraphPhi1->m_fStartValue = m_fStartValuePhi1;
	m_pGraphPhi1->m_fDispValue = m_fDispValuePhi1;
	m_pGraphPhi1->m_pAcfMarkerRec = &m_pAcfMarkerRec;
	m_pGraphPhi1->m_nAcfMarkerRec = &m_nAcfMarkerRec;
	m_cGraph.AddPage(m_pGraphPhi1, IDD_GRAPH_FACTOR, IDS_PHI1);

	if (m_dbAcfRec.nChannel == 2) {
/*
		m_pGraphSPL = new CGraphFactor;
		m_pGraphSPL->m_nFactor = FACTOR_SPL;
		m_pGraphSPL->m_pIAcfFactor = m_pIAcfFactor;
		m_pGraphSPL->m_nData = m_dbAcfFactorRec.nAcfFactor;
		m_pGraphSPL->m_fStepTime = m_AcfConditionData.AcfCondition.fRunningStep;
		m_pGraphSPL->m_fAdjustPhi0 = 0;
		m_cGraph.AddPage(m_pGraphSPL, IDD_GRAPH_FACTOR, "SPL");
*/
		m_pGraphIACC = new CGraphFactor;
		m_pGraphIACC->m_nFactor = FACTOR_IACC;
		m_pGraphIACC->m_pIAcfFactor = m_pIAcfFactor;
		m_pGraphIACC->m_nData = m_dbAcfFactorRec.nAcfFactor;
		m_pGraphIACC->m_fStepTime = m_AcfConditionData.AcfCondition.fRunningStep;
		m_pGraphIACC->m_fAdjustPhi0 = 0;
		m_pGraphIACC->m_fStartTime = m_fStartTimeIACC;
		m_pGraphIACC->m_fDispTime = m_fDispTimeIACC;
		m_pGraphIACC->m_fStartValue = m_fStartValueIACC;
		m_pGraphIACC->m_fDispValue = m_fDispValueIACC;
		m_pGraphIACC->m_pAcfMarkerRec = &m_pAcfMarkerRec;
		m_pGraphIACC->m_nAcfMarkerRec = &m_nAcfMarkerRec;
		m_cGraph.AddPage(m_pGraphIACC, IDD_GRAPH_FACTOR, "IACC");

		m_pGraphTIACC = new CGraphFactor;
		m_pGraphTIACC->m_nFactor = FACTOR_TIACC;
		m_pGraphTIACC->m_pIAcfFactor = m_pIAcfFactor;
		m_pGraphTIACC->m_nData = m_dbAcfFactorRec.nAcfFactor;
		m_pGraphTIACC->m_fStepTime = m_AcfConditionData.AcfCondition.fRunningStep;
		m_pGraphTIACC->m_fAdjustPhi0 = 0;
		m_pGraphTIACC->m_fStartTime = m_fStartTimeTIACC;
		m_pGraphTIACC->m_fDispTime = m_fDispTimeTIACC;
		m_pGraphTIACC->m_fStartValue = m_fStartValueTIACC;
		m_pGraphTIACC->m_fDispValue = m_fDispValueTIACC;
		m_pGraphTIACC->m_pAcfMarkerRec = &m_pAcfMarkerRec;
		m_pGraphTIACC->m_nAcfMarkerRec = &m_nAcfMarkerRec;
		m_cGraph.AddPage(m_pGraphTIACC, IDD_GRAPH_FACTOR, IDS_TIACC);

		m_pGraphWIACC = new CGraphFactor;
		m_pGraphWIACC->m_nFactor = FACTOR_WIACC;
		m_pGraphWIACC->m_pIAcfFactor = m_pIAcfFactor;
		m_pGraphWIACC->m_nData = m_dbAcfFactorRec.nAcfFactor;
		m_pGraphWIACC->m_fStepTime = m_AcfConditionData.AcfCondition.fRunningStep;
		m_pGraphWIACC->m_fAdjustPhi0 = 0;
		m_pGraphWIACC->m_fStartTime = m_fStartTimeWIACC;
		m_pGraphWIACC->m_fDispTime = m_fDispTimeWIACC;
		m_pGraphWIACC->m_fStartValue = m_fStartValueWIACC;
		m_pGraphWIACC->m_fDispValue = m_fDispValueWIACC;
		m_pGraphWIACC->m_pAcfMarkerRec = &m_pAcfMarkerRec;
		m_pGraphWIACC->m_nAcfMarkerRec = &m_nAcfMarkerRec;
		m_cGraph.AddPage(m_pGraphWIACC, IDD_GRAPH_FACTOR, "W_IACC");
	}

	if (m_hWaveData != NULL) {
		m_pGraphACF = new CGraphACF;
		m_pGraphACF->m_hWaveData = m_hWaveData;
		m_pGraphACF->m_pAcfCondition = &m_AcfConditionData.AcfCondition;
		m_pGraphACF->m_pIAcfCondition = &m_AcfConditionData.IAcfCondition;
		m_pGraphACF->m_pNmsMicCal = NULL;
		m_pGraphACF->m_fRate = m_fRate;
		m_pGraphACF->m_nStep = 0;
		m_pGraphACF->m_pAcfFactor = m_pAcfFactor;
		m_pGraphACF->m_fStartTime = m_fStartTimeACF;
		m_pGraphACF->m_fDispTime = m_fDispTimeACF;
		m_cGraph.AddPage(m_pGraphACF, IDD_GRAPH_ACF, "ACF");

		if (m_dbAcfRec.nChannel == 2) {
			m_pGraphIACF = new CGraphIACF;
			m_pGraphIACF->m_hWaveData = m_hWaveData;
			m_pGraphIACF->m_pAcfCondition = &m_AcfConditionData.AcfCondition;
			m_pGraphIACF->m_pIAcfCondition = &m_AcfConditionData.IAcfCondition;
			m_pGraphIACF->m_pNmsMicCal = NULL;
			m_pGraphIACF->m_fRate = m_fRate;
			m_pGraphIACF->m_nStep = 0;
			m_pGraphIACF->m_pIAcfFactor = m_pIAcfFactor;
			m_cGraph.AddPage(m_pGraphIACF, IDD_GRAPH_IACF, "IACF");
		}

		m_pGraphWave = new CGraphWave;
		m_pGraphWave->m_hWaveData = m_hWaveData;
		m_pGraphWave->m_fStartTime = m_fStartTimeWave;
		m_pGraphWave->m_fDispTime = m_fDispTimeWave;
		m_pGraphWave->m_fStartValue = m_fStartValueWave;
		m_pGraphWave->m_fDispValue = m_fDispValueWave;
		m_pGraphWave->m_pAcfCondition = &m_AcfConditionData.AcfCondition;
		m_pGraphWave->m_pAcfMarkerRec = &m_pAcfMarkerRec;
		m_pGraphWave->m_nAcfMarkerRec = &m_nAcfMarkerRec;
		m_cGraph.AddPage(m_pGraphWave, IDD_GRAPH_WAVE, IDS_WAVE);

		m_pGraphSpec = new CGraphSpec;
		m_pGraphSpec->m_hWaveData = m_hWaveData;
		m_pGraphSpec->m_fRate = m_fRate;
		m_pGraphSpec->m_nStep = 0;
		m_pGraphSpec->m_pAcfCondition = &m_AcfConditionData.AcfCondition;
		m_cGraph.AddPage(m_pGraphSpec, IDD_GRAPH_SPEC, IDS_SPECTRUM);
	}

	if (m_nCurrentPage >= m_cGraph.GetItemCount())
		m_nCurrentPage = 0;
	m_cGraph.SetPage(m_nCurrentPage);
}

int CAcfFactor::GetListKind()
{
	switch (m_nCurrentPage) {
	case 2:
		return 2;
	case 3:
		return 3;
	default:
		return 1;
	}
}

void CAcfFactor::InitAcfFactorList()
{
	struct LISTCOLUMN {
		UINT nID;
		LPCSTR text;
		int width;
	};
	static const LISTCOLUMN tColumnHeader1[] = {
		{0, "Time", 68},
		{IDS_PHI0, NULL, 64},
		{IDS_TAUE, NULL, 64},
		{IDS_TAU1, NULL, 64},
		{IDS_PHI1, NULL, 64}
	}, tColumnHeader2[] = {
		{0, "Time", 56},
		{IDS_PHI0, NULL, 59},
		{IDS_TAUE, NULL, 59},
		{IDS_TAU1, NULL, 59},
		{IDS_PHI1, NULL, 59},
		{0, "IACC", 59},
		{IDS_TIACC, NULL, 59},
		{0, "W_IACC", 59}
	}, tColumnHeaderTau[] = {
		{0, "Time", 56},
		{IDS_TAU1, NULL, 42},
		{IDS_TAU2, NULL, 42},
		{IDS_TAU3, NULL, 42},
		{IDS_TAU4, NULL, 42},
		{IDS_TAU5, NULL, 42},
		{IDS_TAU6, NULL, 42},
		{IDS_TAU7, NULL, 42},
		{IDS_TAU8, NULL, 42},
		{IDS_TAU9, NULL, 42},
		{IDS_TAU10, NULL, 43}
	}, tColumnHeaderPhi[] = {
		{0, "Time", 56},
		{IDS_PHI1, NULL, 42},
		{IDS_PHI2, NULL, 42},
		{IDS_PHI3, NULL, 42},
		{IDS_PHI4, NULL, 42},
		{IDS_PHI5, NULL, 42},
		{IDS_PHI6, NULL, 42},
		{IDS_PHI7, NULL, 42},
		{IDS_PHI8, NULL, 42},
		{IDS_PHI9, NULL, 42},
		{IDS_PHI10, NULL, 43}
	};
	const LISTCOLUMN *pColumnHeader;
	static const char *tUnit1[] = {" [s]", " [dB]", " [ms]", " [ms]", ""};

	m_cFactorList.DeleteAllItems();

	while (m_cFactorList.DeleteColumn(0))
		;

	if (!m_bCalculated)
		return;

	const char **pUnit = NULL;
	CString str;
	switch (m_nListKind) {
	case 1:
		if (m_dbAcfRec.nChannel == 1) {
			pColumnHeader = tColumnHeader1;
			m_nColumn = ARRAY_SIZE(tColumnHeader1);
			pUnit = tUnit1;
		} else {
			pColumnHeader = tColumnHeader2;
			m_nColumn = ARRAY_SIZE(tColumnHeader2);
		}
		break;
	case 2:
		pColumnHeader = tColumnHeaderTau;
		m_nColumn = ARRAY_SIZE(tColumnHeaderTau);
		break;
	case 3:
		pColumnHeader = tColumnHeaderPhi;
		m_nColumn = ARRAY_SIZE(tColumnHeaderPhi);
		break;
	default:
		return;
	}

	for (int i = 0; i < m_nColumn; i++, pColumnHeader++) {
		if (pColumnHeader->nID == 0)
			str = pColumnHeader->text;
		else
			str.LoadString(pColumnHeader->nID);

		if (pUnit != NULL)
			str += pUnit[i];

		m_cFactorList.InsertColumn(i, str, LVCFMT_RIGHT, pColumnHeader->width);
	}
}

void CAcfFactor::DispAcfFactorList(int nSel)
{
	m_cFactorList.DeleteAllItems();

	if (!m_bCalculated)
		return;

	switch (m_nListKind) {
	case 1:
		DispAcfFactorList1();
		break;
	case 2:
		DispAcfFactorList2();
		break;
	case 3:
		DispAcfFactorList3();
		break;
	}

	m_cFactorList.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
}

void CAcfFactor::DispAcfFactorList1()
{
	CString str;

	double time = 0;
	for (int i = 0; i < m_dbAcfFactorRec.nAcfFactor; i++) {
		str.Format("%g", time);
		m_cFactorList.InsertItem(i, str);

		str.Format("%.2lf", m_pAcfFactorData[i].Acf.fPhi0 + m_fAdjustPhi0);
		m_cFactorList.SetItemText(i, CLM_PHI0, str);

		str.Format("%.2lf", m_pAcfFactorData[i].Acf.fTauE * 1000);
		m_cFactorList.SetItemText(i, CLM_TAUE, str);

		str.Format("%.2lf", m_pAcfFactorData[i].Acf.fTau1 * 1000);
		m_cFactorList.SetItemText(i, CLM_TAU1, str);

		str.Format("%.2lf", m_pAcfFactorData[i].Acf.fPhi1);
		m_cFactorList.SetItemText(i, CLM_PHI1, str);

		if (m_dbAcfRec.nChannel == 2) {
			str.Format("%.2lf", m_pAcfFactorData[i].IAcf.fIACC);
			m_cFactorList.SetItemText(i, CLM_IACC, str);

			str.Format("%.2lf", m_pAcfFactorData[i].IAcf.fTauIACC * 1000);
			m_cFactorList.SetItemText(i, CLM_TIACC, str);

			str.Format("%.2lf", m_pAcfFactorData[i].IAcf.fWIACC * 1000);
			m_cFactorList.SetItemText(i, CLM_WIACC, str);
		}

		m_cFactorList.SetItemData(i, i);

		time += m_AcfConditionData.AcfCondition.fRunningStep;
	}
}

void CAcfFactor::DispAcfFactorList2()
{
	CString str;

	double time = 0;
	for (int i = 0; i < m_dbAcfFactorRec.nAcfFactor; i++) {
		str.Format("%g", time);
		m_cFactorList.InsertItem(i, str);

		for (int j = 0; j < 10; j++) {
			if (m_pTaunPhinData[i].fTaun[j] != 0) {
				str.Format("%.2lf", m_pTaunPhinData[i].fTaun[j] * 1000);
				m_cFactorList.SetItemText(i, j + 1, str);
			}
		}

		m_cFactorList.SetItemData(i, i);

		time += m_AcfConditionData.AcfCondition.fRunningStep;
	}
}

void CAcfFactor::DispAcfFactorList3()
{
	CString str;

	double time = 0;
	for (int i = 0; i < m_dbAcfFactorRec.nAcfFactor; i++) {
		str.Format("%g", time);
		m_cFactorList.InsertItem(i, str);

		for (int j = 0; j < 10; j++) {
			if (m_pTaunPhinData[i].fPhin[j] != 0) {
				str.Format("%.2lf", m_pTaunPhinData[i].fPhin[j]);
				m_cFactorList.SetItemText(i, j + 1, str);
			}
		}

		m_cFactorList.SetItemData(i, i);

		time += m_AcfConditionData.AcfCondition.fRunningStep;
	}
}

void CAcfFactor::ReDispAcfFactorList()
{
	const int nListKind = GetListKind();
	if (nListKind != m_nListKind) {
		m_nListKind = nListKind;

		int nSel = 0;
		POSITION pos;
		if ((pos = m_cFactorList.GetFirstSelectedItemPosition()) != NULL)
			nSel = m_cFactorList.GetNextSelectedItem(pos);

		const int nTopIndex = m_cFactorList.GetTopIndex();
		m_cFactorList.SetRedraw(FALSE);

		InitAcfFactorList();
		DispAcfFactorList(nSel);

		CRect rect;
		m_cFactorList.GetItemRect(0, rect, LVIR_BOUNDS);
		const SIZE size = {0, nTopIndex * rect.Height()};
		m_cFactorList.SetRedraw(TRUE);
		m_cFactorList.Scroll(size);
	}
}

void CAcfFactor::DispCalcParam()
{
	static constexpr int tFilterName[] = {IDS_FLAT, IDS_AWEIGHTING, IDS_BWEIGHTING, IDS_CWEIGHTING};

	if (m_bCalculated) {
		AcfCondition *pAcfCondition = &m_AcfConditionData.AcfCondition;
		if (pAcfCondition->nPhi0RefData == SPL_ABSOLUTE)
			m_cPhi0RefData.LoadString(IDS_ABSVALUE);
		else if (pAcfCondition->nPhi0RefData == SPL_MAXREF)
			m_cPhi0RefData.LoadString(IDS_MAXVALUE);
		else {
			CDbAcf dbAcf;
			DbAcfRec dbAcfRec;

			if (dbAcf.Open()) {
				if (dbAcf.ReadRecID(pAcfCondition->nPhi0RefData, &dbAcfRec))
					m_cPhi0RefData = dbAcfRec.sTitle;
			}
		}

		if (pAcfCondition->nPhi0RefData == SPL_ABSOLUTE)
			m_cPhi0RefLevel.Blank();
		else
			m_cPhi0RefLevel = pAcfCondition->fPhi0RefLevel;

		m_cTaueInterval = pAcfCondition->fTaueInterval;
		m_cTaueEndTime = pAcfCondition->fTaueEndTime;
		m_cTaueEndLevel = pAcfCondition->fTaueEndLevel;
		m_cIntegrationTime = pAcfCondition->fIntegrationTime;
		m_cCalcCycle = pAcfCondition->fRunningStep;
		m_sFreqFilter.LoadString(tFilterName[pAcfCondition->nFreqWeighting]);
	} else {
		m_cTaueInterval.Blank();
		m_cTaueEndTime.Blank();
		m_cTaueEndLevel.Blank();
		m_cPhi0RefLevel.Blank();
		m_cPhi0RefData.Blank();
		m_cIntegrationTime.Blank();
		m_sFreqFilter.Empty();
	}

	UpdateData(FALSE);
}

void CAcfFactor::DeleteGraphWindow()
{
	if (m_pGraphPhi0 != NULL) {
		delete m_pGraphPhi0;
		m_pGraphPhi0 = NULL;
	}

	if (m_pGraphTauE != NULL) {
		delete m_pGraphTauE;
		m_pGraphTauE = NULL;
	}

	if (m_pGraphTau1 != NULL) {
		delete m_pGraphTau1;
		m_pGraphTau1 = NULL;
	}

	if (m_pGraphPhi1 != NULL) {
		delete m_pGraphPhi1;
		m_pGraphPhi1 = NULL;
	}

	if (m_pGraphSPL != NULL) {
		delete m_pGraphSPL;
		m_pGraphSPL = NULL;
	}

	if (m_pGraphIACC != NULL) {
		delete m_pGraphIACC;
		m_pGraphIACC = NULL;
	}

	if (m_pGraphTIACC != NULL) {
		delete m_pGraphTIACC;
		m_pGraphTIACC = NULL;
	}

	if (m_pGraphWIACC != NULL) {
		delete m_pGraphWIACC;
		m_pGraphWIACC = NULL;
	}

	if (m_pGraphACF != NULL) {
		delete m_pGraphACF;
		m_pGraphACF = NULL;
	}

	if (m_pGraphIACF != NULL) {
		delete m_pGraphIACF;
		m_pGraphIACF = NULL;
	}

	if (m_pGraphWave != NULL) {
		delete m_pGraphWave;
		m_pGraphWave = NULL;
	}

	if (m_pGraphSpec != NULL) {
		delete m_pGraphSpec;
		m_pGraphSpec = NULL;
	}
}

void CAcfFactor::FreeBuffers()
{
	if (m_pAcfFactorData != NULL) {
		delete [] m_pAcfFactorData;
		m_pAcfFactorData = NULL;
	}

	if (m_pAcfFactor != NULL) {
		delete [] m_pAcfFactor;
		m_pAcfFactor = NULL;
	}

	if (m_pIAcfFactor != NULL) {
		delete [] m_pIAcfFactor;
		m_pIAcfFactor = NULL;
	}

	if (m_hWaveData != NULL) {
		::GlobalFree(m_hWaveData);
		m_hWaveData = NULL;
	}

	if (m_pTaunPhinData != NULL) {
		delete [] m_pTaunPhinData;
		m_pTaunPhinData = NULL;
	}
}

void CAcfFactor::OnSelectData()
{
	AfxGetApp()->m_pMainWnd->SetActiveWindow();
}

void CAcfFactor::OnPrevData()
{
	long nAcfID;

	if ((nAcfID = m_pView->GetPrevItem(m_nAcfID)) != -1) {
		m_nAcfID = nAcfID;
		ReadData();
		DispGraph();
	}
}

void CAcfFactor::OnNextData()
{
	long nAcfID;

	if ((nAcfID = m_pView->GetNextItem(m_nAcfID)) != -1) {
		m_nAcfID = nAcfID;
		ReadData();
		DispGraph();
	}
}

void CAcfFactor::EnablePrevNext()
{
	m_cPrevData.EnableWindow(m_nAcfID != -2 && m_pView->GetPrevItem(m_nAcfID) != -1);
	m_cNextData.EnableWindow(m_nAcfID != -2 && m_pView->GetNextItem(m_nAcfID) != -1);
	m_cSelectData.EnableWindow(m_nAcfID != -2);
}

void CAcfFactor::OnColumnclickFactorList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (m_nSortItem != pNMListView->iSubItem)
		m_nSortDir = 1;
	else
		m_nSortDir *= -1;

	m_nSortItem = pNMListView->iSubItem;

	switch (m_nListKind) {
	case 1:
		m_cFactorList.SortItems(CompareItems1, (LPARAM)this);
		break;
	case 2:
		m_cFactorList.SortItems(CompareItems2, (LPARAM)this);
		break;
	case 3:
		m_cFactorList.SortItems(CompareItems3, (LPARAM)this);
		break;
	}

	*pResult = 0;
}

int CALLBACK CAcfFactor::CompareItems1(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CAcfFactor *pWnd = (CAcfFactor *)lParamSort;
	AcfFactorData *pAcfFactorData = pWnd->m_pAcfFactorData;
	double d;

	switch (pWnd->m_nSortItem) {
	case CLM_PHI0:
		d = pAcfFactorData[lParam1].Acf.fPhi0 -
			pAcfFactorData[lParam2].Acf.fPhi0;
		break;
	case CLM_TAUE:
		d = pAcfFactorData[lParam1].Acf.fTauE -
			pAcfFactorData[lParam2].Acf.fTauE;
		break;
	case CLM_TAU1:
		d = pAcfFactorData[lParam1].Acf.fTau1 -
			pAcfFactorData[lParam2].Acf.fTau1;
		break;
	case CLM_PHI1:
		d = pAcfFactorData[lParam1].Acf.fPhi1 -
			pAcfFactorData[lParam2].Acf.fPhi1;
		break;
/*
	case CLM_SPL:
		d = pAcfFactorData[lParam1].IAcf.fSPL -
			pAcfFactorData[lParam2].IAcf.fSPL;
		break;
*/
	case CLM_IACC:
		d = pAcfFactorData[lParam1].IAcf.fIACC -
			pAcfFactorData[lParam2].IAcf.fIACC;
		break;
	case CLM_TIACC:
		d = pAcfFactorData[lParam1].IAcf.fTauIACC -
			pAcfFactorData[lParam2].IAcf.fTauIACC;
		break;
	case CLM_WIACC:
		d = pAcfFactorData[lParam1].IAcf.fWIACC -
			pAcfFactorData[lParam2].IAcf.fWIACC;
		break;
	default:
		return (int)(lParam1 - lParam2) * pWnd->m_nSortDir;
	}

	d *= pWnd->m_nSortDir;

	if (d > 0)
		return 1;
	else if (d < 0)
		return  -1;
	else
		return 0;
}

int CALLBACK CAcfFactor::CompareItems2(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CAcfFactor *pWnd = (CAcfFactor *)lParamSort;
	if (pWnd->m_nSortItem == 0)
		return (int)(lParam1 - lParam2) * pWnd->m_nSortDir;

	const int n = pWnd->m_nSortItem - 1;
	const double d = (pWnd->m_pTaunPhinData[lParam1].fTaun[n] - pWnd->m_pTaunPhinData[lParam2].fTaun[n]) * pWnd->m_nSortDir;

	if (d > 0)
		return 1;
	else if (d < 0)
		return  -1;
	else
		return 0;
}

int CALLBACK CAcfFactor::CompareItems3(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CAcfFactor *pWnd = (CAcfFactor *)lParamSort;
	if (pWnd->m_nSortItem == 0)
		return (int)(lParam1 - lParam2) * pWnd->m_nSortDir;

	const int n = pWnd->m_nSortItem - 1;
	const double d = (pWnd->m_pTaunPhinData[lParam1].fPhin[n] - pWnd->m_pTaunPhinData[lParam2].fPhin[n]) * pWnd->m_nSortDir;

	if (d > 0)
		return 1;
	else if (d < 0)
		return  -1;
	else
		return 0;
}

void CAcfFactor::OnCancel()
{
	DestroyWindow();
}

void CAcfFactor::CloseAll()
{
	CWnd *pWnd;

	while (!m_WindowList.IsEmpty()) {
		pWnd = (CWnd *)m_WindowList.GetHead();
		if (pWnd != NULL)
			pWnd->DestroyWindow();
	}
}

void CAcfFactor::OnSelchangeGraph(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	m_nCurrentPage = m_cGraph.GetCurSel();

	m_bChangeData = TRUE;
	ReDispAcfFactorList();
	m_bChangeData = FALSE;

	*pResult = 0;
}

void CAcfFactor::OnItemchangedFactorList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	//m_cFactorList.UpdateWindow();

	if (pNMListView->uNewState & LVIS_SELECTED && m_nCurrentDataNo != pNMListView->lParam && !m_bChangeData) {
		m_nCurrentDataNo = (int)pNMListView->lParam;

		m_cFactorList.UpdateWindow();

		RedrawGraph();
	}

	*pResult = 0;
}

void CAcfFactor::RedrawGraph()
{
	if (m_pGraphPhi0 != NULL)
		m_pGraphPhi0->ReDraw(m_nCurrentDataNo);

	if (m_pGraphTauE != NULL)
		m_pGraphTauE->ReDraw(m_nCurrentDataNo);

	if (m_pGraphTau1 != NULL)
		m_pGraphTau1->ReDraw(m_nCurrentDataNo);

	if (m_pGraphPhi1 != NULL)
		m_pGraphPhi1->ReDraw(m_nCurrentDataNo);
/*
	if (m_pGraphSPL != NULL)
		m_pGraphSPL->ReDraw(m_nCurrentDataNo);
*/
	if (m_pGraphIACC != NULL)
		m_pGraphIACC->ReDraw(m_nCurrentDataNo);

	if (m_pGraphTIACC != NULL)
		m_pGraphTIACC->ReDraw(m_nCurrentDataNo);

	if (m_pGraphWIACC != NULL)
		m_pGraphWIACC->ReDraw(m_nCurrentDataNo);

	if (m_pGraphACF != NULL)
		m_pGraphACF->ReDraw(m_nCurrentDataNo);

	if (m_pGraphIACF != NULL)
		m_pGraphIACF->ReDraw(m_nCurrentDataNo);

	if (m_pGraphWave != NULL)
		m_pGraphWave->ReDraw(m_nCurrentDataNo);

	if (m_pGraphSpec != NULL)
		m_pGraphSpec->ReDraw(m_nCurrentDataNo);
}

void CAcfFactor::OnPlayback()
{
	m_cPlayback.Play(m_hWaveData);
}

void CAcfFactor::OnScreenCopy()
{
	CAcfScDlg acfScDlg(this);

	if (m_hWaveData == NULL)
		acfScDlg.m_bNoWave = TRUE;

	acfScDlg.m_nChannel = m_dbAcfRec.nChannel;

	switch (acfScDlg.DoModal()) {
	case IDOK:
		{
			CString filter;
			CString title;

			filter.LoadString(IDS_IMAGE_FILE);
			filter += " (*.png)|*.png||";
			CFileDialog dlg(FALSE, "png", NULL, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, filter, this, 0);

			title.LoadString(IDS_SAVE_IMAGE_FILE);
			dlg.m_ofn.lpstrTitle = title;
			if (dlg.DoModal() == IDOK) {
				HDIB hDIB = ScreenCopy();
				if (hDIB != NULL) {
					MMPictureWriteFile(hDIB, (LPSTR)(LPCSTR)dlg.GetPathName());
					::GlobalFree(hDIB);
				}
			}
		}
		break;
	case IDC_MMLIB:
		HDIB hDIB = ScreenCopy();
		SendMMLib(this, m_sFolderTitle, m_sFolderComment, m_sTitle, m_sComment, GetDataText(), hDIB);
		::GlobalFree(hDIB);
		break;
	}
}

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
HDIB CAcfFactor::ScreenCopy()
{
	if (setData.AcfSC.nCopyMode == 0) {
		UpdateWindow();
		return m_cScreenCopy.CreateDIB(this);
	} else if (setData.AcfSC.bMulti)
		return GraphCopy();
	else {
		m_cGraph.UpdateWindow();
		m_cGraph.GetCurrentDlg()->UpdateWindow();

		CWnd *pWnd;
		if (setData.ImpSC.bFrame)
			pWnd = &m_cGraph;
		else
			pWnd = m_cGraph.GetCurrentDlg();

		return m_cScreenCopy.CreateDIB(pWnd);
	}
}

HDIB CAcfFactor::GraphCopy()
{
	int i, nGraph = 0;

	for (i = 0; i < 9; i++) {
		if (m_dbAcfRec.nChannel != 2) {
			switch (i) {
			case 4:
			case 5:
			case 6:
			case 8:
				continue;
			}
		}

		if (m_hWaveData == NULL) {
			switch (i) {
			case 7:
			case 8:
				continue;
			}
		}

		if (setData.AcfSC.bGraph[i])
			nGraph++;
	}

	if (nGraph == 0)
		return NULL;

	int nCol = min(setData.AcfSC.nColumns, nGraph);
	int nRow = (nGraph + setData.AcfSC.nColumns - 1) / setData.AcfSC.nColumns;

	CRect rect;
	if (setData.AcfSC.bFrame)
		m_cGraph.GetWindowRect(rect);
	else
		m_pGraphPhi0->GetWindowRect(rect);

	const int nWidth = (setData.AcfSC.nColSpace + rect.Width()) * nCol + setData.AcfSC.nColSpace;
	const int nHeight = (setData.AcfSC.nRowSpace + rect.Height()) * nRow + setData.AcfSC.nRowSpace;

	CDC *pDC = GetDC();
	CDC DCMem;
	DCMem.CreateCompatibleDC(pDC);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, nWidth, nHeight);

	CGdiObject *pOrg = DCMem.SelectObject(&bmp);
	DCMem.FillSolidRect(0, 0, nWidth, nHeight, GetSysColor(COLOR_3DFACE));

	const int nCurrentPage = m_nCurrentPage;

	int nIndex = 0;
	nCol = nRow = 0;
	for (i = 0; i < 9; i++) {
		if (m_dbAcfRec.nChannel != 2) {
			switch (i) {
			case 4:
			case 5:
			case 6:
			case 8:
				continue;
			}
		}

		if (m_hWaveData == NULL) {
			switch (i) {
			case 7:
			case 8:
				continue;
			}
		}

		if (setData.AcfSC.bGraph[i]) {
			m_cGraph.SetPage(nIndex);
			m_cGraph.UpdateWindow();
			m_cGraph.GetCurrentDlg()->UpdateWindow();

			CWnd *pWnd;
			if (setData.AcfSC.bFrame)
				pWnd = &m_cGraph;
			else
				pWnd = m_cGraph.GetCurrentDlg();

			const int x = setData.AcfSC.nColSpace + nCol * (rect.Width() + setData.AcfSC.nColSpace);
			const int y = setData.AcfSC.nRowSpace + nRow * (rect.Height() + setData.AcfSC.nRowSpace);
			DCMem.BitBlt(x, y, rect.Width(), rect.Height(), pWnd->GetWindowDC(), 0, 0, SRCCOPY);

			if (++nCol == setData.AcfSC.nColumns) {
				nCol = 0;
				nRow++;
			}
		}

		nIndex++;
	}

	m_cGraph.SetPage(nCurrentPage);

	DCMem.SelectObject(pOrg);

	const int nSize = sizeof(BITMAPINFOHEADER) + WIDTHBYTES(nWidth * 24) * nHeight;
	HDIB hDIB = ::GlobalAlloc(GMEM_MOVEABLE, nSize);
	BITMAPINFO *pBmi = (BITMAPINFO *)::GlobalLock(hDIB);

	char *lpvBits = (LPSTR)pBmi + sizeof(BITMAPINFOHEADER);

	memset(pBmi, 0, sizeof(BITMAPINFOHEADER));
	pBmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pBmi->bmiHeader.biWidth = nWidth;
	pBmi->bmiHeader.biHeight = nHeight;
	pBmi->bmiHeader.biPlanes = 1;
	pBmi->bmiHeader.biBitCount = 24;
	pBmi->bmiHeader.biCompression = BI_RGB;

	GetDIBits(DCMem.m_hDC, (HBITMAP)bmp.m_hObject, 0, nHeight, lpvBits, pBmi, DIB_RGB_COLORS);
	::GlobalUnlock(hDIB);

	return hDIB;
}

CString CAcfFactor::GetDataText()
{
	int i, j;
	CString data;
	char text[32];

	LVCOLUMN lvc{
		.mask = LVCF_TEXT,
		.pszText = text,
		.cchTextMax = sizeof(text)
	};

	for (i = 0; i < m_nColumn; i++) {
		m_cFactorList.GetColumn(i, &lvc);
		if (i != 0)
			data += ",";
		data += lvc.pszText;
	}
	data += "\r\n";

	const int nItem = m_cFactorList.GetItemCount();
	for (i = 0; i < nItem; i++) {
		for (j = 0; j < m_nColumn; j++) {
			if (j != 0)
				data += ",";
			data += m_cFactorList.GetItemText(i, j);
		}
		data += "\r\n";
	}

	return data;
}

void CAcfFactor::SaveStartDisp()
{
	if (m_pGraphPhi0 != NULL) {
		m_fStartTimePhi0 = m_pGraphPhi0->m_fStartTime;
		m_fDispTimePhi0 = m_pGraphPhi0->m_fDispTime;
		m_fStartValuePhi0 = m_pGraphPhi0->m_fStartValue;
		m_fDispValuePhi0 = m_pGraphPhi0->m_fDispValue;
	}

	if (m_pGraphTauE != NULL) {
		m_fStartTimeTauE = m_pGraphTauE->m_fStartTime;
		m_fDispTimeTauE = m_pGraphTauE->m_fDispTime;
		m_fStartValueTauE = m_pGraphTauE->m_fStartValue;
		m_fDispValueTauE = m_pGraphTauE->m_fDispValue;
	}

	if (m_pGraphTau1 != NULL) {
		m_fStartTimeTau1 = m_pGraphTau1->m_fStartTime;
		m_fDispTimeTau1 = m_pGraphTau1->m_fDispTime;
		m_fStartValueTau1 = m_pGraphTau1->m_fStartValue;
		m_fDispValueTau1 = m_pGraphTau1->m_fDispValue;
	}

	if (m_pGraphPhi1 != NULL) {
		m_fStartTimePhi1 = m_pGraphPhi1->m_fStartTime;
		m_fDispTimePhi1 = m_pGraphPhi1->m_fDispTime;
		m_fStartValuePhi1 = m_pGraphPhi1->m_fStartValue;
		m_fDispValuePhi1 = m_pGraphPhi1->m_fDispValue;
	}

	if (m_pGraphIACC != NULL) {
		m_fStartTimeIACC = m_pGraphIACC->m_fStartTime;
		m_fDispTimeIACC = m_pGraphIACC->m_fDispTime;
		m_fStartValueIACC = m_pGraphIACC->m_fStartValue;
		m_fDispValueIACC = m_pGraphIACC->m_fDispValue;
	}

	if (m_pGraphTIACC != NULL) {
		m_fStartTimeTIACC = m_pGraphTIACC->m_fStartTime;
		m_fDispTimeTIACC = m_pGraphTIACC->m_fDispTime;
		m_fStartValueTIACC = m_pGraphTIACC->m_fStartValue;
		m_fDispValueTIACC = m_pGraphTIACC->m_fDispValue;
	}

	if (m_pGraphWIACC != NULL) {
		m_fStartTimeWIACC = m_pGraphWIACC->m_fStartTime;
		m_fDispTimeWIACC = m_pGraphWIACC->m_fDispTime;
		m_fStartValueWIACC = m_pGraphWIACC->m_fStartValue;
		m_fDispValueWIACC = m_pGraphWIACC->m_fDispValue;
	}

	if (m_pGraphACF != NULL) {
		m_fStartTimeACF = m_pGraphACF->m_fStartTime;
		m_fDispTimeACF = m_pGraphACF->m_fDispTime;
	}

	if (m_pGraphWave != NULL) {
		m_fStartTimeWave = m_pGraphWave->m_fStartTime;
		m_fDispTimeWave = m_pGraphWave->m_fDispTime;
		m_fStartValueWave = m_pGraphWave->m_fStartValue;
		m_fDispValueWave = m_pGraphWave->m_fDispValue;
	}
}

LRESULT CAcfFactor::OnSelectData(WPARAM wParam, LPARAM /*lParam*/)
{
	const int nIndex = (int)(*(double *)wParam / m_AcfConditionData.AcfCondition.fRunningStep + 0.5);

	if (nIndex >= 0 && nIndex < m_cFactorList.GetItemCount()) {
		m_cFactorList.SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);
		m_cFactorList.EnsureVisible(nIndex, FALSE);
	}

	return 0;
}

void CAcfFactor::OnBnClickedCsvOutput()
{
	CFileDialog fileDlg(FALSE, "csv", NULL,
			OFN_OVERWRITEPROMPT, "CSV File (*.csv)|*.csv|All Files (*.*)|*.*||", this, 0);

	if (fileDlg.DoModal() == IDOK)
		OutputListToCsv(m_cFactorList, fileDlg.GetPathName());
}

BOOL CAcfFactor::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDCANCEL, IDH_ACF_FACTOR_CANCEL,
		IDC_SELECT_DATA, IDH_ACF_FACTOR_SELECT_DATA,
		IDC_PREV_DATA, IDH_ACF_FACTOR_PREV_DATA,
		IDC_NEXT_DATA, IDH_ACF_FACTOR_NEXT_DATA,
		IDC_SCREEN_COPY, IDH_ACF_FACTOR_SCREEN_COPY,
		IDC_CSV_OUTPUT, IDH_ACF_FACTOR_CSV_OUTPUT,
		IDC_GRAPH, IDH_ACF_FACTOR_GRAPH,
		IDC_FOLDER_TITLE, IDH_ACF_FACTOR_FOLDER_TITLE,
		IDC_NAME, IDH_ACF_FACTOR_NAME,
		IDC_PLACE, IDH_ACF_FACTOR_PLACE,
		IDC_FOLDER_COMMENT, IDH_ACF_FACTOR_FOLDER_COMMENT,
		IDC_DATE, IDH_ACF_FACTOR_DATE,
		IDC_SCALE, IDH_ACF_FACTOR_SCALE,
		IDC_SAMPLING, IDH_ACF_FACTOR_SAMPLING,
		IDC_MEASURE_TIME, IDH_ACF_FACTOR_MEASURE_TIME,
		IDC_CHANNEL, IDH_ACF_FACTOR_CHANNEL,
		IDC_TITLE, IDH_ACF_FACTOR_TITLE,
		IDC_COMMENT, IDH_ACF_FACTOR_COMMENT,
		IDC_TIME, IDH_ACF_FACTOR_TIME,
		IDC_PLAYBACK, IDH_ACF_FACTOR_PLAYBACK,
		IDC_INTEGRATION_TIME, IDH_ACF_FACTOR_INTEGRATION_TIME,
		IDC_CALC_CYCLE, IDH_ACF_FACTOR_CALC_CYCLE,
		IDC_FREQ_FILTER, IDH_ACF_FACTOR_FREQ_FILTER,
		IDC_TAUE_INTERVAL, IDH_ACF_FACTOR_TAUE_INTERVAL,
		IDC_TAUE_ENDLEVEL, IDH_ACF_FACTOR_TAUE_ENDLEVEL,
		IDC_TAUE_ENDTIME, IDH_ACF_FACTOR_TAUE_ENDTIME,
		IDC_PHI0_REFDATA, IDH_ACF_FACTOR_PHI0_REFDATA,
		IDC_PHI0_REFLEVEL, IDH_ACF_FACTOR_PHI0_REFLEVEL,
		IDC_FACTOR_LIST, IDH_ACF_FACTOR_FACTOR_LIST,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

static CProgressDlg *pProgressDlg;

BOOL CAcfFactor::CalcMeasurementAcf()
{
	CCalcAcfDlg dlg;

	dlg.m_AcfCondition = setData.AcfCondition;
	dlg.m_nFolderID = m_nFolderID;
	if (dlg.DoModal() == IDCANCEL)
		return FALSE;
	setData.AcfCondition = dlg.m_AcfCondition;

	CString str;
	str.LoadString(IDS_CALCULATING);
	CProgressDlg progressDlg(this, str);
	progressDlg.SetRange(1);
	progressDlg.SetProgressBar(0);

	progressDlg.SetMessage(m_dbAcfRec.sTitle);

	m_AcfConditionData.AcfCondition = setData.AcfCondition;
	m_AcfConditionData.IAcfCondition.fIACCWLevel = 0.1;
	m_AcfConditionData.nStartPos = 0;
	m_AcfConditionData.nDataNum = m_dbAcfRec.nSampleNum;

	double fScale = 1.0;
	int nFactor;
	pProgressDlg = &progressDlg;
	m_pAcfFactorData = CalcAcfFactor(m_hWaveData, &m_AcfConditionData, m_dbAcfRec.nSampling / fScale, &nFactor, AbortCheck, &m_pTaunPhinData);
	if (m_pAcfFactorData == NULL)
		return FALSE;

	m_dbAcfFactorRec.nAcfID = m_dbAcfRec.nAcfID;
	m_dbAcfFactorRec.nConditionSize = sizeof(AcfConditionData);
	m_dbAcfFactorRec.nFactorSize = sizeof(AcfFactor);
	m_dbAcfFactorRec.nAcfFactor = nFactor;

	return TRUE;
}

BOOL CAcfFactor::AbortCheck(int nPercent)
{
	if (pProgressDlg->CheckAbort())
		return TRUE;

	pProgressDlg->SetProgressBarSub(nPercent);

	return FALSE;
}

LRESULT CAcfFactor::OnRightButton(WPARAM wParam, LPARAM /*lParam*/)
{
	m_fCurrentMarkerTime = *(double *)wParam;

	if (m_fCurrentMarkerTime >= 0 && m_fCurrentMarkerTime < m_dbAcfFactorRec.nAcfFactor * m_AcfConditionData.AcfCondition.fRunningStep) {
		m_cPlayback.Stop();
		m_cPlayback.SetCheck(1);
		m_cPlayback.Play(m_hWaveData, WAVE_MAPPER, m_fCurrentMarkerTime);

		CWnd *pDlg = m_cGraph.GetCurrentDlg();
		if (pDlg != NULL)
			pDlg->SendMessage(WM_MARKER_CURSOR, TRUE, (LPARAM)&m_fCurrentMarkerTime);

		CMenu menu;
		menu.LoadMenu(IDR_ADD_MARKER);
		CMenu *pPopup = menu.GetSubMenu(0);
		CPoint point;
		GetCursorPos(&point);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);

		MSG msg;
		if (!PeekMessage(&msg, m_hWnd, WM_COMMAND, WM_COMMAND, PM_NOREMOVE)) {
			if (pDlg != NULL)
				pDlg->SendMessage(WM_MARKER_CURSOR, FALSE);
		}

		if (m_cPlayback.GetCheck()) {
			m_cPlayback.SetCheck(0);
			m_cPlayback.Stop();
		}
	}

	return 0;
}

void CAcfFactor::OnEnterIdle(UINT nWhy, CWnd* pWho)
{
	CDialogML::OnEnterIdle(nWhy, pWho);

	if ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) == 0 && m_cPlayback.GetCheck()) {
		m_cPlayback.SetCheck(0);
		m_cPlayback.Stop();
	}
}

void CAcfFactor::OnAddAcfMarker()
{
	CAcfMarkerDlg dlg(this);

	dlg.m_nAcfID = m_nAcfID;
	dlg.m_fTime = m_fCurrentMarkerTime;
	if (dlg.DoModal() == IDOK) {
		ReadAcfMarker();
	}

	CWnd *pDlg = m_cGraph.GetCurrentDlg();
	if (pDlg != NULL)
		pDlg->SendMessage(WM_MARKER_CURSOR, FALSE);
}

void CAcfFactor::ReadAcfMarker()
{
	CDbAcfMarker dbAcfMarker;

	if (dbAcfMarker.Open()) {
		if (m_pAcfMarkerRec != NULL) {
			delete [] m_pAcfMarkerRec;
			m_pAcfMarkerRec = NULL;
		}

		dbAcfMarker.ReadRecAcfID(m_nAcfID, &m_pAcfMarkerRec, m_nAcfMarkerRec);
		RedrawGraph();
	}
}

LRESULT CAcfFactor::OnSelectMarker(WPARAM wParam, LPARAM /*lParam*/)
{
	m_nSelectedMarkerID = (int)wParam;

	CMenu menu;
	menu.LoadMenu(IDR_EDIT_MARKER);
	CMenu* pPopup = menu.GetSubMenu(0);
	CPoint point;
	GetCursorPos(&point);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);

	return 0;
}

void CAcfFactor::OnEditAcfMarker()
{
	CAcfMarkerDlg dlg(this);

	dlg.m_nAcfID = m_nAcfID;
	dlg.m_nAcfMarkerID = m_nSelectedMarkerID;
	if (dlg.DoModal() == IDOK) {
		ReadAcfMarker();
	}
}

void CAcfFactor::OnDeleteAcfMarker()
{
	if (MessageBox(IDS_DELETE_MARKER, MB_OKCANCEL | MB_ICONQUESTION) == IDOK) {
		CDbAcfMarker dbAcfMarker;
		if (dbAcfMarker.Open()) {
			dbAcfMarker.DeleteRec(m_nSelectedMarkerID);
			ReadAcfMarker();
		}
	}
}

void CAcfFactor::OnPlaybackAcfMarker()
{
	for (int i = 0; i < m_nAcfMarkerRec; i++) {
		if (m_pAcfMarkerRec[i].nMarkerID == m_nSelectedMarkerID) {
			m_cPlayback.Stop();
			m_cPlayback.SetCheck(1);
			m_cPlayback.Play(m_hWaveData, WAVE_MAPPER, m_pAcfMarkerRec[i].fTime);
			break;
		}
	}
}
