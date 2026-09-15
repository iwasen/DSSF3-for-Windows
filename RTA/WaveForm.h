#pragma once

#define MAX_WAVEFORM	5
#define MAX_WAVEVERTEX	100
#define WAVEFORM_FM		1000

struct WaveFormManual {
	int nDataNum;
	BOOL bSmoothing;
	struct {
		float x;
		float y;
	} vertex[MAX_WAVEVERTEX];
};

struct WaveFormFM {
	int nDataType;
	BOOL bDummy;
	float fModAmp;
	float fModFreq;
};

union WaveForm {
	int nDataType;
	WaveFormManual uWaveFormManual;
	WaveFormFM uWaveFormFM;
};

class CWaveForm
{
public:
	CWaveForm(void);
	~CWaveForm(void);

	WaveForm *ReadWaveForm(int id);
	void WriteWaveForm(int id);
	void SetTestWaveForm(WaveForm *pWaveForm);

protected:
	WaveForm *m_pWaveForm[MAX_WAVEFORM];
	WaveForm *m_pTestWaveForm;
};
