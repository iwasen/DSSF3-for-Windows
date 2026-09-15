// OsCalDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "RTA.h"
#include "OsCalDlg.h"
#include "WaveInEx.h"
#include "Help\ContextHelp.h"

// COsCalDlg ダイアログ

COsCalDlg::COsCalDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(COsCalDlg::IDD, pParent)
	, m_iCalType(0)
{
	m_pWaveIn = CWaveInEx::GetInstance();
	m_pWaveData = NULL;
}

COsCalDlg::~COsCalDlg()
{
	m_pWaveIn->ReleaseInstance();

	if (m_pWaveData != NULL)
		delete [] m_pWaveData;
}

void COsCalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CAL_VALUE, m_cEditCalValue);
	DDX_Control(pDX, IDC_EDIT_UNIT, m_cEditUnit);
	DDX_Radio(pDX, IDC_RADIO_EFECTIVE, m_iCalType);
	DDX_Control(pDX, IDOK, m_cOK);
}

BEGIN_MESSAGE_MAP(COsCalDlg, CDialogAF)
	ON_BN_CLICKED(IDC_BUTTON_DISABLE, &COsCalDlg::OnBnClickedButtonDisable)
	ON_WM_DESTROY()
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

// COsCalDlg メッセージ ハンドラ

BOOL COsCalDlg::OnInitDialog()
{
	m_iCalType = g_oSetData.Os.nCalType;

	CDialogAF::OnInitDialog();

	m_cEditCalValue = g_oSetData.Os.fCalValue;
	m_cEditUnit = (g_oSetData.Os.sCalUnit[0] == '\0') ? "V" : g_oSetData.Os.sCalUnit;

	m_cEditCalValue.SetValidChar(VC_NUM | VC_POINT);

	m_pWaveData = new double[g_oSetData.Os.nSamplingRate];

	if (!m_pWaveIn->Open(g_oSetData.Pc.nInputDevice, this, 2, g_oSetData.Os.nSamplingRate, g_oSetData.Os.nSamplingRate, 4)) {
		EndDialog(IDCANCEL);
		return TRUE;
	}

	m_pWaveIn->Start();

	return TRUE;
}

void COsCalDlg::OnOK()
{
	UpdateData(TRUE);

	g_oSetData.Os.nCalType = m_iCalType;
	g_oSetData.Os.fCalValue = m_cEditCalValue;
	g_oSetData.Os.fCalLevel = GetCalLevel();
	strcpy_s(g_oSetData.Os.sCalUnit, g_oSetData.Os.fCalValue != 0 ? (CString)m_cEditUnit : "");

	EndDialog(IDOK);
}

void COsCalDlg::OnBnClickedButtonDisable()
{
	g_oSetData.Os.sCalUnit[0] = '\0';
	g_oSetData.Os.fCalValue = 0;

	EndDialog(IDOK);
}

double COsCalDlg::GetCalLevel()
{
	int i;
	double fLevel = 1;

	switch (g_oSetData.Os.nCalType) {
	case 0:		// 実効値
		{
			double fData = 0;
			for (i = 0; i < g_oSetData.Os.nSamplingRate; i++)
				fData += m_pWaveData[i] * m_pWaveData[i];
			fData /= g_oSetData.Os.nSamplingRate;
			fLevel = sqrt(fData);
		}
		break;
	case 1:		// PeakToPeak
		{
			double fMax = 0, fMin = 0;
			for (i = 0; i < g_oSetData.Os.nSamplingRate; i++) {
				if (m_pWaveData[i] > fMax)
					fMax = m_pWaveData[i];

				if (m_pWaveData[i] < fMin)
					fMin = m_pWaveData[i];
			}
			fLevel = fMax - fMin;
		}
		break;
	}

	return fLevel;
}

int COsCalDlg::OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify)
{
	switch (nCode) {
	case WAVEIN_DATA:
		if (!m_cOK.IsWindowEnabled())
			m_cOK.EnableWindow();

		for (int i = 0; i < g_oSetData.Os.nSamplingRate; i++)
			m_pWaveData[i] = pWaveNotify->pSamplesData[i * 2] * PowDouble(16);

		return TRUE;
	}

	return 0;
}

void COsCalDlg::OnDestroy()
{
	CDialogAF::OnDestroy();

	m_pWaveIn->Close();
}

BOOL COsCalDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_EDIT_CAL_VALUE, IDH_OS_CAL_VALUE,
		IDC_EDIT_UNIT, IDH_OS_CAL_UNIT,
		IDC_RADIO_EFECTIVE, IDH_OS_CAL_EFECTIVE,
		IDC_RADIO_PEAKTOPEAK, IDH_OS_CAL_PEAKTOPEAK,
		IDOK, IDH_OS_CAL_OK,
		IDC_BUTTON_DISABLE, IDH_OS_CAL_DISABLE,
		IDCANCEL, IDH_OS_CAL_CANCEL,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
