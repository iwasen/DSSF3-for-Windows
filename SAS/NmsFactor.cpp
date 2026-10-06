// NmsFactor.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "NmsFactor.h"
#include "SasDoc.h"
#include "SasView.h"
#include "CalcAcf.h"
#include "NmsScDlg.h"
#include "OutputFile.h"
#include "Help\ContextHelp.h"

#define CLM_TIME	0
#define CLM_PHI0	1
#define CLM_TAUE	2
#define CLM_TAU1	3
#define CLM_PHI1	4
#define CLM_SPL		5
#define CLM_IACC	6
#define CLM_TIACC	7
#define CLM_WIACC	8

/////////////////////////////////////////////////////////////////////////////
// CNmsFactor ダイアログ

CObList CNmsFactor::m_WindowList;

CNmsFactor::CNmsFactor(CWnd* pParent /*=NULL*/)
	: CDialogML(CNmsFactor::IDD, pParent)
{
	m_iSampling = 0;
	m_iBit = 0;

	m_bChangeData = FALSE;
	m_pNmsFactorData = NULL;
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
	m_nCurrentDataNo = 0;
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
	m_fStartTimeSPL = 0;
	m_fDispTimeSPL = 0;
	m_fStartValueSPL = 0;
	m_fDispValueSPL = 0;
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
	m_nAcfMarkerRec = 0;

	m_WindowList.AddTail(this);
}

CNmsFactor::~CNmsFactor()
{
	DeleteGraphWindow();
	FreeBuffers();

	POSITION pos = m_WindowList.Find(this);
	m_WindowList.RemoveAt(pos);
}

void CNmsFactor::DoDataExchange(CDataExchange* pDX)
{
	CDialogML::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLAYBACK, m_cPlayback);
	DDX_Control(pDX, IDC_SCREEN_COPY, m_cScreenCopy);
	DDX_Control(pDX, IDC_CALC_CYCLE, m_cCalcCycle);
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
	DDX_Text(pDX, IDC_BIT, m_iBit);
	DDX_Control(pDX, IDC_CSV_OUTPUT, m_cCsvOutput);
}

BEGIN_MESSAGE_MAP(CNmsFactor, CDialogML)
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
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNmsFactor メッセージ ハンドラ

BOOL CNmsFactor::OnInitDialog()
{
	CDialogML::OnInitDialog();

	m_cFactorList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_cGraph.SetMinTabWidth(35);

	SetFolderInfo();

	if (ReadData())
		DispGraph();

	m_cScreenCopy.m_bNoProcess = TRUE;

	return TRUE;
}

BOOL CNmsFactor::SetFolderInfo()
{
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
	m_fScale = dbFolderRec.fScale;
	m_sScale.Format("1/%g", m_fScale);

	UpdateData(FALSE);

	return TRUE;
}

BOOL CNmsFactor::ReadData()
{
	FreeBuffers();

	CDbNms dbNms;
	if (!dbNms.Open())
		return FALSE;

	if (!dbNms.ReadRecID(m_NmsID, &m_dbNmsRec, &m_NmsConditionData, &m_pNmsFactorData, NULL))
		return FALSE;

	MMWaveReadDB(&m_hWaveData, dbNms.GetFCB(), "WAVEDATA");

	m_iSampling = m_dbNmsRec.nSampling;
	m_sMeasureTime.Format("%.2f", m_dbNmsRec.fDataTime);
	m_sChannel = ((m_dbNmsRec.nChannel & 0x03) == 1) ? "Mono" : "Stereo";
	m_iBit = (m_dbNmsRec.nChannel & 0x04) ? 24 : 16;
//	m_sTitle = m_dbNmsRec.title;
//	m_sComment = m_dbNmsRec.comment;
	m_sTime = FormatTime(m_dbNmsRec.sTime);
	m_cPlayback.EnableWindow(m_hWaveData != NULL);

	m_fRate = m_dbNmsRec.nSampling / m_fScale;

	m_bCalculated = TRUE;

	InitNmsFactorList();

	DispNmsFactorList();
	DispCalcParam();

	EnablePrevNext();

	m_cCsvOutput.EnableWindow(m_dbNmsRec.nNmsFactor);

	CString str;
	str.Format("No.%ld", m_dbNmsRec.nNumber + 1);
	SetWindowText(str);

	m_bChangeData = FALSE;

	return TRUE;
}

