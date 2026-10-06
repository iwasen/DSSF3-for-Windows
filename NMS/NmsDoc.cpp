// NmsDoc.cpp : CNmsDoc クラスの動作の定義を行います。
//

#include "stdafx.h"
#include "Nms.h"
#include "NmsDoc.h"
#include "MeasureSetDlg.h"
#include "SplWnd.h"
#include "PeakLevelWnd.h"
#include "NmsFactor.h"
#include "InputDeviceDlg.h"
#include "SaveDlg.h"
#include "CalcAcf.h"
#include "TemplateDlg.h"
#include "SetData.h"
#include "ManualMode.h"
#include "WaveFile.h"
#include "Regist.h"

/////////////////////////////////////////////////////////////////////////////
// CNmsDoc

IMPLEMENT_DYNCREATE(CNmsDoc, CDocument)

BEGIN_MESSAGE_MAP(CNmsDoc, CDocument)
	ON_COMMAND(IDM_MEASURE_SET, OnMeasureSet)
	ON_UPDATE_COMMAND_UI(IDM_MEASURE_SET, OnUpdateMeasureSet)
	ON_COMMAND(IDM_MEASURE_START, OnMeasureStart)
	ON_UPDATE_COMMAND_UI(IDM_MEASURE_START, OnUpdateMeasureStart)
	ON_COMMAND(IDM_MEASURE_STOP, OnMeasureStop)
	ON_UPDATE_COMMAND_UI(IDM_MEASURE_STOP, OnUpdateMeasureStop)
	ON_COMMAND(IDM_INPUT_LEVEL, OnInputLevel)
	ON_UPDATE_COMMAND_UI(IDM_INPUT_LEVEL, OnUpdateInputLevel)
	ON_COMMAND(IDM_INPUT_DEVICE, OnInputDevice)
	ON_UPDATE_COMMAND_UI(IDM_INPUT_DEVICE, OnUpdateInputDevice)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(IDM_PEAK_LEVEL, OnPeakLevel)
	ON_UPDATE_COMMAND_UI(IDM_PEAK_LEVEL, OnUpdatePeakLevel)
	ON_COMMAND(IDM_CLEAR_DATA, OnClearData)
	ON_UPDATE_COMMAND_UI(IDM_CLEAR_DATA, OnUpdateClearData)
	ON_COMMAND(IDM_TEMPLATE, OnTemplate)
	ON_UPDATE_COMMAND_UI(IDM_TEMPLATE, OnUpdateTemplate)
	ON_COMMAND(IDM_RECALC, OnRecalc)
	ON_UPDATE_COMMAND_UI(IDM_RECALC, OnUpdateRecalc)
	ON_COMMAND(IDM_AUTO_LEARNING, OnAutoLearning)
	ON_UPDATE_COMMAND_UI(IDM_AUTO_LEARNING, OnUpdateAutoLearning)
	ON_COMMAND(IDM_MANUAL_LEARNING, OnManualLearning)
	ON_UPDATE_COMMAND_UI(IDM_MANUAL_LEARNING, OnUpdateManualLearning)
	ON_COMMAND(IDM_MEASURE_MANUAL, OnMeasureManual)
	ON_UPDATE_COMMAND_UI(IDM_MEASURE_MANUAL, OnUpdateMeasureManual)
	ON_COMMAND(IDM_AUTO_CALC, OnAutoCalc)
	ON_UPDATE_COMMAND_UI(IDM_AUTO_CALC, OnUpdateAutoCalc)
	ON_COMMAND(IDM_READ_WAVE, OnReadWave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNmsDoc クラスの構築/消滅

BOOL CNmsDoc::m_bAbortCalc;

CNmsDoc::CNmsDoc()
{
	m_nMeasurementMode = MEASUREMENT_STOP;
	m_bAbortCalc = FALSE;
	m_oMeasurement.m_pDoc = this;
}

CNmsDoc::~CNmsDoc()
{
	while (!m_cItemList.IsEmpty()) {
		CNmsItem *pNmsItem = (CNmsItem *)m_cItemList.RemoveHead();
		delete pNmsItem;
	}
}

BOOL CNmsDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	CString str;
	str.LoadString(IDS_STOPPED);
	SetTitle(str);

	if (g_oSetData.SplWindow.bShow)
		OnInputLevel();

	if (g_oSetData.PeakLevelWindow.bShow)
		OnPeakLevel();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CNmsDoc コマンド

void CNmsDoc::OnMeasureSet()
{
	CMeasureSetDlg dlg;

	dlg.DoModal();
}

void CNmsDoc::OnUpdateMeasureSet(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nMeasurementMode == MEASUREMENT_STOP);
}

