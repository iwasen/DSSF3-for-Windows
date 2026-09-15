// Parameter.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "ImpFactor.h"
#include "SasDoc.h"
#include "SasView.h"
#include "Calc.h"
#include "ImpScDlg.h"
#include "OutputFile.h"
#include "CalcImpDlg.h"
#include "Help\ContextHelp.h"

#define CLM_FREQ	0
#define CLM_SPL_LR	1
#define CLM_SPL_L	2
#define CLM_SPL_R	3
#define CLM_DT1_LR	4
#define CLM_DT1_L	5
#define CLM_DT1_R	6
#define CLM_A_LR	7
#define CLM_A_L		8
#define CLM_A_R		9
#define CLM_TSUB_LR	10
#define CLM_TSUB_L	11
#define CLM_TSUB_R	12
#define CLM_IACC	13
#define CLM_TIACC	14
#define CLM_WIACC	15
#define CLM_PREF	16

#define CLM_SPL		1
#define CLM_DT1		2
#define CLM_A		3
#define CLM_TSUB	4

#define CLM_G_LR	1
#define CLM_G_L		2
#define CLM_G_R		3
#define CLM_T20_LR	4
#define CLM_T20_L	5
#define CLM_T20_R	6
#define CLM_T30_LR	7
#define CLM_T30_L	8
#define CLM_T30_R	9
#define CLM_TCUSTOM_LR	10
#define CLM_TCUSTOM_L	11
#define CLM_TCUSTOM_R	12
#define CLM_EDT_LR	13
#define CLM_EDT_L	14
#define CLM_EDT_R	15
#define CLM_C50_LR	16
#define CLM_C50_L	17
#define CLM_C50_R	18
#define CLM_C80_LR	19
#define CLM_C80_L	20
#define CLM_C80_R	21
#define CLM_CCUSTOM_LR	22
#define CLM_CCUSTOM_L	23
#define CLM_CCUSTOM_R	24
#define CLM_D50_LR	25
#define CLM_D50_L	26
#define CLM_D50_R	27
#define CLM_TS_LR	28
#define CLM_TS_L	29
#define CLM_TS_R	30
#define CLM_IACCE	31
#define CLM_IACCL	32

#define CLM_G		1
#define CLM_T20		2
#define CLM_T30		3
#define CLM_TCUSTOM	4
#define CLM_EDT		5
#define CLM_C50		6
#define CLM_C80		7
#define CLM_CCUSTOM	8
#define CLM_D50		9
#define CLM_TS		10

/////////////////////////////////////////////////////////////////////////////
// CImpFactor ダイアログ

CObList CImpFactor::m_WindowList;

CImpFactor::CImpFactor(CWnd* pParent /*=NULL*/)
	: CDialogML(CImpFactor::IDD, pParent)
{
	m_iBit = 0;
	m_iMeasureNum = 0;
	m_iSampling = 0;

	m_pGraphIR = NULL;
	m_pGraphEcho = NULL;
	m_pGraphIACC = NULL;
	m_pGraphFreq = NULL;
	m_pGraphParam = NULL;
	m_pGraphPref = NULL;
	m_pGraphMTF = NULL;
	m_pWaveLeft = NULL;
	m_pWaveRight = NULL;
	m_pIRLeft = NULL;
	m_pIRRight = NULL;
	m_nCurrentDataNo = 0;
	m_hWaveData = NULL;
	m_nCurrentPage = 0;
	m_bChangeData = FALSE;
	m_pMethod = NULL;
	m_nParamItem = 0;
	m_fStartTimeIR = 0;
	m_fDispTimeIR = 0;
	m_fStartTimeEcho = 0;
	m_fDispTimeEcho = 0;
	m_nSortItem = 0;
	m_nSortDir = 1;
	memset(m_nScrollX, 0, sizeof(m_nScrollX));
	m_nScrollY = 0;

	m_WindowList.AddTail(this);
}

CImpFactor::~CImpFactor()
{
	DeleteGraphWindow();
	FreeBuffers();

	POSITION pos = m_WindowList.Find(this);
	m_WindowList.RemoveAt(pos);
}

void CImpFactor::DoDataExchange(CDataExchange* pDX)
{
	CDialogML::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLAYBACK, m_cPlayback);
	DDX_Control(pDX, IDC_SCREEN_COPY, m_cScreenCopy);
	DDX_Control(pDX, IDC_SELECT_DATA, m_cSelectData);
	DDX_Control(pDX, IDC_NEXT_DATA, m_cNextData);
	DDX_Control(pDX, IDC_PREV_DATA, m_cPrevData);
	DDX_Control(pDX, IDC_PARAM_LIST, m_cParameterList);
	DDX_Control(pDX, IDC_GRAPH, m_cGraph);
	DDX_Text(pDX, IDC_BIT, m_iBit);
	DDX_Text(pDX, IDC_COMMENT, m_sComment);
	DDX_Text(pDX, IDC_MEASURE_NUM, m_iMeasureNum);
	DDX_Text(pDX, IDC_MEASURE_TIME, m_sMeasureTime);
	DDX_Text(pDX, IDC_SAMPLING, m_iSampling);
	DDX_Text(pDX, IDC_TIME, m_sTime);
	DDX_Text(pDX, IDC_TITLE, m_sTitle);
	DDX_Text(pDX, IDC_CHANNEL, m_sChannel);
	DDX_Text(pDX, IDC_FOLDER_TITLE, m_sFolderTitle);
	DDX_Text(pDX, IDC_FOLDER_COMMENT, m_sFolderComment);
	DDX_Text(pDX, IDC_DATE, m_sDate);
	DDX_Text(pDX, IDC_NAME, m_sName);
	DDX_Text(pDX, IDC_PLACE, m_sPlace);
	DDX_Text(pDX, IDC_SCALE, m_sScale);
	DDX_Control(pDX, IDC_CALC_CONDITION, m_cCalcCondition);
	DDX_Control(pDX, IDC_CSV_OUTPUT, m_cCsvOutput);
}

BEGIN_MESSAGE_MAP(CImpFactor, CDialogML)
	ON_BN_CLICKED(IDC_PREV_DATA, OnPrevData)
	ON_BN_CLICKED(IDC_NEXT_DATA, OnNextData)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_PARAM_LIST, OnColumnclickParamList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PARAM_LIST, OnItemchangedParamList)
	ON_NOTIFY(TCN_SELCHANGE, IDC_GRAPH, OnSelchangeGraph)
	ON_BN_CLICKED(IDC_SELECT_DATA, OnSelectData)
	ON_BN_CLICKED(IDC_PLAYBACK, OnPlayback)
	ON_NOTIFY(TCN_SELCHANGE, IDC_CALC_CONDITION, OnTcnSelchangeCalcCondition)
	ON_BN_CLICKED(IDC_SCREEN_COPY, OnScreenCopy)
	ON_BN_CLICKED(IDC_CSV_OUTPUT, OnBnClickedCsvOutput)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImpFactor メッセージ ハンドラ

BOOL CImpFactor::OnInitDialog()
{
	CDialogML::OnInitDialog();

	m_cParameterList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	SetFolderInfo();

	if (ReadData())
		DispGraph();
	else if (m_nImpulseID == -2) {
		ShowWindow(SW_MINIMIZE);
		PostMessage(WM_CLOSE);
	}

	m_cScreenCopy.m_bNoProcess = TRUE;

	return TRUE;
}

BOOL CImpFactor::SetFolderInfo()
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

	m_cCalcCondition.AddPage(&m_Param1, IDD_PARAM1, IDS_PARAM1, TRUE);
	m_cCalcCondition.AddPage(&m_Param2, IDD_PARAM2, IDS_PARAM2, TRUE);

	m_nParam = 0;
	m_nParamPrev = 0;
	m_cCalcCondition.SetPage(m_nParam);

	UpdateData(FALSE);

	return TRUE;
}