void CNmsFactor::DispGraph()
{
	int i;

	if (!m_bCalculated)
		return;

	if (m_pAcfFactor != NULL)
		delete [] m_pAcfFactor;

	m_pAcfFactor = new AcfFactor[m_dbNmsRec.nNmsFactor];
	for (i = 0; i < m_dbNmsRec.nNmsFactor; i++)
		m_pAcfFactor[i] = m_pNmsFactorData[i].Acf;

	if (m_pIAcfFactor != NULL)
		delete [] m_pIAcfFactor;

	m_pIAcfFactor = new IAcfFactor[m_dbNmsRec.nNmsFactor];
	for (i = 0; i < m_dbNmsRec.nNmsFactor; i++)
		m_pIAcfFactor[i] = m_pNmsFactorData[i].IAcf;

	m_cGraph.DeleteAllPages();
	SaveStartDisp();
	DeleteGraphWindow();

	if (m_dbNmsRec.nNmsFactor != 0) {
		m_pGraphPhi0 = new CGraphFactor;
		m_pGraphPhi0->m_nFactor = FACTOR_PHI0;
		m_pGraphPhi0->m_pAcfFactor = m_pAcfFactor;
		m_pGraphPhi0->m_nData = m_dbNmsRec.nNmsFactor;
		m_pGraphPhi0->m_fStepTime = m_NmsConditionData.AcfCondition.fRunningStep;
		m_pGraphPhi0->m_fAdjustPhi0 = 0;
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
		m_pGraphTauE->m_nData = m_dbNmsRec.nNmsFactor;
		m_pGraphTauE->m_fStepTime = m_NmsConditionData.AcfCondition.fRunningStep;
		m_pGraphTauE->m_fAdjustPhi0 = 0;
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
		m_pGraphTau1->m_nData = m_dbNmsRec.nNmsFactor;
		m_pGraphTau1->m_fStepTime = m_NmsConditionData.AcfCondition.fRunningStep;
		m_pGraphTau1->m_fAdjustPhi0 = 0;
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
		m_pGraphPhi1->m_nData = m_dbNmsRec.nNmsFactor;
		m_pGraphPhi1->m_fStepTime = m_NmsConditionData.AcfCondition.fRunningStep;
		m_pGraphPhi1->m_fAdjustPhi0 = 0;
		m_pGraphPhi1->m_fStartTime = m_fStartTimePhi1;
		m_pGraphPhi1->m_fDispTime = m_fDispTimePhi1;
		m_pGraphPhi1->m_fStartValue = m_fStartValuePhi1;
		m_pGraphPhi1->m_fDispValue = m_fDispValuePhi1;
		m_pGraphPhi1->m_pAcfMarkerRec = &m_pAcfMarkerRec;
		m_pGraphPhi1->m_nAcfMarkerRec = &m_nAcfMarkerRec;
		m_cGraph.AddPage(m_pGraphPhi1, IDD_GRAPH_FACTOR, IDS_PHI1);

		m_pGraphSPL = new CGraphFactor;
		m_pGraphSPL->m_nFactor = FACTOR_SPL;
		m_pGraphSPL->m_pIAcfFactor = m_pIAcfFactor;
		m_pGraphSPL->m_nData = m_dbNmsRec.nNmsFactor;
		m_pGraphSPL->m_fStepTime = m_NmsConditionData.AcfCondition.fRunningStep;
		m_pGraphSPL->m_fAdjustPhi0 = 0;
		m_pGraphSPL->m_fStartTime = m_fStartTimeSPL;
		m_pGraphSPL->m_fDispTime = m_fDispTimeSPL;
		m_pGraphSPL->m_fStartValue = m_fStartValueSPL;
		m_pGraphSPL->m_fDispValue = m_fDispValueSPL;
		m_pGraphSPL->m_pAcfMarkerRec = &m_pAcfMarkerRec;
		m_pGraphSPL->m_nAcfMarkerRec = &m_nAcfMarkerRec;
		m_cGraph.AddPage(m_pGraphSPL, IDD_GRAPH_FACTOR, "SPL");

		m_pGraphIACC = new CGraphFactor;
		m_pGraphIACC->m_nFactor = FACTOR_IACC;
		m_pGraphIACC->m_pIAcfFactor = m_pIAcfFactor;
		m_pGraphIACC->m_nData = m_dbNmsRec.nNmsFactor;
		m_pGraphIACC->m_fStepTime = m_NmsConditionData.AcfCondition.fRunningStep;
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
		m_pGraphTIACC->m_nData = m_dbNmsRec.nNmsFactor;
		m_pGraphTIACC->m_fStepTime = m_NmsConditionData.AcfCondition.fRunningStep;
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
		m_pGraphWIACC->m_nData = m_dbNmsRec.nNmsFactor;
		m_pGraphWIACC->m_fStepTime = m_NmsConditionData.AcfCondition.fRunningStep;
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
		m_pGraphACF->m_pAcfCondition = &m_NmsConditionData.AcfCondition;
		m_pGraphACF->m_pIAcfCondition = &m_NmsConditionData.IAcfCondition;
		m_pGraphACF->m_pNmsMicCal = &m_NmsConditionData.NmsMicCal;
		m_pGraphACF->m_fRate = m_fRate;
		m_pGraphACF->m_nStep = m_nCurrentDataNo;
		m_pGraphACF->m_pAcfFactor = m_pAcfFactor;
		m_pGraphACF->m_fStartTime = m_fStartTimeACF;
		m_pGraphACF->m_fDispTime = m_fDispTimeACF;
		m_cGraph.AddPage(m_pGraphACF, IDD_GRAPH_ACF, "ACF");

		m_pGraphIACF = new CGraphIACF;
		m_pGraphIACF->m_hWaveData = m_hWaveData;
		m_pGraphIACF->m_pAcfCondition = &m_NmsConditionData.AcfCondition;
		m_pGraphIACF->m_pIAcfCondition = &m_NmsConditionData.IAcfCondition;
		m_pGraphIACF->m_pNmsMicCal = &m_NmsConditionData.NmsMicCal;
		m_pGraphIACF->m_fRate = m_fRate;
		m_pGraphIACF->m_nStep = m_nCurrentDataNo;
		m_pGraphIACF->m_pIAcfFactor = m_pIAcfFactor;
		m_cGraph.AddPage(m_pGraphIACF, IDD_GRAPH_IACF, "IACF");

		m_pGraphWave = new CGraphWave;
		m_pGraphWave->m_hWaveData = m_hWaveData;
		m_pGraphWave->m_fStartTime = m_fStartTimeWave;
		m_pGraphWave->m_fDispTime = m_fDispTimeWave;
		m_pGraphWave->m_fStartValue = m_fStartValueWave;
		m_pGraphWave->m_fDispValue = m_fDispValueWave;
		m_pGraphWave->m_pAcfCondition = &m_NmsConditionData.AcfCondition;
		m_cGraph.AddPage(m_pGraphWave, IDD_GRAPH_WAVE, IDS_WAVE);

		m_pGraphSpec = new CGraphSpec;
		m_pGraphSpec->m_hWaveData = m_hWaveData;
		m_pGraphSpec->m_fRate = m_fRate;
		m_pGraphSpec->m_nStep = m_nCurrentDataNo;
		m_pGraphSpec->m_pAcfCondition = &m_NmsConditionData.AcfCondition;
		m_cGraph.AddPage(m_pGraphSpec, IDD_GRAPH_SPEC, IDS_SPECTRUM);
	}

	if (m_nCurrentPage >= m_cGraph.GetItemCount())
		m_nCurrentPage = 0;
	m_cGraph.SetPage(m_nCurrentPage);
}

