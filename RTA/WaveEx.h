#pragma once

#include "DigitalFilter.h"

// CWaveEx コマンド ターゲット

class CWaveEx : public CObject
{
public:
	CWaveEx();

protected:
	int m_nFilterUpdateCounter;
	CDigitalFilter m_cFilterLeft;
	CDigitalFilter m_cFilterRight;

	void FilteringWave(LPWAVENOTIFY pWaveNotify);
};

