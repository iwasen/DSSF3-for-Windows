// ChasDoc.cpp : CSasDoc クラスの動作の定義を行います。
//

#include "stdafx.h"
#include "Sas.h"
#include "MainFrm.h"
#include "SasDoc.h"
#include "SasView.h"
#include "Calc.h"
#include "CalcImpDlg.h"
#include "CalcAcf.h"
#include "CalcAcfDlg.h"
#include "CalcNmsDlg.h"
#include "ProgressDlg.h"
#include "OutputFile.h"
#include "TemplateDlg.h"
#include "Regist.h"
#include "ImportExport.h"

// 計算時間測定
//#define CALC_TIME

/////////////////////////////////////////////////////////////////////////////
// CSasDoc

IMPLEMENT_DYNCREATE(CSasDoc, CDocument)

BEGIN_MESSAGE_MAP(CSasDoc, CDocument)
	ON_COMMAND(IDM_VIEW_UPDATE, OnViewUpdate)
	ON_COMMAND(IDM_PARAM_CALC, OnParamCalc)
	ON_UPDATE_COMMAND_UI(IDM_PARAM_CALC, OnUpdateParamCalc)
	ON_COMMAND(IDM_PARAM_OUTPUT, OnParamOutput)
	ON_UPDATE_COMMAND_UI(IDM_PARAM_OUTPUT, OnUpdateParamOutput)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(IDM_TEMPLATE, OnTemplate)
	ON_UPDATE_COMMAND_UI(IDM_TEMPLATE, OnUpdateTemplate)
	ON_COMMAND(IDM_PARAM_CALC2, &CSasDoc::OnParamCalc2)
	ON_UPDATE_COMMAND_UI(IDM_PARAM_CALC2, OnUpdateParamCalc)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSasDoc クラスの構築/消滅

CSasDoc::CSasDoc()
{
	m_nFolderID = -1;
	m_nType = 0;
}