void CNmsFactor::InitNmsFactorList()
{
	struct LISTCOLUMN {
		UINT nID;
		LPCSTR text;
		int width;
	};
	static constexpr LISTCOLUMN tColumnHeader[] = {
		{0, "Time", 53},
		{IDS_PHI0, NULL, 53},
		{IDS_TAUE, NULL, 53},
		{IDS_TAU1, NULL, 53},
		{IDS_PHI1, NULL, 53},
		{0, "SPL", 53},
		{0, "IACC", 53},
		{IDS_TIACC, NULL, 53},
		{0, "W_IACC", 53}
	};
	const LISTCOLUMN *pColumnHeader;
	static const char *tUnit[] = {" [sec]", " [dB]", " [ms]", " [ms]", "", "", "", ""};
	CString str;

	m_cFactorList.DeleteAllItems();

	while (m_cFactorList.DeleteColumn(0))
		;

	if (!m_bCalculated)
		return;

	pColumnHeader = tColumnHeader;
	m_nColumn = ARRAY_SIZE(tColumnHeader);

	for (int i = 0; i < m_nColumn; i++, pColumnHeader++) {
		if (pColumnHeader->nID == 0)
			str = pColumnHeader->text;
		else
			str.LoadString(pColumnHeader->nID);
//		str += tUnit[i];

		m_cFactorList.InsertColumn(i, str, LVCFMT_RIGHT, pColumnHeader->width);
	}
}