BOOL CImpFactor::ReadData()
{
	m_cPlayback.Stop();

	FreeBuffers();

	CDbImpulse dbImpulse;
	if (!dbImpulse.Open())
		return FALSE;

	CDbAcParam dbAcParam;
	if (!dbAcParam.Open())
		return FALSE;

	if (!dbImpulse.ReadRecID(m_nImpulseID, &m_dbImpulseRec))
		return FALSE;

	if (MMWaveReadDB(&m_hWaveData, dbImpulse.GetFCB(), "WAVEDATA") != 0) {
		MessageBox(IDS_ERR_READDATA, MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	m_nData = 1 << m_dbImpulseRec.nMeasTime;

	m_pWaveLeft = new double[m_nData];
	m_pIRLeft = new double[m_nData];
	if (m_dbImpulseRec.nChannel == 2) {
		m_pWaveRight = new double[m_nData];
		m_pIRRight = new double[m_nData];
	}

	m_nWaveData = ReadWaveData(m_hWaveData, m_pWaveLeft, m_pWaveRight, m_nData);

	m_iSampling = m_dbImpulseRec.nSampling;
	m_sMeasureTime.Format("%.3f", (double)m_nWaveData / m_dbImpulseRec.nSampling);
	m_iMeasureNum = m_dbImpulseRec.nMeasNum;
	m_sChannel = (m_dbImpulseRec.nChannel == 1) ? "Mono" : "Stereo";
	m_iBit = m_dbImpulseRec.nBit & 0xfc;
	m_sTitle = m_dbImpulseRec.sTitle;
	m_sComment = m_dbImpulseRec.sComment;
	m_sTime = FormatTime(m_dbImpulseRec.sTime);

	m_fRate = m_dbImpulseRec.nSampling / m_fScale;
	m_nMethod = m_dbImpulseRec.nBit & 0x03;

	if (m_nImpulseID != -2) {
		if (dbAcParam.ReadRecID(m_nImpulseID, &m_dbAcParamRec)) {
			m_fAdjustSPL = GetAdjustSPL(m_nFolderID, m_dbAcParamRec.dbAcParamCond.nSplRefData, m_dbAcParamRec.dbAcParamCond.fSplRefLevel);
			m_bCalculated = TRUE;
		} else {
			memset(&m_dbAcParamRec, 0, sizeof(m_dbAcParamRec));
			m_fAdjustSPL = 0;
			m_bCalculated = FALSE;
		}
	} else {
		if (!CalcMeasurementIR())
			return FALSE;
		m_fAdjustSPL = 1.0;
		m_bCalculated = TRUE;
	}

	InitDispAcParamList();

	DispCalcParam();

	EnablePrevNext();

	m_cScreenCopy.EnableWindow(m_bCalculated);
	m_cCsvOutput.EnableWindow(m_bCalculated);

	if (m_nMethod == METHOD_MSEQ)
		m_pMethod = new CMSeq;
	else if (m_nMethod == METHOD_TSP)
		m_pMethod = new CTSP;

	m_pMethod->InitMethod(m_dbImpulseRec.bImpulseData ? 0 : m_dbImpulseRec.nMeasTime);

	CalcIR(m_dbAcParamRec.dbAcParamData[m_nCurrentDataNo].nFreq, m_dbAcParamRec.dbAcParamCond.nFreqBand);

	SetWindowText(m_nImpulseID != -2 ? m_dbImpulseRec.sTitle : SA_IR_TITLE);

	m_bChangeData = FALSE;

	UpdateData(FALSE);

	return TRUE;
}

void CImpFactor::DispGraph()
{
	DbAcParamRec *pDbAcParamRec;

	if (m_bCalculated)
		pDbAcParamRec = &m_dbAcParamRec;
	else
		pDbAcParamRec = NULL;

	m_cGraph.DeleteAllPages();
	SaveStartDisp();
	DeleteGraphWindow();

	m_pGraphIR = new CGraphIR;
	m_pGraphIR->m_pParent = this;
	m_pGraphIR->m_hWaveData = m_hWaveData;
	m_pGraphIR->m_pDbImpulseRec = &m_dbImpulseRec;
	m_pGraphIR->m_pDbAcParamRec = pDbAcParamRec;
	m_pGraphIR->m_pIRLeft = m_pIRLeft;
	m_pGraphIR->m_pIRRight = m_pIRRight;
	m_pGraphIR->m_fRate = m_fRate;
	m_pGraphIR->m_nData = m_nWaveData;
	m_pGraphIR->m_fStartTime = m_fStartTimeIR;
	m_pGraphIR->m_fDispTime = m_fDispTimeIR;
	m_cGraph.AddPage(m_pGraphIR, IDD_GRAPH_IR, IDS_TAB_IR);

	m_pGraphEcho = new CGraphEcho;
	m_cGraph.AddPage(m_pGraphEcho, IDD_GRAPH_ECHO, IDS_TAB_REVERBERATION);
	m_pGraphEcho->m_pParent = this;
	m_pGraphEcho->m_hWaveData = m_hWaveData;
	m_pGraphEcho->m_pDbImpulseRec = &m_dbImpulseRec;
	m_pGraphEcho->m_pDbAcParamRec = pDbAcParamRec;
	m_pGraphEcho->m_pIRLeft = m_pIRLeft;
	m_pGraphEcho->m_pIRRight = m_pIRRight;
	m_pGraphEcho->m_nData = m_nWaveData;
	m_pGraphEcho->m_fRate = m_fRate;
	m_pGraphEcho->m_fStartTime = m_fStartTimeEcho;
	m_pGraphEcho->m_fDispTime = m_fDispTimeEcho;

	if (m_dbImpulseRec.nChannel == 2) {
		m_pGraphIACC = new CGraphIACC;
		m_cGraph.AddPage(m_pGraphIACC, IDD_GRAPH_IACC, IDS_TAB_CCF);
		m_pGraphIACC->m_pParent = this;
		m_pGraphIACC->m_pDbImpulseRec = &m_dbImpulseRec;
		m_pGraphIACC->m_pDbAcParamRec = pDbAcParamRec;
		m_pGraphIACC->m_pIRLeft = m_pIRLeft;
		m_pGraphIACC->m_pIRRight = m_pIRRight;
		m_pGraphIACC->m_fRate = m_fRate;
		m_pGraphIACC->m_hWaveData = m_hWaveData;
	}

	m_pGraphFreq = new CGraphFreq;
	m_cGraph.AddPage(m_pGraphFreq, IDD_GRAPH_FREQ, IDS_TAB_SPECTRUM);
	m_pGraphFreq->m_pDbImpulseRec = &m_dbImpulseRec;
	m_pGraphFreq->m_pWaveLeft = m_pIRLeft;
	m_pGraphFreq->m_pWaveRight = m_pIRRight;
	m_pGraphFreq->m_fRate = m_fRate;

	if (m_dbImpulseRec.nChannel == 2 && m_bCalculated) {
		m_pGraphPref = new CGraphPref;
		m_cGraph.AddPage(m_pGraphPref, IDD_GRAPH_PREF, IDS_TAB_PREFERENCE);
		m_pGraphPref->m_pDbImpulseRec = &m_dbImpulseRec;
		m_pGraphPref->m_pDbAcParamRec = pDbAcParamRec;
		m_pGraphPref->m_fScale = m_fScale;
		m_pGraphPref->m_fAdjustSPL = m_fAdjustSPL;
	}

	if (m_bCalculated) {
		m_pGraphParam = new CGraphParam;
		m_cGraph.AddPage(m_pGraphParam, IDD_GRAPH_PARAM, IDS_TAB_PARAMETERS);
		m_pGraphParam->m_pParent = this;
		m_pGraphParam->m_pDbImpulseRec = &m_dbImpulseRec;
		m_pGraphParam->m_pDbAcParamRec = pDbAcParamRec;
		m_pGraphParam->m_fAdjustSPL = m_fAdjustSPL;

		m_pGraphMTF = new CGraphMTF;
		m_cGraph.AddPage(m_pGraphMTF, IDD_GRAPH_MTF, "MTF/STI");
		m_pGraphMTF->m_pParent = this;
		m_pGraphMTF->m_pDbImpulseRec = &m_dbImpulseRec;
		m_pGraphMTF->m_pIRLeft = m_pIRLeft;
		m_pGraphMTF->m_pIRRight = m_pIRRight;
		m_pGraphMTF->m_nData = m_nWaveData;
		m_pGraphMTF->m_fRate = m_fRate;
		m_pGraphMTF->m_pDbAcParamResult = &pDbAcParamRec->dbAcParamResult;
	}

	if (m_nCurrentPage >= m_cGraph.GetItemCount())
		m_nCurrentPage = 0;
	m_cGraph.SetPage(m_nCurrentPage);
}

void CImpFactor::InitDispAcParamList()
{
	m_nScrollX[m_nParamPrev] = m_cParameterList.GetScrollPos(SB_HORZ);
	m_nScrollY = m_cParameterList.GetScrollPos(SB_VERT);
	m_nParamPrev = m_nParam;

	m_cParameterList.SetRedraw(FALSE);

	InitAcParamList();
	DispAcParamList();

	m_cParameterList.SetRedraw(TRUE);

	if (m_nScrollX[m_nParam] != 0 || m_nScrollY != 0) {
		RECT rect;
		m_cParameterList.GetItemRect(0, &rect, LVIR_BOUNDS);
		SIZE size;
		size.cx = m_nScrollX[m_nParam];
		size.cy = m_nScrollY * (rect.bottom - rect.top);
		m_cParameterList.Scroll(size);
	}
}

void CImpFactor::InitAcParamList()
{
	struct LISTCOLUMN {
		UINT nID;
		char *text;
		int width;
	};
	static const LISTCOLUMN tParam1Stereo[] = {
		{0, "Freq.", 53},
		{0, "SPL", 53},
		{0, "SPL-L", 53},
		{0, "SPL-R", 53},
		{IDS_DT1, NULL, 53},
		{IDS_DT1, "-L", 53},
		{IDS_DT1, "-R", 53},
		{0, "A", 53},
		{0, "A-L", 53},
		{0, "A-R", 53},
		{0, "Tsub", 53},
		{0, "Tsub-L", 53},
		{0, "Tsub-R", 53},
		{0, "IACC", 53},
		{IDS_TIACC, NULL, 53},
		{0, "W_IACC", 53},
		{0, "S", 53}
	}, tParam1Mono[] = {
		{0, "Freq.", 53},
		{0, "SPL", 53},
		{IDS_DT1, NULL, 53},
		{0, "A-value", 53},
		{0, "Tsub", 53}
	}, tParam2Stereo[] = {
		{0, "Freq.", 53},
		{0, "G", 53},
		{0, "G-L", 53},
		{0, "G-R", 53},
		{0, "T20", 53},
		{0, "T20-L", 53},
		{0, "T20-R", 53},
		{0, "T30", 53},
		{0, "T30-L", 53},
		{0, "T30-R", 53},
		{0, "Tc", 53},
		{0, "Tc-L", 53},
		{0, "Tc-R", 53},
		{0, "EDT", 53},
		{0, "EDT-L", 53},
		{0, "EDT-R", 53},
		{0, "C50", 53},
		{0, "C50-L", 53},
		{0, "C50-R", 53},
		{0, "C80", 53},
		{0, "C80-L", 53},
		{0, "C80-R", 53},
		{0, "Cc", 53},
		{0, "Cc-L", 53},
		{0, "Cc-R", 53},
		{0, "D50", 53},
		{0, "D50-L", 53},
		{0, "D50-R", 53},
		{0, "Ts", 53},
		{0, "Ts-L", 53},
		{0, "Ts-R", 53},
		{0, "IACCE", 53},
		{0, "IACCL", 53}
	}, tParam2Mono[] = {
		{0, "Freq.", 53},
		{0, "G", 53},
		{0, "T20", 53},
		{0, "T30", 53},
		{0, "Tc", 53},
		{0, "EDT", 53},
		{0, "C50", 53},
		{0, "C80", 53},
		{0, "Cc", 53},
		{0, "D50", 53},
		{0, "Ts", 53}
	};
	const LISTCOLUMN *pListColumn;
	int size;
	CString str;

	m_cParameterList.DeleteAllItems();

	while (m_cParameterList.DeleteColumn(0))
		;

	if (!m_bCalculated)
		return;

	if (m_nParam == 0) {
		if (m_dbImpulseRec.nChannel == 1) {
			pListColumn = tParam1Mono;
			size = sizeof(tParam1Mono) / sizeof(LISTCOLUMN);
		} else {
			pListColumn = tParam1Stereo;
			size = sizeof(tParam1Stereo) / sizeof(LISTCOLUMN);
		}
	} else {
		if (m_dbImpulseRec.nChannel == 1) {
			pListColumn = tParam2Mono;
			size = sizeof(tParam2Mono) / sizeof(LISTCOLUMN);
		} else {
			pListColumn = tParam2Stereo;
			size = sizeof(tParam2Stereo) / sizeof(LISTCOLUMN);
		}
	}

	for (int i = 0; i < size; i++, pListColumn++) {
		if (pListColumn->nID == 0)
			str = pListColumn->text;
		else {
			str.LoadString(pListColumn->nID);
			if (pListColumn->text != NULL)
				str += pListColumn->text;
		}
#ifdef _LANG_JPN
		int nWidth = CDpi::AdjustDpi(pListColumn->width, 90);
#else
		int nWidth = CDpi::AdjustDpi(pListColumn->width);
#endif
		m_cParameterList.InsertColumn(i, str, LVCFMT_RIGHT, nWidth);
	}
}

void CImpFactor::DispAcParamList()
{
	m_cParameterList.DeleteAllItems();

	if (!m_bCalculated)
		return;

	if (m_nParam == 0)
		DispAcParam1List();
	else
		DispAcParam2List();

	LVFINDINFO fi;
	fi.flags = LVFI_STRING;
	fi.psz = m_sCurrentFreq;
	m_nCurrentDataNo = m_cParameterList.FindItem(&fi);
	if (m_nCurrentDataNo == -1)
		m_nCurrentDataNo = 0;
	m_cParameterList.SetItemState(m_nCurrentDataNo, LVIS_SELECTED, LVIS_SELECTED);
}

void CImpFactor::DispAcParam1List()
{
	CString str;
	double s0;
	DbAcParamData *pDbAcParamData = m_dbAcParamRec.dbAcParamData;

	for (int i = 0; i < m_dbAcParamRec.nDataNum; i++) {
		GetDispFreq(pDbAcParamData[i].nFreq, m_dbAcParamRec.dbAcParamCond.nFreqBand, str);
		m_cParameterList.InsertItem(i, str);

		if (m_dbImpulseRec.nChannel == 1) {
			str.Format("%.2lf", pDbAcParamData[i].fSplL + m_fAdjustSPL);
			m_cParameterList.SetItemText(i, CLM_SPL, str);

			if (pDbAcParamData[i].nFreq == FREQ_ALL) {
				str.Format("%.2lf", m_dbAcParamRec.dbAcParamResult.fDeltaT1L - m_dbAcParamRec.dbAcParamResult.fDeltaT0L);
				m_cParameterList.SetItemText(i, CLM_DT1, str);
			}

			str.Format("%.2lf", pDbAcParamData[i].fAL);
			m_cParameterList.SetItemText(i, CLM_A, str);

			str.Format("%.3lf", pDbAcParamData[i].tSubL);
			m_cParameterList.SetItemText(i, CLM_TSUB, str);
		} else {
			str.Format("%.2lf", MeanSPL(pDbAcParamData[i].fSplL, pDbAcParamData[i].fSplR) + m_fAdjustSPL);
			m_cParameterList.SetItemText(i, CLM_SPL_LR, str);
			str.Format("%.2lf", pDbAcParamData[i].fSplL + m_fAdjustSPL);
			m_cParameterList.SetItemText(i, CLM_SPL_L, str);
			str.Format("%.2lf", pDbAcParamData[i].fSplR + m_fAdjustSPL);
			m_cParameterList.SetItemText(i, CLM_SPL_R, str);

			if (pDbAcParamData[i].nFreq == FREQ_ALL) {
				str.Format("%.2lf", MeanDT1(m_dbAcParamRec.dbAcParamResult.fDeltaT1L - m_dbAcParamRec.dbAcParamResult.fDeltaT0L, m_dbAcParamRec.dbAcParamResult.fDeltaT1R - m_dbAcParamRec.dbAcParamResult.fDeltaT0R));
				m_cParameterList.SetItemText(i, CLM_DT1_LR, str);
				str.Format("%.2lf", m_dbAcParamRec.dbAcParamResult.fDeltaT1L - m_dbAcParamRec.dbAcParamResult.fDeltaT0L);
				m_cParameterList.SetItemText(i, CLM_DT1_L, str);
				str.Format("%.2lf", m_dbAcParamRec.dbAcParamResult.fDeltaT1R - m_dbAcParamRec.dbAcParamResult.fDeltaT0R);
				m_cParameterList.SetItemText(i, CLM_DT1_R, str);
			}

			str.Format("%.2lf", MeanA(pDbAcParamData[i].fAL, pDbAcParamData[i].fAR));
			m_cParameterList.SetItemText(i, CLM_A_LR, str);
			str.Format("%.2lf", pDbAcParamData[i].fAL);
			m_cParameterList.SetItemText(i, CLM_A_L, str);
			str.Format("%.2lf", pDbAcParamData[i].fAR);
			m_cParameterList.SetItemText(i, CLM_A_R, str);

			str.Format("%.3lf", MeanTsub(pDbAcParamData[i].tSubL, pDbAcParamData[i].tSubR));
			m_cParameterList.SetItemText(i, CLM_TSUB_LR, str);
			str.Format("%.3lf", pDbAcParamData[i].tSubL);
			m_cParameterList.SetItemText(i, CLM_TSUB_L, str);
			str.Format("%.3lf", pDbAcParamData[i].tSubR);
			m_cParameterList.SetItemText(i, CLM_TSUB_R, str);

			str.Format("%.2lf", pDbAcParamData[i].fIACC);
			m_cParameterList.SetItemText(i, CLM_IACC, str);
			str.Format("%.2lf", pDbAcParamData[i].tIACC);
			m_cParameterList.SetItemText(i, CLM_TIACC, str);
			str.Format("%.2lf", pDbAcParamData[i].wIACC);
			m_cParameterList.SetItemText(i, CLM_WIACC, str);

			if (pDbAcParamData[i].nFreq == FREQ_ALL || pDbAcParamData[i].nFreq == FREQ_AFILTER) {
				if (CalcS0(&m_dbAcParamRec, i, m_fAdjustSPL, &s0))
					str.Format("%.2lf", s0);
				else
					str = "----";
				m_cParameterList.SetItemText(i, CLM_PREF, str);
			}
		}
		m_cParameterList.SetItemData(i, i);
	}
}

void CImpFactor::DispAcParam2List()
{
	CString str;
	DbAcParamData *pDbAcParamData = m_dbAcParamRec.dbAcParamData;

	for (int i = 0; i < m_dbAcParamRec.nDataNum; i++) {
		GetDispFreq(pDbAcParamData[i].nFreq, m_dbAcParamRec.dbAcParamCond.nFreqBand, str);
		m_cParameterList.InsertItem(i, str);

		if (m_dbImpulseRec.nChannel == 1) {
			str.Format("%.2lf", pDbAcParamData[i].fG_L + m_fAdjustSPL);
			m_cParameterList.SetItemText(i, CLM_G, str);

			str.Format("%.3lf", pDbAcParamData[i].fT20_L);
			m_cParameterList.SetItemText(i, CLM_T20, str);

			str.Format("%.3lf", pDbAcParamData[i].fT30_L);
			m_cParameterList.SetItemText(i, CLM_T30, str);

			str.Format("%.3lf", pDbAcParamData[i].fTCustom_L);
			m_cParameterList.SetItemText(i, CLM_TCUSTOM, str);

			str.Format("%.3lf", pDbAcParamData[i].fEDT_L);
			m_cParameterList.SetItemText(i, CLM_EDT, str);

			str.Format("%.2lf", pDbAcParamData[i].fC50_L);
			m_cParameterList.SetItemText(i, CLM_C50, str);

			str.Format("%.2lf", pDbAcParamData[i].fC80_L);
			m_cParameterList.SetItemText(i, CLM_C80, str);

			str.Format("%.2lf", pDbAcParamData[i].fCCustom_L);
			m_cParameterList.SetItemText(i, CLM_CCUSTOM, str);

			str.Format("%.3lf", pDbAcParamData[i].fD50_L);
			m_cParameterList.SetItemText(i, CLM_D50, str);

			str.Format("%.3lf", pDbAcParamData[i].fTs_L);
			m_cParameterList.SetItemText(i, CLM_TS, str);
		} else {
			str.Format("%.2lf", MeanG(pDbAcParamData[i].fG_L, pDbAcParamData[i].fG_R) + m_fAdjustSPL);
			m_cParameterList.SetItemText(i, CLM_G_LR, str);

			str.Format("%.2lf", pDbAcParamData[i].fG_L + m_fAdjustSPL);
			m_cParameterList.SetItemText(i, CLM_G_L, str);

			str.Format("%.2lf", pDbAcParamData[i].fG_R + m_fAdjustSPL);
			m_cParameterList.SetItemText(i, CLM_G_R, str);

			str.Format("%.3lf", MeanT(pDbAcParamData[i].fT20_L, pDbAcParamData[i].fT20_R));
			m_cParameterList.SetItemText(i, CLM_T20_LR, str);

			str.Format("%.3lf", pDbAcParamData[i].fT20_L);
			m_cParameterList.SetItemText(i, CLM_T20_L, str);

			str.Format("%.3lf", pDbAcParamData[i].fT20_R);
			m_cParameterList.SetItemText(i, CLM_T20_R, str);

			str.Format("%.3lf", MeanT(pDbAcParamData[i].fT30_L, pDbAcParamData[i].fT30_R));
			m_cParameterList.SetItemText(i, CLM_T30_LR, str);

			str.Format("%.3lf", pDbAcParamData[i].fT30_L);
			m_cParameterList.SetItemText(i, CLM_T30_L, str);

			str.Format("%.3lf", pDbAcParamData[i].fT30_R);
			m_cParameterList.SetItemText(i, CLM_T30_R, str);

			str.Format("%.3lf", MeanT(pDbAcParamData[i].fTCustom_L, pDbAcParamData[i].fTCustom_R));
			m_cParameterList.SetItemText(i, CLM_TCUSTOM_LR, str);

			str.Format("%.3lf", pDbAcParamData[i].fTCustom_L);
			m_cParameterList.SetItemText(i, CLM_TCUSTOM_L, str);

			str.Format("%.3lf", pDbAcParamData[i].fTCustom_R);
			m_cParameterList.SetItemText(i, CLM_TCUSTOM_R, str);

			str.Format("%.3lf", MeanT(pDbAcParamData[i].fEDT_L, pDbAcParamData[i].fEDT_R));
			m_cParameterList.SetItemText(i, CLM_EDT_LR, str);

			str.Format("%.3lf", pDbAcParamData[i].fEDT_L);
			m_cParameterList.SetItemText(i, CLM_EDT_L, str);

			str.Format("%.3lf", pDbAcParamData[i].fEDT_R);
			m_cParameterList.SetItemText(i, CLM_EDT_R, str);

			str.Format("%.2lf", MeanC(pDbAcParamData[i].fC50_L, pDbAcParamData[i].fC50_R));
			m_cParameterList.SetItemText(i, CLM_C50_LR, str);

			str.Format("%.2lf", pDbAcParamData[i].fC50_L);
			m_cParameterList.SetItemText(i, CLM_C50_L, str);

			str.Format("%.2lf", pDbAcParamData[i].fC50_R);
			m_cParameterList.SetItemText(i, CLM_C50_R, str);

			str.Format("%.2lf", MeanC(pDbAcParamData[i].fC80_L, pDbAcParamData[i].fC80_R));
			m_cParameterList.SetItemText(i, CLM_C80_LR, str);

			str.Format("%.2lf", pDbAcParamData[i].fC80_L);
			m_cParameterList.SetItemText(i, CLM_C80_L, str);

			str.Format("%.2lf", pDbAcParamData[i].fC80_R);
			m_cParameterList.SetItemText(i, CLM_C80_R, str);

			str.Format("%.2lf", MeanC(pDbAcParamData[i].fCCustom_L, pDbAcParamData[i].fCCustom_R));
			m_cParameterList.SetItemText(i, CLM_CCUSTOM_LR, str);

			str.Format("%.2lf", pDbAcParamData[i].fCCustom_L);
			m_cParameterList.SetItemText(i, CLM_CCUSTOM_L, str);

			str.Format("%.2lf", pDbAcParamData[i].fCCustom_R);
			m_cParameterList.SetItemText(i, CLM_CCUSTOM_R, str);

			str.Format("%.2lf", MeanD(pDbAcParamData[i].fD50_L, pDbAcParamData[i].fD50_R));
			m_cParameterList.SetItemText(i, CLM_D50_LR, str);

			str.Format("%.2lf", pDbAcParamData[i].fD50_L);
			m_cParameterList.SetItemText(i, CLM_D50_L, str);

			str.Format("%.2lf", pDbAcParamData[i].fD50_R);
			m_cParameterList.SetItemText(i, CLM_D50_R, str);

			str.Format("%.3lf", MeanTs(pDbAcParamData[i].fTs_L, pDbAcParamData[i].fTs_R));
			m_cParameterList.SetItemText(i, CLM_TS_LR, str);

			str.Format("%.3lf", pDbAcParamData[i].fTs_L);
			m_cParameterList.SetItemText(i, CLM_TS_L, str);

			str.Format("%.3lf", pDbAcParamData[i].fTs_R);
			m_cParameterList.SetItemText(i, CLM_TS_R, str);

			str.Format("%.2lf", pDbAcParamData[i].fIACCE);
			m_cParameterList.SetItemText(i, CLM_IACCE, str);

			str.Format("%.2lf", pDbAcParamData[i].fIACCL);
			m_cParameterList.SetItemText(i, CLM_IACCL, str);
		}
		m_cParameterList.SetItemData(i, i);
	}
}

void CImpFactor::DispCalcParam()
{

	if (m_bCalculated) {
		m_Param1.m_iFreqBand = m_dbAcParamRec.dbAcParamCond.nFreqBand;

		if (m_dbAcParamRec.dbAcParamCond.nSplRefData == SPL_ABSOLUTE)
			m_Param1.m_cSplRefData.LoadString(IDS_ABSVALUE);
		else if (m_dbAcParamRec.dbAcParamCond.nSplRefData == SPL_MAXREF)
			m_Param1.m_cSplRefData.LoadString(IDS_MAXVALUE);
		else {
			CDbImpulse dbImpulse;
			DbImpulseRec dbImpulseRec;

			if (dbImpulse.Open()) {
				if (dbImpulse.ReadRecID(m_dbAcParamRec.dbAcParamCond.nSplRefData, &dbImpulseRec))
					m_Param1.m_cSplRefData = dbImpulseRec.sTitle;
			}
		}

		if (m_dbAcParamRec.dbAcParamCond.nSplRefData == SPL_ABSOLUTE)
			m_Param1.m_cSplRefLevel.Blank();
		else
			m_Param1.m_cSplRefLevel = m_dbAcParamRec.dbAcParamCond.fSplRefLevel;

		if (m_dbAcParamRec.dbAcParamCond.bTsubAuto)
			m_Param1.m_cTsubEnd.Blank();
		else
			m_Param1.m_cTsubEnd = m_dbAcParamRec.dbAcParamCond.fTsubEnd;

		m_Param1.m_bTsubAuto = m_dbAcParamRec.dbAcParamCond.bTsubAuto;
		m_Param1.m_cTsubNoise = m_dbAcParamRec.dbAcParamCond.fTsubNoise * 100;
		m_Param1.m_cDT1MinTime = m_dbAcParamRec.dbAcParamCond.fDT1MinTime;
		m_Param1.m_cIACCWLevel = m_dbAcParamRec.dbAcParamCond.fWIACCLevel;
		m_Param1.m_cPrefSPL = m_dbAcParamRec.dbAcParamCond.fPrefSPL;
		m_Param1.m_cPrefTauE = m_dbAcParamRec.dbAcParamCond.fPrefTauE;

		int index = 1;
		if (m_dbAcParamRec.dbAcParamCond.bAFilter)
			index++;
		if (m_dbAcParamRec.nDataNum > index) {
			GetDispFreq(m_dbAcParamRec.dbAcParamData[index].nFreq, m_dbAcParamRec.dbAcParamCond.nFreqBand, m_Param1.m_sStartFreq);
			GetDispFreq(m_dbAcParamRec.dbAcParamData[m_dbAcParamRec.nDataNum - 1].nFreq, m_dbAcParamRec.dbAcParamCond.nFreqBand, m_Param1.m_sEndFreq);
		} else {
			m_Param1.m_sStartFreq.Empty();
			m_Param1.m_sEndFreq.Empty();
		}
		m_Param1.m_bAFilter = m_dbAcParamRec.dbAcParamCond.bAFilter;

		m_Param2.m_cGRefData = m_dbAcParamRec.dbAcParamCond.fGRefData;
		m_Param2.m_cTCustom1 = m_dbAcParamRec.dbAcParamCond.fTCustom1;
		m_Param2.m_cTCustom2 = m_dbAcParamRec.dbAcParamCond.fTCustom2;
		m_Param2.m_cCCustom = m_dbAcParamRec.dbAcParamCond.fCCustom;
	} else {
		m_Param1.m_iFreqBand = -1;
		m_Param1.m_cSplRefData.Blank();
		m_Param1.m_cSplRefLevel.Blank();
		m_Param1.m_cTsubEnd.Blank();
		m_Param1.m_bTsubAuto = 0;
		m_Param1.m_cTsubNoise.Blank();
		m_Param1.m_cDT1MinTime.Blank();
		m_Param1.m_cIACCWLevel.Blank();
		m_Param1.m_cPrefSPL.Blank();
		m_Param1.m_cPrefTauE.Blank();
		m_Param1.m_sStartFreq.Empty();
		m_Param1.m_sEndFreq.Empty();
		m_Param1.m_bAFilter = 0;

		m_Param2.m_cGRefData.Blank();
		m_Param2.m_cTCustom1.Blank();
		m_Param2.m_cTCustom2.Blank();
		m_Param2.m_cCCustom.Blank();
	}

	m_Param2.m_iFreqBand = m_Param1.m_iFreqBand;
	m_Param2.m_sStartFreq = m_Param1.m_sStartFreq;
	m_Param2.m_sEndFreq = m_Param1.m_sEndFreq;
	m_Param2.m_bAFilter = m_Param1.m_bAFilter;

	m_Param1.UpdateData(FALSE);
	m_Param2.UpdateData(FALSE);
}

void CImpFactor::DeleteGraphWindow()
{
	if (m_pGraphIR != NULL) {
		delete m_pGraphIR;
		m_pGraphIR = NULL;
	}
	if (m_pGraphEcho != NULL) {
		delete m_pGraphEcho;
		m_pGraphEcho = NULL;
	}

	if (m_pGraphIACC != NULL) {
		delete m_pGraphIACC;
		m_pGraphIACC = NULL;
	}

	if (m_pGraphFreq != NULL) {
		delete m_pGraphFreq;
		m_pGraphFreq = NULL;
	}

	if (m_pGraphParam != NULL) {
		delete m_pGraphParam;
		m_pGraphParam = NULL;
	}

	if (m_pGraphPref != NULL) {
		delete m_pGraphPref;
		m_pGraphPref = NULL;
	}

	if (m_pGraphMTF != NULL) {
		delete m_pGraphMTF;
		m_pGraphMTF = NULL;
	}
}

void CImpFactor::FreeBuffers()
{
	if (m_pWaveLeft != NULL) {
		delete [] m_pWaveLeft;
		m_pWaveLeft = NULL;
	}

	if (m_pWaveRight != NULL) {
		delete [] m_pWaveRight;
		m_pWaveRight = NULL;
	}

	if (m_pIRLeft != NULL) {
		delete [] m_pIRLeft;
		m_pIRLeft = NULL;
	}

	if (m_pIRRight != NULL) {
		delete [] m_pIRRight;
		m_pIRRight = NULL;
	}

	if (m_hWaveData != NULL) {
		::GlobalFree(m_hWaveData);
		m_hWaveData = NULL;
	}

	if (m_pMethod != NULL) {
		delete m_pMethod;
		m_pMethod = NULL;
	}
}

void CImpFactor::CalcIR(int nFreq, int freqBand)
{
	CWaitCursor waitCursor;

	memcpy(m_pIRLeft, m_pWaveLeft, m_nData * sizeof(double));

	if (m_dbImpulseRec.nChannel == 2)
		memcpy(m_pIRRight, m_pWaveRight, m_nData * sizeof(double));

	m_pMethod->CalcImpulse(m_pIRLeft);
	if (m_dbImpulseRec.nChannel == 2)
		m_pMethod->CalcImpulse(m_pIRRight);

	if (m_dbImpulseRec.bImpulseData) {
		for (int i = 0; i < m_nData; i++) {
			m_pIRLeft[i] *= m_nData;
			if (m_dbImpulseRec.nChannel == 2)
				m_pIRRight[i] *= m_nData;
		}
	}

	CalcFilterAuto(m_pIRLeft, m_pIRRight, m_nData, m_fRate, nFreq, freqBand);
}

void CImpFactor::OnSelectData()
{
	AfxGetApp()->m_pMainWnd->SetActiveWindow();
}

void CImpFactor::OnPrevData()
{
	long impulseID;

	if ((impulseID = m_pView->GetPrevItem(m_nImpulseID)) != -1) {
		SaveDataCheck();

		m_nImpulseID = impulseID;
		ReadData();
		DispGraph();
	}
}

void CImpFactor::OnNextData()
{
	long impulseID;

	if ((impulseID = m_pView->GetNextItem(m_nImpulseID)) != -1) {
		SaveDataCheck();

		m_nImpulseID = impulseID;
		ReadData();
		DispGraph();
	}
}

void CImpFactor::EnablePrevNext()
{
	m_cPrevData.EnableWindow(m_nImpulseID != -2 && m_pView->GetPrevItem(m_nImpulseID) != -1);
	m_cNextData.EnableWindow(m_nImpulseID != -2 && m_pView->GetNextItem(m_nImpulseID) != -1);
	m_cSelectData.EnableWindow(m_nImpulseID != -2);
}

void CImpFactor::OnColumnclickParamList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (m_nSortItem != pNMListView->iSubItem)
		m_nSortDir = 1;
	else
		m_nSortDir *= -1;

	m_nSortItem = pNMListView->iSubItem;
	m_cParameterList.SortItems(m_nParam == 0 ? CompareItems1 : CompareItems2, (LPARAM)this);

	*pResult = 0;
}

int CALLBACK CImpFactor::CompareItems1(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CImpFactor *pWnd = (CImpFactor *)lParamSort;
	DbAcParamData *pDbAcParamData = pWnd->m_dbAcParamRec.dbAcParamData;
	double d;

	if (pWnd->m_dbImpulseRec.nChannel == 1) {
		switch (pWnd->m_nSortItem) {
		case CLM_SPL:
			d = pDbAcParamData[lParam1].fSplL - pDbAcParamData[lParam2].fSplL;
			break;
		case CLM_A:
			d = pDbAcParamData[lParam1].fAL - pDbAcParamData[lParam2].fAL;
			break;
		case CLM_TSUB:
			d = pDbAcParamData[lParam1].tSubL - pDbAcParamData[lParam2].tSubL;
			break;
		default:
			return (int)(lParam1 - lParam2) * pWnd->m_nSortDir;
		}
	} else {
		switch (pWnd->m_nSortItem) {
		case CLM_SPL_LR:
			d = MeanSPL(pDbAcParamData[lParam1].fSplL, pDbAcParamData[lParam1].fSplR) -
				MeanSPL(pDbAcParamData[lParam2].fSplL, pDbAcParamData[lParam2].fSplR);
			break;
		case CLM_SPL_L:
			d = pDbAcParamData[lParam1].fSplL - pDbAcParamData[lParam2].fSplL;
			break;
		case CLM_SPL_R:
			d = pDbAcParamData[lParam1].fSplR - pDbAcParamData[lParam2].fSplR;
			break;
		case CLM_A_LR:
			d = MeanA(pDbAcParamData[lParam1].fAL, pDbAcParamData[lParam1].fAR) -
				MeanA(pDbAcParamData[lParam2].fAL, pDbAcParamData[lParam2].fAR);
			break;
		case CLM_A_L:
			d = pDbAcParamData[lParam1].fAL - pDbAcParamData[lParam2].fAL;
			break;
		case CLM_A_R:
			d = pDbAcParamData[lParam1].fAR - pDbAcParamData[lParam2].fAR;
			break;
		case CLM_TSUB_LR:
			d = MeanTsub(pDbAcParamData[lParam1].tSubL, pDbAcParamData[lParam1].tSubR) -
				MeanTsub(pDbAcParamData[lParam2].tSubL, pDbAcParamData[lParam2].tSubR);
			break;
		case CLM_TSUB_L:
			d = pDbAcParamData[lParam1].tSubL - pDbAcParamData[lParam2].tSubL;
			break;
		case CLM_TSUB_R:
			d = pDbAcParamData[lParam1].tSubR - pDbAcParamData[lParam2].tSubR;
			break;
		case CLM_IACC:
			d = pDbAcParamData[lParam1].fIACC - pDbAcParamData[lParam2].fIACC;
			break;
		case CLM_TIACC:
			d = pDbAcParamData[lParam1].tIACC - pDbAcParamData[lParam2].tIACC;
			break;
		case CLM_WIACC:
			d = pDbAcParamData[lParam1].wIACC - pDbAcParamData[lParam2].wIACC;
			break;
		default:
			return (int)(lParam1 - lParam2) * pWnd->m_nSortDir;
		}
	}

	d *= pWnd->m_nSortDir;

	if (d > 0)
		return 1;
	else if (d < 0)
		return  -1;
	else
		return 0;
}

int CALLBACK CImpFactor::CompareItems2(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CImpFactor *pWnd = (CImpFactor *)lParamSort;
	DbAcParamData *pDbAcParamData = pWnd->m_dbAcParamRec.dbAcParamData;
	double d;

	if (pWnd->m_dbImpulseRec.nChannel == 1) {
		switch (pWnd->m_nSortItem) {
		case CLM_G:
			d = pDbAcParamData[lParam1].fG_L -
				pDbAcParamData[lParam2].fG_L;
			break;
		case CLM_T20:
			d = pDbAcParamData[lParam1].fT20_L -
				pDbAcParamData[lParam2].fT20_L;
			break;
		case CLM_T30:
			d = pDbAcParamData[lParam1].fT30_L -
				pDbAcParamData[lParam2].fT30_L;
			break;
		case CLM_TCUSTOM:
			d = pDbAcParamData[lParam1].fTCustom_L -
				pDbAcParamData[lParam2].fTCustom_L;
			break;
		case CLM_EDT:
			d = pDbAcParamData[lParam1].fEDT_L -
				pDbAcParamData[lParam2].fEDT_L;
			break;
		case CLM_C50:
			d = pDbAcParamData[lParam1].fC50_L -
				pDbAcParamData[lParam2].fC50_L;
			break;
		case CLM_C80:
			d = pDbAcParamData[lParam1].fC80_L -
				pDbAcParamData[lParam2].fC80_L;
			break;
		case CLM_CCUSTOM:
			d = pDbAcParamData[lParam1].fCCustom_L -
				pDbAcParamData[lParam2].fCCustom_L;
			break;
		case CLM_D50:
			d = pDbAcParamData[lParam1].fD50_L -
				pDbAcParamData[lParam2].fD50_L;
			break;
		case CLM_TS:
			d = pDbAcParamData[lParam1].fTs_L -
				pDbAcParamData[lParam2].fTs_L;
			break;
		default:
			return (int)(lParam1 - lParam2) * pWnd->m_nSortDir;
		}
	} else {
		switch (pWnd->m_nSortItem) {
		case CLM_G_LR:
			d = MeanG(pDbAcParamData[lParam1].fG_L, pDbAcParamData[lParam1].fG_R) -
				MeanG(pDbAcParamData[lParam2].fG_L, pDbAcParamData[lParam2].fG_R);
			break;
		case CLM_G_L:
			d = pDbAcParamData[lParam1].fG_L -
				pDbAcParamData[lParam2].fG_L;
			break;
		case CLM_G_R:
			d = pDbAcParamData[lParam1].fG_R -
				pDbAcParamData[lParam2].fG_R;
			break;
		case CLM_T20_LR:
			d = MeanT(pDbAcParamData[lParam1].fT20_L, pDbAcParamData[lParam1].fT20_R) -
				MeanT(pDbAcParamData[lParam2].fT20_L, pDbAcParamData[lParam2].fT20_R);
			break;
		case CLM_T20_L:
			d = pDbAcParamData[lParam1].fT20_L -
				pDbAcParamData[lParam2].fT20_L;
			break;
		case CLM_T20_R:
			d = pDbAcParamData[lParam1].fT20_R -
				pDbAcParamData[lParam2].fT20_R;
			break;
		case CLM_T30_LR:
			d = MeanT(pDbAcParamData[lParam1].fT30_L, pDbAcParamData[lParam1].fT30_R) -
				MeanT(pDbAcParamData[lParam2].fT30_L, pDbAcParamData[lParam2].fT30_R);
			break;
		case CLM_T30_L:
			d = pDbAcParamData[lParam1].fT30_L -
				pDbAcParamData[lParam2].fT30_L;
			break;
		case CLM_T30_R:
			d = pDbAcParamData[lParam1].fT30_R -
				pDbAcParamData[lParam2].fT30_R;
			break;
		case CLM_TCUSTOM_LR:
			d = MeanT(pDbAcParamData[lParam1].fTCustom_L, pDbAcParamData[lParam1].fTCustom_R) -
				MeanT(pDbAcParamData[lParam2].fTCustom_L, pDbAcParamData[lParam2].fTCustom_R);
			break;
		case CLM_TCUSTOM_L:
			d = pDbAcParamData[lParam1].fTCustom_L -
				pDbAcParamData[lParam2].fTCustom_L;
			break;
		case CLM_TCUSTOM_R:
			d = pDbAcParamData[lParam1].fTCustom_R -
				pDbAcParamData[lParam2].fTCustom_R;
			break;
		case CLM_EDT_LR:
			d = MeanT(pDbAcParamData[lParam1].fEDT_L, pDbAcParamData[lParam1].fEDT_R) -
				MeanT(pDbAcParamData[lParam2].fEDT_L, pDbAcParamData[lParam2].fEDT_R);
			break;
		case CLM_EDT_L:
			d = pDbAcParamData[lParam1].fEDT_L -
				pDbAcParamData[lParam2].fEDT_L;
			break;
		case CLM_EDT_R:
			d = pDbAcParamData[lParam1].fEDT_R -
				pDbAcParamData[lParam2].fEDT_R;
			break;
		case CLM_C50_LR:
			d = MeanC(pDbAcParamData[lParam1].fC50_L, pDbAcParamData[lParam1].fC50_R) -
				MeanC(pDbAcParamData[lParam2].fC50_L, pDbAcParamData[lParam2].fC50_R);
			break;
		case CLM_C50_L:
			d = pDbAcParamData[lParam1].fC50_L -
				pDbAcParamData[lParam2].fC50_L;
			break;
		case CLM_C50_R:
			d = pDbAcParamData[lParam1].fC50_R -
				pDbAcParamData[lParam2].fC50_R;
			break;
		case CLM_C80_LR:
			d = MeanC(pDbAcParamData[lParam1].fC80_L, pDbAcParamData[lParam1].fC80_R) -
				MeanC(pDbAcParamData[lParam2].fC80_L, pDbAcParamData[lParam2].fC80_R);
			break;
		case CLM_C80_L:
			d = pDbAcParamData[lParam1].fC80_L -
				pDbAcParamData[lParam2].fC80_L;
			break;
		case CLM_C80_R:
			d = pDbAcParamData[lParam1].fC80_R -
				pDbAcParamData[lParam2].fC80_R;
			break;
		case CLM_CCUSTOM_LR:
			d = MeanC(pDbAcParamData[lParam1].fCCustom_L, pDbAcParamData[lParam1].fCCustom_R) -
				MeanC(pDbAcParamData[lParam2].fCCustom_L, pDbAcParamData[lParam2].fCCustom_R);
			break;
		case CLM_CCUSTOM_L:
			d = pDbAcParamData[lParam1].fCCustom_L -
				pDbAcParamData[lParam2].fCCustom_L;
			break;
		case CLM_CCUSTOM_R:
			d = pDbAcParamData[lParam1].fCCustom_R -
				pDbAcParamData[lParam2].fCCustom_R;
			break;
		case CLM_D50_LR:
			d = MeanD(pDbAcParamData[lParam1].fD50_L, pDbAcParamData[lParam1].fD50_R) -
				MeanD(pDbAcParamData[lParam2].fD50_L, pDbAcParamData[lParam2].fD50_R);
			break;
		case CLM_D50_L:
			d = pDbAcParamData[lParam1].fD50_L -
				pDbAcParamData[lParam2].fD50_L;
			break;
		case CLM_D50_R:
			d = pDbAcParamData[lParam1].fD50_R -
				pDbAcParamData[lParam2].fD50_R;
			break;
		case CLM_TS_LR:
			d = MeanTs(pDbAcParamData[lParam1].fTs_L, pDbAcParamData[lParam1].fTs_R) -
				MeanTs(pDbAcParamData[lParam2].fTs_L, pDbAcParamData[lParam2].fTs_R);
			break;
		case CLM_TS_L:
			d = pDbAcParamData[lParam1].fTs_L -
				pDbAcParamData[lParam2].fTs_L;
			break;
		case CLM_TS_R:
			d = pDbAcParamData[lParam1].fTs_R -
				pDbAcParamData[lParam2].fTs_R;
			break;
		case CLM_IACCE:
			d = pDbAcParamData[lParam1].fIACCE -
				pDbAcParamData[lParam2].fIACCE;
			break;
		case CLM_IACCL:
			d = pDbAcParamData[lParam1].fIACCL -
				pDbAcParamData[lParam2].fIACCL;
			break;
		default:
			return (int)(lParam1 - lParam2) * pWnd->m_nSortDir;
		}
	}

	d *= pWnd->m_nSortDir;

	if (d > 0)
		return 1;
	else if (d < 0)
		return  -1;
	else
		return 0;
}

void CImpFactor::ChangeData()
{
	DispCalcParam();
	DispAcParamList();

	ReDrawAll();

	m_bChangeData = TRUE;
}

void CImpFactor::OnItemchangedParamList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (pNMListView->uNewState & LVIS_SELECTED && m_nCurrentDataNo != pNMListView->lParam) {
		m_nCurrentDataNo = (int)pNMListView->lParam;
		m_sCurrentFreq = m_cParameterList.GetItemText(m_nCurrentDataNo, 0);
		CalcIR(m_dbAcParamRec.dbAcParamData[m_nCurrentDataNo].nFreq, m_dbAcParamRec.dbAcParamCond.nFreqBand);
		ReDrawAll();
	}

	*pResult = 0;
}