BOOL CSasDoc::OnNewDocument()
{
//	if (!CDocument::OnNewDocument())
//		return FALSE;

//	SetTitle(NULL);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSasDoc コマンド

void CSasDoc::OnViewUpdate()
{
	UpdateAllViews(NULL, ID_SYSTEM);
}

void CSasDoc::OnParamCalc()
{
	ParamCalc(0);
}

void CSasDoc::OnParamCalc2()
{
	ParamCalc(1);
}

void CSasDoc::ParamCalc(int nCalcData)
{
	if (!CHECK_LICENSE1(APP_SA))
		return;

	switch (m_nType) {
	case FOLDER_TYPE_IR:
		CalcIR(nCalcData);
		break;
	case FOLDER_TYPE_ACF:
		CalcAcf(nCalcData);
		break;
	case FOLDER_TYPE_NMS:
		CalcNms(nCalcData);
		break;
	}
}

void CSasDoc::CalcIR(int nCalcData)
{
	CCalcImpDlg dlg;

	dlg.m_CalcParam = setData.CalcParam;
	dlg.m_nFolderID = m_nFolderID;
	dlg.m_iCalcData = nCalcData;
	if (dlg.DoModal() == IDCANCEL)
		return;
	setData.CalcParam = dlg.m_CalcParam;

	CDbFolder dbFolder;
	if (!dbFolder.Open())
		return;

	DbFolderRec dbFolderRec;
	if (!dbFolder.ReadRecID(m_nFolderID, &dbFolderRec))
		return;

	double fScale = (dbFolderRec.fScale != 0) ? dbFolderRec.fScale : 1.0;
	dbFolder.DBClose();

	CDbImpulse dbImpulse;
	if (!dbImpulse.Open())
		return;

	CDbAcParam dbAcParam;
	if (!dbAcParam.Open())
		return;

	CMainFrame *pMainWnd = (CMainFrame *)::AfxGetApp()->m_pMainWnd;
	CString str;
	str.LoadString(IDS_CALCULATING);
	CProgressDlg progressDlg(pMainWnd, str);

	long nRec;
	dbImpulse.DBChgIdx(dbImpulse.m_nIdxImpulseID);
	str.Format("#%d=%ld", dbImpulse.m_nIdxFolderID, m_nFolderID);
	dbImpulse.DBSelect(str, &nRec);
	progressDlg.SetRange(nRec);
#ifdef CALC_TIME
DWORD s = GetTickCount();
#endif

	CSasView* pView = (CSasView *)pMainWnd->GetListView();
	BOOL eof;
	HWAVEDATA hWaveData;
	DbImpulseRec dbImpulseRec;
	for (int i = 0; ; i++, dbImpulse.DBSkip(1)) {
		progressDlg.SetProgressBar(i);

		if (dbImpulse.DBEof(&eof) != 0 || eof)
			break;

		if (!dbImpulse.ReadRecID(-1, &dbImpulseRec))
			break;

		switch (dlg.m_iCalcData) {
		case 0:
			if (dbAcParam.SearchID(dbImpulseRec.nImpulseID))
				continue;
			break;
		case 1:
			if (!pView->CheckSelect(dbImpulseRec.nImpulseID))
				continue;
			break;
		}

		if (MMWaveReadDB(&hWaveData, dbImpulse.GetFCB(), "WAVEDATA") != 0)
			continue;

		DbAcParamRec dbAcParamRec{};
		dbAcParamRec.nImpulseID = dbImpulseRec.nImpulseID;
		if (CalcAcParam(&dbImpulseRec, hWaveData, &dbAcParamRec, dbImpulseRec.nSampling / fScale, &progressDlg)) {
			::GlobalFree(hWaveData);
			break;
		}

		::GlobalFree(hWaveData);

		if (dbAcParam.SearchID(dbAcParamRec.nImpulseID))
			dbAcParam.UpdateRec(&dbAcParamRec);
		else
			dbAcParam.StoreRec(&dbAcParamRec);
	}
#ifdef CALC_TIME
DWORD e = GetTickCount();
CString ss;
ss.Format("%d", e - s);
AfxMessageBox(ss);
#endif

	UpdateAllViews(NULL);
}

static CProgressDlg *pProgressDlg;

void CSasDoc::CalcAcf(int nCalcData)
{
	CCalcAcfDlg dlg;

	dlg.m_AcfCondition = setData.AcfCondition;
	dlg.m_nFolderID = m_nFolderID;
	dlg.m_iCalcData = nCalcData;
	if (dlg.DoModal() == IDCANCEL)
		return;
	setData.AcfCondition = dlg.m_AcfCondition;

	CDbFolder dbFolder;
	if (!dbFolder.Open())
		return;

	DbFolderRec dbFolderRec;
	if (!dbFolder.ReadRecID(m_nFolderID, &dbFolderRec))
		return;

	double fScale = (dbFolderRec.fScale != 0) ? dbFolderRec.fScale : 1.0;
	dbFolder.DBClose();

	CDbAcf dbAcf;
	if (!dbAcf.Open())
		return;

	CDbAcfFactor dbAcfFactor;
	if (!dbAcfFactor.Open())
		return;

	CMainFrame *pMainWnd = (CMainFrame *)::AfxGetApp()->m_pMainWnd;
	CString str;
	str.LoadString(IDS_CALCULATING);
	CProgressDlg progressDlg(pMainWnd, str);

	long nRec;
	dbAcf.DBChgIdx(dbAcf.m_nIdxAcfID);
	str.Format("#%d=%ld", dbAcf.m_nIdxFolderID, m_nFolderID);
	dbAcf.DBSelect(str, &nRec);
	progressDlg.SetRange(nRec);

	CSasView* pView = (CSasView *)pMainWnd->GetListView();
	int nFactor;
	AcfConditionData acfConditionData;
	AcfFactorData *pAcfFactorData;
	TaunPhinData *pTaunPhinData;
	BOOL eof;
	HWAVEDATA hWaveData;
	int find;
	DbAcfBuf dbAcfBuf;
	DbAcfRec dbAcfRec;
	DbAcfFactorBuf dbAcfFactorBuf;
	DbAcfFactorRec dbAcfFactorRec;
	for (int i = 0; ; i++, dbAcf.DBSkip(1)) {
		progressDlg.SetProgressBar(i);

		if (dbAcf.DBEof(&eof) != 0 || eof)
			break;

		if (dbAcf.DBRead(&dbAcfBuf) != 0)
			break;

		dbAcf.DBGetFieldBuf(&dbAcfBuf, &dbAcfRec);
		switch (dlg.m_iCalcData) {
		case 0:
			if (dbAcfFactor.SearchID(dbAcfRec.nAcfID))
				continue;
			break;
		case 1:
			if (!pView->CheckSelect(dbAcfRec.nAcfID))
				continue;
			break;
		}

		if (MMWaveReadDB(&hWaveData, dbAcf.GetFCB(), "WAVEDATA") != 0)
			continue;

		progressDlg.SetMessage(dbAcfRec.sTitle);

		acfConditionData.AcfCondition = setData.AcfCondition;
		acfConditionData.IAcfCondition.fIACCWLevel = 0.1;
		acfConditionData.nStartPos = 0;
		acfConditionData.nDataNum = dbAcfRec.nSampleNum;

		pProgressDlg = &progressDlg;
		if ((pAcfFactorData = CalcAcfFactor(hWaveData, &acfConditionData, dbAcfRec.nSampling / fScale, &nFactor, AbortCheck, &pTaunPhinData)) == NULL) {
			::GlobalFree(hWaveData);
			break;
		}

		::GlobalFree(hWaveData);

		dbAcfFactorRec.nAcfID = dbAcfRec.nAcfID;
		dbAcfFactorRec.nConditionSize = sizeof(AcfConditionData);
		dbAcfFactorRec.nFactorSize = sizeof(AcfFactor);
		dbAcfFactorRec.nAcfFactor = nFactor;

		dbAcfFactor.DBSetFieldBuf(&dbAcfFactorBuf, &dbAcfFactorRec);
		dbAcfFactor.DBSearch2(dbAcfFactorBuf.aAcfID, LEN_ID, &find);
		if (find == 1)
			dbAcfFactor.DBUpdate(&dbAcfFactorBuf);
		else
			dbAcfFactor.DBStore(&dbAcfFactorBuf);

		dbAcfFactor.DBWriteBinary("ACFCOND2", &acfConditionData, sizeof(AcfConditionData));
		dbAcfFactor.DBWriteBinary("ACFFACTOR", pAcfFactorData, sizeof(AcfFactorData) * nFactor);
		dbAcfFactor.DBWriteBinary(NULL, pTaunPhinData, sizeof(TaunPhinData) * nFactor);

		delete [] pAcfFactorData;
		delete [] pTaunPhinData;
	}

	UpdateAllViews(NULL);
}

void CSasDoc::CalcNms(int nCalcData)
{
	CCalcNmsDlg dlg;

	dlg.m_AcfCondition = setData.NmsCondition.AcfCondition;
	dlg.m_IdentCondition = setData.NmsCondition.IdentCondition;
	dlg.m_nFolderID = m_nFolderID;
	dlg.m_iCalcData = nCalcData;
	if (dlg.DoModal() == IDCANCEL)
		return;
	setData.NmsCondition.AcfCondition = dlg.m_AcfCondition;
	setData.NmsCondition.IdentCondition = dlg.m_IdentCondition;

	CDbFolder dbFolder;
	if (!dbFolder.Open())
		return;

	DbFolderRec dbFolderRec;
	if (!dbFolder.ReadRecID(m_nFolderID, &dbFolderRec))
		return;

	double fScale = (dbFolderRec.fScale != 0) ? dbFolderRec.fScale : 1.0;
	dbFolder.DBClose();

	CDbNms dbNms;
	if (!dbNms.Open())
		return;

	CMainFrame *pMainWnd = (CMainFrame *)::AfxGetApp()->m_pMainWnd;
	CString str;
	str.LoadString(IDS_CALCULATING);
	CProgressDlg progressDlg(pMainWnd, str);

	long nRec;
	dbNms.DBChgIdx(dbNms.m_nIdxNmsID);
	str.Format("#%d=%ld", dbNms.m_nIdxFolderID, m_nFolderID);
	dbNms.DBSelect(str, &nRec);
	progressDlg.SetRange(nRec);

	DbNmsRec dbNmsRec;
	DbNmsBuf dbNmsBuf;
	int nFactor;
	NmsConditionData nmsConditionData;
	NmsFactorData *pNmsFactorData;
	NoiseSrcData noiseSrcData2;
	BOOL eof;
	long nBytes;
	HWAVEDATA hWaveData;
	long nBinarySize;
	CSasView* pView = (CSasView *)pMainWnd->GetListView();
	for (int i = 0; ; i++, dbNms.DBSkip(1)) {
		progressDlg.SetProgressBar(i);

		if (dbNms.DBEof(&eof) != 0 || eof)
			break;

		if (!dbNms.ReadRecID(-1, &dbNmsRec, &nmsConditionData, NULL, NULL))
			break;

		switch (dlg.m_iCalcData) {
		case 0:
			if (dbNms.DBGetBinarySize("NMSFACTOR", &nBinarySize))
				continue;
			if (nBinarySize != 0)
				continue;
			break;
		case 1:
			if (!pView->CheckSelect(dbNmsRec.nNmsID))
				continue;
			break;
		}

		if (MMWaveReadDB(&hWaveData, dbNms.GetFCB(), "WAVEDATA") != 0)
			continue;

		str.Format("No. %ld", dbNmsRec.nNumber + 1);
		progressDlg.SetMessage(str);

		nmsConditionData.AcfCondition = setData.NmsCondition.AcfCondition;
		nmsConditionData.IAcfCondition = setData.NmsCondition.IAcfCondition;
		nmsConditionData.nStartPos = 0;
		nmsConditionData.nDataNum = dbNmsRec.nSampleNum;

		pProgressDlg = &progressDlg;
		if ((pNmsFactorData = CalcNmsFactor(hWaveData, &nmsConditionData, dbNmsRec.nSampling / fScale, &nFactor, AbortCheck)) == NULL) {
			::GlobalFree(hWaveData);
			break;
		}

		::GlobalFree(hWaveData);

		NoiseSrcData noiseSrcData{};
		CalcNoiseSource(pNmsFactorData, nFactor, &noiseSrcData, &setData.NmsCondition.IdentCondition);

		dbNms.DBWriteBinary("NMSCOND", &nmsConditionData, sizeof(NmsConditionData));
		dbNms.DBWriteBinary("NMSFACTOR", pNmsFactorData, sizeof(NmsFactorData) * nFactor);
		dbNms.DBReadBinary("NOISESRC", &noiseSrcData2, sizeof(NoiseSrcData), &nBytes);
		if (noiseSrcData2.aName[0] == '=')	// '='が付いているものは手動で騒音源を変更したものなので変えない
			strcpy_s(noiseSrcData.aName, noiseSrcData2.aName);
		dbNms.DBWriteBinary("NOISESRC", &noiseSrcData, sizeof(NoiseSrcData));

		dbNmsRec.nConditionSize = sizeof(NmsConditionData);
		dbNmsRec.nFactorSize = sizeof(NmsFactorData);
		dbNmsRec.nNmsFactor = nFactor;
		dbNms.DBSetFieldBuf(&dbNmsBuf, &dbNmsRec);
		dbNms.DBUpdate(&dbNmsBuf);

		delete [] pNmsFactorData;
	}

	UpdateAllViews(NULL);
}

BOOL CSasDoc::AbortCheck(int nPercent)
{
	if (pProgressDlg->CheckAbort())
		return TRUE;

	pProgressDlg->SetProgressBarSub(nPercent);

	return FALSE;
}

void CSasDoc::OnUpdateParamCalc(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nFolderID != -1);
}