void CNmsFactor::DispNmsFactorList()
{
	m_cFactorList.DeleteAllItems();

	if (!m_bCalculated)
		return;

	CString str;
	double time = 0;
	for (int i = 0; i < m_dbNmsRec.nNmsFactor; i++) {
		str.Format("%g", time);
		m_cFactorList.InsertItem(i, str);

		str.Format("%.2lf", m_pNmsFactorData[i].Acf.fPhi0);
		m_cFactorList.SetItemText(i, CLM_PHI0, str);

		str.Format("%.2lf", m_pNmsFactorData[i].Acf.fTauE * 1000);
		m_cFactorList.SetItemText(i, CLM_TAUE, str);

		str.Format("%.2lf", m_pNmsFactorData[i].Acf.fTau1 * 1000);
		m_cFactorList.SetItemText(i, CLM_TAU1, str);

		str.Format("%.2lf", m_pNmsFactorData[i].Acf.fPhi1);
		m_cFactorList.SetItemText(i, CLM_PHI1, str);

		str.Format("%.2lf", m_pNmsFactorData[i].IAcf.fSPL);
		m_cFactorList.SetItemText(i, CLM_SPL, str);

		str.Format("%.2lf", m_pNmsFactorData[i].IAcf.fIACC);
		m_cFactorList.SetItemText(i, CLM_IACC, str);

		str.Format("%.2lf", m_pNmsFactorData[i].IAcf.fTauIACC * 1000);
		m_cFactorList.SetItemText(i, CLM_TIACC, str);

		str.Format("%.2lf", m_pNmsFactorData[i].IAcf.fWIACC * 1000);
		m_cFactorList.SetItemText(i, CLM_WIACC, str);

		m_cFactorList.SetItemData(i, i);

		time += m_NmsConditionData.AcfCondition.fRunningStep;
	}

	m_cFactorList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
}

