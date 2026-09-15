#pragma once

#include <mmsystem.h>

#define WAVEOUT_OPEN	0
#define WAVEOUT_CLOSE	1
#define WAVEOUT_DATA	2
#define WAVEOUT_CLOSE_WAIT	3
#define WAVEIN_OPEN		4
#define WAVEIN_CLOSE	5
#define WAVEIN_DATA		6
#define WAVEIN_QUERY_RATE	7
#define WAVEIN_QUERY_BITS	8

struct WAVENOTIFY {
	double *pSamplesData;
	int nSamplesNum;
	int nSamplesRecorded;
	int nFlags;
	int nChannels;
	int nSamplesPerSec;
	int nWindow;
};
typedef WAVENOTIFY *LPWAVENOTIFY;

interface IWaveNotify
{
	virtual int OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify) = 0;
};