void CSasDoc::OnParamOutput()
{
	CFileDialog fileDlg(FALSE, "csv", m_sFolderName,
			OFN_OVERWRITEPROMPT, "CSV File (*.csv)|*.csv|All Files (*.*)|*.*||", ::AfxGetApp()->m_pMainWnd, 0);

	if (fileDlg.DoModal() == IDCANCEL)
		return;

	switch (m_nType) {
	case FOLDER_TYPE_IR:
		::OutputCsvFileIR(m_nFolderID, fileDlg.GetPathName());
		break;
	case FOLDER_TYPE_ACF:
		::OutputCsvFileAcf(m_nFolderID, fileDlg.GetPathName());
		break;
	case FOLDER_TYPE_NMS:
		::OutputCsvFileNms(m_nFolderID, fileDlg.GetPathName());
		break;
	}
}

void CSasDoc::OnUpdateParamOutput(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nFolderID != -1);
}

void CSasDoc::OnFileSave()
{
	OnParamOutput();
}

void CSasDoc::OnUpdateFileSave(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nFolderID != -1);
}

void CSasDoc::OnTemplate()
{
	CTemplateDlg dlg;

	dlg.DoModal();
}

void CSasDoc::OnUpdateTemplate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nType == FOLDER_TYPE_NMS);
}

