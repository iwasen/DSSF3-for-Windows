// FileDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "FileDlg.h"
#include "Help\ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CFolderDlg ダイアログ

CFolderDlg::CFolderDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(CFolderDlg::IDD, pParent)
{
}

void CFolderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FOLDER_LIST, m_cFolderList);
	DDX_Control(pDX, IDC_COMMENT, m_cComment);
	DDX_Control(pDX, IDC_DATE, m_cDate);
	DDX_Control(pDX, IDC_NAME, m_cName);
	DDX_Control(pDX, IDC_PLACE, m_cPlace);
	DDX_Control(pDX, IDC_TITLE, m_cTitle);
	DDX_Control(pDX, IDC_SCALE, m_cScale);
}

BEGIN_MESSAGE_MAP(CFolderDlg, CDialogAF)
	ON_BN_CLICKED(IDC_REGIST, OnRegist)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_LBN_SELCHANGE(IDC_FOLDER_LIST, OnSelchangeFolderList)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFolderDlg メッセージ ハンドラ

BOOL CFolderDlg::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	SYSTEMTIME st;
	::GetLocalTime(&st);
	m_cDate.Format("%04d/%02d/%02d", st.wYear, st.wMonth, st.wDay);
	m_cScale = 1.0;

	m_cFolderList.SetFolderList(m_nFolderType);

	m_cTitle.SetMaxLength(LEN_TITLE);
	m_cName.SetMaxLength(LEN_NAME);
	m_cPlace.SetMaxLength(LEN_PLACE);
	m_cComment.SetMaxLength(LEN_COMMENT);
	m_cScale.SetValidChar(VC_NUM | VC_POINT);

	return TRUE;
}

// フォルダリスト選択
void CFolderDlg::OnSelchangeFolderList()
{
	DbFolderRec *pDbFolderRec;

	if ((pDbFolderRec = m_cFolderList.GetSelFolderRec()) == NULL)
		return;

	m_cName = pDbFolderRec->sName;
	m_cPlace = pDbFolderRec->sPlace;
	m_cDate = ::FormatTime(pDbFolderRec->sDate);
	m_cTitle = pDbFolderRec->sTitle;
	m_cComment = pDbFolderRec->sComment;
	m_cScale = pDbFolderRec->fScale;
	if (pDbFolderRec->fScale == 0)
		m_cScale = 1.0;
}

