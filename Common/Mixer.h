#pragma once

#include <mmsystem.h>

extern DWORD_PTR dwMixerInDevice;
extern DWORD_PTR dwMixerOutDevice;
extern DWORD dwMixerOutMasterLineID;
extern DWORD dwMixerInMasterLineID;
extern const DWORD dwMixerDefaultOutputVol;

extern BOOL MixerInitialize(INT_PTR nWaveInDevice, INT_PTR nWaveOutDevice);
extern void MixerSetOutput();
extern void MixerRestoreOutput();
extern void MixerSetInput();
extern void MixerRestoreInput();
extern DWORD MixerGetControlID(DWORD_PTR dwMixerDevice, DWORD dwLineID, DWORD dwControlType);
extern UINT MixerGetControlsByType(DWORD_PTR dwMixerDevice, DWORD dwLineID, DWORD dwControlType, MIXERCONTROL &mxc);
extern LONG MixerGetBoolControl(DWORD_PTR dwMixerDevice, DWORD dwControlID);
extern LONG MixerGetUnsignedControl(DWORD_PTR dwMixerDevice, DWORD dwControlID);
extern void MixerSetBoolControl(DWORD_PTR dwMixerDevice, DWORD dwControlID, LONG value);
extern void MixerSetUnsignedControl(DWORD_PTR dwMixerDevice, DWORD dwControlID, DWORD value);
extern UINT MixerGetControlValue(DWORD_PTR dwMixerDevice, DWORD dwControlID, DWORD cMultipleItems, LPVOID paDetails, DWORD cbDetails);
extern UINT MixerGetControlList(DWORD_PTR dwMixerDevice, DWORD dwControlID, DWORD cMultipleItems, LPVOID paDetails, DWORD cbDetails);
extern UINT MixerSetControlValue(DWORD_PTR dwMixerDevice, DWORD dwControlID, DWORD cMultipleItems, LPVOID paDetails, DWORD cbDetails);
extern void DisableInputMasterVolume();
