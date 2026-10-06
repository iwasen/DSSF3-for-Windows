// WaveEx.cpp : 実装ファイル
//

#include "stdafx.h"
#include "RTA.h"
#include "WaveEx.h"
#include "RtaDlg.h"

// CWaveEx

CWaveEx::CWaveEx()
{
	m_nFilterUpdateCounter = 0;
}

// CWaveEx メンバ関数

void CWaveEx::FilteringWave(LPWAVENOTIFY pWaveNotify)
{
	if (pWaveNotify != NULL) {
		CRtaApp *pWinApp = (CRtaApp *)::AfxGetApp();
		if (pWinApp->m_pWndRta->m_pWndFilter != NULL) {
			if (m_nFilterUpdateCounter != g_nFilterUpdateCounter) {
				m_cFilterLeft.InitIIR((EDFShape)g_oSetData.Flt.nShape, (EDFBand)g_oSetData.Flt.nBand, g_oSetData.Flt.nOrder, pWaveNotify->nSamplesPerSec, g_oSetData.Flt.fCutoff1, g_oSetData.Flt.fCutoff2, g_oSetData.Flt.fRipple1);
				m_cFilterRight.InitIIR((EDFShape)g_oSetData.Flt.nShape, (EDFBand)g_oSetData.Flt.nBand, g_oSetData.Flt.nOrder, pWaveNotify->nSamplesPerSec, g_oSetData.Flt.fCutoff1, g_oSetData.Flt.fCutoff2, g_oSetData.Flt.fRipple1);

				m_nFilterUpdateCounter = g_nFilterUpdateCounter;
			}

			const int nData = pWaveNotify->nSamplesNum;
			double *pData = pWaveNotify->pSamplesData;
			for (int i = 0; i < nData; i++) {
				*pData++ = m_cFilterLeft.ExecIIR(*pData);
				if (pWaveNotify->nChannels == 2)
					*pData++ = m_cFilterRight.ExecIIR(*pData);
			}
		}
	}
}
