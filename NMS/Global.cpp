#include "stdafx.h"
#include "Nms.h"
#include "mixer.h"

CSetData g_oSetData;
HICON g_hIcon;
CString g_sTempDir;
CSplWnd *g_pSplWnd;
CPeakLevelWnd *g_pPeakLevelWnd;
CNmsFactor *g_pNmsFactor;
CManualMode *g_pManualWnd;

#define MAX_CONTROL_ID	20
void SetInputDevice(int nInputDevice, int nInputSelector, int nInputVolume)
{
	MIXERCONTROL mxc;
	MIXERCONTROLDETAILS_BOOLEAN mxcdb[MAX_CONTROL_ID];
	MIXERCONTROLDETAILS_LISTTEXT mxcdl[MAX_CONTROL_ID];
	DWORD i, dst, src;
	DWORD dwDstControlID;
	DWORD dwMultipleItems;
	DWORD dwSrcItems;
	DWORD dwSrcRecVolID[MAX_CONTROL_ID];
	DWORD dwSrcRecMuteID;

	MixerInitialize(nInputDevice, 0);

	// 録音マスターの LineID を取得
	MIXERLINE mxl{
		.cbStruct = sizeof(mxl),
		.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN
	};
	if (mixerGetLineInfo((HMIXEROBJ)dwMixerInDevice, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
		return;

	// 録音入力セレクタ
	if (MixerGetControlsByType(dwMixerInDevice, mxl.dwLineID, MIXERCONTROL_CONTROLTYPE_MUX, mxc) == MMSYSERR_NOERROR ||
			MixerGetControlsByType(dwMixerInDevice, mxl.dwLineID, MIXERCONTROL_CONTROLTYPE_MIXER, mxc) == MMSYSERR_NOERROR) {
		dwDstControlID = mxc.dwControlID;
		dwMultipleItems = mxc.cMultipleItems;
		if (dwMultipleItems > MAX_CONTROL_ID)
			dwMultipleItems = MAX_CONTROL_ID;

		// セレクタリスト取得
		if (MixerGetControlList(dwMixerInDevice, dwDstControlID, dwMultipleItems, mxcdl, sizeof(MIXERCONTROLDETAILS_LISTTEXT)) != MMSYSERR_NOERROR)
			return;

		// セレクト状態取得
		if (MixerGetControlValue(dwMixerInDevice, dwDstControlID, dwMultipleItems, mxcdb, sizeof(MIXERCONTROLDETAILS_BOOLEAN)) != MMSYSERR_NOERROR)
			return;

		// 入力機器のボリュームの ControlID 取得
		for (i = 0; i < dwMultipleItems; i++) {
			dwSrcRecVolID[i] = MixerGetControlID(dwMixerInDevice, mxcdl[i].dwParam1, MIXERCONTROL_CONTROLTYPE_VOLUME);

			mxcdb[i].fValue = (i == (DWORD)nInputSelector) ? 1 : 0;
		}
		MixerSetControlValue(dwMixerInDevice, dwDstControlID, dwMultipleItems, mxcdb, sizeof(MIXERCONTROLDETAILS_BOOLEAN));
	} else {
		dwSrcItems = mxl.cConnections;
		if (dwSrcItems > MAX_CONTROL_ID)
			dwSrcItems = MAX_CONTROL_ID;
		dst = mxl.dwDestination;
		for (src = 0; src < dwSrcItems; src++) {
			// 入力機器の LineID 取得
			MIXERLINE mxl{
				.cbStruct = sizeof(mxl),
				.dwDestination = dst,
				.dwSource = src
			};
			if (mixerGetLineInfo((HMIXEROBJ)dwMixerInDevice, &mxl, MIXER_GETLINEINFOF_SOURCE) != MMSYSERR_NOERROR)
				return;

			// 入力機器のボリュームの ControlID 取得
			dwSrcRecVolID[src] = MixerGetControlID(dwMixerInDevice, mxl.dwLineID, MIXERCONTROL_CONTROLTYPE_VOLUME);
			dwSrcRecMuteID = MixerGetControlID(dwMixerInDevice, mxl.dwLineID, MIXERCONTROL_CONTROLTYPE_MUTE);

			// 入力機器のミュート状態設定
			if (dwSrcRecMuteID != -1)
				MixerSetBoolControl(dwMixerInDevice, dwSrcRecMuteID, (src == (DWORD)nInputSelector) ? 0 : 1);
		}
	}

	MixerSetUnsignedControl(dwMixerInDevice, dwSrcRecVolID[nInputSelector], nInputVolume);
}