// フォルダ登録ボタン
void CFolderDlg::OnRegist()
{
	if (m_cTitle.IsEmpty()) {
		::AfxMessageBox(IDS_ERR_TITLE, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	if (::AfxMessageBox(IDS_MSG_MAKEFOLDER, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
		return;

	CDbFolder dbFolder;
	if (!dbFolder.Open())
		return;

	DbFolderRec dbFolderRec;
	if (!dbFolder.GetNewID(&dbFolderRec.nFolderID))
		return;

	dbFolderRec.sType = (char)m_nFolderType;
	dbFolderRec.sName = m_cName;
	dbFolderRec.sPlace = m_cPlace;
	CString sDate = m_cDate;
	dbFolderRec.sDate = sDate.Left(4) + sDate.Mid(5, 2) + sDate.Mid(8, 2);
	dbFolderRec.sTitle = m_cTitle;
	dbFolderRec.sComment = m_cComment;
	dbFolderRec.fScale = m_cScale;

	if (!dbFolder.StoreRec(&dbFolderRec))
		return;

	m_cFolderList.SetFolderList(m_nFolderType);
}

// フォルダ変更ボタン
void CFolderDlg::OnChange()
{
	DbFolderRec *pDbFolderRec;
	if ((pDbFolderRec = m_cFolderList.GetSelFolderRec()) == NULL) {
		::AfxMessageBox(IDS_MSG_SELECTFOLDER);
		return;
	}

	if (::AfxMessageBox(IDS_MSG_CHANGEFOLDER, MB_OKCANCEL + MB_ICONQUESTION) == IDCANCEL)
		return;

	CDbFolder dbFolder;
	if (!dbFolder.Open())
		return;

	pDbFolderRec->sName = m_cName;
	pDbFolderRec->sPlace = m_cPlace;
	CString sDate = m_cDate;
	pDbFolderRec->sDate = sDate.Left(4) + sDate.Mid(5, 2) + sDate.Mid(8, 2);
	pDbFolderRec->sTitle = m_cTitle;
	pDbFolderRec->sComment = m_cComment;
	pDbFolderRec->fScale = m_cScale;

	if (!dbFolder.UpdateRec(pDbFolderRec))
		return;

	m_cFolderList.SetFolderList(m_nFolderType);
}

// フォルダ削除ボタン
void CFolderDlg::OnDelete()
{
	DbFolderRec *pDbFolderRec;

	if ((pDbFolderRec = m_cFolderList.GetSelFolderRec()) == NULL) {
		::AfxMessageBox(IDS_MSG_SELECTFOLDER);
		return;
	}

	if (::AfxMessageBox(IDS_MSG_DELETEFOLDER, MB_OKCANCEL + MB_ICONQUESTION) == IDCANCEL)
		return;

	DeleteFolder(pDbFolderRec->nFolderID);

	m_cFolderList.SetFolderList(m_nFolderType);
}

BOOL CFolderDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDC_FOLDER_LIST, IDH_IMP_FOLDER_FOLDER_LIST,
		IDC_REGIST, IDH_IMP_FOLDER_REGIST,
		IDC_CHANGE, IDH_IMP_FOLDER_CHANGE,
		IDC_DELETE, IDH_IMP_FOLDER_DELETE,
		IDCANCEL, IDH_IMP_FOLDER_CLOSE,
		IDC_TITLE, IDH_IMP_FOLDER_TITLE,
		IDC_NAME, IDH_IMP_FOLDER_NAME,
		IDC_PLACE, IDH_IMP_FOLDER_PLACE,
		IDC_COMMENT, IDH_IMP_FOLDER_COMMENT,
		IDC_DATE, IDH_IMP_FOLDER_DATE,
		IDC_SCALE, IDH_IMP_FOLDER_SCALE,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSaveIR ダイアログ

CSaveIR::CSaveIR(CWnd* pParent /*=NULL*/)
	: CDialogAF(CSaveIR::IDD, pParent)
{
	m_iBit = 0;
	m_iSampling = 0;
	m_iMeasureNum = 0;
}

void CSaveIR::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATA_LIST, m_cDataList);
	DDX_Control(pDX, IDC_FOLDER_LIST, m_cFolderList);
	DDX_Control(pDX, IDC_TITLE, m_cTitle);
	DDX_Control(pDX, IDC_COMMENT, m_cComment);
	DDX_Text(pDX, IDC_BIT, m_iBit);
	DDX_Text(pDX, IDC_SAMPLING, m_iSampling);
	DDX_Text(pDX, IDC_CHANNEL, m_sChannel);
	DDX_Text(pDX, IDC_MEASURE_NUM, m_iMeasureNum);
	DDX_Text(pDX, IDC_MEASURE_TIME, m_sMeasureTime);
	DDX_Text(pDX, IDC_TIME, m_sTime);
}

BEGIN_MESSAGE_MAP(CSaveIR, CDialogAF)
	ON_BN_CLICKED(IDC_FOLDER, OnFolder)
	ON_LBN_SELCHANGE(IDC_FOLDER_LIST, OnSelchangeFolderList)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaveIR メッセージ ハンドラ

BOOL CSaveIR::OnInitDialog()
{
	m_sMeasureTime.Format("%.3f", (1 << m_nStage) / (double)m_iSampling);
	m_sChannel = (m_nChannel == 1) ? "Mono" : "Stereo";
	m_sTime = FormatTime(m_Time);

	CDialogAF::OnInitDialog();

	m_cFolderList.SetFolderList(FOLDER_TYPE_IR);

	m_cTitle.SetMaxLength(LEN_TITLE);
	m_cComment.SetMaxLength(LEN_COMMENT);

	return TRUE;
}

void CSaveIR::OnFolder()
{
	CFolderDlg dlg(this);

	dlg.m_nFolderType = FOLDER_TYPE_IR;
	dlg.DoModal();

	m_cFolderList.SetFolderList(FOLDER_TYPE_IR);
}

void CSaveIR::OnOK()
{
	DbFolderRec *pDbFolderRec;
	if ((pDbFolderRec = m_cFolderList.GetSelFolderRec()) == NULL) {
		::AfxMessageBox(IDS_MSG_SELECTFOLDER);
		return;
	}

	UpdateData(TRUE);
	if (m_cTitle.IsEmpty()) {
		::AfxMessageBox(IDS_ERR_TITLE, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	if (::AfxMessageBox(IDS_MSG_SAVEDATA, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
		return;

	CDbImpulse dbImpulse;
	if (!dbImpulse.Open())
		return;

	DbImpulseRec dbImpulseRec;
	if (!dbImpulse.GetNewID(&dbImpulseRec.nImpulseID))
		return;

	dbImpulseRec.nFolderID = pDbFolderRec->nFolderID;
	dbImpulseRec.nSampling = m_iSampling;
	dbImpulseRec.nMeasTime = m_nStage;
	dbImpulseRec.nMeasNum = m_iMeasureNum;
	dbImpulseRec.nChannel = m_nChannel;
	dbImpulseRec.nBit = m_iBit | m_nMethod;
	dbImpulseRec.sTime = m_Time;
	dbImpulseRec.sTitle= m_cTitle;
	dbImpulseRec.sComment = m_cComment;
	dbImpulseRec.nUseFilter = m_bUseFilter;
	dbImpulseRec.nIFilterID = m_nIFilterID;
	dbImpulseRec.fTemperature = 0.0;
	dbImpulseRec.fHumidity = 0.0;
	dbImpulseRec.nMultiplier = m_nMultiplier;
	dbImpulseRec.bImpulseData = m_bImpulseData;

	if (!dbImpulse.StoreRec(&dbImpulseRec))
		return;

	if (m_hWaveData != NULL) {
		if (MMWaveWriteDB(m_hWaveData, dbImpulse.GetFCB(), "WAVEDATA", MMTYPE_WAV) != 0) {
			::AfxMessageBox(IDS_ERR_WRITEDATA, MB_OK | MB_ICONEXCLAMATION);
			return;
		}
	}

	CDialogAF::OnOK();
}

void CSaveIR::OnSelchangeFolderList()
{
	DbFolderRec *pDbFolderRec;

	if ((pDbFolderRec = m_cFolderList.GetSelFolderRec()) != NULL)
		m_cDataList.SetDataList(pDbFolderRec->nFolderID);
}

BOOL CSaveIR::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDC_FOLDER_LIST, IDH_IMP_SAVE_FOLDER_LIST,
		IDC_DATA_LIST, IDH_IMP_SAVE_DATA_LIST,
		IDOK, IDH_IMP_SAVE_SAVE,
		IDC_FOLDER, IDH_IMP_SAVE_FOLDER,
		IDCANCEL, IDH_IMP_SAVE_CANCEL,
		IDC_SAMPLING, IDH_IMP_SAVE_SAMPLING,
		IDC_MEASURE_TIME, IDH_IMP_SAVE_MEASURE_TIME,
		IDC_MEASURE_NUM, IDH_IMP_SAVE_MEASURE_NUM,
		IDC_CHANNEL, IDH_IMP_SAVE_CHANNEL,
		IDC_BIT, IDH_IMP_SAVE_BIT,
		IDC_TITLE, IDH_IMP_SAVE_TITLE,
		IDC_COMMENT, IDH_IMP_SAVE_COMMENT,
		IDC_TIME, IDH_IMP_SAVE_TIME,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CLoadIR ダイアログ

CLoadIR::CLoadIR(CWnd* pParent /*=NULL*/)
	: CDialogAF(CLoadIR::IDD, pParent)
{
}

void CLoadIR::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATA_LIST, m_cDataList);
	DDX_Control(pDX, IDC_FOLER_TITLE, m_cFolderTitle);
	DDX_Control(pDX, IDC_FOLDER_PLACE, m_cFolderPlace);
	DDX_Control(pDX, IDC_FOLDER_NAME, m_cFolderName);
	DDX_Control(pDX, IDC_FOLDER_DATE, m_cFolderDate);
	DDX_Control(pDX, IDC_FOLDER_COMMENT, m_cFolderComment);
	DDX_Control(pDX, IDC_DATA_TITLE, m_cDataTitle);
	DDX_Control(pDX, IDC_DATA_TIME, m_cDataTime);
	DDX_Control(pDX, IDC_DATA_COMMENT, m_cDataComment);
	DDX_Control(pDX, IDC_FOLDER_LIST, m_cFolderList);
}

BEGIN_MESSAGE_MAP(CLoadIR, CDialogAF)
	ON_LBN_SELCHANGE(IDC_FOLDER_LIST, OnSelchangeFolderList)
	ON_LBN_SELCHANGE(IDC_DATA_LIST, OnSelchangeDataList)
	ON_LBN_DBLCLK(IDC_DATA_LIST, OnDblclkDataList)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_FOLDER, OnFolder)
	ON_BN_CLICKED(IDC_WAVE_FILE, OnWaveFile)
	ON_BN_CLICKED(IDC_IMPORT_DATA, OnImportData)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoadIR メッセージ ハンドラ

BOOL CLoadIR::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	m_cFolderList.SetFolderList(FOLDER_TYPE_IR);

	return TRUE;
}

void CLoadIR::OnSelchangeFolderList()
{
	DbFolderRec *pDbFolderRec;

	if ((pDbFolderRec = m_cFolderList.GetSelFolderRec()) == NULL)
		return;

	m_nFolderID = pDbFolderRec->nFolderID;
	m_cFolderTitle = pDbFolderRec->sTitle;
	m_cFolderName = pDbFolderRec->sName;
	m_cFolderPlace = pDbFolderRec->sPlace;
	m_cFolderComment = pDbFolderRec->sComment;
	m_cFolderDate = ::FormatTime(pDbFolderRec->sDate);

	m_cDataList.SetDataList(m_nFolderID);
}

void CLoadIR::OnSelchangeDataList()
{
	DbImpulseRec *pDbImpulseRec;

	if ((pDbImpulseRec = m_cDataList.GetSelImpulseRec()) == NULL)
		return;

	m_cDataTitle = pDbImpulseRec->sTitle;
	m_cDataComment = pDbImpulseRec->sComment;
	m_cDataTime = ::FormatTime(pDbImpulseRec->sTime);
}

void CLoadIR::OnOK()
{
	DbImpulseRec *pDbImpulseRec;

	if ((pDbImpulseRec = m_cDataList.GetSelImpulseRec()) == NULL) {
		::AfxMessageBox(IDS_MSG_SELECTDATA);
		return;
	}

	m_nImpulseID = pDbImpulseRec->nImpulseID;

	EndDialog(IDOK);
}

void CLoadIR::OnDelete()
{
	DbImpulseRec *pDbImpulseRec;

	if ((pDbImpulseRec = m_cDataList.GetSelImpulseRec()) == NULL) {
		::AfxMessageBox(IDS_MSG_SELECTDATA);
		return;
	}

	if (::AfxMessageBox(IDS_MSG_DELETESELECTDATA, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
		return;

	DeleteImpulse(pDbImpulseRec->nImpulseID);

	m_cDataList.SetDataList(m_nFolderID);
}

void CLoadIR::OnFolder()
{
	CFolderDlg dlg(this);

	dlg.m_nFolderType = FOLDER_TYPE_IR;
	dlg.DoModal();

	m_cFolderList.SetFolderList(FOLDER_TYPE_IR);
}

void CLoadIR::OnDblclkDataList()
{
	OnOK();
}

void CLoadIR::OnWaveFile()
{
	CFileDialog fileDlg(TRUE, "wav", "*.wav",
			OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
			"Wave File (*.wav)|*.wav|All Files (*.*)|*.*||", this, 0);

	if (fileDlg.DoModal() == IDCANCEL)
		return;

	m_sWaveFile = fileDlg.GetPathName();
	EndDialog(IDC_WAVE_FILE);
}

void CLoadIR::OnImportData()
{
	CFileDialog fileDlg(TRUE, "dbf", "folder.dbf",
			OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
			"Sound Data File|folder.dbf||", this, 0);

	if (fileDlg.DoModal() == IDCANCEL)
		return;

	ImportIR(GetDirName(fileDlg.GetPathName()));
	m_cFolderList.SetFolderList(FOLDER_TYPE_IR);
}

BOOL CLoadIR::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDC_FOLDER_LIST, IDH_IMP_LOAD_FOLDER_LIST,
		IDC_FOLER_TITLE, IDH_IMP_LOAD_FOLER_TITLE,
		IDC_FOLDER_NAME, IDH_IMP_LOAD_FOLDER_NAME,
		IDC_FOLDER_PLACE, IDH_IMP_LOAD_FOLDER_PLACE,
		IDC_FOLDER_COMMENT, IDH_IMP_LOAD_FOLDER_COMMENT,
		IDC_FOLDER_DATE, IDH_IMP_LOAD_FOLDER_DATE,
		IDC_DATA_LIST, IDH_IMP_LOAD_DATA_LIST,
		IDC_DATA_TITLE, IDH_IMP_LOAD_DATA_TITLE,
		IDC_DATA_COMMENT, IDH_IMP_LOAD_DATA_COMMENT,
		IDC_DATA_TIME, IDH_IMP_LOAD_DATA_TIME,
		IDOK, IDH_IMP_LOAD_READ,
		IDC_DELETE, IDH_IMP_LOAD_DELETE,
		IDC_FOLDER, IDH_IMP_LOAD_FOLDER,
		IDC_WAVE_FILE, IDH_IMP_LOAD_WAVE_FILE,
		IDC_IMPORT_DATA, IDH_IMP_LOAD_IMPORT_DATA,
		IDCANCEL, IDH_IMP_LOAD_CANCEL,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CLoadACF ダイアログ

CLoadACF::CLoadACF(CWnd* pParent /*=NULL*/)
	: CDialogAF(CLoadACF::IDD, pParent)
{
}

void CLoadACF::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATA_LIST, m_cDataList);
	DDX_Control(pDX, IDC_FOLER_TITLE, m_cFolderTitle);
	DDX_Control(pDX, IDC_FOLDER_PLACE, m_cFolderPlace);
	DDX_Control(pDX, IDC_FOLDER_NAME, m_cFolderName);
	DDX_Control(pDX, IDC_FOLDER_DATE, m_cFolderDate);
	DDX_Control(pDX, IDC_FOLDER_COMMENT, m_cFolderComment);
	DDX_Control(pDX, IDC_DATA_TITLE, m_cDataTitle);
	DDX_Control(pDX, IDC_DATA_TIME, m_cDataTime);
	DDX_Control(pDX, IDC_DATA_COMMENT, m_cDataComment);
	DDX_Control(pDX, IDC_FOLDER_LIST, m_cFolderList);
}

BEGIN_MESSAGE_MAP(CLoadACF, CDialogAF)
	ON_LBN_SELCHANGE(IDC_FOLDER_LIST, OnSelchangeFolderList)
	ON_LBN_SELCHANGE(IDC_DATA_LIST, OnSelchangeDataList)
	ON_LBN_DBLCLK(IDC_DATA_LIST, OnDblclkDataList)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_FOLDER, OnFolder)
	ON_BN_CLICKED(IDC_IMPORT_DATA, OnImportData)
	ON_BN_CLICKED(IDC_WAVE_FILE, OnWaveFile)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoadACF メッセージ ハンドラ

BOOL CLoadACF::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	m_cFolderList.SetFolderList(FOLDER_TYPE_ACF);

	return TRUE;
}