void CImpFactor::ReDrawAll()
{
	if (m_pGraphIR != NULL)
		m_pGraphIR->ReDraw();

	if (m_pGraphEcho != NULL)
		m_pGraphEcho->ReDraw();

	if (m_pGraphIACC != NULL)
		m_pGraphIACC->ReDraw();

	if (m_pGraphFreq != NULL)
		m_pGraphFreq->ReDraw();

	if (m_pGraphPref != NULL)
		m_pGraphPref->ReDraw();

	if (m_pGraphParam != NULL)
		m_pGraphParam->ReDraw();

	if (m_pGraphMTF != NULL)
		m_pGraphMTF->ReDraw();
}

void CImpFactor::OnCancel()
{
	SaveDataCheck();
	DestroyWindow();
}

void CImpFactor::SaveDataCheck()
{
	if (m_bChangeData) {
		if (MessageBox(IDS_MSG_SAVECALC, MB_OKCANCEL | MB_ICONQUESTION) == IDOK) {
			CDbAcParam dbAcParam;

			if (dbAcParam.Open())
				dbAcParam.UpdateRec(&m_dbAcParamRec);
		}
	}
}

void CImpFactor::OnSelchangeGraph(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	m_nCurrentPage = m_cGraph.GetCurSel();

	*pResult = 0;
}

