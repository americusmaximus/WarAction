/*
Copyright (c) 2024 Americus Maximus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "Basic.hxx"

#define METHOD_AUX32MESSAGE_NAME                            "aux32Message"
#define METHOD_AUXGETDEVCAPSA_NAME                          "auxGetDevCapsA"
#define METHOD_AUXGETDEVCAPSW_NAME                          "auxGetDevCapsW"
#define METHOD_AUXGETNUMDEVS_NAME                           "auxGetNumDevs"
#define METHOD_AUXGETVOLUME_NAME                            "auxGetVolume"
#define METHOD_AUXOUTMESSAGE_NAME                           "auxOutMessage"
#define METHOD_AUXSETVOLUME_NAME                            "auxSetVolume"
#define METHOD_CLOSEDRIVER_NAME                             "CloseDriver"
#define METHOD_DEFDRIVERPROC_NAME                           "DefDriverProc"
#define METHOD_DRIVERCALLBACK_NAME                          "DriverCallback"
#define METHOD_DRVGETMODULEHANDLE_NAME                      "DrvGetModuleHandle"
#define METHOD_GETDRIVERMODULEHANDLE_NAME                   "GetDriverModuleHandle"
#define METHOD_JOY32MESSAGE_NAME                            "joy32Message"
#define METHOD_JOYCONFIGCHANGED_NAME                        "joyConfigChanged"
#define METHOD_JOYGETDEVCAPSA_NAME                          "joyGetDevCapsA"
#define METHOD_JOYGETDEVCAPSW_NAME                          "joyGetDevCapsW"
#define METHOD_JOYGETNUMDEVS_NAME                           "joyGetNumDevs"
#define METHOD_JOYGETPOS_NAME                               "joyGetPos"
#define METHOD_JOYGETPOSEX_NAME                             "joyGetPosEx"
#define METHOD_JOYGETTHRESHOLD_NAME                         "joyGetThreshold"
#define METHOD_JOYRELEASECAPTURE_NAME                       "joyReleaseCapture"
#define METHOD_JOYSETCAPTURE_NAME                           "joySetCapture"
#define METHOD_JOYSETTHRESHOLD_NAME                         "joySetThreshold"
#define METHOD_MCI32MESSAGE_NAME                            "mci32Message"
#define METHOD_MCIDRIVERNOTIFY_NAME                         "mciDriverNotify"
#define METHOD_MCIDRIVERYIELD_NAME                          "mciDriverYield"
#define METHOD_MCIEXECUTE_NAME                              "mciExecute"
#define METHOD_MCIFREECOMMANDRESOURCE_NAME                  "mciFreeCommandResource"
#define METHOD_MCIGETCREATORTASK_NAME                       "mciGetCreatorTask"
#define METHOD_MCIGETDEVICEIDA_NAME                         "mciGetDeviceIDA"
#define METHOD_MCIGETDEVICEIDFROMELEMENTIDA_NAME            "mciGetDeviceIDFromElementIDA"
#define METHOD_MCIGETDEVICEIDFROMELEMENTIDW_NAME            "mciGetDeviceIDFromElementIDW"
#define METHOD_MCIGETDEVICEIDW_NAME                         "mciGetDeviceIDW"
#define METHOD_MCIGETDRIVERDATA_NAME                        "mciGetDriverData"
#define METHOD_MCIGETERRORSTRINGA_NAME                      "mciGetErrorStringA"
#define METHOD_MCIGETERRORSTRINGW_NAME                      "mciGetErrorStringW"
#define METHOD_MCIGETYIELDPROC_NAME                         "mciGetYieldProc"
#define METHOD_MCILOADCOMMANDRESOURCE_NAME                  "mciLoadCommandResource"
#define METHOD_MCISENDCOMMANDA_NAME                         "mciSendCommandA"
#define METHOD_MCISENDCOMMANDW_NAME                         "mciSendCommandW"
#define METHOD_MCISENDSTRINGA_NAME                          "mciSendStringA"
#define METHOD_MCISENDSTRINGW_NAME                          "mciSendStringW"
#define METHOD_MCISETDRIVERDATA_NAME                        "mciSetDriverData"
#define METHOD_MCISETYIELDPROC_NAME                         "mciSetYieldProc"
#define METHOD_MID32MESSAGE_NAME                            "mid32Message"
#define METHOD_MIDICONNECT_NAME                             "midiConnect"
#define METHOD_MIDIDISCONNECT_NAME                          "midiDisconnect"
#define METHOD_MIDIINADDBUFFER_NAME                         "midiInAddBuffer"
#define METHOD_MIDIINCLOSE_NAME                             "midiInClose"
#define METHOD_MIDIINGETDEVCAPSA_NAME                       "midiInGetDevCapsA"
#define METHOD_MIDIINGETDEVCAPSW_NAME                       "midiInGetDevCapsW"
#define METHOD_MIDIINGETERRORTEXTA_NAME                     "midiInGetErrorTextA"
#define METHOD_MIDIINGETERRORTEXTW_NAME                     "midiInGetErrorTextW"
#define METHOD_MIDIINGETID_NAME                             "midiInGetID"
#define METHOD_MIDIINGETNUMDEVS_NAME                        "midiInGetNumDevs"
#define METHOD_MIDIINMESSAGE_NAME                           "midiInMessage"
#define METHOD_MIDIINOPEN_NAME                              "midiInOpen"
#define METHOD_MIDIINPREPAREHEADER_NAME                     "midiInPrepareHeader"
#define METHOD_MIDIINRESET_NAME                             "midiInReset"
#define METHOD_MIDIINSTART_NAME                             "midiInStart"
#define METHOD_MIDIINSTOP_NAME                              "midiInStop"
#define METHOD_MIDIINUNPREPAREHEADER_NAME                   "midiInUnprepareHeader"
#define METHOD_MIDIOUTCACHEDRUMPATCHES_NAME                 "midiOutCacheDrumPatches"
#define METHOD_MIDIOUTCACHEPATCHES_NAME                     "midiOutCachePatches"
#define METHOD_MIDIOUTCLOSE_NAME                            "midiOutClose"
#define METHOD_MIDIOUTGETDEVCAPSA_NAME                      "midiOutGetDevCapsA"
#define METHOD_MIDIOUTGETDEVCAPSW_NAME                      "midiOutGetDevCapsW"
#define METHOD_MIDIOUTGETERRORTEXTA_NAME                    "midiOutGetErrorTextA"
#define METHOD_MIDIOUTGETERRORTEXTW_NAME                    "midiOutGetErrorTextW"
#define METHOD_MIDIOUTGETID_NAME                            "midiOutGetID"
#define METHOD_MIDIOUTGETNUMDEVS_NAME                       "midiOutGetNumDevs"
#define METHOD_MIDIOUTGETVOLUME_NAME                        "midiOutGetVolume"
#define METHOD_MIDIOUTLONGMSG_NAME                          "midiOutLongMsg"
#define METHOD_MIDIOUTMESSAGE_NAME                          "midiOutMessage"
#define METHOD_MIDIOUTOPEN_NAME                             "midiOutOpen"
#define METHOD_MIDIOUTPREPAREHEADER_NAME                    "midiOutPrepareHeader"
#define METHOD_MIDIOUTRESET_NAME                            "midiOutReset"
#define METHOD_MIDIOUTSETVOLUME_NAME                        "midiOutSetVolume"
#define METHOD_MIDIOUTSHORTMSG_NAME                         "midiOutShortMsg"
#define METHOD_MIDIOUTUNPREPAREHEADER_NAME                  "midiOutUnprepareHeader"
#define METHOD_MIDISTREAMCLOSE_NAME                         "midiStreamClose"
#define METHOD_MIDISTREAMOPEN_NAME                          "midiStreamOpen"
#define METHOD_MIDISTREAMOUT_NAME                           "midiStreamOut"
#define METHOD_MIDISTREAMPAUSE_NAME                         "midiStreamPause"
#define METHOD_MIDISTREAMPOSITION_NAME                      "midiStreamPosition"
#define METHOD_MIDISTREAMPROPERTY_NAME                      "midiStreamProperty"
#define METHOD_MIDISTREAMRESTART_NAME                       "midiStreamRestart"
#define METHOD_MIDISTREAMSTOP_NAME                          "midiStreamStop"
#define METHOD_MIXERCLOSE_NAME                              "mixerClose"
#define METHOD_MIXERGETCONTROLDETAILSA_NAME                 "mixerGetControlDetailsA"
#define METHOD_MIXERGETCONTROLDETAILSW_NAME                 "mixerGetControlDetailsW"
#define METHOD_MIXERGETDEVCAPSA_NAME                        "mixerGetDevCapsA"
#define METHOD_MIXERGETDEVCAPSW_NAME                        "mixerGetDevCapsW"
#define METHOD_MIXERGETID_NAME                              "mixerGetID"
#define METHOD_MIXERGETLINECONTROLSA_NAME                   "mixerGetLineControlsA"
#define METHOD_MIXERGETLINECONTROLSW_NAME                   "mixerGetLineControlsW"
#define METHOD_MIXERGETLINEINFOA_NAME                       "mixerGetLineInfoA"
#define METHOD_MIXERGETLINEINFOW_NAME                       "mixerGetLineInfoW"
#define METHOD_MIXERGETNUMDEVS_NAME                         "mixerGetNumDevs"
#define METHOD_MIXERMESSAGE_NAME                            "mixerMessage"
#define METHOD_MIXEROPEN_NAME                               "mixerOpen"
#define METHOD_MIXERSETCONTROLDETAILS_NAME                  "mixerSetControlDetails"
#define METHOD_MMDRVINSTALL_NAME                            "mmDrvInstall"
#define METHOD_MMGETCURRENTTASK_NAME                        "mmGetCurrentTask"
#define METHOD_MMIOADVANCE_NAME                             "mmioAdvance"
#define METHOD_MMIOASCEND_NAME                              "mmioAscend"
#define METHOD_MMIOCLOSE_NAME                               "mmioClose"
#define METHOD_MMIOCREATECHUNK_NAME                         "mmioCreateChunk"
#define METHOD_MMIODESCEND_NAME                             "mmioDescend"
#define METHOD_MMIOFLUSH_NAME                               "mmioFlush"
#define METHOD_MMIOGETINFO_NAME                             "mmioGetInfo"
#define METHOD_MMIOINSTALLIOPROCA_NAME                      "mmioInstallIOProcA"
#define METHOD_MMIOINSTALLIOPROCW_NAME                      "mmioInstallIOProcW"
#define METHOD_MMIOOPENA_NAME                               "mmioOpenA"
#define METHOD_MMIOOPENW_NAME                               "mmioOpenW"
#define METHOD_MMIOREAD_NAME                                "mmioRead"
#define METHOD_MMIORENAMEA_NAME                             "mmioRenameA"
#define METHOD_MMIORENAMEW_NAME                             "mmioRenameW"
#define METHOD_MMIOSEEK_NAME                                "mmioSeek"
#define METHOD_MMIOSENDMESSAGE_NAME                         "mmioSendMessage"
#define METHOD_MMIOSETBUFFER_NAME                           "mmioSetBuffer"
#define METHOD_MMIOSETINFO_NAME                             "mmioSetInfo"
#define METHOD_MMIOSTRINGTOFOURCCA_NAME                     "mmioStringToFOURCCA"
#define METHOD_MMIOSTRINGTOFOURCCW_NAME                     "mmioStringToFOURCCW"
#define METHOD_MMIOWRITE_NAME                               "mmioWrite"
#define METHOD_MMSYSTEMGETVERSION_NAME                      "mmsystemGetVersion"
#define METHOD_MMTASKBLOCK_NAME                             "mmTaskBlock"
#define METHOD_MMTASKCREATE_NAME                            "mmTaskCreate"
#define METHOD_MMTASKSIGNAL_NAME                            "mmTaskSignal"
#define METHOD_MMTASKYIELD_NAME                             "mmTaskYield"
#define METHOD_MOD32MESSAGE_NAME                            "mod32Message"
#define METHOD_MXD32MESSAGE_NAME                            "mxd32Message"
#define METHOD_NOTIFYCALLBACKDATA_NAME                      "NotifyCallbackData"
#define METHOD_OPENDRIVER_NAME                              "OpenDriver"
#define METHOD_PLAYSOUND_NAME                               "PlaySound"
#define METHOD_PLAYSOUNDA_NAME                              "PlaySoundA"
#define METHOD_PLAYSOUNDW_NAME                              "PlaySoundW"
#define METHOD_SENDDRIVERMESSAGE_NAME                       "SendDriverMessage"
#define METHOD_SNDPLAYSOUNDA_NAME                           "sndPlaySoundA"
#define METHOD_SNDPLAYSOUNDW_NAME                           "sndPlaySoundW"
#define METHOD_TID32MESSAGE_NAME                            "tid32Message"
#define METHOD_TIMEBEGINPERIOD_NAME                         "timeBeginPeriod"
#define METHOD_TIMEENDPERIOD_NAME                           "timeEndPeriod"
#define METHOD_TIMEGETDEVCAPS_NAME                          "timeGetDevCaps"
#define METHOD_TIMEGETSYSTEMTIME_NAME                       "timeGetSystemTime"
#define METHOD_TIMEGETTIME_NAME                             "timeGetTime"
#define METHOD_TIMEKILLEVENT_NAME                           "timeKillEvent"
#define METHOD_TIMESETEVENT_NAME                            "timeSetEvent"
#define METHOD_WAVEINADDBUFFER_NAME                         "waveInAddBuffer"
#define METHOD_WAVEINCLOSE_NAME                             "waveInClose"
#define METHOD_WAVEINGETDEVCAPSA_NAME                       "waveInGetDevCapsA"
#define METHOD_WAVEINGETDEVCAPSW_NAME                       "waveInGetDevCapsW"
#define METHOD_WAVEINGETERRORTEXTA_NAME                     "waveInGetErrorTextA"
#define METHOD_WAVEINGETERRORTEXTW_NAME                     "waveInGetErrorTextW"
#define METHOD_WAVEINGETID_NAME                             "waveInGetID"
#define METHOD_WAVEINGETNUMDEVS_NAME                        "waveInGetNumDevs"
#define METHOD_WAVEINGETPOSITION_NAME                       "waveInGetPosition"
#define METHOD_WAVEINMESSAGE_NAME                           "waveInMessage"
#define METHOD_WAVEINOPEN_NAME                              "waveInOpen"
#define METHOD_WAVEINPREPAREHEADER_NAME                     "waveInPrepareHeader"
#define METHOD_WAVEINRESET_NAME                             "waveInReset"
#define METHOD_WAVEINSTART_NAME                             "waveInStart"
#define METHOD_WAVEINSTOP_NAME                              "waveInStop"
#define METHOD_WAVEINUNPREPAREHEADER_NAME                   "waveInUnprepareHeader"
#define METHOD_WAVEOUTBREAKLOOP_NAME                        "waveOutBreakLoop"
#define METHOD_WAVEOUTCLOSE_NAME                            "waveOutClose"
#define METHOD_WAVEOUTGETDEVCAPSA_NAME                      "waveOutGetDevCapsA"
#define METHOD_WAVEOUTGETDEVCAPSW_NAME                      "waveOutGetDevCapsW"
#define METHOD_WAVEOUTGETERRORTEXTA_NAME                    "waveOutGetErrorTextA"
#define METHOD_WAVEOUTGETERRORTEXTW_NAME                    "waveOutGetErrorTextW"
#define METHOD_WAVEOUTGETID_NAME                            "waveOutGetID"
#define METHOD_WAVEOUTGETNUMDEVS_NAME                       "waveOutGetNumDevs"
#define METHOD_WAVEOUTGETPITCH_NAME                         "waveOutGetPitch"
#define METHOD_WAVEOUTGETPLAYBACKRATE_NAME                  "waveOutGetPlaybackRate"
#define METHOD_WAVEOUTGETPOSITION_NAME                      "waveOutGetPosition"
#define METHOD_WAVEOUTGETVOLUME_NAME                        "waveOutGetVolume"
#define METHOD_WAVEOUTMESSAGE_NAME                          "waveOutMessage"
#define METHOD_WAVEOUTOPEN_NAME                             "waveOutOpen"
#define METHOD_WAVEOUTPAUSE_NAME                            "waveOutPause"
#define METHOD_WAVEOUTPREPAREHEADER_NAME                    "waveOutPrepareHeader"
#define METHOD_WAVEOUTRESET_NAME                            "waveOutReset"
#define METHOD_WAVEOUTRESTART_NAME                          "waveOutRestart"
#define METHOD_WAVEOUTSETPITCH_NAME                         "waveOutSetPitch"
#define METHOD_WAVEOUTSETPLAYBACKRATE_NAME                  "waveOutSetPlaybackRate"
#define METHOD_WAVEOUTSETVOLUME_NAME                        "waveOutSetVolume"
#define METHOD_WAVEOUTUNPREPAREHEADER_NAME                  "waveOutUnprepareHeader"
#define METHOD_WAVEOUTWRITE_NAME                            "waveOutWrite"
#define METHOD_WID32MESSAGE_NAME                            "wid32Message"
#define METHOD_WOD32MESSAGE_NAME                            "wod32Message"
#define METHOD_WOW32DRIVERCALLBACK_NAME                     "WOW32DriverCallback"
#define METHOD_WOW32RESOLVEMULTIMEDIAHANDLE_NAME            "WOW32ResolveMultiMediaHandle"
#define METHOD_WOWAPPEXIT_NAME                              "WOWAppExit"

#ifdef PlaySound
#undef PlaySound
#endif

#define CALLBACK_ARGS_SIZE      16

typedef struct _CALLBACK_ARGS
{
    DWORD       dwFlags;        // Flags to identify the type of callback.
    DWORD       dwFunctionAddr; // 16:16 address of the function to be called
    WORD        wHandle;        // The handle or ID of the device
    WORD        wMessage;       // The message to be passed to function
    DWORD       dwInstance;     // User data
    DWORD       dwParam1;       // Device data 1
    DWORD       dwParam2;       // Device data 2
} CALLBACK_ARGS;

typedef struct _CALLBACK_DATA
{
    WORD            wRecvCount;    // The number of interrupts received
    WORD            wSendCount;    // The number of interrupts sent
    CALLBACK_ARGS   args[CALLBACK_ARGS_SIZE];  // Interrupt arguments
    WORD            wIntsCount;    // The number of interrupts received
} CALLBACK_DATA, * VPCALLBACK_DATA;

typedef DWORD(WINAPI* LPAUX32MESSAGE)(UINT uDeviceID, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
typedef MMRESULT(WINAPI* LPAUXGETDEVCAPSA)(UINT_PTR uDeviceID, LPAUXCAPSA pac, UINT cbac);
typedef MMRESULT(WINAPI* LPAUXGETDEVCAPSW)(UINT_PTR uDeviceID, LPAUXCAPSW pac, UINT cbac);
typedef UINT(WINAPI* LPAUXGETNUMDEVS)(VOID);
typedef MMRESULT(WINAPI* LPAUXGETVOLUME)(UINT uDeviceID, LPDWORD lpdwVolume);
typedef MMRESULT(WINAPI* LPAUXOUTMESSAGE)(UINT uDeviceID, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2);
typedef MMRESULT(WINAPI* LPAUXSETVOLUME)(UINT uDeviceID, DWORD dwVolume);
typedef LRESULT(WINAPI* LPCLOSEDRIVER)(HDRVR hDriver, LPARAM lParam1, LPARAM lParam2);
typedef LRESULT(WINAPI* LPDEFDRIVERPROC)(DWORD_PTR dwDriverIdentifier, HDRVR hdrvr, UINT uMsg, LPARAM lParam1, LPARAM lParam2);
typedef BOOL(WINAPI* LPDRIVERCALLBACK)(DWORD_PTR dwCallback, DWORD dwFlags, HDRVR hDevice, DWORD dwMsg, DWORD_PTR dwUser, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
typedef HMODULE(WINAPI* LPDRVGETMODULEHANDLE)(HDRVR hDriver);
typedef HMODULE(WINAPI* LPGETDRIVERMODULEHANDLE)(HDRVR hDriver);
typedef DWORD(WINAPI* LPJOY32MESSAGE)(UINT uJoyID, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
typedef MMRESULT(WINAPI* LPJOYCONFIGCHANGED)(DWORD dwFlags);
typedef MMRESULT(WINAPI* LPJOYGETDEVCAPSA)(INT_PTR uJoyID, LPJOYCAPSA pjc, UINT cbjc);
typedef MMRESULT(WINAPI* LPJOYGETDEVCAPSW)(UINT_PTR uJoyID, LPJOYCAPSW pjc, UINT cbjc);
typedef UINT(WINAPI* LPJOYGETNUMDEVS)(VOID);
typedef MMRESULT(WINAPI* LPJOYGETPOS)(UINT uJoyID, LPJOYINFO pji);
typedef MMRESULT(WINAPI* LPJOYGETPOSEX)(UINT uJoyID, LPJOYINFOEX pji);
typedef MMRESULT(WINAPI* LPJOYGETTHRESHOLD)(UINT uJoyID, LPUINT puThreshold);
typedef MMRESULT(WINAPI* LPJOYRELEASECAPTURE)(UINT uJoyID);
typedef MMRESULT(WINAPI* LPJOYSETCAPTURE)(HWND hwnd, UINT uJoyID, UINT uPeriod, BOOL fChanged);
typedef MMRESULT(WINAPI* LPJOYSETTHRESHOLD)(UINT uJoyID, UINT uThreshold);
typedef DWORD(WINAPI* LPMCI32MESSAGE)(DWORD dwApi, DWORD dwF1, DWORD dwF2, DWORD dwF3, DWORD dwF4);
typedef BOOL(WINAPI* LPMCIDRIVERNOTIFY)(HWND hwndCallback, UINT uDeviceID, UINT uStatus);
typedef UINT(WINAPI* LPMCIDRIVERYIELD)(UINT uDeviceID);
typedef BOOL(WINAPI* LPMCIEXECUTE)(LPCSTR pszCommand);
typedef BOOL(WINAPI* LPMCIFREECOMMANDRESOURCE)(UINT wTable);
typedef HANDLE(WINAPI* LPMCIGETCREATORTASK)(MCIDEVICEID mciId);
typedef MCIDEVICEID(WINAPI* LPMCIGETDEVICEIDA)(LPCSTR pszDevice);
typedef MCIDEVICEID(WINAPI* LPMCIGETDEVICEIDFROMELEMENTIDA)(DWORD dwElementID, LPCSTR lpstrType);
typedef MCIDEVICEID(WINAPI* LPMCIGETDEVICEIDFROMELEMENTIDW)(DWORD dwElementID, LPCWSTR lpstrType);
typedef MCIDEVICEID(WINAPI* LPMCIGETDEVICEIDW)(LPCWSTR pszDevice);
typedef DWORD_PTR(WINAPI* LPMCIGETDRIVERDATA)(MCIDEVICEID wDeviceID);
typedef BOOL(WINAPI* LPMCIGETERRORSTRINGA)(MCIERROR mcierr, LPSTR pszText, UINT cchText);
typedef BOOL(WINAPI* LPMCIGETERRORSTRINGW)(MCIERROR mcierr, LPWSTR pszText, UINT cchText);
typedef YIELDPROC(WINAPI* LPMCIGETYIELDPROC)(MCIDEVICEID mciId, LPDWORD pdwYieldData);
typedef UINT(WINAPI* LPMCILOADCOMMANDRESOURCE)(HANDLE hInstance, LPCWSTR lpResName, UINT wType);
typedef MCIERROR(WINAPI* LPMCISENDCOMMANDA)(MCIDEVICEID mciId, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
typedef MCIERROR(WINAPI* LPMCISENDCOMMANDW)(MCIDEVICEID mciId, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
typedef MCIERROR(WINAPI* LPMCISENDSTRINGA)(LPCSTR lpstrCommand, LPSTR lpstrReturnString, UINT uReturnLength, HANDLE hwndCallback);
typedef MCIERROR(WINAPI* LPMCISENDSTRINGW)(LPCWSTR lpstrCommand, LPWSTR lpstrReturnString, UINT uReturnLength, HANDLE hwndCallback);
typedef BOOL(WINAPI* LPMCISETDRIVERDATA)(MCIDEVICEID wDeviceID, DWORD_PTR dwData);
typedef BOOL(WINAPI* LPMCISETYIELDPROC)(MCIDEVICEID mciId, YIELDPROC fpYieldProc, DWORD dwYieldData);
typedef DWORD(WINAPI* LPMID32MESSAGE)(UINT uId, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
typedef MMRESULT(WINAPI* LPMIDICONNECT)(HMIDI hmi, HMIDIOUT hmo, LPVOID pReserved);
typedef MMRESULT(WINAPI* LPMIDIDISCONNECT)(HMIDI hmi, HMIDIOUT hmo, LPVOID pReserved);
typedef MMRESULT(WINAPI* LPMIDIINADDBUFFER)(HMIDIIN hmi, LPMIDIHDR pmh, UINT cbmh);
typedef MMRESULT(WINAPI* LPMIDIINCLOSE)(HMIDIIN hmi);
typedef MMRESULT(WINAPI* LPMIDIINGETDEVCAPSA)(UINT_PTR uDeviceID, LPMIDIINCAPSA pmic, UINT cbmic);
typedef MMRESULT(WINAPI* LPMIDIINGETDEVCAPSW)(UINT_PTR uDeviceID, LPMIDIINCAPSW pmic, UINT cbmic);
typedef MMRESULT(WINAPI* LPMIDIINGETERRORTEXTA)(MMRESULT mmrError, LPSTR pszText, UINT cchText);
typedef MMRESULT(WINAPI* LPMIDIINGETERRORTEXTW)(MMRESULT mmrError, LPWSTR pszText, UINT cchText);
typedef MMRESULT(WINAPI* LPMIDIINGETID)(HMIDIIN hmi, LPUINT puDeviceID);
typedef UINT(WINAPI* LPMIDIINGETNUMDEVS)(VOID);
typedef MMRESULT(WINAPI* LPMIDIINMESSAGE)(HMIDIIN hmi, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2);
typedef MMRESULT(WINAPI* LPMIDIINOPEN)(LPHMIDIIN phmi, UINT uDeviceID, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen);
typedef MMRESULT(WINAPI* LPMIDIINPREPAREHEADER)(HMIDIIN hmi, LPMIDIHDR pmh, UINT cbmh);
typedef MMRESULT(WINAPI* LPMIDIINRESET)(HMIDIIN hmi);
typedef MMRESULT(WINAPI* LPMIDIINSTART)(HMIDIIN hmi);
typedef MMRESULT(WINAPI* LPMIDIINSTOP)(HMIDIIN hmi);
typedef MMRESULT(WINAPI* LPMIDIINUNPREPAREHEADER)(HMIDIIN hmi, LPMIDIHDR pmh, UINT cbmh);
typedef MMRESULT(WINAPI* LPMIDIOUTCACHEDRUMPATCHES)(HMIDIOUT hmo, UINT uPatch, LPWORD pwkya, UINT fuCache);
typedef MMRESULT(WINAPI* LPMIDIOUTCACHEPATCHES)(HMIDIOUT hmo, UINT uBank, LPWORD pwpa, UINT fuCache);
typedef MMRESULT(WINAPI* LPMIDIOUTCLOSE)(HMIDIOUT hmo);
typedef MMRESULT(WINAPI* LPMIDIOUTGETDEVCAPSA)(UINT_PTR uDeviceID, LPMIDIOUTCAPSA pmoc, UINT cbmoc);
typedef MMRESULT(WINAPI* LPMIDIOUTGETDEVCAPSW)(UINT_PTR uDeviceID, LPMIDIOUTCAPSW pmoc, UINT cbmoc);
typedef MMRESULT(WINAPI* LPMIDIOUTGETERRORTEXTA)(MMRESULT mmrError, LPSTR pszText, UINT cchText);
typedef MMRESULT(WINAPI* LPMIDIOUTGETERRORTEXTW)(MMRESULT mmrError, LPWSTR pszText, UINT cchText);
typedef MMRESULT(WINAPI* LPMIDIOUTGETID)(HMIDIOUT hmo, LPUINT puDeviceID);
typedef UINT(WINAPI* LPMIDIOUTGETNUMDEVS)(VOID);
typedef MMRESULT(WINAPI* LPMIDIOUTGETVOLUME)(HMIDIOUT hmo, LPDWORD pdwVolume);
typedef MMRESULT(WINAPI* LPMIDIOUTLONGMSG)(HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh);
typedef MMRESULT(WINAPI* LPMIDIOUTMESSAGE)(HMIDIOUT hmo, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2);
typedef MMRESULT(WINAPI* LPMIDIOUTOPEN)(LPHMIDIOUT phmo, UINT uDeviceID, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen);
typedef MMRESULT(WINAPI* LPMIDIOUTPREPAREHEADER)(HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh);
typedef MMRESULT(WINAPI* LPMIDIOUTRESET)(HMIDIOUT hmo);
typedef MMRESULT(WINAPI* LPMIDIOUTSETVOLUME)(HMIDIOUT hmo, DWORD dwVolume);
typedef MMRESULT(WINAPI* LPMIDIOUTSHORTMSG)(HMIDIOUT hmo, DWORD dwMsg);
typedef MMRESULT(WINAPI* LPMIDIOUTUNPREPAREHEADER)(HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh);
typedef MMRESULT(WINAPI* LPMIDISTREAMCLOSE)(HMIDISTRM hms);
typedef MMRESULT(WINAPI* LPMIDISTREAMOPEN)(LPHMIDISTRM phms, LPUINT puDeviceID, DWORD cMidi, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen);
typedef MMRESULT(WINAPI* LPMIDISTREAMOUT)(HMIDISTRM hms, LPMIDIHDR pmh, UINT cbmh);
typedef MMRESULT(WINAPI* LPMIDISTREAMPAUSE)(HMIDISTRM hms);
typedef MMRESULT(WINAPI* LPMIDISTREAMPOSITION)(HMIDISTRM hms, LPMMTIME lpmmt, UINT cbmmt);
typedef MMRESULT(WINAPI* LPMIDISTREAMPROPERTY)(HMIDISTRM hms, LPBYTE lppropdata, DWORD dwProperty);
typedef MMRESULT(WINAPI* LPMIDISTREAMRESTART)(HMIDISTRM hms);
typedef MMRESULT(WINAPI* LPMIDISTREAMSTOP)(HMIDISTRM hms);
typedef MMRESULT(WINAPI* LPMIXERCLOSE)(HMIXER hmx);
typedef MMRESULT(WINAPI* LPMIXERGETCONTROLDETAILSA)(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails);
typedef MMRESULT(WINAPI* LPMIXERGETCONTROLDETAILSW)(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails);
typedef MMRESULT(WINAPI* LPMIXERGETDEVCAPSA)(UINT_PTR uMxId, LPMIXERCAPSA pmxcaps, UINT cbmxcaps);
typedef MMRESULT(WINAPI* LPMIXERGETDEVCAPSW)(UINT_PTR uMxId, LPMIXERCAPSW pmxcaps, UINT cbmxcaps);
typedef MMRESULT(WINAPI* LPMIXERGETID)(HMIXEROBJ hmxobj, UINT* puMxId, DWORD fdwId);
typedef MMRESULT(WINAPI* LPMIXERGETLINECONTROLSA)(HMIXEROBJ hmxobj, LPMIXERLINECONTROLSA pmxlc, DWORD fdwControls);
typedef MMRESULT(WINAPI* LPMIXERGETLINECONTROLSW)(HMIXEROBJ hmxobj, LPMIXERLINECONTROLSW pmxlc, DWORD fdwControls);
typedef MMRESULT(WINAPI* LPMIXERGETLINEINFOA)(HMIXEROBJ hmxobj, LPMIXERLINEA pmxl, DWORD fdwInfo);
typedef MMRESULT(WINAPI* LPMIXERGETLINEINFOW)(HMIXEROBJ hmxobj, LPMIXERLINEW pmxl, DWORD fdwInfo);
typedef UINT(WINAPI* LPMIXERGETNUMDEVS)(VOID);
typedef DWORD(WINAPI* LPMIXERMESSAGE)(HMIXER hmx, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
typedef MMRESULT(WINAPI* LPMIXEROPEN)(LPHMIXER phmx, UINT uMxId, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen);
typedef MMRESULT(WINAPI* LPMIXERSETCONTROLDETAILS)(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails);
typedef UINT(WINAPI* LPMMDRVINSTALL)(HDRVR hDriver, LPCWSTR wszDrvEntry, DRIVERMSGPROC drvMessage, UINT wFlags);
typedef DWORD(WINAPI* LPMMGETCURRENTTASK)(VOID);
typedef MMRESULT(WINAPI* LPMMIOADVANCE)(HMMIO hmmio, LPMMIOINFO pmmioinfo, UINT fuAdvance);
typedef MMRESULT(WINAPI* LPMMIOASCEND)(HMMIO hmmio, LPMMCKINFO pmmcki, UINT fuAscend);
typedef MMRESULT(WINAPI* LPMMIOCLOSE)(HMMIO hmmio, UINT fuClose);
typedef MMRESULT(WINAPI* LPMMIOCREATECHUNK)(HMMIO hmmio, LPMMCKINFO pmmcki, UINT fuCreate);
typedef MMRESULT(WINAPI* LPMMIODESCEND)(HMMIO hmmio, LPMMCKINFO pmmcki, CONST MMCKINFO* pmmckiParent, UINT fuDescend);
typedef MMRESULT(WINAPI* LPMMIOFLUSH)(HMMIO hmmio, UINT fuFlush);
typedef MMRESULT(WINAPI* LPMMIOGETINFO)(HMMIO hmmio, LPMMIOINFO pmmioinfo, UINT fuInfo);
typedef LPMMIOPROC(WINAPI* LPMMIOINSTALLIOPROCA)(FOURCC fccIOProc, LPMMIOPROC pIOProc, DWORD dwFlags);
typedef LPMMIOPROC(WINAPI* LPMMIOINSTALLIOPROCW)(FOURCC fccIOProc, LPMMIOPROC pIOProc, DWORD dwFlags);
typedef HMMIO(WINAPI* LPMMIOOPENA)(LPSTR pszFileName, LPMMIOINFO pmmioinfo, DWORD fdwOpen);
typedef HMMIO(WINAPI* LPMMIOOPENW)(LPWSTR pszFileName, LPMMIOINFO pmmioinfo, DWORD fdwOpen);
typedef LONG(WINAPI* LPMMIOREAD)(HMMIO hmmio, HPSTR pch, LONG cch);
typedef MMRESULT(WINAPI* LPMMIORENAMEA)(LPCSTR pszFileName, LPCSTR pszNewFileName, LPCMMIOINFO pmmioinfo, DWORD fdwRename);
typedef MMRESULT(WINAPI* LPMMIORENAMEW)(LPCWSTR pszFileName, LPCWSTR pszNewFileName, LPCMMIOINFO pmmioinfo, DWORD fdwRename);
typedef LONG(WINAPI* LPMMIOSEEK)(HMMIO hmmio, LONG lOffset, INT iOrigin);
typedef LRESULT(WINAPI* LPMMIOSENDMESSAGE)(HMMIO hmmio, UINT uMsg, LPARAM lParam1, LPARAM lParam2);
typedef MMRESULT(WINAPI* LPMMIOSETBUFFER)(HMMIO hmmio, LPSTR pchBuffer, LONG cchBuffer, UINT fuBuffer);
typedef MMRESULT(WINAPI* LPMMIOSETINFO)(HMMIO hmmio, LPCMMIOINFO pmmioinfo, UINT fuInfo);
typedef FOURCC(WINAPI* LPMMIOSTRINGTOFOURCCA)(LPCSTR sz, UINT uFlags);
typedef FOURCC(WINAPI* LPMMIOSTRINGTOFOURCCW)(LPCWSTR sz, UINT uFlags);
typedef LONG(WINAPI* LPMMIOWRITE)(HMMIO hmmio, CONST CHAR* pch, LONG cch);
typedef UINT(WINAPI* LPMMSYSTEMGETVERSION)(VOID);
typedef VOID(WINAPI* LPMMTASKBLOCK)(DWORD h);
typedef UINT(WINAPI* LPMMTASKCREATE)(LPTASKCALLBACK lpfn, HANDLE* lph, DWORD_PTR dwInst);
typedef BOOL(WINAPI* LPMMTASKSIGNAL)(DWORD h);
typedef VOID(WINAPI* LPMMTASKYIELD)(VOID);
typedef DWORD(WINAPI* LPMOD32MESSAGE)(UINT uId, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
typedef DWORD(WINAPI* LPMXD32MESSAGE)(UINT uId, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
typedef DWORD(WINAPI* LPNOTIFYCALLBACKDATA)(UINT uId, UINT uMsg, DWORD dwInstance, DWORD dwParam1, VPCALLBACK_DATA dwParam2);
typedef HDRVR(WINAPI* LPOPENDRIVER)(LPCWSTR szDriverName, LPCWSTR szSectionName, LPARAM lParam2);
typedef BOOL(WINAPI* LPPLAYSOUND)(LPCSTR pszSound, HMODULE hmod, DWORD fdwSound);
typedef BOOL(WINAPI* LPPLAYSOUNDA)(LPCSTR pszSound, HMODULE hmod, DWORD fdwSound);
typedef BOOL(WINAPI* LPPLAYSOUNDW)(LPCWSTR pszSound, HMODULE hmod, DWORD fdwSound);
typedef LRESULT(WINAPI* LPSENDDRIVERMESSAGE)(HDRVR hDriver, UINT uMsg, LPARAM lParam1, LPARAM lParam2);
typedef BOOL(WINAPI* LPSNDPLAYSOUNDA)(LPCSTR pszSound, UINT fuSound);
typedef BOOL(WINAPI* LPSNDPLAYSOUNDW)(LPCWSTR pszSound, UINT fuSound);
typedef DWORD(WINAPI* LPTID32MESSAGE)(UINT uId, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
typedef MMRESULT(WINAPI* LPTIMEBEGINPERIOD)(UINT uPeriod);
typedef MMRESULT(WINAPI* LPTIMEENDPERIOD)(UINT uPeriod);
typedef MMRESULT(WINAPI* LPTIMEGETDEVCAPS)(LPTIMECAPS ptc, UINT cbtc);
typedef MMRESULT(WINAPI* LPTIMEGETSYSTEMTIME)(LPMMTIME pmmt, UINT cbmmt);
typedef DWORD(WINAPI* LPTIMEGETTIME)(VOID);
typedef MMRESULT(WINAPI* LPTIMEKILLEVENT)(UINT uTimerID);
typedef MMRESULT(WINAPI* LPTIMESETEVENT)(UINT uDelay, UINT uResolution, LPTIMECALLBACK fptc, DWORD_PTR dwUser, UINT fuEvent);
typedef MMRESULT(WINAPI* LPWAVEINADDBUFFER)(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
typedef MMRESULT(WINAPI* LPWAVEINCLOSE)(HWAVEIN hwi);
typedef MMRESULT(WINAPI* LPWAVEINGETDEVCAPSA)(UINT_PTR uDeviceID, LPWAVEINCAPSA pwic, UINT cbwic);
typedef MMRESULT(WINAPI* LPWAVEINGETDEVCAPSW)(UINT_PTR uDeviceID, LPWAVEINCAPSW pwic, UINT cbwic);
typedef MMRESULT(WINAPI* LPWAVEINGETERRORTEXTA)(MMRESULT mmrError, LPSTR pszText, UINT cchText);
typedef MMRESULT(WINAPI* LPWAVEINGETERRORTEXTW)(MMRESULT mmrError, LPWSTR pszText, UINT cchText);
typedef MMRESULT(WINAPI* LPWAVEINGETID)(HWAVEIN hwi, LPUINT puDeviceID);
typedef UINT(WINAPI* LPWAVEINGETNUMDEVS)(VOID);
typedef MMRESULT(WINAPI* LPWAVEINGETPOSITION)(HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt);
typedef MMRESULT(WINAPI* LPWAVEINMESSAGE)(HWAVEIN hwi, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2);
typedef MMRESULT(WINAPI* LPWAVEINOPEN)(LPHWAVEIN phwi, UINT uDeviceID, LPCWAVEFORMATEX pwfx, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen);
typedef MMRESULT(WINAPI* LPWAVEINPREPAREHEADER)(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
typedef MMRESULT(WINAPI* LPWAVEINRESET)(HWAVEIN hwi);
typedef MMRESULT(WINAPI* LPWAVEINSTART)(HWAVEIN hwi);
typedef MMRESULT(WINAPI* LPWAVEINSTOP)(HWAVEIN hwi);
typedef MMRESULT(WINAPI* LPWAVEINUNPREPAREHEADER)(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
typedef MMRESULT(WINAPI* LPWAVEOUTBREAKLOOP)(HWAVEOUT hwo);
typedef MMRESULT(WINAPI* LPWAVEOUTCLOSE)(HWAVEOUT hwo);
typedef MMRESULT(WINAPI* LPWAVEOUTGETDEVCAPSA)(UINT_PTR uDeviceID, LPWAVEOUTCAPSA pwoc, UINT cbwoc);
typedef MMRESULT(WINAPI* LPWAVEOUTGETDEVCAPSW)(UINT_PTR uDeviceID, LPWAVEOUTCAPSW pwoc, UINT cbwoc);
typedef MMRESULT(WINAPI* LPWAVEOUTGETERRORTEXTA)(MMRESULT mmrError, LPSTR pszText, UINT cchText);
typedef MMRESULT(WINAPI* LPWAVEOUTGETERRORTEXTW)(MMRESULT mmrError, LPWSTR pszText, UINT cchText);
typedef MMRESULT(WINAPI* LPWAVEOUTGETID)(HWAVEOUT hwo, LPUINT puDeviceID);
typedef UINT(WINAPI* LPWAVEOUTGETNUMDEVS)(VOID);
typedef MMRESULT(WINAPI* LPWAVEOUTGETPITCH)(HWAVEOUT hwo, LPDWORD pdwPitch);
typedef MMRESULT(WINAPI* LPWAVEOUTGETPLAYBACKRATE)(HWAVEOUT hwo, LPDWORD pdwRate);
typedef MMRESULT(WINAPI* LPWAVEOUTGETPOSITION)(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt);
typedef MMRESULT(WINAPI* LPWAVEOUTGETVOLUME)(HWAVEOUT hwo, LPDWORD pdwVolume);
typedef MMRESULT(WINAPI* LPWAVEOUTMESSAGE)(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2);
typedef MMRESULT(WINAPI* LPWAVEOUTOPEN)(LPHWAVEOUT phwo, UINT uDeviceID, LPCWAVEFORMATEX pwfx, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen);
typedef MMRESULT(WINAPI* LPWAVEOUTPAUSE)(HWAVEOUT hwo);
typedef MMRESULT(WINAPI* LPWAVEOUTPREPAREHEADER)(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
typedef MMRESULT(WINAPI* LPWAVEOUTRESET)(HWAVEOUT hwo);
typedef MMRESULT(WINAPI* LPWAVEOUTRESTART)(HWAVEOUT hwo);
typedef MMRESULT(WINAPI* LPWAVEOUTSETPITCH)(HWAVEOUT hwo, DWORD dwPitch);
typedef MMRESULT(WINAPI* LPWAVEOUTSETPLAYBACKRATE)(HWAVEOUT hwo, DWORD dwRate);
typedef MMRESULT(WINAPI* LPWAVEOUTSETVOLUME)(HWAVEOUT hwo, DWORD dwVolume);
typedef MMRESULT(WINAPI* LPWAVEOUTUNPREPAREHEADER)(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
typedef MMRESULT(WINAPI* LPWAVEOUTWRITE)(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
typedef DWORD(WINAPI* LPWID32MESSAGE)(UINT uId, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
typedef DWORD(WINAPI* LPWOD32MESSAGE)(UINT uId, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
typedef BOOL(WINAPI* LPWOW32DRIVERCALLBACK)(DWORD dwCallback, DWORD dwFlags, WORD wID, WORD wMsg, DWORD dwUser, DWORD dw1, DWORD dw2);
typedef BOOL(WINAPI* LPWOW32RESOLVEMULTIMEDIAHANDLE)(UINT uHandleType, UINT uMappingDirection, WORD wHandle16_In, LPWORD lpwHandle16_Out, DWORD dwHandle32_In, LPDWORD lpdwHandle32_Out);
typedef VOID(WINAPI* LPWOWAPPEXIT)(HANDLE hTask);

struct ModuleStateContainer
{
    HMODULE Module;

    LPAUX32MESSAGE Aux32Message;
    //LPAUXGETDEVCAPSA AuxGetDevCapsA;
    LPAUXGETDEVCAPSW AuxGetDevCapsW; // 0x6ad52298
    //LPAUXGETNUMDEVS AuxGetNumDevs;
    //LPAUXGETVOLUME AuxGetVolume;
    LPAUXOUTMESSAGE AuxOutMessage; // 0x6ad52294
    //LPAUXSETVOLUME AuxSetVolume;
    LPCLOSEDRIVER CloseDriver; // 0x6ad52290
    LPDEFDRIVERPROC DefDriverProc; // 0x6ad5228c
    LPDRIVERCALLBACK DriverCallback; // 0x6ad52288
    LPDRVGETMODULEHANDLE DrvGetModuleHandle; // 0x6ad52284
    LPGETDRIVERMODULEHANDLE GetDriverModuleHandle; // 0x6ad52280
    LPJOY32MESSAGE Joy32Message;
    LPJOYCONFIGCHANGED JoyConfigChanged; // 0x6ad5227c
    LPJOYGETDEVCAPSA JoyGetDevCapsA; // 0x6ad52278
    LPJOYGETDEVCAPSW JoyGetDevCapsW; // 0x6ad52274
    LPJOYGETNUMDEVS JoyGetNumDevs; // 0x6ad52270
    LPJOYGETPOS JoyGetPos; // 0x6ad5226c
    LPJOYGETPOSEX JoyGetPosEx; // 0x6ad52268
    LPJOYGETTHRESHOLD JoyGetThreshold; // 0x6ad52264
    LPJOYRELEASECAPTURE JoyReleaseCapture; // 0x6ad52260
    LPJOYSETCAPTURE JoySetCapture; // 0x6ad5225c
    LPJOYSETTHRESHOLD JoySetThreshold; // 0x6ad52258
    LPMCI32MESSAGE Mci32Message;
    LPMCIDRIVERNOTIFY MciDriverNotify; // 0x6ad52054
    LPMCIDRIVERYIELD MciDriverYield; // 0x6ad52058
    LPMCIEXECUTE MciExecute; // 0x6ad52254
    LPMCIFREECOMMANDRESOURCE MciFreeCommandResource; // 0x6ad52050
    LPMCIGETCREATORTASK MciGetCreatorTask; // 0x6ad52250
    LPMCIGETDEVICEIDA MciGetDeviceIDA; // 0x6ad5224c
    LPMCIGETDEVICEIDFROMELEMENTIDA MciGetDeviceIDFromElementIDA; // 0x6ad52248
    LPMCIGETDEVICEIDFROMELEMENTIDW MciGetDeviceIDFromElementIDW; // 0x6ad52244
    LPMCIGETDEVICEIDW MciGetDeviceIDW; // 0x6ad52240
    LPMCIGETDRIVERDATA MciGetDriverData; // 0x6ad5205c
    LPMCIGETERRORSTRINGA MciGetErrorStringA; // 0x6ad522dc
    LPMCIGETERRORSTRINGW MciGetErrorStringW; // 0x6ad522d8
    LPMCIGETYIELDPROC MciGetYieldProc; // 0x6ad5223c
    LPMCILOADCOMMANDRESOURCE MciLoadCommandResource; // 0x6ad5204c
    //LPMCISENDCOMMANDA MciSendCommandA;
    LPMCISENDCOMMANDW MciSendCommandW; // 0x6ad52238
    LPMCISENDSTRINGA MciSendStringA; // 0x6ad522f0
    LPMCISENDSTRINGW MciSendStringW; // 0x6ad52234
    LPMCISETDRIVERDATA MciSetDriverData; // 0x6ad52060
    LPMCISETYIELDPROC MciSetYieldProc; // 0x6ad52230
    LPMID32MESSAGE Mid32Message;
    LPMIDICONNECT MidiConnect; // 0x6ad5222c
    LPMIDIDISCONNECT MidiDisconnect; // 0x6ad52228
    LPMIDIINADDBUFFER MidiInAddBuffer; // 0x6ad52224
    LPMIDIINCLOSE MidiInClose; // 0x6ad52220
    LPMIDIINGETDEVCAPSA MidiInGetDevCapsA; // 0x6ad5221c
    LPMIDIINGETDEVCAPSW MidiInGetDevCapsW; // 0x6ad52218
    LPMIDIINGETERRORTEXTA MidiInGetErrorTextA; // 0x6ad52214
    LPMIDIINGETERRORTEXTW MidiInGetErrorTextW; // 0x6ad52210
    LPMIDIINGETID MidiInGetID; // 0x6ad5220c
    LPMIDIINGETNUMDEVS MidiInGetNumDevs; // 0x6ad52208
    LPMIDIINMESSAGE MidiInMessage; // 0x6ad52204
    LPMIDIINOPEN MidiInOpen; // 0x6ad52200
    LPMIDIINPREPAREHEADER MidiInPrepareHeader; // 0x6ad521fc
    LPMIDIINRESET MidiInReset; // 0x6ad521f8
    LPMIDIINSTART MidiInStart; // 0x6ad521f4
    LPMIDIINSTOP MidiInStop; // 0x6ad521f0
    LPMIDIINUNPREPAREHEADER MidiInUnprepareHeader; // 0x6ad521ec
    LPMIDIOUTCACHEDRUMPATCHES MidiOutCacheDrumPatches; // 0x6ad521e8
    LPMIDIOUTCACHEPATCHES MidiOutCachePatches; // 0x6ad521e4
    LPMIDIOUTCLOSE MidiOutClose; // 0x6ad521e0
    LPMIDIOUTGETDEVCAPSA MidiOutGetDevCapsA; // 0x6ad521dc
    LPMIDIOUTGETDEVCAPSW MidiOutGetDevCapsW; // 0x6ad521d8
    LPMIDIOUTGETERRORTEXTA MidiOutGetErrorTextA; // 0x6ad521d4
    LPMIDIOUTGETERRORTEXTW MidiOutGetErrorTextW; // 0x6ad521d0
    LPMIDIOUTGETID MidiOutGetID; // 0x6ad521cc
    LPMIDIOUTGETNUMDEVS MidiOutGetNumDevs; // 0x6ad521c8
    LPMIDIOUTGETVOLUME MidiOutGetVolume; // 0x6ad521c4
    LPMIDIOUTLONGMSG MidiOutLongMsg; // 0x6ad521c0
    LPMIDIOUTMESSAGE MidiOutMessage; // 0x6ad521bc
    LPMIDIOUTOPEN MidiOutOpen; // 0x6ad521b8
    LPMIDIOUTPREPAREHEADER MidiOutPrepareHeader; // 0x6ad521b4
    LPMIDIOUTRESET MidiOutReset; // 0x6ad521b0
    LPMIDIOUTSETVOLUME MidiOutSetVolume; // 0x6ad521ac
    LPMIDIOUTSHORTMSG MidiOutShortMsg; // 0x6ad521a8
    LPMIDIOUTUNPREPAREHEADER MidiOutUnprepareHeader; // 0x6ad521a4
    LPMIDISTREAMCLOSE MidiStreamClose; // 0x6ad521a0
    LPMIDISTREAMOPEN MidiStreamOpen; // 0x6ad5219c
    LPMIDISTREAMOUT MidiStreamOut; // 0x6ad52198
    LPMIDISTREAMPAUSE MidiStreamPause; // 0x6ad52194
    LPMIDISTREAMPOSITION MidiStreamPosition; // 0x6ad52190
    LPMIDISTREAMPROPERTY MidiStreamProperty; // 0x6ad5218c
    LPMIDISTREAMRESTART MidiStreamRestart; // 0x6ad52188
    LPMIDISTREAMSTOP MidiStreamStop; // 0x6ad52184
    LPMIXERCLOSE MixerClose; // 0x6ad52180
    LPMIXERGETCONTROLDETAILSA MixerGetControlDetailsA; // 0x6ad5217c
    LPMIXERGETCONTROLDETAILSW MixerGetControlDetailsW; // 0x6ad52178
    LPMIXERGETDEVCAPSA MixerGetDevCapsA; // 0x6ad52174
    LPMIXERGETDEVCAPSW MixerGetDevCapsW; // 0x6ad52170
    LPMIXERGETID MixerGetID; // 0x6ad5216c
    LPMIXERGETLINECONTROLSA MixerGetLineControlsA; // 0x6ad522ec
    LPMIXERGETLINECONTROLSW MixerGetLineControlsW; // 0x6ad522e8
    LPMIXERGETLINEINFOA MixerGetLineInfoA; // 0x6ad522ac
    LPMIXERGETLINEINFOW MixerGetLineInfoW; // 0x6ad522a8
    LPMIXERGETNUMDEVS MixerGetNumDevs; // 0x6ad522b4
    LPMIXERMESSAGE MixerMessage; // 0x6ad52168
    LPMIXEROPEN MixerOpen; // 0x6ad52164
    LPMIXERSETCONTROLDETAILS MixerSetControlDetails; // 0x6ad522c8
    LPMMDRVINSTALL MmDrvInstall;
    LPMMGETCURRENTTASK MmGetCurrentTask; // 0x6ad52064
    LPMMIOADVANCE MmioAdvance; // 0x6ad52160
    LPMMIOASCEND MmioAscend; // 0x6ad5215c
    LPMMIOCLOSE MmioClose; // 0x6ad52158
    LPMMIOCREATECHUNK MmioCreateChunk; // 0x6ad522d4
    LPMMIODESCEND MmioDescend; // 0x6ad522e0
    LPMMIOFLUSH MmioFlush; // 0x6ad52154
    LPMMIOGETINFO MmioGetInfo; // 0x6ad52150
    LPMMIOINSTALLIOPROCA MmioInstallIOProcA; // 0x6ad5214c
    LPMMIOINSTALLIOPROCW MmioInstallIOProcW; // 0x6ad52148
    LPMMIOOPENA MmioOpenA; // 0x6ad52144
    LPMMIOOPENW MmioOpenW; // 0x6ad52140
    LPMMIOREAD MmioRead; // 0x6ad522c0
    LPMMIORENAMEA MmioRenameA; // 0x6ad5213c
    LPMMIORENAMEW MmioRenameW; // 0x6ad52138
    LPMMIOSEEK MmioSeek; // 0x6ad52134
    LPMMIOSENDMESSAGE MmioSendMessage; // 0x6ad52130
    LPMMIOSETBUFFER MmioSetBuffer; // 0x6ad5212c
    LPMMIOSETINFO MmioSetInfo; // 0x6ad52128
    LPMMIOSTRINGTOFOURCCA MmioStringToFOURCCA; // 0x6ad52124
    LPMMIOSTRINGTOFOURCCW MmioStringToFOURCCW; // 0x6ad52120
    LPMMIOWRITE MmioWrite; // 0x6ad522a0
    LPMMSYSTEMGETVERSION MmsystemGetVersion; // 0x6ad5211c
    LPMMTASKBLOCK MmTaskBlock; // 0x6ad52070
    LPMMTASKCREATE MmTaskCreate; // 0x6ad52074
    LPMMTASKSIGNAL MmTaskSignal; // 0x6ad5206c
    LPMMTASKYIELD MmTaskYield; // 0x6ad52068
    LPMOD32MESSAGE Mod32Message;
    LPMXD32MESSAGE Mxd32Message;
    LPNOTIFYCALLBACKDATA NotifyCallbackData;
    LPOPENDRIVER OpenDriver; // 0x6ad52118
    LPPLAYSOUND PlaySound; // 0x6ad52114
    LPPLAYSOUNDA PlaySoundA; // 0x6ad52110
    LPPLAYSOUNDW PlaySoundW; // 0x6ad5210c
    LPSENDDRIVERMESSAGE SendDriverMessage; // 0x6ad52108
    LPSNDPLAYSOUNDA SndPlaySoundA; // 0x6ad52104
    LPSNDPLAYSOUNDW SndPlaySoundW; // 0x6ad52100
    LPTID32MESSAGE Tid32Message;
    LPTIMEBEGINPERIOD TimeBeginPeriod; // 0x6ad522a4
    LPTIMEENDPERIOD TimeEndPeriod; // 0x6ad5229c
    LPTIMEGETDEVCAPS TimeGetDevCaps; // 0x6ad520fc
    LPTIMEGETSYSTEMTIME TimeGetSystemTime; // 0x6ad520f8
    LPTIMEGETTIME TimeGetTime; // 0x6ad59d20
    LPTIMEKILLEVENT TimeKillEvent; // 0x6ad522c4
    LPTIMESETEVENT TimeSetEvent; // 0x6ad522b0
    LPWAVEINADDBUFFER WaveInAddBuffer; // 0x6ad520f4
    LPWAVEINCLOSE WaveInClose; // 0x6ad520f0
    LPWAVEINGETDEVCAPSA WaveInGetDevCapsA; // 0x6ad522d0
    LPWAVEINGETDEVCAPSW WaveInGetDevCapsW; // 0x6ad522cc
    LPWAVEINGETERRORTEXTA WaveInGetErrorTextA; // 0x6ad520ec
    LPWAVEINGETERRORTEXTW WaveInGetErrorTextW; // 0x6ad520e8
    LPWAVEINGETID WaveInGetID; // 0x6ad520e4
    LPWAVEINGETNUMDEVS WaveInGetNumDevs; // 0x6ad522e4
    LPWAVEINGETPOSITION WaveInGetPosition; // 0x6ad520e0
    LPWAVEINMESSAGE WaveInMessage; // 0x6ad520dc
    LPWAVEINOPEN WaveInOpen; // 0x6ad520d8
    LPWAVEINPREPAREHEADER WaveInPrepareHeader; // 0x6ad520d4
    LPWAVEINRESET WaveInReset; // 0x6ad520d0
    LPWAVEINSTART WaveInStart; // 0x6ad520cc
    LPWAVEINSTOP WaveInStop; // 0x6ad520c8
    LPWAVEINUNPREPAREHEADER WaveInUnprepareHeader; // 0x6ad520c4
    LPWAVEOUTBREAKLOOP WaveOutBreakLoop; // 0x6ad520c0
    LPWAVEOUTCLOSE WaveOutClose; // 0x6ad520bc
    LPWAVEOUTGETDEVCAPSA WaveOutGetDevCapsA; // 0x6ad522bc
    LPWAVEOUTGETDEVCAPSW WaveOutGetDevCapsW; // 0x6ad522b8
    LPWAVEOUTGETERRORTEXTA WaveOutGetErrorTextA; // 0x6ad520b8
    LPWAVEOUTGETERRORTEXTW WaveOutGetErrorTextW; // 0x6ad520b4
    LPWAVEOUTGETID WaveOutGetID; // 0x6ad520b0
    LPWAVEOUTGETNUMDEVS WaveOutGetNumDevs; // 0x6ad520ac
    LPWAVEOUTGETPITCH WaveOutGetPitch; // 0x6ad520a8
    LPWAVEOUTGETPLAYBACKRATE WaveOutGetPlaybackRate; // 0x6ad520a4
    LPWAVEOUTGETPOSITION WaveOutGetPosition; // 0x6ad520a0
    //LPWAVEOUTGETVOLUME WaveOutGetVolume;
    LPWAVEOUTMESSAGE WaveOutMessage; // 0x6ad5209c
    LPWAVEOUTOPEN WaveOutOpen; // 0x6ad52098
    LPWAVEOUTPAUSE WaveOutPause; // 0x6ad52094
    LPWAVEOUTPREPAREHEADER WaveOutPrepareHeader; // 0x6ad52090
    LPWAVEOUTRESET WaveOutReset; // 0x6ad5208c
    LPWAVEOUTRESTART WaveOutRestart; // 0x6ad52088
    LPWAVEOUTSETPITCH WaveOutSetPitch; // 0x6ad52084
    LPWAVEOUTSETPLAYBACKRATE WaveOutSetPlaybackRate; // 0x6ad52080
    //LPWAVEOUTSETVOLUME WaveOutSetVolume;
    LPWAVEOUTUNPREPAREHEADER WaveOutUnprepareHeader; // 0x6ad5207c
    LPWAVEOUTWRITE WaveOutWrite; // 0x6ad52078
    LPWID32MESSAGE Wid32Message;
    LPWOD32MESSAGE Wod32Message;
    LPWOW32DRIVERCALLBACK WOW32DriverCallback;
    LPWOW32RESOLVEMULTIMEDIAHANDLE WOW32ResolveMultiMediaHandle;
    LPWOWAPPEXIT WOWAppExit;
};

extern struct ModuleStateContainer ModuleState;