void CLoadACF::OnSelchangeFolderList()
{
	DbFolderRec *pDbFolderRec;

	if ((pDbFolderRec = m_cFolderList.GetSelFolderRec()) == NULL)
		return;

	m_nFolderID = pDbFolderRec->nFolderID;
	m_cFolderTitle = pDbFolderRec->sTitle;
	m_cFolderName = pDbFolderRec->sName;
	m_cFolderPlace = pDbFolderRec->sPlace;
	m_cFolderComment = pDbFolderRec->sComment;
	m_cFolderDate = ::FormatTime(pDbFolderRec->sDate);

	m_cDataList.SetDataList(m_nFolderID);
}

void CLoadACF::OnSelchangeDataList()
{
	DbAcfRec *pDbAcfRec;

	if ((pDbAcfRec = m_cDataList.GetSelAcfRec()) == NULL)
		return;

	m_cDataTitle = pDbAcfRec->sTitle;
	m_cDataComment = pDbAcfRec->sComment;
	m_cDataTime = ::FormatTime(pDbAcfRec->sTime);
}

void CLoadACF::OnOK()
{
	DbAcfRec *pDbAcfRec;

	if ((pDbAcfRec = m_cDataList.GetSelAcfRec()) == NULL) {
		::AfxMessageBox(IDS_MSG_SELECTDATA);
		return;
	}

	m_nAcfID = pDbAcfRec->nAcfID;

	CDialogAF::OnOK();
}