void CImpFactor::CloseAll()
{
	CWnd *pWnd;

	while (!m_WindowList.IsEmpty()) {
		pWnd = (CWnd *)m_WindowList.GetHead();
		if (pWnd != NULL)
			pWnd->DestroyWindow();
	}
}

void CImpFactor::OnPlayback()
{
	m_cPlayback.Play(m_pIRLeft, m_pIRRight, m_nData, (int)m_fRate);
}

void CImpFactor::OnScreenCopy()
{
	CImpScDlg cImpScDlg(this);

	cImpScDlg.m_nChannel = m_dbImpulseRec.nChannel;

	switch (cImpScDlg.DoModal()) {
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
					MMPictureWriteFile(hDIB, (LPTSTR)(LPCSTR)dlg.GetPathName());
					::GlobalFree(hDIB);
				}
			}
		}
		break;
	case IDC_MMLIB:
		HDIB hDIB = ScreenCopy();
		SendMMLib(this, m_sFolderTitle, m_sFolderComment, m_sTitle, m_sComment, "", hDIB);
		::GlobalFree(hDIB);
		break;
	}
}

HDIB CImpFactor::ScreenCopy()
{
	if (setData.ImpSC.nCopyMode == 0) {
		UpdateWindow();
		return m_cScreenCopy.CreateDIB(this);
	} else if (setData.ImpSC.bMulti)
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

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
HDIB CImpFactor::GraphCopy()
{
	int i, nGraph = 0;
	CWnd *pWnd;
	int x, y;
	int nIndex = 0;

	for (i = 0; i < 7; i++) {
		if (m_dbImpulseRec.nChannel != 2) {
			switch (i) {
			case 2:
			case 4:
				continue;
			}
		}

		if (setData.ImpSC.bGraph[i])
			nGraph++;
	}

	if (nGraph == 0)
		return NULL;

	int nCol = min(setData.ImpSC.nColumns, nGraph);
	int nRow = (nGraph + setData.ImpSC.nColumns - 1) / setData.ImpSC.nColumns;

	CRect rect;
	if (setData.ImpSC.bFrame)
		m_cGraph.GetWindowRect(rect);
	else
		m_pGraphIR->GetWindowRect(rect);

	int nWidth = (setData.ImpSC.nColSpace + rect.Width()) * nCol + setData.ImpSC.nColSpace;
	int nHeight = (setData.ImpSC.nRowSpace + rect.Height()) * nRow + setData.ImpSC.nRowSpace;

	CDC *pDC = GetDC();
	CDC DCMem;
	DCMem.CreateCompatibleDC(pDC);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, nWidth, nHeight);

	CGdiObject *pOrg = DCMem.SelectObject(&bmp);
	DCMem.FillSolidRect(0, 0, nWidth, nHeight, GetSysColor(COLOR_3DFACE));

	int nCurrentPage = m_nCurrentPage;

	nCol = nRow = 0;
	for (i = 0; i < 7; i++) {
		if (m_dbImpulseRec.nChannel != 2) {
			switch (i) {
			case 2:
			case 4:
				continue;
			}
		}

		if (setData.ImpSC.bGraph[i]) {
			m_cGraph.SetPage(nIndex);
			m_cGraph.UpdateWindow();
			m_cGraph.GetCurrentDlg()->UpdateWindow();
			if (setData.ImpSC.bFrame)
				pWnd = &m_cGraph;
			else
				pWnd = m_cGraph.GetCurrentDlg();

			x = setData.ImpSC.nColSpace + nCol * (rect.Width() + setData.ImpSC.nColSpace);
			y = setData.ImpSC.nRowSpace + nRow * (rect.Height() + setData.ImpSC.nRowSpace);
			DCMem.BitBlt(x, y, rect.Width(), rect.Height(), pWnd->GetWindowDC(), 0, 0, SRCCOPY);

			if (++nCol == setData.ImpSC.nColumns) {
				nCol = 0;
				nRow++;
			}
		}

		nIndex++;
	}

	m_cGraph.SetPage(nCurrentPage);

	DCMem.SelectObject(pOrg);

	int nSize = sizeof(BITMAPINFOHEADER) + WIDTHBYTES(nWidth * 24) * nHeight;
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

void CImpFactor::SaveStartDisp()
{
	if (m_pGraphIR != NULL) {
		m_fStartTimeIR = m_pGraphIR->m_fStartTime;
		m_fDispTimeIR = m_pGraphIR->m_fDispTime;
	}

	if (m_pGraphEcho != NULL) {
		m_fStartTimeEcho = m_pGraphEcho->m_fStartTime;
		m_fDispTimeEcho = m_pGraphEcho->m_fDispTime;
	}
}

void CImpFactor::OnTcnSelchangeCalcCondition(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	m_nParam = m_cCalcCondition.GetCurSel();

	InitDispAcParamList();

	*pResult = 0;
}

void CImpFactor::OnBnClickedCsvOutput()
{
	CFileDialog fileDlg(FALSE, "csv", NULL,
			OFN_OVERWRITEPROMPT, "CSV File (*.csv)|*.csv|All Files (*.*)|*.*||", this, 0);

	if (fileDlg.DoModal() == IDOK)
		OutputListToCsv(m_cParameterList, fileDlg.GetPathName());
}

BOOL CImpFactor::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDCANCEL, IDH_IMP_FACTOR_CANCEL,
		IDC_SELECT_DATA, IDH_IMP_FACTOR_SELECT_DATA,
		IDC_PREV_DATA, IDH_IMP_FACTOR_PREV_DATA,
		IDC_NEXT_DATA, IDH_IMP_FACTOR_NEXT_DATA,
		IDC_SCREEN_COPY, IDH_IMP_FACTOR_SCREEN_COPY,
		IDC_CSV_OUTPUT, IDH_IMP_FACTOR_CSV_OUTPUT,
		IDC_GRAPH, IDH_IMP_FACTOR_GRAPH,
		IDC_FOLDER_TITLE, IDH_IMP_FACTOR_FOLDER_TITLE,
		IDC_NAME, IDH_IMP_FACTOR_NAME,
		IDC_PLACE, IDH_IMP_FACTOR_PLACE,
		IDC_FOLDER_COMMENT, IDH_IMP_FACTOR_FOLDER_COMMENT,
		IDC_DATE, IDH_IMP_FACTOR_DATE,
		IDC_SCALE, IDH_IMP_FACTOR_SCALE,
		IDC_SAMPLING, IDH_IMP_FACTOR_SAMPLING,
		IDC_MEASURE_TIME, IDH_IMP_FACTOR_MEASURE_TIME,
		IDC_MEASURE_NUM, IDH_IMP_FACTOR_MEASURE_NUM,
		IDC_CHANNEL, IDH_IMP_FACTOR_CHANNEL,
		IDC_BIT, IDH_IMP_FACTOR_BIT,
		IDC_TITLE, IDH_IMP_FACTOR_TITLE,
		IDC_COMMENT, IDH_IMP_FACTOR_COMMENT,
		IDC_TIME, IDH_IMP_FACTOR_TIME,
		IDC_PLAYBACK, IDH_IMP_FACTOR_PLAYBACK,
		IDC_CALC_CONDITION, IDH_IMP_FACTOR_CALC_CONDITION,
		IDC_PARAM_LIST, IDH_IMP_FACTOR_PARAM_LIST,
		IDC_1OCT, IDH_IMP_FACTOR_1OCT,
		IDC_3OCT, IDH_IMP_FACTOR_3OCT,
		IDC_START_FREQ, IDH_IMP_FACTOR_START_FREQ,
		IDC_END_FREQ, IDH_IMP_FACTOR_END_FREQ,
		IDC_AFILTER, IDH_IMP_FACTOR_AFILTER,
		IDC_SPL_REFDATA, IDH_IMP_FACTOR_SPL_REFDATA,
		IDC_SPL_REFLEVEL, IDH_IMP_FACTOR_SPL_REFLEVEL,
		IDC_TSUB_END, IDH_IMP_FACTOR_TSUB_END,
		IDC_TSUB_AUTO, IDH_IMP_FACTOR_TSUB_AUTO,
		IDC_TSUB_NOISE, IDH_IMP_FACTOR_TSUB_NOISE,
		IDC_DT1_MINTIME, IDH_IMP_FACTOR_DT1_MINTIME,
		IDC_IACC_WLEVEL, IDH_IMP_FACTOR_IACC_WLEVEL,
		IDC_PREF_SPL, IDH_IMP_FACTOR_PREF_SPL,
		IDC_PREF_TAUE, IDH_IMP_FACTOR_PREF_TAUE,
		IDC_G_REFLEVEL, IDH_IMP_FACTOR_G_REFLEVEL,
		IDC_T_CUSTOM1, IDH_IMP_FACTOR_T_CUSTOM1,
		IDC_T_CUSTOM2, IDH_IMP_FACTOR_T_CUSTOM2,
		IDC_C_CUSTOM, IDH_IMP_FACTOR_C_CUSTOM,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

BOOL CImpFactor::CalcMeasurementIR()
{
	CCalcImpDlg dlg;

	dlg.m_CalcParam = setData.CalcParam;
	dlg.m_nFolderID = m_nFolderID;
	if (dlg.DoModal() == IDCANCEL)
		return FALSE;
	setData.CalcParam = dlg.m_CalcParam;

	double fScale = 1.0;

	CString str;
	str.LoadString(IDS_CALCULATING);
	CProgressDlg progressDlg(this, str);
	progressDlg.SetRange(1);
	progressDlg.SetProgressBar(0);

	DbAcParamRec dbAcParamRec;
	memset(&m_dbAcParamRec, 0, sizeof(m_dbAcParamRec));
	dbAcParamRec.nImpulseID = m_dbImpulseRec.nImpulseID;
	if (CalcAcParam(&m_dbImpulseRec, m_hWaveData, &m_dbAcParamRec, m_dbImpulseRec.nSampling / fScale, &progressDlg))
		return FALSE;

	return TRUE;
}