void CSasDoc::SelectSystem()
{
	m_nType = 0;
	m_nFolderID = -1;
	UpdateAllViews(NULL);
}

void CSasDoc::SelectDB(char type)
{
	m_nType = type;
	m_nFolderID = -1;
	UpdateAllViews(NULL);
}

void CSasDoc::SelectFolder(int folderID)
{
	CDbFolder dbFolder;
	if (!dbFolder.Open())
		return;

	DbFolderRec dbFolderRec;
	if (!dbFolder.ReadRecID(folderID, &dbFolderRec))
		return;

	m_nFolderID = folderID;
	m_nType = dbFolderRec.sType[0];
	m_sFolderName = dbFolderRec.sTitle;
	UpdateAllViews(NULL);

	if (setData.CalcParam.nSplRefData != SPL_MAXREF && setData.CalcParam.nSplRefData != SPL_ABSOLUTE)
		setData.CalcParam.nSplRefData = SPL_MAXREF;
}

void CSasDoc::DeleteItem(long nItemID)
{
	switch (nItemID & ID_KIND) {
	case ID_FOLDER:
		DeleteFolder(nItemID & ID_VALUE);
		break;
	case ID_DATA:
		DeleteData(nItemID & ID_VALUE);
		break;
	}

	UpdateAllViews(NULL, nItemID, PHINT_DELETE);
}