void CNmsFactor::DispCalcParam()
{
	static constexpr int tFilterName[] = {IDS_FLAT, IDS_AWEIGHTING, IDS_BWEIGHTING, IDS_CWEIGHTING};

	if (m_bCalculated) {
		AcfCondition *pAcfCondition = &m_NmsConditionData.AcfCondition;
		m_cTaueInterval = pAcfCondition->fTaueInterval;
		m_cTaueEndTime = pAcfCondition->fTaueEndTime;
		m_cTaueEndLevel = pAcfCondition->fTaueEndLevel;
		m_cPhi0RefData.LoadString(IDS_ABSVALUE);
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

void CNmsFactor::DeleteGraphWindow()
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

void CNmsFactor::FreeBuffers()
{
	if (m_pNmsFactorData != NULL) {
		delete [] m_pNmsFactorData;
		m_pNmsFactorData = NULL;
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
}

void CNmsFactor::OnSelectData()
{
	m_cPlayback.Stop();
	AfxGetApp()->m_pMainWnd->SetActiveWindow();
}

void CNmsFactor::OnPrevData()
{
	long nmsID;

	if ((nmsID = m_pView->GetPrevItem(m_NmsID)) != -1) {
		m_cPlayback.Stop();
		m_NmsID = nmsID;
		ReadData();
		DispGraph();
	}
}

void CNmsFactor::OnNextData()
{
	long nmsID;

	if ((nmsID = m_pView->GetNextItem(m_NmsID)) != -1) {
		m_cPlayback.Stop();
		m_NmsID = nmsID;
		ReadData();
		DispGraph();
	}
}

void CNmsFactor::EnablePrevNext()
{
	m_cPrevData.EnableWindow(m_pView->GetPrevItem(m_NmsID) != -1);
	m_cNextData.EnableWindow(m_pView->GetNextItem(m_NmsID) != -1);
}

void CNmsFactor::OnColumnclickFactorList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (m_nSortItem != pNMListView->iSubItem)
		m_nSortDir = 1;
	else
		m_nSortDir *= -1;

	m_nSortItem = pNMListView->iSubItem;
	m_cFactorList.SortItems(CompareItems, (LPARAM)this);

	*pResult = 0;
}

int CALLBACK CNmsFactor::CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CNmsFactor *pWnd = (CNmsFactor *)lParamSort;
	NmsFactorData *pNmsFactorData = pWnd->m_pNmsFactorData;
	double d;

	switch (pWnd->m_nSortItem) {
	case CLM_PHI0:
		d = pNmsFactorData[lParam1].Acf.fPhi0 -
			pNmsFactorData[lParam2].Acf.fPhi0;
		break;
	case CLM_TAUE:
		d = pNmsFactorData[lParam1].Acf.fTauE -
			pNmsFactorData[lParam2].Acf.fTauE;
		break;
	case CLM_TAU1:
		d = pNmsFactorData[lParam1].Acf.fTau1 -
			pNmsFactorData[lParam2].Acf.fTau1;
		break;
	case CLM_PHI1:
		d = pNmsFactorData[lParam1].Acf.fPhi1 -
			pNmsFactorData[lParam2].Acf.fPhi1;
		break;
	case CLM_SPL:
		d = pNmsFactorData[lParam1].IAcf.fSPL -
			pNmsFactorData[lParam2].IAcf.fSPL;
		break;
	case CLM_IACC:
		d = pNmsFactorData[lParam1].IAcf.fIACC -
			pNmsFactorData[lParam2].IAcf.fIACC;
		break;
	case CLM_TIACC:
		d = pNmsFactorData[lParam1].IAcf.fTauIACC -
			pNmsFactorData[lParam2].IAcf.fTauIACC;
		break;
	case CLM_WIACC:
		d = pNmsFactorData[lParam1].IAcf.fWIACC -
			pNmsFactorData[lParam2].IAcf.fWIACC;
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

void CNmsFactor::OnCancel()
{
	DestroyWindow();
}

void CNmsFactor::CloseAll()
{
	CWnd *pWnd;

	while (!m_WindowList.IsEmpty()) {
		pWnd = (CWnd *)m_WindowList.GetHead();
		if (pWnd != NULL)
			pWnd->DestroyWindow();
	}
}

void CNmsFactor::OnSelchangeGraph(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	m_nCurrentPage = m_cGraph.GetCurSel();

	*pResult = 0;
}

void CNmsFactor::OnItemchangedFactorList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	m_cFactorList.UpdateWindow();
	if (pNMListView->uNewState & LVIS_SELECTED && m_nCurrentDataNo != pNMListView->lParam) {
		m_nCurrentDataNo = (int)pNMListView->lParam;

		m_cFactorList.UpdateWindow();

		if (m_pGraphPhi0 != NULL)
			m_pGraphPhi0->ReDraw(m_nCurrentDataNo);

		if (m_pGraphTauE != NULL)
			m_pGraphTauE->ReDraw(m_nCurrentDataNo);

		if (m_pGraphTau1 != NULL)
			m_pGraphTau1->ReDraw(m_nCurrentDataNo);

		if (m_pGraphPhi1 != NULL)
			m_pGraphPhi1->ReDraw(m_nCurrentDataNo);

		if (m_pGraphSPL != NULL)
			m_pGraphSPL->ReDraw(m_nCurrentDataNo);

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

	*pResult = 0;
}

void CNmsFactor::OnPlayback()
{
	m_cPlayback.Play(m_hWaveData);
}

void CNmsFactor::OnScreenCopy()
{
	CNmsScDlg nmsScDlg(this);
	CString title;

	if (m_hWaveData == NULL)
		nmsScDlg.m_bNoWave = TRUE;

	switch (nmsScDlg.DoModal()) {
	case IDOK:
		{
			CString filter;
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
		GetWindowText(title);
		HDIB hDIB = ScreenCopy();
		SendMMLib(this, m_sFolderTitle, m_sFolderComment, title, m_sComment, GetDataText(), hDIB);
		::GlobalFree(hDIB);
		break;
	}
}

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
HDIB CNmsFactor::ScreenCopy()
{
	if (setData.NmsSC.nCopyMode == 0) {
		UpdateWindow();
		return m_cScreenCopy.CreateDIB(this);
	} else if (setData.NmsSC.bMulti)
		return GraphCopy();
	else {
		CWnd *pWnd;

		m_cGraph.UpdateWindow();
		m_cGraph.GetCurrentDlg()->UpdateWindow();

		if (setData.ImpSC.bFrame)
			pWnd = &m_cGraph;
		else
			pWnd = m_cGraph.GetCurrentDlg();

		return m_cScreenCopy.CreateDIB(pWnd);
	}
}

HDIB CNmsFactor::GraphCopy()
{
	int i, nGraph = 0;
	CWnd *pWnd;
	int x, y;

	for (i = 0; i < 10; i++) {
		if (m_hWaveData == NULL) {
			switch (i) {
			case 8:
			case 9:
				continue;
			}
		}

		if (setData.NmsSC.bGraph[i])
			nGraph++;
	}

	if (nGraph == 0)
		return NULL;

	int nCol = min(setData.NmsSC.nColumns, nGraph);
	int nRow = (nGraph + setData.NmsSC.nColumns - 1) / setData.NmsSC.nColumns;

	CRect rect;
	if (setData.NmsSC.bFrame)
		m_cGraph.GetWindowRect(rect);
	else
		m_pGraphPhi0->GetWindowRect(rect);

	const int nWidth = (setData.NmsSC.nColSpace + rect.Width()) * nCol + setData.NmsSC.nColSpace;
	const int nHeight = (setData.NmsSC.nRowSpace + rect.Height()) * nRow + setData.NmsSC.nRowSpace;

	CDC *pDC = GetDC();
	CDC DCMem;
	DCMem.CreateCompatibleDC(pDC);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, nWidth, nHeight);

	CGdiObject *pOrg = DCMem.SelectObject(&bmp);
	DCMem.FillSolidRect(0, 0, nWidth, nHeight, GetSysColor(COLOR_3DFACE));

	const int nCurrentPage = m_nCurrentPage;

	nCol = nRow = 0;
	for (i = 0; i < 10; i++) {
		if (m_hWaveData == NULL) {
			switch (i) {
			case 8:
			case 9:
				continue;
			}
		}

		if (setData.NmsSC.bGraph[i]) {
			m_cGraph.SetPage(i);
			m_cGraph.UpdateWindow();
			m_cGraph.GetCurrentDlg()->UpdateWindow();
			if (setData.NmsSC.bFrame)
				pWnd = &m_cGraph;
			else
				pWnd = m_cGraph.GetCurrentDlg();

			x = setData.NmsSC.nColSpace + nCol * (rect.Width() + setData.NmsSC.nColSpace);
			y = setData.NmsSC.nRowSpace + nRow * (rect.Height() + setData.NmsSC.nRowSpace);
			DCMem.BitBlt(x, y, rect.Width(), rect.Height(), pWnd->GetWindowDC(), 0, 0, SRCCOPY);

			if (++nCol == setData.NmsSC.nColumns) {
				nCol = 0;
				nRow++;
			}
		}
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

CString CNmsFactor::GetDataText()
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

void CNmsFactor::SaveStartDisp()
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

	if (m_pGraphSPL != NULL) {
		m_fStartTimeSPL = m_pGraphSPL->m_fStartTime;
		m_fDispTimeSPL = m_pGraphSPL->m_fDispTime;
		m_fStartValueSPL = m_pGraphSPL->m_fStartValue;
		m_fDispValueSPL = m_pGraphSPL->m_fDispValue;
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

LRESULT CNmsFactor::OnSelectData(WPARAM wParam, LPARAM /*lParam*/)
{
	const int nIndex = (int)(*(double *)wParam / m_NmsConditionData.AcfCondition.fRunningStep + 0.5);

	if (nIndex >= 0 && nIndex < m_cFactorList.GetItemCount()) {
		m_cFactorList.SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);
		m_cFactorList.EnsureVisible(nIndex, FALSE);
	}

	return 0;
}

void CNmsFactor::OnBnClickedCsvOutput()
{
	CFileDialog fileDlg(FALSE, "csv", NULL,
			OFN_OVERWRITEPROMPT, "CSV File (*.csv)|*.csv|All Files (*.*)|*.*||", this, 0);

	if (fileDlg.DoModal() == IDOK)
		OutputListToCsv(m_cFactorList, fileDlg.GetPathName());
}

BOOL CNmsFactor::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDCANCEL, IDH_NMS_FACTOR_CANCEL,
		IDC_SELECT_DATA, IDH_NMS_FACTOR_SELECT_DATA,
		IDC_PREV_DATA, IDH_NMS_FACTOR_PREV_DATA,
		IDC_NEXT_DATA, IDH_NMS_FACTOR_NEXT_DATA,
		IDC_SCREEN_COPY, IDH_NMS_FACTOR_SCREEN_COPY,
		IDC_CSV_OUTPUT, IDH_NMS_FACTOR_CSV_OUTPUT,
		IDC_GRAPH, IDH_NMS_FACTOR_GRAPH,
		IDC_FOLDER_TITLE, IDH_NMS_FACTOR_FOLDER_TITLE,
		IDC_NAME, IDH_NMS_FACTOR_NAME,
		IDC_PLACE, IDH_NMS_FACTOR_PLACE,
		IDC_FOLDER_COMMENT, IDH_NMS_FACTOR_FOLDER_COMMENT,
		IDC_DATE, IDH_NMS_FACTOR_DATE,
		IDC_SCALE, IDH_NMS_FACTOR_SCALE,
		IDC_SAMPLING, IDH_NMS_FACTOR_SAMPLING,
		IDC_MEASURE_TIME, IDH_NMS_FACTOR_MEASURE_TIME,
		IDC_CHANNEL, IDH_NMS_FACTOR_CHANNEL,
		IDC_BIT, IDH_NMS_FACTOR_BIT,
		IDC_TITLE, IDH_NMS_FACTOR_TITLE,
		IDC_COMMENT, IDH_NMS_FACTOR_COMMENT,
		IDC_TIME, IDH_NMS_FACTOR_TIME,
		IDC_PLAYBACK, IDH_NMS_FACTOR_PLAYBACK,
		IDC_INTEGRATION_TIME, IDH_NMS_FACTOR_INTEGRATION_TIME,
		IDC_CALC_CYCLE, IDH_NMS_FACTOR_CALC_CYCLE,
		IDC_FREQ_FILTER, IDH_NMS_FACTOR_FREQ_FILTER,
		IDC_TAUE_INTERVAL, IDH_NMS_FACTOR_TAUE_INTERVAL,
		IDC_TAUE_ENDLEVEL, IDH_NMS_FACTOR_TAUE_ENDLEVEL,
		IDC_TAUE_ENDTIME, IDH_NMS_FACTOR_TAUE_ENDTIME,
		IDC_PHI0_REFDATA, IDH_NMS_FACTOR_PHI0_REFDATA,
		IDC_PHI0_REFLEVEL, IDH_NMS_FACTOR_PHI0_REFLEVEL,
		IDC_FACTOR_LIST, IDH_NMS_FACTOR_FACTOR_LIST,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
