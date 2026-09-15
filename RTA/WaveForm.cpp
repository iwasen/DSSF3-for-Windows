#include "stdafx.h"
#include "Rta.h"
#include "WaveForm.h"
#include "FileIO.h"

CWaveForm::CWaveForm()
{
	memset(m_pWaveForm, 0, sizeof(m_pWaveForm));
	m_pTestWaveForm = NULL;
}

CWaveForm::~CWaveForm()
{
	for (int i = 0; i < MAX_WAVEFORM; i++) {
		if (m_pWaveForm[i] != NULL) {
			delete m_pWaveForm[i];
			m_pWaveForm[i] = NULL;
		}
	}
}

WaveForm *CWaveForm::ReadWaveForm(int id)
{
	if (m_pTestWaveForm != NULL)
		return m_pTestWaveForm;

	if (m_pWaveForm[id] == NULL) {
		CDbWaveForm dbWaveForm;
		DbWaveFormRec dbRec;
		WaveForm *pWaveForm;

		pWaveForm = new WaveForm;
		memset(pWaveForm, 0, sizeof(WaveForm));

		if (dbWaveForm.Open()) {
			if (dbWaveForm.ReadRecID(id, &dbRec, (WaveFormData *)pWaveForm->uWaveFormManual.vertex)) {
				switch (dbRec.nDataNum) {
				case WAVEFORM_FM:
					pWaveForm->uWaveFormFM.nDataType = dbRec.nDataNum;
					break;
				default:
					pWaveForm->uWaveFormManual.nDataNum = dbRec.nDataNum;
					pWaveForm->uWaveFormManual.bSmoothing = dbRec.nSmoothFlag;
					break;
				}
			}
		}

		if (pWaveForm->nDataType < 2) {
			pWaveForm->uWaveFormManual.nDataNum = 2;
			pWaveForm->uWaveFormManual.vertex[0].x = 0;
			pWaveForm->uWaveFormManual.vertex[0].y = 0;
			pWaveForm->uWaveFormManual.vertex[1].x = 1;
			pWaveForm->uWaveFormManual.vertex[1].y = 0;
		}

		m_pWaveForm[id] = pWaveForm;
	}

	return m_pWaveForm[id];
}

void CWaveForm::WriteWaveForm(int id)
{
	WaveForm *pWaveForm = m_pWaveForm[id];

	if (pWaveForm != NULL) {
		CDbWaveForm dbWaveForm;
		DbWaveFormRec dbRec;

		dbRec.nWaveFormID = id;

		switch (pWaveForm->nDataType) {
		case WAVEFORM_FM:
			dbRec.nDataNum = pWaveForm->uWaveFormFM.nDataType;
			dbRec.nSmoothFlag = FALSE;
			break;
		default:
			dbRec.nDataNum = pWaveForm->uWaveFormManual.nDataNum;
			dbRec.nSmoothFlag = pWaveForm->uWaveFormManual.bSmoothing;
			break;
		}

		if (dbWaveForm.Open())
			dbWaveForm.WriteRec(&dbRec, (WaveFormData *)pWaveForm->uWaveFormManual.vertex);
	}
}

void CWaveForm::SetTestWaveForm(WaveForm *pWaveForm)
{
	m_pTestWaveForm = pWaveForm;
}
