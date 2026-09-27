// Mixer.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Mixer.h"

DWORD_PTR dwMixerInDevice;
DWORD_PTR dwMixerOutDevice;
DWORD dwMixerInMasterLineID;
DWORD dwMixerOutMasterLineID;

DWORD dwMixerDefaultOutputVol = 32768 + 16384;

static DWORD dwInMuteID = (DWORD)-1;
static DWORD dwInVolumeID = (DWORD)-1;
static DWORD dwInMuteVal;
static DWORD dwInVolumeVal;

static DWORD dwOutMuteID = (DWORD)-1;
static DWORD dwOutVolumeID = (DWORD)-1;
static DWORD dwOutTrebleID = (DWORD)-1;
static DWORD dwOutBassID = (DWORD)-1;
static DWORD dwOutMuteVal;
static DWORD dwOutVolumeVal;
static DWORD dwOutTrebleVal;
static DWORD dwOutBassVal;

BOOL MixerInitialize(INT_PTR nWaveInDevice, INT_PTR nWaveOutDevice)
{
	MIXERLINE mxl{};
	int i;
	MIXERCAPS mxcaps;
	int nMixerNum = mixerGetNumDevs();

	if (nWaveOutDevice >= 0) {
		dwMixerOutDevice = nWaveOutDevice;

		WAVEOUTCAPS woc;
		waveOutGetDevCaps(nWaveOutDevice, &woc, sizeof(woc));
		for (i = 0; i < nMixerNum; i++) {
			mixerGetDevCaps(i, &mxcaps, sizeof(mxcaps));
			if (strcmp(mxcaps.szPname, woc.szPname) == 0) {
				dwMixerOutDevice = i;
				break;
			}
		}

		// 出力マスターの LineID を取得
		mxl.cbStruct = sizeof(mxl);
		mxl.dwComponentType	= MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
		if (mixerGetLineInfo((HMIXEROBJ)dwMixerOutDevice, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
			return FALSE;

		dwMixerOutMasterLineID = mxl.dwLineID;

		// ControlID 取得
		dwOutMuteID = MixerGetControlID(dwMixerOutDevice, dwMixerOutMasterLineID, MIXERCONTROL_CONTROLTYPE_MUTE);
		dwOutVolumeID = MixerGetControlID(dwMixerOutDevice, dwMixerOutMasterLineID, MIXERCONTROL_CONTROLTYPE_VOLUME);
		dwOutTrebleID = MixerGetControlID(dwMixerOutDevice, dwMixerOutMasterLineID, MIXERCONTROL_CONTROLTYPE_TREBLE);
		dwOutBassID = MixerGetControlID(dwMixerOutDevice, dwMixerOutMasterLineID, MIXERCONTROL_CONTROLTYPE_BASS);
	}

	if (nWaveInDevice >= 0) {
		dwMixerInDevice = nWaveInDevice;

		WAVEINCAPS wic;
		waveInGetDevCaps(nWaveInDevice, &wic, sizeof(wic));
		for (i = 0; i < nMixerNum; i++) {
			mixerGetDevCaps(i, &mxcaps, sizeof(mxcaps));
			if (strcmp(mxcaps.szPname, wic.szPname) == 0) {
				dwMixerInDevice = i;
				break;
			}
		}

		// 録音マスターの LineID を取得
		mxl.cbStruct = sizeof(mxl);
		mxl.dwComponentType	= MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
		if (mixerGetLineInfo((HMIXEROBJ)dwMixerInDevice, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
			return FALSE;

		dwMixerInMasterLineID = mxl.dwLineID;

		// ControlID 取得
		dwInMuteID = MixerGetControlID(dwMixerInDevice, dwMixerInMasterLineID, MIXERCONTROL_CONTROLTYPE_MUTE);
		dwInVolumeID = MixerGetControlID(dwMixerInDevice, dwMixerInMasterLineID, MIXERCONTROL_CONTROLTYPE_VOLUME);
	}

	return TRUE;
}

void MixerSetOutput()
{
	if (dwOutMuteID != -1) {
		dwOutMuteVal = MixerGetBoolControl(dwMixerOutDevice, dwOutMuteID);
		MixerSetBoolControl(dwMixerOutDevice, dwOutMuteID, 0);
	}

	if (dwOutVolumeID != -1) {
		dwOutVolumeVal = MixerGetUnsignedControl(dwMixerOutDevice, dwOutVolumeID);
		MixerSetUnsignedControl(dwMixerOutDevice, dwOutVolumeID, dwMixerDefaultOutputVol);
	}

	if (dwOutTrebleID != -1) {
		dwOutTrebleVal = MixerGetUnsignedControl(dwMixerOutDevice, dwOutTrebleID);
		MixerSetUnsignedControl(dwMixerOutDevice, dwOutTrebleID, 32768);
	}

	if (dwOutBassID != -1) {
		dwOutBassVal = MixerGetUnsignedControl(dwMixerOutDevice, dwOutBassID);
		MixerSetUnsignedControl(dwMixerOutDevice, dwOutBassID, 32768);
	}
}

void MixerRestoreOutput()
{
	if (dwOutMuteID != -1)
		MixerSetBoolControl(dwMixerOutDevice, dwOutMuteID, dwOutMuteVal);

	if (dwOutVolumeID != -1)
		MixerSetUnsignedControl(dwMixerOutDevice, dwOutVolumeID, dwOutVolumeVal);

	if (dwOutTrebleID != -1)
		MixerSetUnsignedControl(dwMixerOutDevice, dwOutTrebleID, dwOutTrebleVal);

	if (dwOutBassID != -1)
		MixerSetUnsignedControl(dwMixerOutDevice, dwOutBassID, dwOutBassVal);
}

void MixerSetInput()
{
	if (dwInMuteID != -1) {
		dwInMuteVal = MixerGetBoolControl(dwMixerInDevice, dwInMuteID);
		MixerSetBoolControl(dwMixerInDevice, dwInMuteID, 0);
	}

	if (dwInVolumeID != -1) {
		dwInVolumeVal = MixerGetUnsignedControl(dwMixerInDevice, dwInVolumeID);
		MixerSetUnsignedControl(dwMixerInDevice, dwInVolumeID, 32768);
	}
}

void MixerRestoreInput()
{
	if (dwInMuteID != -1)
		MixerSetBoolControl(dwMixerInDevice, dwInMuteID, dwInMuteVal);

	if (dwInVolumeID != -1)
		MixerSetUnsignedControl(dwMixerInDevice, dwInVolumeID, dwInVolumeVal);
}

DWORD MixerGetControlID(DWORD_PTR dwMixerDevice, DWORD dwLineID, DWORD dwControlType)
{
	MIXERCONTROL mxc;

	if (MixerGetControlsByType(dwMixerDevice, dwLineID, dwControlType, mxc) == MMSYSERR_NOERROR)
		return mxc.dwControlID;
	else
		return (DWORD)-1;
}

UINT MixerGetControlsByType(DWORD_PTR dwMixerDevice, DWORD dwLineID, DWORD dwControlType, MIXERCONTROL &mxc)
{
	mxc = {};

	MIXERLINECONTROLS mxlc{};
	mxlc.cbStruct = sizeof(mxlc);
	mxlc.dwLineID = dwLineID;
	mxlc.dwControlType = dwControlType;
	mxlc.cbmxctrl = sizeof(MIXERCONTROL);
	mxlc.pamxctrl = &mxc;
	return mixerGetLineControls((HMIXEROBJ)dwMixerDevice, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);
}

LONG MixerGetBoolControl(DWORD_PTR dwMixerDevice, DWORD dwControlID)
{
	MIXERCONTROLDETAILS_BOOLEAN mxcdb;

	MixerGetControlValue(dwMixerDevice, dwControlID, 0, &mxcdb, sizeof(MIXERCONTROLDETAILS_BOOLEAN));
	return mxcdb.fValue;
}

LONG MixerGetUnsignedControl(DWORD_PTR dwMixerDevice, DWORD dwControlID)
{
	MIXERCONTROLDETAILS_UNSIGNED mxcdu;

	MixerGetControlValue(dwMixerDevice, dwControlID, 0, &mxcdu, sizeof(MIXER_GETCONTROLDETAILSF_VALUE));
	return mxcdu.dwValue;
}

void MixerSetBoolControl(DWORD_PTR dwMixerDevice, DWORD dwControlID, LONG value)
{
	MIXERCONTROLDETAILS_BOOLEAN mxcdb;

	mxcdb.fValue = value;
	MixerSetControlValue(dwMixerDevice, dwControlID, 0, &mxcdb, sizeof(MIXERCONTROLDETAILS_BOOLEAN));
}

void MixerSetUnsignedControl(DWORD_PTR dwMixerDevice, DWORD dwControlID, DWORD value)
{
	MIXERCONTROLDETAILS_UNSIGNED mxcdu;

	mxcdu.dwValue = value;
	MixerSetControlValue(dwMixerDevice, dwControlID, 0, &mxcdu, sizeof(MIXERCONTROLDETAILS_UNSIGNED));
}

UINT MixerGetControlValue(DWORD_PTR dwMixerDevice, DWORD dwControlID, DWORD cMultipleItems, LPVOID paDetails, DWORD cbDetails)
{
	MIXERCONTROLDETAILS mxcd{};

	mxcd.cbStruct = sizeof(mxcd);
	mxcd.dwControlID = dwControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = cMultipleItems;
	mxcd.cbDetails = cbDetails;
	mxcd.paDetails = paDetails;
	return mixerGetControlDetails((HMIXEROBJ)dwMixerDevice, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE);
}

UINT MixerGetControlList(DWORD_PTR dwMixerDevice, DWORD dwControlID, DWORD cMultipleItems, LPVOID paDetails, DWORD cbDetails)
{
	MIXERCONTROLDETAILS mxcd{};

	mxcd.cbStruct = sizeof(mxcd);
	mxcd.dwControlID = dwControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = cMultipleItems;
	mxcd.cbDetails = cbDetails;
	mxcd.paDetails = paDetails;
	return mixerGetControlDetails((HMIXEROBJ)dwMixerDevice, &mxcd, MIXER_GETCONTROLDETAILSF_LISTTEXT);
}

UINT MixerSetControlValue(DWORD_PTR dwMixerDevice, DWORD dwControlID, DWORD cMultipleItems, LPVOID paDetails, DWORD cbDetails)
{
	MIXERCONTROLDETAILS mxcd{};

	mxcd.cbStruct = sizeof(mxcd);
	mxcd.dwControlID = dwControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = cMultipleItems;
	mxcd.cbDetails = cbDetails;
	mxcd.paDetails = paDetails;
	return mixerSetControlDetails((HMIXEROBJ)dwMixerDevice, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE);
}

void DisableInputMasterVolume()
{
	dwInVolumeID = (DWORD)-1;
}