void CLoadACF::OnDelete()
{
	DbAcfRec *pDbAcfRec;

	if ((pDbAcfRec = m_cDataList.GetSelAcfRec()) == NULL) {
		::AfxMessageBox(IDS_MSG_SELECTDATA);
		return;
	}

	if (::AfxMessageBox(IDS_MSG_DELETESELECTDATA, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
		return;

	DeleteAcf(pDbAcfRec->nAcfID);

	m_cDataList.SetDataList(m_nFolderID);
}

void CLoadACF::OnFolder()
{
	CFolderDlg dlg(this);

	dlg.m_nFolderType = FOLDER_TYPE_ACF;
	dlg.DoModal();

	m_cFolderList.SetFolderList(FOLDER_TYPE_ACF);
}

void CLoadACF::OnDblclkDataList()
{
	OnOK();
}

void CLoadACF::OnImportData()
{
	CFileDialog fileDlg(TRUE, "dbf", "folder.dbf",
			OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
			"Sound Data File|folder.dbf||", this, 0);

	if (fileDlg.DoModal() == IDCANCEL)
		return;

	ImportACF(GetDirName(fileDlg.GetPathName()));
	m_cFolderList.SetFolderList(FOLDER_TYPE_ACF);
}

void CLoadACF::OnWaveFile()
{
	CFileDialog fileDlg(TRUE, "wav", "*.wav",
			OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
			"Wave File (*.wav)|*.wav|All Files (*.*)|*.*||", this, 0);

	if (fileDlg.DoModal() == IDCANCEL)
		return;

	m_sWaveFile = fileDlg.GetPathName();
	EndDialog(IDC_WAVE_FILE);
}

BOOL CLoadACF::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDC_FOLDER_LIST, IDH_IMP_LOAD_FOLDER_LIST,
		IDC_FOLER_TITLE, IDH_IMP_LOAD_FOLER_TITLE,
		IDC_FOLDER_NAME, IDH_IMP_LOAD_FOLDER_NAME,
		IDC_FOLDER_PLACE, IDH_IMP_LOAD_FOLDER_PLACE,
		IDC_FOLDER_COMMENT, IDH_IMP_LOAD_FOLDER_COMMENT,
		IDC_FOLDER_DATE, IDH_IMP_LOAD_FOLDER_DATE,
		IDC_DATA_LIST, IDH_IMP_LOAD_DATA_LIST,
		IDC_DATA_TITLE, IDH_IMP_LOAD_DATA_TITLE,
		IDC_DATA_COMMENT, IDH_IMP_LOAD_DATA_COMMENT,
		IDC_DATA_TIME, IDH_IMP_LOAD_DATA_TIME,
		IDOK, IDH_IMP_LOAD_READ,
		IDC_DELETE, IDH_IMP_LOAD_DELETE,
		IDC_FOLDER, IDH_IMP_LOAD_FOLDER,
		IDC_WAVE_FILE, IDH_IMP_LOAD_WAVE_FILE,
		IDC_IMPORT_DATA, IDH_IMP_LOAD_IMPORT_DATA,
		IDCANCEL, IDH_IMP_LOAD_CANCEL,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSaveACF ダイアログ

CSaveACF::CSaveACF(CWnd* pParent /*=NULL*/)
	: CDialogAF(CSaveACF::IDD, pParent)
{
	m_iSampling = 0;
}

void CSaveACF::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATA_LIST, m_cDataList);
	DDX_Control(pDX, IDC_FOLDER_LIST, m_cFolderList);
	DDX_Control(pDX, IDC_TITLE, m_cTitle);
	DDX_Control(pDX, IDC_COMMENT, m_cComment);
	DDX_Text(pDX, IDC_SAMPLING, m_iSampling);
	DDX_Text(pDX, IDC_TIME, m_sTime);
	DDX_Text(pDX, IDC_DATA_TIME, m_sDataTime);
	DDX_Text(pDX, IDC_CHANNEL, m_sChannel);
}