void CSasDoc::DeleteFolder(long nFolderID)
{
	::DeleteFolder(nFolderID);
}

void CSasDoc::DeleteData(long nDataID)
{
	switch (m_nType) {
	case FOLDER_TYPE_IR:
		::DeleteImpulse(nDataID);
		break;
	case FOLDER_TYPE_ACF:
		::DeleteAcf(nDataID);
		break;
	case FOLDER_TYPE_NMS:
		::DeleteNms(nDataID);
		break;
	}
}

void CSasDoc::ChangeTitle(long nItemID, LPCTSTR pTitle)
{
	long nID = nItemID & ID_VALUE;
	BOOL bOK = FALSE;

	switch (nItemID & ID_KIND) {
	case ID_FOLDER:
		{
			CDbFolder oDbFolder;
			if (oDbFolder.Open()) {
				DbFolderRec oDbFolderRec;
				if (oDbFolder.ReadRecID(nID, &oDbFolderRec)) {
					if (oDbFolderRec.sTitle != pTitle) {
						oDbFolderRec.sTitle = pTitle;
						if (oDbFolder.UpdateRec(&oDbFolderRec))
							bOK = TRUE;
					}
				}
			}
		}
		break;
	case ID_DATA:
		switch (m_nType) {
		case FOLDER_TYPE_IR:
			{
				CDbImpulse oDbImpulse;
				if (oDbImpulse.Open()) {
					DbImpulseRec oDbImpulseRec;
					if (oDbImpulse.ReadRecID(nID, &oDbImpulseRec)) {
						if (oDbImpulseRec.sTitle != pTitle) {
							oDbImpulseRec.sTitle = pTitle;
							if (oDbImpulse.UpdateRec(&oDbImpulseRec))
								bOK = TRUE;
						}
					}
				}
			}
			break;
		case FOLDER_TYPE_ACF:
			{
				CDbAcf oDbAcf;
				if (oDbAcf.Open()) {
					DbAcfRec oDbAcfRec;
					if (oDbAcf.ReadRecID(nID, &oDbAcfRec)) {
						if (oDbAcfRec.sTitle != pTitle) {
							oDbAcfRec.sTitle = pTitle;
							if (oDbAcf.UpdateRec(&oDbAcfRec))
								bOK = TRUE;
						}
					}
				}
			}
			break;
		}
		break;
	}

	if (bOK)
		UpdateAllViews(NULL, nItemID, (CObject *)pTitle);
}

void CSasDoc::Export(long *pIDs, int nItem, LPCTSTR pItemName) const
{
	CFileDialog fileDlg(FALSE, "dssf", pItemName,
			OFN_OVERWRITEPROMPT, "DSSF3 File (*.dssf)|*.dssf|All Files (*.*)|*.*||", ::AfxGetApp()->m_pMainWnd, 0);

	if (fileDlg.DoModal() == IDCANCEL)
		return;

	CWaitCursor oWaitCursor;
	CImportExport oImportExport;
	oImportExport.Export(fileDlg.GetPathName(), pIDs, nItem, m_nType);
}

void CSasDoc::Import(long nID)
{
	CFileDialog fileDlg(TRUE, "dssf", "",
			0, "DSSF3 File (*.dssf)|*.dssf|All Files (*.*)|*.*||", ::AfxGetApp()->m_pMainWnd, 0);

	if (fileDlg.DoModal() == IDCANCEL)
		return;

	CWaitCursor oWaitCursor;
	CImportExport oImportExport;
	long nImportID = oImportExport.Import(fileDlg.GetPathName(), nID);
	UpdateAllViews(NULL, nImportID);
}