void CNmsDoc::OnMeasureStart()
{

	SetMicCalData();

	if (m_oMeasurement.Start(MEASUREMENT_AUTO)) {
		m_nMeasurementMode = MEASUREMENT_AUTO;

		CString str;
		str.LoadString(IDS_AUTO_MEASURING);
		SetTitle(str);
	}
}

void CNmsDoc::OnUpdateMeasureStart(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nMeasurementMode == MEASUREMENT_STOP);
}

void CNmsDoc::OnMeasureStop()
{
	if (m_nMeasurementMode != MEASUREMENT_STOP) {
		m_oMeasurement.Stop();
		m_nMeasurementMode = MEASUREMENT_STOP;

		CString str;
		str.LoadString(IDS_STOPPED);
		SetTitle(str);
	}
}

void CNmsDoc::OnUpdateMeasureStop(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nMeasurementMode == MEASUREMENT_AUTO);
}

void CNmsDoc::OnInputLevel()
{
	if (g_pSplWnd == NULL)
		g_pSplWnd = new CSplWnd;
	else {
		g_pSplWnd->DestroyWindow();
	}
}

void CNmsDoc::OnUpdateInputLevel(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(g_pSplWnd != NULL);
}

void CNmsDoc::AddNmsItem(int nIndex, double fDataTime, CTime *pTime)
{
	CNmsItem *pNmsItem = new CNmsItem;

	pNmsItem->m_cTime = (pTime != NULL) ? *pTime : CTime::GetCurrentTime();
	pNmsItem->m_nStatus = STATUS_WAIT_CALC;
	pNmsItem->m_nIndex = nIndex;
	pNmsItem->m_fDataTime = fDataTime;

	m_cItemList.AddTail(pNmsItem);

	UpdateAllViews(NULL, VIEWHINT_ADDITEM, pNmsItem);
}

void CNmsDoc::ChangeStatus(int nIndex, int nStatus)
{
	POSITION pos;
	if ((pos = m_cItemList.FindIndex(nIndex)) == NULL)
		return;

	CNmsItem *pNmsItem = (CNmsItem *)m_cItemList.GetAt(pos);
	pNmsItem->m_nStatus = nStatus;

	UpdateAllViews(NULL, VIEWHINT_UPDATEITEM, pNmsItem);
}