BEGIN_MESSAGE_MAP(CSaveACF, CDialogAF)
	ON_BN_CLICKED(IDC_FOLDER, OnFolder)
	ON_BN_CLICKED(IDC_WAVE_FILE, OnWaveFile)
	ON_LBN_SELCHANGE(IDC_FOLDER_LIST, OnSelchangeFolderList)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaveACF メッセージ ハンドラ

BOOL CSaveACF::OnInitDialog()
{
	m_sDataTime.Format("%.2f", m_fDataTime);
	m_sTime = ::FormatTime(m_Time);
	m_sChannel = (m_nChannel == 1) ? "Mono" : "Stereo";

	CDialogAF::OnInitDialog();

	m_cFolderList.SetFolderList(FOLDER_TYPE_ACF);

	m_cTitle.SetMaxLength(LEN_TITLE);
	m_cComment.SetMaxLength(LEN_COMMENT);

	return TRUE;
}

void CSaveACF::OnFolder()
{
	CFolderDlg dlg(this);

	dlg.m_nFolderType = FOLDER_TYPE_ACF;
	dlg.DoModal();

	m_cFolderList.SetFolderList(FOLDER_TYPE_ACF);
}

void CSaveACF::OnOK()
{
	DbFolderRec *pDbFolderRec;
	if ((pDbFolderRec = m_cFolderList.GetSelFolderRec()) == NULL) {
		::AfxMessageBox(IDS_MSG_SELECTFOLDER);
		return;
	}

	UpdateData(TRUE);
	if (m_cTitle.IsEmpty()) {
		::AfxMessageBox(IDS_ERR_TITLE, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	if (::AfxMessageBox(IDS_MSG_SAVEDATA, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
		return;

	CDbAcf dbAcf;
	if (!dbAcf.Open())
		return;

	DbAcfRec dbAcfRec;
	if (!dbAcf.GetNewID(&dbAcfRec.nAcfID))
		return;

	dbAcfRec.nFolderID = pDbFolderRec->nFolderID;
	dbAcfRec.nSampling = m_iSampling;
	dbAcfRec.nChannel = m_nChannel | (m_nBitsPerSample >= 24 ? 0x04 : 0);
	dbAcfRec.nSampleNum = m_nSample;
	dbAcfRec.fDataTime = m_fDataTime;
	dbAcfRec.sTime = m_Time;
	dbAcfRec.sTitle= m_cTitle;
	dbAcfRec.sComment = m_cComment;

	if (!dbAcf.StoreRec(&dbAcfRec))
		return;

	if (m_hWaveData != NULL) {
		if (MMWaveWriteDB(m_hWaveData, dbAcf.GetFCB(), "WAVEDATA", MMTYPE_WAV) != 0) {
			::AfxMessageBox(IDS_ERR_WRITEDATA, MB_OK | MB_ICONEXCLAMATION);
			return;
		}
	}

	CDialogAF::OnOK();
}

void CSaveACF::OnWaveFile()
{
	CFileDialog fileDlg(FALSE, "wav", "",
			OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
			"Wave File (*.wav)|*.wav|All Files (*.*)|*.*||", this, 0);

	if (fileDlg.DoModal() == IDOK) {
		if (MMWaveWriteFile(m_hWaveData, (LPSTR)(LPCSTR)fileDlg.GetPathName()) != 0)
			::AfxMessageBox(IDS_ERR_WRITEWAVEFILE, MB_OK | MB_ICONEXCLAMATION);
	}
}

void CSaveACF::OnSelchangeFolderList()
{
	DbFolderRec *pDbFolderRec;

	if ((pDbFolderRec = m_cFolderList.GetSelFolderRec()) != NULL)
		m_cDataList.SetDataList(pDbFolderRec->nFolderID);
}

BOOL CSaveACF::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDC_FOLDER_LIST, IDH_ACF_SAVE_FOLDER_LIST,
		IDC_DATA_LIST, IDH_ACF_SAVE_DATA_LIST,
		IDOK, IDH_ACF_SAVE_OK,
		IDC_FOLDER, IDH_ACF_SAVE_FOLDER,
		IDC_WAVE_FILE, IDH_ACF_SAVE_WAVE_FILE,
		IDCANCEL, IDH_ACF_SAVE_CANCEL,
		IDC_SAMPLING, IDH_ACF_SAVE_SAMPLING,
		IDC_CHANNEL, IDH_ACF_SAVE_CHANNEL,
		IDC_DATA_TIME, IDH_ACF_SAVE_DATA_TIME,
		IDC_TITLE, IDH_ACF_SAVE_TITLE,
		IDC_COMMENT, IDH_ACF_SAVE_COMMENT,
		IDC_TIME, IDH_ACF_SAVE_TIME,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CFolderList

CFolderList::CFolderList()
{
	m_pFolder = NULL;
}

CFolderList::~CFolderList()
{
	if (m_pFolder != NULL)
		delete [] m_pFolder;
}

BEGIN_MESSAGE_MAP(CFolderList, CListBox)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFolderList メッセージ ハンドラ

void CFolderList::SetFolderList(int nFolderType)
{
	CDbFolder dbFolder;
	long nRec;
	int i;
	int nIndex;

	if (m_pFolder != NULL) {
		delete [] m_pFolder;
		m_pFolder = NULL;
		ResetContent();
	}

	if (!dbFolder.Open())
		return;

	CString strFilter;
	strFilter.Format("TYPE=%c", nFolderType);
	dbFolder.DBSetFilter(strFilter);

	if (dbFolder.DBRecCount(&nRec) != 0)
		return;

	if ((m_pFolder = new DbFolderRec[nRec]) == NULL)
		return;

	for (i = 0; i < nRec; i++) {
		if (!dbFolder.ReadRecNext(&m_pFolder[i]))
			break;

		nIndex = AddString(m_pFolder[i].sTitle);
		SetItemData(nIndex, i);
	}

	dbFolder.DBClose();
}

DbFolderRec *CFolderList::GetSelFolderRec()
{
	int index;

	if ((index = GetCurSel()) == LB_ERR)
		return NULL;

	return &m_pFolder[GetItemData(index)];
}

/////////////////////////////////////////////////////////////////////////////
// CIRDataList

CIRDataList::CIRDataList()
{
	m_pData = NULL;
}

CIRDataList::~CIRDataList()
{
	if (m_pData != NULL)
		delete [] m_pData;
}

BEGIN_MESSAGE_MAP(CIRDataList, CListBox)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIRDataList メッセージ ハンドラ

void CIRDataList::SetDataList(long folderID)
{
	if (m_pData != NULL) {
		delete [] m_pData;
		m_pData = NULL;
		ResetContent();
	}

	CDbImpulse dbImpulse;
	if (!dbImpulse.Open())
		return;

	if (dbImpulse.DBChgIdx(dbImpulse.m_nIdxImpulseID) != 0)
		return;

	long nRec;
	CString select;
	select.Format("#%d=%ld", dbImpulse.m_nIdxFolderID, folderID);
	if (dbImpulse.DBSelect(select, &nRec) != 0)
		return;

	if ((m_pData = new DbImpulseRec[nRec]) == NULL)
		return;

	int nIndex;
	for (int i = 0; i < nRec; i++) {
		if (!dbImpulse.ReadRecNext(&m_pData[i]))
			break;

		nIndex = AddString(m_pData[i].sTitle);
		SetItemData(nIndex, i);
	}
}

DbImpulseRec *CIRDataList::GetSelImpulseRec()
{
	int index;

	if ((index = GetCurSel()) == LB_ERR)
		return NULL;

	return &m_pData[GetItemData(index)];
}

/////////////////////////////////////////////////////////////////////////////
// CAcfDataList

CAcfDataList::CAcfDataList()
{
	m_pData = NULL;
}

CAcfDataList::~CAcfDataList()
{
	if (m_pData != NULL)
		delete [] m_pData;
}

BEGIN_MESSAGE_MAP(CAcfDataList, CListBox)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAcfDataList メッセージ ハンドラ

void CAcfDataList::SetDataList(long folderID)
{
	if (m_pData != NULL) {
		delete [] m_pData;
		m_pData = NULL;
		ResetContent();
	}

	CDbAcf dbAcf;
	if (!dbAcf.Open())
		return;

	if (dbAcf.DBChgIdx(dbAcf.m_nIdxAcfID) != 0)
		return;

	long nRec;
	CString select;
	select.Format("#%d=%ld", dbAcf.m_nIdxFolderID, folderID);
	if (dbAcf.DBSelect(select, &nRec) != 0)
		return;

	if ((m_pData = new DbAcfRec[nRec]) == NULL)
		return;

	int nIndex;
	for (int i = 0; i < nRec; i++) {
		if (!dbAcf.ReadRecNext(&m_pData[i]))
			break;

		nIndex = AddString(m_pData[i].sTitle);
		SetItemData(nIndex, i);
	}
}

DbAcfRec *CAcfDataList::GetSelAcfRec()
{
	int index;

	if ((index = GetCurSel()) == LB_ERR)
		return NULL;

	return &m_pData[GetItemData(index)];
}