BOOL CNmsDoc::CanCloseFrame(CFrameWnd* /*pFrame*/)
{
	if (m_nMeasurementMode != MEASUREMENT_STOP) {
		if (::AfxMessageBox(IDS_MSG_MEAS_EXIT, MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
			return TRUE;
		else
			return FALSE;
	}

	if (m_oMeasurement.InCalculating()) {
		if (::AfxMessageBox(IDS_MSG_CALC_EXIT, MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
			return TRUE;
		else
			return FALSE;
	}

	BOOL bNoSave = FALSE;
	POSITION pos = m_cItemList.GetHeadPosition();
	while (pos != NULL) {
		CNmsItem *pNmsItem = (CNmsItem *)m_cItemList.GetNext(pos);
		if (!pNmsItem->m_bSaved) {
			bNoSave = TRUE;
			break;
		}
	}

	if (bNoSave) {
		if (::AfxMessageBox(IDS_MSG_NOSAVE_EXIT, MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
			return TRUE;
		else
			return FALSE;
	}

	return TRUE;
}

void CNmsDoc::SetMicCalData()
{
	m_oMeasurement.m_oMicCalData.fInputSens = 0;
	m_oMeasurement.m_oMicCalData.nFreqData = 0;

	if (g_oSetData.Measurement.nMicCalID != -1) {
		CDbMicCal dbMicCal;

		if (!dbMicCal.Open())
			return;

		if (!dbMicCal.ReadRecID(g_oSetData.Measurement.nMicCalID, &m_oMeasurement.m_oMicCalData))
			return;

//		m_cRtaMessage = m_Rta.m_oMicCalData.micInfo;

//		CPcAudioApp *pWinApp = (CPcAudioApp *)::AfxGetApp();
//		CPcAudioDlg *pDlg = (CPcAudioDlg *)pWinApp->m_pWndPcAudio;
		SetInputDevice(g_oSetData.InputDevice.nInputDevice, m_oMeasurement.m_oMicCalData.nInputSel, m_oMeasurement.m_oMicCalData.nInputVol);
	}
}

void CNmsDoc::OnInputDevice()
{
	CInputDeviceDlg dlg;

	dlg.m_MicCalID = g_oSetData.Measurement.nMicCalID;
	if (dlg.DoModal() == IDOK) {
		g_oSetData.Measurement.nMicCalID = dlg.m_MicCalID;
		SetMicCalData();
//		SetFFT();
//		m_Rta.ChangeDispMode();
	}
}

void CNmsDoc::OnUpdateInputDevice(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nMeasurementMode == MEASUREMENT_STOP);
}

// 測定データをDBに保存
void CNmsDoc::OnFileSave()
{
	if (!CHECK_LICENSE1(APP_EA))
		return;

	CSaveDlg dlg;

	dlg.m_pItemList = &m_cItemList;
	if (dlg.DoModal() == IDOK)
		DeleteAllItems();
}

void CNmsDoc::OnUpdateFileSave(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nMeasurementMode == MEASUREMENT_STOP && !m_oMeasurement.InCalculating() && !m_cItemList.IsEmpty());
}

BOOL CNmsDoc::SetCondition(int nCalcCounter)
{
	m_bAbortCalc = FALSE;

	POSITION pos;
	if ((pos = m_cItemList.FindIndex(nCalcCounter)) == NULL)
		return FALSE;

	DWORD dwSize;
	PCMWAVEFORMAT wf;
	CWaveFile waveFile;
	CString tempFileName;
	tempFileName.Format("%s\\%d.WAV", (LPCSTR)g_sTempDir, nCalcCounter);
	if (!waveFile.Open(tempFileName, &wf, &dwSize))
		return FALSE;

	const int nSampleNum = dwSize / (wf.wf.nChannels * wf.wBitsPerSample / 8);

	NmsConditionData nmsConditionData{
		.nStartPos = 0,
		.nDataNum = nSampleNum,
		.AcfCondition = g_oSetData.Measurement.AcfCondition,
		.IAcfCondition = g_oSetData.Measurement.IAcfCondition,
		.NmsMicCal{
			.fInputSens = m_oMeasurement.m_oMicCalData.fInputSens,
			.nFreqData = m_oMeasurement.m_oMicCalData.nFreqData
		}
	};
	for (int i = 0; i < MICCAL_NFREQ; i++)
		nmsConditionData.NmsMicCal.freq[i] = m_oMeasurement.m_oMicCalData.aFreq[i];

	CNmsItem *pNmsItem = (CNmsItem *)m_cItemList.GetAt(pos);
	pNmsItem->m_NmsConditionData = nmsConditionData;

	return TRUE;
}

BOOL CNmsDoc::CalcFactor(int nCalcCounter)
{
	m_bAbortCalc = FALSE;

	POSITION pos;
	if ((pos = m_cItemList.FindIndex(nCalcCounter)) == NULL)
		return FALSE;

	CNmsItem *pNmsItem = (CNmsItem *)m_cItemList.GetAt(pos);

	HWAVEDATA hWaveData;
	CString tempFileName;
	tempFileName.Format("%s\\%d.WAV", (LPCSTR)g_sTempDir, nCalcCounter);
	if (MMWaveReadFile(&hWaveData, (LPSTR)(LPCSTR)tempFileName) != 0)
		return FALSE;

	PWAVEDATA pWaveData = (PWAVEDATA)::GlobalLock(hWaveData);
	if (pWaveData == NULL)
		return FALSE;
	const int nSamplingRate = pWaveData->nSamplesPerSec;
	::GlobalUnlock(hWaveData);

	pNmsItem->m_NmsConditionData.AcfCondition = g_oSetData.Measurement.AcfCondition;
	pNmsItem->m_NmsConditionData.IAcfCondition = g_oSetData.Measurement.IAcfCondition;

	int nNmsFactor;
	NmsFactorData *pNmsFactorData;
	if ((pNmsFactorData = CalcNmsFactor(hWaveData, &pNmsItem->m_NmsConditionData, nSamplingRate, &nNmsFactor, AbortCheck)) == NULL) {
		::GlobalFree(hWaveData);
		return FALSE;
	}

	::GlobalFree(hWaveData);

	pNmsItem->m_nNmsFactorData = nNmsFactor;
	pNmsItem->m_pNmsFactorData = pNmsFactorData;

	pNmsItem->m_nNoiseTmpID = CalcNoiseSource(pNmsFactorData, nNmsFactor, &pNmsItem->m_NoiseSrcData, &g_oSetData.Measurement.IdentCondition);

	if (g_oSetData.Measurement.bAutoLearning)
		pNmsItem->LearningNoiseSrc();

	return TRUE;
}

void CNmsDoc::DeleteAllItems()
{
	CNmsItem *pNmsItem;
	CString tempFileName;

	while (!m_cItemList.IsEmpty()) {
		pNmsItem = (CNmsItem *)m_cItemList.RemoveHead();

		tempFileName.Format("%s\\%d.WAV", (LPCSTR)g_sTempDir, pNmsItem->m_nIndex);
		::DeleteFile(tempFileName);

		delete pNmsItem;
	}

	m_oMeasurement.ResetCounter();

	UpdateAllViews(NULL, VIEWHINT_CLEARITEM);
}

void CNmsDoc::OnPeakLevel()
{
	if (g_pPeakLevelWnd == NULL)
		g_pPeakLevelWnd = new CPeakLevelWnd;
	else {
		g_pPeakLevelWnd->DestroyWindow();
	}
}

void CNmsDoc::OnUpdatePeakLevel(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(g_pPeakLevelWnd != NULL);
}

void CNmsDoc::OnClearData()
{
	if (::AfxMessageBox(IDS_MSG_CLEAR_DATA, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
		return;

	m_bAbortCalc = TRUE;

	DeleteAllItems();
}

void CNmsDoc::OnUpdateClearData(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_cItemList.IsEmpty());
}

BOOL CNmsDoc::AbortCheck(int /*nPercent*/)
{
	return m_bAbortCalc;
}

void CNmsDoc::OnTemplate()
{
	CTemplateDlg dlg;

	dlg.DoModal();
}

void CNmsDoc::OnUpdateTemplate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nMeasurementMode == MEASUREMENT_STOP);
}

// 再計算処理
void CNmsDoc::OnRecalc()
{
	m_bAbortCalc = TRUE;

	POSITION pos = m_cItemList.GetHeadPosition();
	while (pos != NULL) {
		CNmsItem *pNmsItem = (CNmsItem *)m_cItemList.GetNext(pos);
		pNmsItem->m_nStatus = STATUS_WAIT_CALC;
		if (pNmsItem->m_pNmsFactorData != NULL) {
			delete [] pNmsItem->m_pNmsFactorData;
			pNmsItem->m_pNmsFactorData = NULL;
		}
		pNmsItem->m_NoiseSrcData = {};
		UpdateAllViews(NULL, VIEWHINT_UPDATEITEM, pNmsItem);
	}

	m_oMeasurement.Recalclation();
}

void CNmsDoc::OnUpdateRecalc(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nMeasurementMode == MEASUREMENT_STOP && !m_cItemList.IsEmpty());
}

void CNmsDoc::OnAutoLearning()
{
	g_oSetData.Measurement.bAutoLearning = !g_oSetData.Measurement.bAutoLearning;
}

void CNmsDoc::OnUpdateAutoLearning(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(g_oSetData.Measurement.bAutoLearning);
}

void CNmsDoc::OnManualLearning()
{
	POSITION pos = m_cItemList.GetHeadPosition();
	while (pos != NULL) {
		CNmsItem *pNmsItem = (CNmsItem *)m_cItemList.GetNext(pos);
		if (pNmsItem->m_nStatus == STATUS_FINISH_CALC) {
			pNmsItem->LearningNoiseSrc();
			UpdateAllViews(NULL, VIEWHINT_UPDATEITEM, pNmsItem);
		}
	}
}

void CNmsDoc::OnUpdateManualLearning(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!g_oSetData.Measurement.bAutoLearning);
}

void CNmsDoc::OnMeasureManual()
{
	if (g_pManualWnd == NULL) {
		SetMicCalData();
		if (m_oMeasurement.Start(MEASUREMENT_MANUAL)) {
			g_pManualWnd = new CManualMode;
			g_pManualWnd->m_pDoc = this;

			m_nMeasurementMode = MEASUREMENT_MANUAL;

			CString str;
			str.LoadString(IDS_MANUAL_MEASURING);
			SetTitle(str);
		}
	} else {
		g_pManualWnd->DestroyWindow();
	}
}

void CNmsDoc::OnUpdateMeasureManual(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nMeasurementMode != MEASUREMENT_AUTO);
	pCmdUI->SetCheck(g_pManualWnd != NULL);
}

void CNmsDoc::StopManualMode()
{
	OnMeasureStop();
}

void CNmsDoc::OnAutoCalc()
{
	g_oSetData.Calc.bAutoCalc = !g_oSetData.Calc.bAutoCalc;
}

void CNmsDoc::OnUpdateAutoCalc(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(g_oSetData.Calc.bAutoCalc);
}

void CNmsDoc::OnReadWave()
{
	if (!CHECK_LICENSE1(APP_EA))
		return;

	CFileDialog fileDlg(TRUE, "wav", "*.wav",
			OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
			"Wave File (*.wav)|*.wav|All Files (*.*)|*.*||", NULL, 0);

	if (fileDlg.DoModal() == IDOK)
		m_oMeasurement.WaveFileMeasuring(fileDlg.GetPathName());
}
