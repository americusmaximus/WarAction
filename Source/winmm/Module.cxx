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

#include "Audio.hxx"
#include "Module.hxx"
#include "State.hxx"

#include <stdio.h>

#define MM_CREATIVE 2
#define MM_CREATIVE_AUX_CD 401

#define WINMM_DRIVER_VERSION 1

// Original: ogg-winmm virtual CD
#define AUDIO_DEVICE_NAME "WINMM Virtual CD Drive"

#define MAX_AUDIO_DEVICE_COUNT 1

#define MISSING_DEVICE_ID 0

ModuleStateContainer ModuleState;

extern "C" DWORD WINAPI Aux32Message(UINT uDeviceID, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    if (ModuleState.Aux32Message == NULL)
    {
        ModuleState.Aux32Message = (LPAUX32MESSAGE)GetProcAddress(ModuleState.Module, METHOD_AUX32MESSAGE_NAME);
    }

    return ModuleState.Aux32Message(uDeviceID, uMsg, dwInstance, dwParam1, dwParam2);
}

// 0x6ad41fd0
extern "C" MMRESULT WINAPI AuxGetDevCapsA(UINT_PTR uDeviceID, LPAUXCAPSA pac, UINT cbac)
{
    pac->wMid = MM_CREATIVE;
    pac->wPid = MM_CREATIVE_AUX_CD;
    pac->vDriverVersion = WINMM_DRIVER_VERSION;

    strcpy(pac->szPname, AUDIO_DEVICE_NAME);

    pac->wTechnology = AUXCAPS_CDAUDIO;
    pac->dwSupport = AUXCAPS_VOLUME;

    return MMSYSERR_NOERROR;
}

// 0x6ad427b0
extern "C" MMRESULT WINAPI AuxGetDevCapsW(UINT_PTR uDeviceID, LPAUXCAPSW pac, UINT cbac)
{
    if (ModuleState.AuxGetDevCapsW == NULL)
    {
        ModuleState.AuxGetDevCapsW = (LPAUXGETDEVCAPSW)GetProcAddress(ModuleState.Module, METHOD_AUXGETDEVCAPSW_NAME);
    }

    return ModuleState.AuxGetDevCapsW(uDeviceID, pac, cbac);
}

// 0x6ad41fc0
extern "C" UINT WINAPI AuxGetNumDevs()
{
    return MAX_AUDIO_DEVICE_COUNT;
}

// 0x6ad42030
extern "C" MMRESULT WINAPI AuxGetVolume(UINT uDeviceID, LPDWORD lpdwVolume)
{
    *lpdwVolume = State.Volume;

    return MMSYSERR_NOERROR;
}

// 0x6ad42800
extern "C" MMRESULT WINAPI AuxOutMessage(UINT uDeviceID, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2)
{
    if (ModuleState.AuxOutMessage == NULL)
    {
        ModuleState.AuxOutMessage = (LPAUXOUTMESSAGE)GetProcAddress(ModuleState.Module, METHOD_AUXOUTMESSAGE_NAME);
    }

    return ModuleState.AuxOutMessage(uDeviceID, uMsg, dw1, dw2);
}

// 0x6ad42060
extern "C" MMRESULT WINAPI AuxSetVolume(UINT uDeviceID, DWORD dwVolume)
{
    static DWORD volume = DEFAULT_VOLUME; // 0x6ad4c004

    if (dwVolume != volume)
    {
        volume = dwVolume;

        SelectAudioVolume((INT)((LOWORD(dwVolume) * 100) / 65535.0f));

        State.Volume = dwVolume;
    }

    return MMSYSERR_NOERROR;
}

// 0x6ad42850
extern "C" LRESULT WINAPI CloseDriver(HDRVR hDriver, LPARAM lParam1, LPARAM lParam2)
{
    if (ModuleState.CloseDriver == NULL)
    {
        ModuleState.CloseDriver = (LPCLOSEDRIVER)GetProcAddress(ModuleState.Module, METHOD_CLOSEDRIVER_NAME);
    }

    return ModuleState.CloseDriver(hDriver, lParam1, lParam2);
}

// 0x6ad428a0
extern "C" LRESULT WINAPI DefDriverProc(DWORD_PTR dwDriverIdentifier, HDRVR hdrvr, UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
    if (ModuleState.DefDriverProc == NULL)
    {
        ModuleState.DefDriverProc = (LPDEFDRIVERPROC)GetProcAddress(ModuleState.Module, METHOD_DEFDRIVERPROC_NAME);
    }

    return ModuleState.DefDriverProc(dwDriverIdentifier, hdrvr, uMsg, lParam1, lParam2);
}

// 0x6ad42900
extern "C" BOOL WINAPI DriverCallback(DWORD_PTR dwCallback, DWORD dwFlags, HDRVR hDevice, DWORD dwMsg, DWORD_PTR dwUser, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    if (ModuleState.DriverCallback == NULL)
    {
        ModuleState.DriverCallback = (LPDRIVERCALLBACK)GetProcAddress(ModuleState.Module, METHOD_DRIVERCALLBACK_NAME);
    }

    return ModuleState.DriverCallback(dwCallback, dwFlags, hDevice, dwMsg, dwUser, dwParam1, dwParam2);
}

// 0x6ad42970
extern "C" HMODULE WINAPI DrvGetModuleHandle(HDRVR hDriver)
{
    if (ModuleState.DrvGetModuleHandle == NULL)
    {
        ModuleState.DrvGetModuleHandle = (LPDRVGETMODULEHANDLE)GetProcAddress(ModuleState.Module, METHOD_DRVGETMODULEHANDLE_NAME);
    }

    return ModuleState.DrvGetModuleHandle(hDriver);
}

// 0x6ad429c0
extern "C" HMODULE WINAPI GetDriverModuleHandle(HDRVR hDriver)
{
    if (ModuleState.GetDriverModuleHandle == NULL)
    {
        ModuleState.GetDriverModuleHandle = (LPGETDRIVERMODULEHANDLE)GetProcAddress(ModuleState.Module, METHOD_GETDRIVERMODULEHANDLE_NAME);
    }

    return ModuleState.GetDriverModuleHandle(hDriver);
}

extern "C" DWORD WINAPI Joy32Message(UINT uJoyID, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    if (ModuleState.Joy32Message == NULL)
    {
        ModuleState.Joy32Message = (LPJOY32MESSAGE)GetProcAddress(ModuleState.Module, METHOD_JOY32MESSAGE_NAME);
    }

    return ModuleState.Joy32Message(uJoyID, uMsg, dwInstance, dwParam1, dwParam2);
}

// 0x6ad42a10
extern "C" MMRESULT WINAPI JoyConfigChanged(DWORD dwFlags)
{
    if (ModuleState.JoyConfigChanged == NULL)
    {
        ModuleState.JoyConfigChanged = (LPJOYCONFIGCHANGED)GetProcAddress(ModuleState.Module, METHOD_JOYCONFIGCHANGED_NAME);
    }

    return ModuleState.JoyConfigChanged(dwFlags);
}

// 0x6ad42a60
extern "C" MMRESULT WINAPI JoyGetDevCapsA(UINT_PTR uJoyID, LPJOYCAPSA pjc, UINT cbjc)
{
    if (ModuleState.JoyGetDevCapsA == NULL)
    {
        ModuleState.JoyGetDevCapsA = (LPJOYGETDEVCAPSA)GetProcAddress(ModuleState.Module, METHOD_JOYGETDEVCAPSA_NAME);
    }

    return ModuleState.JoyGetDevCapsA(uJoyID, pjc, cbjc);
}

// 0x6ad42ab0
extern "C" MMRESULT WINAPI JoyGetDevCapsW(UINT_PTR uJoyID, LPJOYCAPSW pjc, UINT cbjc)
{
    if (ModuleState.JoyGetDevCapsW == NULL)
    {
        ModuleState.JoyGetDevCapsW = (LPJOYGETDEVCAPSW)GetProcAddress(ModuleState.Module, METHOD_JOYGETDEVCAPSW_NAME);
    }

    return ModuleState.JoyGetDevCapsW(uJoyID, pjc, cbjc);
}

// 0x6ad42b00
extern "C" UINT WINAPI JoyGetNumDevs()
{
    if (ModuleState.JoyGetNumDevs == NULL)
    {
        ModuleState.JoyGetNumDevs = (LPJOYGETNUMDEVS)GetProcAddress(ModuleState.Module, METHOD_JOYGETNUMDEVS_NAME);
    }

    return ModuleState.JoyGetNumDevs();
}

// 0x6ad42b40
extern "C" MMRESULT WINAPI JoyGetPos(UINT uJoyID, LPJOYINFO pji)
{
    if (ModuleState.JoyGetPos == NULL)
    {
        ModuleState.JoyGetPos = (LPJOYGETPOS)GetProcAddress(ModuleState.Module, METHOD_JOYGETPOS_NAME);
    }

    return ModuleState.JoyGetPos(uJoyID, pji);
}

// 0x6ad42b90
extern "C" MMRESULT WINAPI JoyGetPosEx(UINT uJoyID, LPJOYINFOEX pji)
{
    if (ModuleState.JoyGetPosEx == NULL)
    {
        ModuleState.JoyGetPosEx = (LPJOYGETPOSEX)GetProcAddress(ModuleState.Module, METHOD_JOYGETPOSEX_NAME);
    }

    return ModuleState.JoyGetPosEx(uJoyID, pji);
}

// 0x6ad42be0
extern "C" MMRESULT WINAPI JoyGetThreshold(UINT uJoyID, LPUINT puThreshold)
{
    if (ModuleState.JoyGetThreshold == NULL)
    {
        ModuleState.JoyGetThreshold = (LPJOYGETTHRESHOLD)GetProcAddress(ModuleState.Module, METHOD_JOYSETTHRESHOLD_NAME);
    }

    return ModuleState.JoyGetThreshold(uJoyID, puThreshold);
}

// 0x6ad42c30
extern "C" MMRESULT WINAPI JoyReleaseCapture(UINT uJoyID)
{
    if (ModuleState.JoyReleaseCapture == NULL)
    {
        ModuleState.JoyReleaseCapture = (LPJOYRELEASECAPTURE)GetProcAddress(ModuleState.Module, METHOD_JOYRELEASECAPTURE_NAME);
    }

    return ModuleState.JoyReleaseCapture(uJoyID);
}

// 0x6ad42c80
extern "C" MMRESULT WINAPI JoySetCapture(HWND hwnd, UINT uJoyID, UINT uPeriod, BOOL fChanged)
{
    if (ModuleState.JoySetCapture == NULL)
    {
        ModuleState.JoySetCapture = (LPJOYSETCAPTURE)GetProcAddress(ModuleState.Module, METHOD_JOYSETCAPTURE_NAME);
    }

    return ModuleState.JoySetCapture(hwnd, uJoyID, uPeriod, fChanged);
}

// 0x6ad42cd0
extern "C" MMRESULT WINAPI JoySetThreshold(UINT uJoyID, UINT uThreshold)
{
    if (ModuleState.JoySetThreshold == NULL)
    {
        ModuleState.JoySetThreshold = (LPJOYSETTHRESHOLD)GetProcAddress(ModuleState.Module, METHOD_JOYSETTHRESHOLD_NAME);
    }

    return ModuleState.JoySetThreshold(uJoyID, uThreshold);
}

extern "C" DWORD WINAPI Mci32Message(DWORD dwApi, DWORD dwF1, DWORD dwF2, DWORD dwF3, DWORD dwF4)
{
    if (ModuleState.Mci32Message == NULL)
    {
        ModuleState.Mci32Message = (LPMCI32MESSAGE)GetProcAddress(ModuleState.Module, METHOD_MCI32MESSAGE_NAME);
    }

    return ModuleState.Mci32Message(dwApi, dwF1, dwF2, dwF3, dwF4);
}

// 0x6ad45520
extern "C" BOOL WINAPI MciDriverNotify(HWND hwndCallback, UINT uDeviceID, UINT uStatus)
{
    if (ModuleState.MciDriverNotify == NULL)
    {
        ModuleState.MciDriverNotify = (LPMCIDRIVERNOTIFY)GetProcAddress(ModuleState.Module, METHOD_MCIDRIVERNOTIFY_NAME);
    }

    return ModuleState.MciDriverNotify(hwndCallback, uDeviceID, uStatus);
}

// 0x6ad454d0
extern "C" UINT WINAPI MciDriverYield(UINT uDeviceID)
{
    if (ModuleState.MciDriverYield == NULL)
    {
        ModuleState.MciDriverYield = (LPMCIDRIVERYIELD)GetProcAddress(ModuleState.Module, METHOD_MCIDRIVERYIELD_NAME);
    }

    return ModuleState.MciDriverYield(uDeviceID);
}

// 0x6ad42d20
extern "C" BOOL WINAPI MciExecute(LPCSTR pszCommand)
{
    if (ModuleState.MciExecute == NULL)
    {
        ModuleState.MciExecute = (LPMCIEXECUTE)GetProcAddress(ModuleState.Module, METHOD_MCIEXECUTE_NAME);
    }

    return ModuleState.MciExecute(pszCommand);
}

// 0x6ad45570
extern "C" BOOL WINAPI MciFreeCommandResource(UINT wTable)
{
    if (ModuleState.MciFreeCommandResource == NULL)
    {
        ModuleState.MciFreeCommandResource = (LPMCIFREECOMMANDRESOURCE)GetProcAddress(ModuleState.Module, METHOD_MCIFREECOMMANDRESOURCE_NAME);
    }

    return ModuleState.MciFreeCommandResource(wTable);
}

// 0x6ad42d70
extern "C" HANDLE WINAPI MciGetCreatorTask(MCIDEVICEID mciId)
{
    if (ModuleState.MciGetCreatorTask == NULL)
    {
        ModuleState.MciGetCreatorTask = (LPMCIGETCREATORTASK)GetProcAddress(ModuleState.Module, METHOD_MCIGETCREATORTASK_NAME);
    }

    return ModuleState.MciGetCreatorTask(mciId);
}

// 0x6ad42dc0
extern "C" MCIDEVICEID WINAPI MciGetDeviceIDA(LPCSTR pszDevice)
{
    if (ModuleState.MciGetDeviceIDA == NULL)
    {
        ModuleState.MciGetDeviceIDA = (LPMCIGETDEVICEIDA)GetProcAddress(ModuleState.Module, METHOD_MCIGETDEVICEIDA_NAME);
    }

    return ModuleState.MciGetDeviceIDA(pszDevice);
}

// 0x6ad42e10
extern "C" MCIDEVICEID WINAPI MciGetDeviceIDFromElementIDA(DWORD dwElementID, LPCSTR lpstrType)
{
    if (ModuleState.MciGetDeviceIDFromElementIDA == NULL)
    {
        ModuleState.MciGetDeviceIDFromElementIDA = (LPMCIGETDEVICEIDFROMELEMENTIDA)GetProcAddress(ModuleState.Module, METHOD_MCIGETDEVICEIDFROMELEMENTIDA_NAME);
    }

    return ModuleState.MciGetDeviceIDFromElementIDA(dwElementID, lpstrType);
}

// 0x6ad42e60
extern "C" MCIDEVICEID WINAPI MciGetDeviceIDFromElementIDW(DWORD dwElementID, LPCWSTR lpstrType)
{
    if (ModuleState.MciGetDeviceIDFromElementIDW == NULL)
    {
        ModuleState.MciGetDeviceIDFromElementIDW = (LPMCIGETDEVICEIDFROMELEMENTIDW)GetProcAddress(ModuleState.Module, METHOD_MCIGETDEVICEIDFROMELEMENTIDW_NAME);
    }

    return ModuleState.MciGetDeviceIDFromElementIDW(dwElementID, lpstrType);
}

// 0x6ad42eb0
extern "C" MCIDEVICEID WINAPI MciGetDeviceIDW(LPCWSTR pszDevice)
{
    if (ModuleState.MciGetDeviceIDW == NULL)
    {
        ModuleState.MciGetDeviceIDW = (LPMCIGETDEVICEIDW)GetProcAddress(ModuleState.Module, METHOD_MCIGETDEVICEIDW_NAME);
    }

    return ModuleState.MciGetDeviceIDW(pszDevice);
}

// 0x6ad45480
extern "C" DWORD_PTR WINAPI MciGetDriverData(MCIDEVICEID wDeviceID)
{
    if (ModuleState.MciGetDriverData == NULL)
    {
        ModuleState.MciGetDriverData = (LPMCIGETDRIVERDATA)GetProcAddress(ModuleState.Module, METHOD_MCIGETDRIVERDATA_NAME);
    }

    return ModuleState.MciGetDriverData(wDeviceID);
}

// 0x6ad42240
extern "C" BOOL WINAPI MciGetErrorStringA(MCIERROR mcierr, LPSTR pszText, UINT cchText)
{
    if (ModuleState.MciGetErrorStringA == NULL)
    {
        ModuleState.MciGetErrorStringA = (LPMCIGETERRORSTRINGA)GetProcAddress(ModuleState.Module, METHOD_MCIGETERRORSTRINGA_NAME);
    }

    return ModuleState.MciGetErrorStringA(mcierr, pszText, cchText);
}

// 0x6ad42290
extern "C" BOOL WINAPI MciGetErrorStringW(MCIERROR mcierr, LPWSTR pszText, UINT cchText)
{
    if (ModuleState.MciGetErrorStringW == NULL)
    {
        ModuleState.MciGetErrorStringW = (LPMCIGETERRORSTRINGW)GetProcAddress(ModuleState.Module, METHOD_MCIGETERRORSTRINGW_NAME);
    }

    return ModuleState.MciGetErrorStringW(mcierr, pszText, cchText);
}

// 0x6ad42f00
extern "C" YIELDPROC WINAPI MciGetYieldProc(MCIDEVICEID mciId, LPDWORD pdwYieldData)
{
    if (ModuleState.MciGetYieldProc == NULL)
    {
        ModuleState.MciGetYieldProc = (LPMCIGETYIELDPROC)GetProcAddress(ModuleState.Module, METHOD_MCIGETYIELDPROC_NAME);
    }

    return ModuleState.MciGetYieldProc(mciId, pdwYieldData);
}

// 0x6ad455c0
extern "C" UINT WINAPI MciLoadCommandResource(HANDLE hInstance, LPCWSTR lpResName, UINT wType)
{
    if (ModuleState.MciLoadCommandResource == NULL)
    {
        ModuleState.MciLoadCommandResource = (LPMCILOADCOMMANDRESOURCE)GetProcAddress(ModuleState.Module, METHOD_MCILOADCOMMANDRESOURCE_NAME);
    }

    return ModuleState.MciLoadCommandResource(hInstance, lpResName, wType);
}

// 0x6ad41660
extern "C" MCIERROR WINAPI MciSendCommandA(MCIDEVICEID mciId, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    if (uMsg == MCI_OPEN)
    {
        LPMCI_OPEN_PARMSA parms = (LPMCI_OPEN_PARMSA)dwParam2;

        if ((dwParam1 & MCI_OPEN_TYPE_ID) && LOWORD(parms->lpstrDeviceType) == MCI_DEVTYPE_CD_AUDIO)
        {
            parms->wDeviceID = AUDIO_DEVICE_ID;

            return MMSYSERR_NOERROR;
        }

        if ((dwParam1 & MCI_OPEN_TYPE) && !(dwParam1 & MCI_OPEN_TYPE_ID))
        {
            if (strncmp(parms->lpstrDeviceType, "cdaudio", 8) == 0)
            {
                parms->wDeviceID = AUDIO_DEVICE_ID;

                return MMSYSERR_NOERROR;
            }
        }
    }

    if (mciId != AUDIO_DEVICE_ID && mciId != MISSING_DEVICE_ID && mciId != MCI_ALL_DEVICE_ID) { return MCIERR_UNRECOGNIZED_COMMAND; }

    if (uMsg == MCI_SET)
    {
        LPMCI_SET_PARMS parms = (LPMCI_SET_PARMS)dwParam2;

        if (dwParam1 & MCI_SET_TIME_FORMAT) { State.TimeFormat = parms->dwTimeFormat; }
    }
    else if (uMsg == MCI_PLAY)
    {
        LPMCI_PLAY_PARMS parms = (LPMCI_PLAY_PARMS)dwParam2;

        if (dwParam1 & MCI_FROM)
        {
            if (State.TimeFormat == MCI_FORMAT_MILLISECONDS)
            {
                AudioState.Context.First = State.Tracks.Current;

                UINT position = State.Tracks.Tracks[AudioState.Context.First].Position;

                if (AudioState.Context.First < State.Tracks.Count && parms->dwFrom < position)
                {
                    for (AudioState.Context.First = AudioState.Context.First + 1;
                        parms->dwFrom < position; AudioState.Context.First = AudioState.Context.First + 1)
                    {
                        position = State.Tracks.Tracks[AudioState.Context.First].Position;

                        if (AudioState.Context.First == State.Tracks.Count) { break; }
                    }
                }

                AudioState.Position = ((parms->dwFrom - position) / 1000.0);
            }
            else if (State.TimeFormat == MCI_FORMAT_TMSF)
            {
                AudioState.Context.First = MCI_TMSF_TRACK(parms->dwFrom);

                AudioState.Position = MCI_TMSF_MINUTE(parms->dwFrom) * 60 + MCI_TMSF_SECOND(parms->dwFrom);
            }
            else { AudioState.Context.First = parms->dwFrom; }

            if (AudioState.Context.First < State.Tracks.Current) { AudioState.Context.First = State.Tracks.Current; }

            AudioState.Context.Last = AudioState.Context.First;

            if (State.Tracks.Count < AudioState.Context.First)
            {
                AudioState.Context.First = State.Tracks.Count;
                AudioState.Context.Last = AudioState.Context.First;
            }
        }
        
        if (dwParam1 & MCI_TO)
        {
            if (State.TimeFormat == MCI_FORMAT_MILLISECONDS)
            {
                if (AudioState.Context.First < State.Tracks.Count)
                {
                    AudioState.Context.Last = AudioState.Context.First;

                    for (AudioState.Context.First = AudioState.Context.First + 1;
                        State.Tracks.Tracks[AudioState.Context.First].Position + State.Tracks.Tracks[AudioState.Context.First].Length <= parms->dwFrom;
                        AudioState.Context.First = AudioState.Context.First + 1)
                    {
                        AudioState.Context.Last = AudioState.Context.Last + 1;

                        if (AudioState.Context.Last == State.Tracks.Count) { break; }
                    }
                }
            }
            else if (State.TimeFormat == MCI_FORMAT_TMSF) { AudioState.Context.Last = MCI_TMSF_TRACK(parms->dwTo); }
            else { AudioState.Context.Last = parms->dwTo; }

            if (AudioState.Context.Last < AudioState.Context.First) { AudioState.Context.Last = AudioState.Context.First; }
            if (AudioState.Context.Last > State.Tracks.Count) { AudioState.Context.Last = State.Tracks.Count; }
        }

        if (AudioState.Context.First != 0 && (dwParam1 & MCI_FROM))
        {
            EnterCriticalSection(&State.Mutex);

            if (AudioState.Worker != NULL) { AudioState.IsActive = TRUE; }

            AudioState.Alerts.IsActive = (dwParam1 & MCI_NOTIFY);
            AudioState.Alerts.Callback = AudioState.Alerts.IsActive ? NULL : parms->dwCallback;

            LeaveCriticalSection(&State.Mutex);

            if (AudioState.Worker == NULL)
            {
                AudioState.Worker = CreateThread(NULL, 0, AudioWorker, &AudioState.Context, 0, NULL);
            }

            AudioState.State = TRUE;
        }
    }
    else if (uMsg == MCI_SEEK)
    {
        LPMCI_SEEK_PARMS parms = (LPMCI_SEEK_PARMS)dwParam2;

        if (dwParam1 & MCI_TO)
        {
            AudioState.Context.First = parms->dwTo;
            AudioState.Context.Last = parms->dwTo;
        }
    }
    else if (uMsg == MCI_STOP) { AudioState.State = FALSE; }
    else if (uMsg == MCI_GETDEVCAPS)
    {
        LPMCI_GETDEVCAPS_PARMS parms = (LPMCI_GETDEVCAPS_PARMS)dwParam2;

        if (dwParam1 & MCI_NOTIFY) { parms->dwReturn = MAKEMCIRESOURCE(TRUE, MCI_TRUE); }
        
        if (dwParam1 & MCI_GETDEVCAPS_ITEM)
        {
            if (parms->dwItem == MCI_GETDEVCAPS_DEVICE_TYPE) { parms->dwReturn = MCI_DEVTYPE_CD_AUDIO; }
            else if (parms->dwItem == MCI_GETDEVCAPS_CAN_EJECT) { parms->dwReturn = MAKEMCIRESOURCE(FALSE, MCI_FALSE); }
        }
    }
    else if (uMsg == MCI_STATUS)
    {
        LPMCI_STATUS_PARMS parms = (LPMCI_STATUS_PARMS)dwParam2;

        parms->dwReturn = 0;

        if (dwParam1 & MCI_STATUS_ITEM)
        {
            if (parms->dwItem == MCI_STATUS_LENGTH)
            {
                if (State.Tracks.Current <= parms->dwTrack && parms->dwTrack <= State.Tracks.Count)
                {
                    CONST UINT length = State.Tracks.Tracks[parms->dwTrack].Length;

                    if (length != 0)
                    {
                        if (State.TimeFormat == MCI_FORMAT_MILLISECONDS) { parms->dwReturn = length; }
                        else { parms->dwReturn = MCI_MAKE_MSF(length / 60000, (length / 1000) % 60, 0); }
                    }
                }
            }
            else if (parms->dwItem == MCI_STATUS_POSITION)
            {
                if (State.TimeFormat == MCI_FORMAT_MILLISECONDS)
                {
                    if (dwParam1 & MCI_TRACK) { parms->dwReturn = State.Tracks.Tracks[parms->dwTrack].Position; }
                    else
                    {
                        EnterCriticalSection(&State.Mutex);

                        if (AudioState.Worker != NULL)
                        {
                            parms->dwReturn =
                                State.Tracks.Tracks[AudioState.Track].Position + (UINT)(1000.0f * AcquireAudioPosition());
                        }

                        LeaveCriticalSection(&State.Mutex);
                    }
                }
            }
            else if (parms->dwItem == MCI_STATUS_NUMBER_OF_TRACKS) { parms->dwReturn = State.Tracks.Max + 1; }
            else if (parms->dwItem == MCI_STATUS_MODE) { parms->dwReturn = AudioState.State ? MCI_MODE_PLAY : MCI_MODE_STOP; }
            else if (parms->dwItem == MCI_STATUS_MEDIA_PRESENT) { parms->dwReturn = State.Tracks.Count > 0; }
            else if (parms->dwItem == MCI_STATUS_TIME_FORMAT) { parms->dwReturn = State.TimeFormat; }
            else if (parms->dwItem == MCI_STATUS_READY) { parms->dwReturn = MAKEMCIRESOURCE(TRUE, MCI_TRUE); }
            else if (parms->dwItem == MCI_STATUS_CURRENT_TRACK) { parms->dwReturn = AudioState.Context.First; }
            else if (parms->dwItem == MCI_CDA_STATUS_TYPE_TRACK)
            {
                parms->dwReturn =
                    State.Tracks.Current <= parms->dwTrack && parms->dwTrack <= State.Tracks.Count
                    ? MCI_CDA_TRACK_AUDIO : MCI_CDA_TRACK_OTHER;
            }
        }
    }

    return MMSYSERR_NOERROR;
}

// 0x6ad42f50
extern "C" UINT WINAPI MciSendCommandW(MCIDEVICEID mciId, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    if (ModuleState.MciSendCommandW == NULL)
    {
        ModuleState.MciSendCommandW = (LPMCISENDCOMMANDW)GetProcAddress(ModuleState.Module, METHOD_MCISENDCOMMANDW_NAME);
    }

    return ModuleState.MciSendCommandW(mciId, uMsg, dwParam1, dwParam2);
}

// 0x6ad41d30
extern "C" MCIERROR WINAPI MciSendStringA(LPCTSTR lpstrCommand, LPTSTR lpstrReturnString, UINT uReturnLength, HANDLE hwndCallback)
{
    if (strstr(lpstrCommand, "status") != NULL && strstr(lpstrCommand, "mode") != NULL)
    {
        if (lpstrReturnString != NULL && AudioState.State) { strcpy(lpstrReturnString, "playing"); }
        else { *lpstrReturnString = NULL; }

        return MMSYSERR_NOERROR;
    }

    if (strstr(lpstrCommand, "sysinfo") != NULL)
    {
        strcpy(lpstrReturnString, "cd");

        return MMSYSERR_NOERROR;
    }

    if (strstr(lpstrCommand, "stop cd") != NULL || strstr(lpstrCommand, "stop cdaudio") != NULL)
    {
        AudioState.State = FALSE;

        return MMSYSERR_NOERROR;
    }

    if (strstr(lpstrCommand, "pause cdaudio") != NULL)
    {
        PauseAudio();

        return MMSYSERR_NOERROR;
    }

    if (strstr(lpstrCommand, "play cdaudio to ") != NULL)
    {
        StartAudio();

        return MMSYSERR_NOERROR;
    }

    INT start = DEFAULT_TRACK_INDEX;
    INT end = DEFAULT_TRACK_INDEX;

    DWORD_PTR cmd = MCI_TO | MCI_FROM;

    MCI_PLAY_PARMS parms;
    ZeroMemory(&parms, sizeof(MCI_PLAY_PARMS));

    if (sscanf(lpstrCommand, "play cdaudio from %d to %d notify", &start, &end) == 2
        || sscanf(lpstrCommand, "play cdx from %d to %d, ret", &start, &end) == 2)
    {
        parms.dwFrom = start;
        parms.dwTo = end;

        if (strstr(lpstrCommand, "notify") != NULL)
        {
            cmd = MCI_TO | MCI_FROM | MCI_NOTIFY;
            parms.dwCallback = (DWORD_PTR)hwndCallback;
        }

        MciSendCommandA(AUDIO_DEVICE_ID, MCI_PLAY, cmd, (DWORD_PTR)&parms);

        return MMSYSERR_NOERROR;
    }
    else if (sscanf(lpstrCommand, "play cd from %d to %d", &start, &end) == 2
        || sscanf(lpstrCommand, "play cdx from %d to %d", &start, &end) == 2)
    {
        parms.dwFrom = start;
        parms.dwTo = end;

        MciSendCommandA(AUDIO_DEVICE_ID, MCI_PLAY, cmd, (DWORD_PTR)&parms);

        return MMSYSERR_NOERROR;
    }

    if (ModuleState.MciSendStringA == NULL)
    {
        ModuleState.MciSendStringA = (LPMCISENDSTRINGA)GetProcAddress(ModuleState.Module, METHOD_MCISENDSTRINGA_NAME);
    }

    return ModuleState.MciSendStringA(lpstrCommand, lpstrReturnString, uReturnLength, hwndCallback);
}

// 0x6ad42fa0
extern "C" MCIERROR WINAPI MciSendStringW(LPCWSTR cmd, LPWSTR ret, UINT uReturnLength, HANDLE hwndCallback)
{
    if (ModuleState.MciSendStringW == NULL)
    {
        ModuleState.MciSendStringW = (LPMCISENDSTRINGW)GetProcAddress(ModuleState.Module, METHOD_MCISENDSTRINGW_NAME);
    }

    return ModuleState.MciSendStringW(cmd, ret, uReturnLength, hwndCallback);
}

// 0x6ad45430
extern "C" BOOL WINAPI MciSetDriverData(MCIDEVICEID wDeviceID, DWORD_PTR dwData)
{
    if (ModuleState.MciSetDriverData == NULL)
    {
        ModuleState.MciSetDriverData = (LPMCISETDRIVERDATA)GetProcAddress(ModuleState.Module, METHOD_MCISETDRIVERDATA_NAME);
    }

    return ModuleState.MciSetDriverData(wDeviceID, dwData);
}

// 0x6ad42ff0
extern "C" BOOL WINAPI MciSetYieldProc(MCIDEVICEID mciId, YIELDPROC fpYieldProc, DWORD dwYieldData)
{
    if (ModuleState.MciSetYieldProc == NULL)
    {
        ModuleState.MciSetYieldProc = (LPMCISETYIELDPROC)GetProcAddress(ModuleState.Module, METHOD_MCISETYIELDPROC_NAME);
    }

    return ModuleState.MciSetYieldProc(mciId, fpYieldProc, dwYieldData);
}

extern "C" DWORD WINAPI Mid32Message(UINT uID, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    if (ModuleState.Mid32Message == NULL)
    {
        ModuleState.Mid32Message = (LPMID32MESSAGE)GetProcAddress(ModuleState.Module, METHOD_MID32MESSAGE_NAME);
    }

    return ModuleState.Mid32Message(uID, uMsg, dwInstance, dwParam1, dwParam2);
}

// 0x6ad43040
extern "C" MMRESULT WINAPI MidiConnect(HMIDI hmi, HMIDIOUT hmo, LPVOID pReserved)
{
    if (ModuleState.MidiConnect == NULL)
    {
        ModuleState.MidiConnect = (LPMIDICONNECT)GetProcAddress(ModuleState.Module, METHOD_MIDICONNECT_NAME);
    }

    return ModuleState.MidiConnect(hmi, hmo, pReserved);
}

// 0x6ad52228
extern "C" MMRESULT WINAPI MidiDisconnect(HMIDI hmi, HMIDIOUT hmo, LPVOID pReserved)
{
    if (ModuleState.MidiDisconnect == NULL)
    {
        ModuleState.MidiDisconnect = (LPMIDIDISCONNECT)GetProcAddress(ModuleState.Module, METHOD_MIDIDISCONNECT_NAME);
    }

    return ModuleState.MidiDisconnect(hmi, hmo, pReserved);
}

// 0x6ad430e0
extern "C" MMRESULT WINAPI MidiInAddBuffer(HMIDIIN hmi, LPMIDIHDR pmh, UINT cbmh)
{
    if (ModuleState.MidiInAddBuffer == NULL)
    {
        ModuleState.MidiInAddBuffer = (LPMIDIINADDBUFFER)GetProcAddress(ModuleState.Module, METHOD_MIDIINADDBUFFER_NAME);
    }

    return ModuleState.MidiInAddBuffer(hmi, pmh, cbmh);
}

// 0x6ad43130
extern "C" MMRESULT WINAPI MidiInClose(HMIDIIN hmi)
{
    if (ModuleState.MidiInClose == NULL)
    {
        ModuleState.MidiInClose = (LPMIDIINCLOSE)GetProcAddress(ModuleState.Module, METHOD_MIDIINCLOSE_NAME);
    }

    return ModuleState.MidiInClose(hmi);
}

// 0x6ad43180
extern "C" MMRESULT WINAPI MidiInGetDevCapsA(UINT_PTR uDeviceID, LPMIDIINCAPSA pmic, UINT cbmic)
{
    if (ModuleState.MidiInGetDevCapsA == NULL)
    {
        ModuleState.MidiInGetDevCapsA = (LPMIDIINGETDEVCAPSA)GetProcAddress(ModuleState.Module, METHOD_MIDIINGETDEVCAPSA_NAME);
    }

    return ModuleState.MidiInGetDevCapsA(uDeviceID, pmic, cbmic);
}

// 0x6ad431d0
extern "C" MMRESULT WINAPI MidiInGetDevCapsW(UINT_PTR uDeviceID, LPMIDIINCAPSW pmic, UINT cbmic)
{
    if (ModuleState.MidiInGetDevCapsW == NULL)
    {
        ModuleState.MidiInGetDevCapsW = (LPMIDIINGETDEVCAPSW)GetProcAddress(ModuleState.Module, METHOD_MIDIINGETDEVCAPSW_NAME);
    }

    return ModuleState.MidiInGetDevCapsW(uDeviceID, pmic, cbmic);
}

// 0x6ad43220
extern "C" MMRESULT WINAPI MidiInGetErrorTextA(MMRESULT mmrError, LPSTR pszText, UINT cchText)
{
    if (ModuleState.MidiInGetErrorTextA == NULL)
    {
        ModuleState.MidiInGetErrorTextA = (LPMIDIINGETERRORTEXTA)GetProcAddress(ModuleState.Module, METHOD_MIDIINGETERRORTEXTA_NAME);
    }

    return ModuleState.MidiInGetErrorTextA(mmrError, pszText, cchText);
}

// 0x6ad43270
extern "C" MMRESULT WINAPI MidiInGetErrorTextW(MMRESULT mmrError, LPWSTR pszText, UINT cchText)
{
    if (ModuleState.MidiInGetErrorTextW == NULL)
    {
        ModuleState.MidiInGetErrorTextW = (LPMIDIINGETERRORTEXTW)GetProcAddress(ModuleState.Module, METHOD_MIDIINGETERRORTEXTW_NAME);
    }

    return ModuleState.MidiInGetErrorTextW(mmrError, pszText, cchText);
}

// 0x6ad432c0
extern "C" MMRESULT WINAPI MidiInGetID(HMIDIIN hmi, LPUINT puDeviceID)
{
    if (ModuleState.MidiInGetID == NULL)
    {
        ModuleState.MidiInGetID = (LPMIDIINGETID)GetProcAddress(ModuleState.Module, METHOD_MIDIINGETID_NAME);
    }

    return ModuleState.MidiInGetID(hmi, puDeviceID);
}

// 0x6ad43310
extern "C" UINT WINAPI MidiInGetNumDevs(VOID)
{
    if (ModuleState.MidiInGetNumDevs == NULL)
    {
        ModuleState.MidiInGetNumDevs = (LPMIDIINGETNUMDEVS)GetProcAddress(ModuleState.Module, METHOD_MIDIINGETNUMDEVS_NAME);
    }

    return ModuleState.MidiInGetNumDevs();
}

// 0x6ad43350
extern "C" MMRESULT WINAPI MidiInMessage(HMIDIIN hmi, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2)
{
    if (ModuleState.MidiInMessage == NULL)
    {
        ModuleState.MidiInMessage = (LPMIDIINMESSAGE)GetProcAddress(ModuleState.Module, METHOD_MIDIINMESSAGE_NAME);
    }

    return ModuleState.MidiInMessage(hmi, uMsg, dw1, dw2);
}

// 0x6ad433a0
extern "C" MMRESULT WINAPI MidiInOpen(LPHMIDIIN phmi, UINT uDeviceID, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen)
{
    if (ModuleState.MidiInOpen == NULL)
    {
        ModuleState.MidiInOpen = (LPMIDIINOPEN)GetProcAddress(ModuleState.Module, METHOD_MIDIINOPEN_NAME);
    }

    return ModuleState.MidiInOpen(phmi, uDeviceID, dwCallback, dwInstance, fdwOpen);
}

// 0x6ad43400
extern "C" MMRESULT WINAPI MidiInPrepareHeader(HMIDIIN hmi, LPMIDIHDR pmh, UINT cbmh)
{
    if (ModuleState.MidiInPrepareHeader == NULL)
    {
        ModuleState.MidiInPrepareHeader = (LPMIDIINPREPAREHEADER)GetProcAddress(ModuleState.Module, METHOD_MIDIINPREPAREHEADER_NAME);
    }

    return ModuleState.MidiInPrepareHeader(hmi, pmh, cbmh);
}

// 0x6ad43450
extern "C" MMRESULT WINAPI MidiInReset(HMIDIIN hmi)
{
    if (ModuleState.MidiInReset == NULL)
    {
        ModuleState.MidiInReset = (LPMIDIINRESET)GetProcAddress(ModuleState.Module, METHOD_MIDIINRESET_NAME);
    }

    return ModuleState.MidiInReset(hmi);
}

// 0x6ad434a0
extern "C" MMRESULT WINAPI MidiInStart(HMIDIIN hmi)
{
    if (ModuleState.MidiInStart == NULL)
    {
        ModuleState.MidiInStart = (LPMIDIINSTART)GetProcAddress(ModuleState.Module, METHOD_MIDIINSTART_NAME);
    }

    return ModuleState.MidiInStart(hmi);
}

// 0x6ad434f0
extern "C" MMRESULT WINAPI MidiInStop(HMIDIIN hmi)
{
    if (ModuleState.MidiInStop == NULL)
    {
        ModuleState.MidiInStop = (LPMIDIINSTOP)GetProcAddress(ModuleState.Module, METHOD_MIDIINSTOP_NAME);
    }

    return ModuleState.MidiInStop(hmi);
}

// 0x6ad43540
extern "C" MMRESULT WINAPI MidiInUnprepareHeader(HMIDIIN hmi, LPMIDIHDR pmh, UINT cbmh)
{
    if (ModuleState.MidiInUnprepareHeader == NULL)
    {
        ModuleState.MidiInUnprepareHeader = (LPMIDIINUNPREPAREHEADER)GetProcAddress(ModuleState.Module, METHOD_MIDIINUNPREPAREHEADER_NAME);
    }

    return ModuleState.MidiInUnprepareHeader(hmi, pmh, cbmh);
}

// 0x6ad43590
extern "C" MMRESULT WINAPI MidiOutCacheDrumPatches(HMIDIOUT hmo, UINT uPatch, LPWORD pwkya, UINT fuCache)
{
    if (ModuleState.MidiOutCacheDrumPatches == NULL)
    {
        ModuleState.MidiOutCacheDrumPatches = (LPMIDIOUTCACHEDRUMPATCHES)GetProcAddress(ModuleState.Module, METHOD_MIDIOUTCACHEDRUMPATCHES_NAME);
    }

    return ModuleState.MidiOutCacheDrumPatches(hmo, uPatch, pwkya, fuCache);
}

// 0x6ad435e0
extern "C" MMRESULT WINAPI MidiOutCachePatches(HMIDIOUT hmo, UINT uBank, LPWORD pwpa, UINT fuCache)
{
    if (ModuleState.MidiOutCachePatches == NULL)
    {
        ModuleState.MidiOutCachePatches = (LPMIDIOUTCACHEPATCHES)GetProcAddress(ModuleState.Module, METHOD_MIDIOUTCACHEPATCHES_NAME);
    }

    return ModuleState.MidiOutCachePatches(hmo, uBank, pwpa, fuCache);
}

// 0x6ad43630
extern "C" MMRESULT WINAPI MidiOutClose(HMIDIOUT hmo)
{
    if (ModuleState.MidiOutClose == NULL)
    {
        ModuleState.MidiOutClose = (LPMIDIOUTCLOSE)GetProcAddress(ModuleState.Module, METHOD_MIDIOUTCLOSE_NAME);
    }

    return ModuleState.MidiOutClose(hmo);
}

// 0x6ad43680
extern "C" MMRESULT WINAPI MidiOutGetDevCapsA(UINT_PTR uDeviceID, LPMIDIOUTCAPSA pmoc, UINT cbmoc)
{
    if (ModuleState.MidiOutGetDevCapsA == NULL)
    {
        ModuleState.MidiOutGetDevCapsA = (LPMIDIOUTGETDEVCAPSA)GetProcAddress(ModuleState.Module, METHOD_MIDIOUTGETDEVCAPSA_NAME);
    }

    return ModuleState.MidiOutGetDevCapsA(uDeviceID, pmoc, cbmoc);
}

// 0x6ad436d0
extern "C" MMRESULT WINAPI MidiOutGetDevCapsW(UINT_PTR uDeviceID, LPMIDIOUTCAPSW pmoc, UINT cbmoc)
{
    if (ModuleState.MidiOutGetDevCapsW == NULL)
    {
        ModuleState.MidiOutGetDevCapsW = (LPMIDIOUTGETDEVCAPSW)GetProcAddress(ModuleState.Module, METHOD_MIDIOUTGETDEVCAPSW_NAME);
    }

    return ModuleState.MidiOutGetDevCapsW(uDeviceID, pmoc, cbmoc);
}

// 0x6ad43720
extern "C" MMRESULT WINAPI MidiOutGetErrorTextA(MMRESULT mmrError, LPSTR pszText, UINT cchText)
{
    if (ModuleState.MidiOutGetErrorTextA == NULL)
    {
        ModuleState.MidiOutGetErrorTextA = (LPMIDIOUTGETERRORTEXTA)GetProcAddress(ModuleState.Module, METHOD_MIDIOUTGETERRORTEXTA_NAME);
    }

    return ModuleState.MidiOutGetErrorTextA(mmrError, pszText, cchText);
}

// 0x6ad43770
extern "C" MMRESULT WINAPI MidiOutGetErrorTextW(MMRESULT mmrError, LPWSTR pszText, UINT cchText)
{
    if (ModuleState.MidiOutGetErrorTextW == NULL)
    {
        ModuleState.MidiOutGetErrorTextW = (LPMIDIOUTGETERRORTEXTW)GetProcAddress(ModuleState.Module, METHOD_MIDIOUTGETERRORTEXTW_NAME);
    }

    return ModuleState.MidiOutGetErrorTextW(mmrError, pszText, cchText);
}

// 0x6ad437c0
extern "C" MMRESULT WINAPI MidiOutGetID(HMIDIOUT hmo, LPUINT puDeviceID)
{
    if (ModuleState.MidiOutGetID == NULL)
    {
        ModuleState.MidiOutGetID = (LPMIDIOUTGETID)GetProcAddress(ModuleState.Module, METHOD_MIDIOUTGETID_NAME);
    }

    return ModuleState.MidiOutGetID(hmo, puDeviceID);
}

// 0x6ad43810
extern "C" UINT WINAPI MidiOutGetNumDevs(VOID)
{
    if (ModuleState.MidiOutGetNumDevs == NULL)
    {
        ModuleState.MidiOutGetNumDevs = (LPMIDIOUTGETNUMDEVS)GetProcAddress(ModuleState.Module, METHOD_MIDIOUTGETNUMDEVS_NAME);
    }

    return ModuleState.MidiOutGetNumDevs();
}

// 0x6ad43850
extern "C" MMRESULT WINAPI MidiOutGetVolume(HMIDIOUT hmo, LPDWORD pdwVolume)
{
    if (ModuleState.MidiOutGetVolume == NULL)
    {
        ModuleState.MidiOutGetVolume = (LPMIDIOUTGETVOLUME)GetProcAddress(ModuleState.Module, METHOD_MIDIOUTGETVOLUME_NAME);
    }

    return ModuleState.MidiOutGetVolume(hmo, pdwVolume);
}

// 0x6ad438a0
extern "C" MMRESULT WINAPI MidiOutLongMsg(HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh)
{
    if (ModuleState.MidiOutLongMsg == NULL)
    {
        ModuleState.MidiOutLongMsg = (LPMIDIOUTLONGMSG)GetProcAddress(ModuleState.Module, METHOD_MIDIOUTLONGMSG_NAME);
    }

    return ModuleState.MidiOutLongMsg(hmo, pmh, cbmh);
}

// 0x6ad438f0
extern "C" MMRESULT WINAPI MidiOutMessage(HMIDIOUT hmo, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2)
{
    if (ModuleState.MidiOutMessage == NULL)
    {
        ModuleState.MidiOutMessage = (LPMIDIOUTMESSAGE)GetProcAddress(ModuleState.Module, METHOD_MIDIOUTMESSAGE_NAME);
    }

    return ModuleState.MidiOutMessage(hmo, uMsg, dw1, dw2);
}

// 0x6ad43940
extern "C" MMRESULT WINAPI MidiOutOpen(LPHMIDIOUT phmo, UINT uDeviceID, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen)
{
    if (ModuleState.MidiOutOpen == NULL)
    {
        ModuleState.MidiOutOpen = (LPMIDIOUTOPEN)GetProcAddress(ModuleState.Module, METHOD_MIDIOUTOPEN_NAME);
    }

    return ModuleState.MidiOutOpen(phmo, uDeviceID, dwCallback, dwInstance, fdwOpen);
}

// 0x6ad439a0
extern "C" MMRESULT WINAPI MidiOutPrepareHeader(HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh)
{
    if (ModuleState.MidiOutPrepareHeader == NULL)
    {
        ModuleState.MidiOutPrepareHeader = (LPMIDIOUTPREPAREHEADER)GetProcAddress(ModuleState.Module, METHOD_MIDIOUTPREPAREHEADER_NAME);
    }

    return ModuleState.MidiOutPrepareHeader(hmo, pmh, cbmh);
}

// 0x6ad439f0
extern "C" MMRESULT WINAPI MidiOutReset(HMIDIOUT hmo)
{
    if (ModuleState.MidiOutReset == NULL)
    {
        ModuleState.MidiOutReset = (LPMIDIOUTRESET)GetProcAddress(ModuleState.Module, METHOD_MIDIOUTRESET_NAME);
    }

    return ModuleState.MidiOutReset(hmo);
}

// 0x6ad43a40
extern "C" MMRESULT WINAPI MidiOutSetVolume(HMIDIOUT hmo, DWORD dwVolume)
{
    if (ModuleState.MidiOutSetVolume == NULL)
    {
        ModuleState.MidiOutSetVolume = (LPMIDIOUTSETVOLUME)GetProcAddress(ModuleState.Module, METHOD_MIDIOUTSETVOLUME_NAME);
    }

    return ModuleState.MidiOutSetVolume(hmo, dwVolume);
}

// 0x6ad43a90
extern "C" MMRESULT WINAPI MidiOutShortMsg(HMIDIOUT hmo, DWORD dwMsg)
{
    if (ModuleState.MidiOutShortMsg == NULL)
    {
        ModuleState.MidiOutShortMsg = (LPMIDIOUTSHORTMSG)GetProcAddress(ModuleState.Module, METHOD_MIDIOUTSHORTMSG_NAME);
    }

    return ModuleState.MidiOutShortMsg(hmo, dwMsg);
}

// 0x6ad43ae0
extern "C" MMRESULT WINAPI MidiOutUnprepareHeader(HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh)
{
    if (ModuleState.MidiOutUnprepareHeader == NULL)
    {
        ModuleState.MidiOutUnprepareHeader = (LPMIDIOUTUNPREPAREHEADER)GetProcAddress(ModuleState.Module, METHOD_MIDIOUTUNPREPAREHEADER_NAME);
    }

    return ModuleState.MidiOutUnprepareHeader(hmo, pmh, cbmh);
}

// 0x6ad43b30
extern "C" MMRESULT WINAPI MidiStreamClose(HMIDISTRM hms)
{
    if (ModuleState.MidiStreamClose == NULL)
    {
        ModuleState.MidiStreamClose = (LPMIDISTREAMCLOSE)GetProcAddress(ModuleState.Module, METHOD_MIDISTREAMCLOSE_NAME);
    }

    return ModuleState.MidiStreamClose(hms);
}

// 0x6ad43b80
extern "C" MMRESULT WINAPI MidiStreamOpen(LPHMIDISTRM phms, LPUINT puDeviceID, DWORD cMidi, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen)
{
    if (ModuleState.MidiStreamOpen == NULL)
    {
        ModuleState.MidiStreamOpen = (LPMIDISTREAMOPEN)GetProcAddress(ModuleState.Module, METHOD_MIDISTREAMOPEN_NAME);
    }

    return ModuleState.MidiStreamOpen(phms, puDeviceID, cMidi, dwCallback, dwInstance, fdwOpen);
}

// 0x6ad43be0
extern "C" MMRESULT WINAPI MidiStreamOut(HMIDISTRM hms, LPMIDIHDR pmh, UINT cbmh)
{
    if (ModuleState.MidiStreamOut == NULL)
    {
        ModuleState.MidiStreamOut = (LPMIDISTREAMOUT)GetProcAddress(ModuleState.Module, METHOD_MIDISTREAMOUT_NAME);
    }

    return ModuleState.MidiStreamOut(hms, pmh, cbmh);
}

// 0x6ad43c30
extern "C" MMRESULT WINAPI MidiStreamPause(HMIDISTRM hms)
{
    if (ModuleState.MidiStreamPause == NULL)
    {
        ModuleState.MidiStreamPause = (LPMIDISTREAMPAUSE)GetProcAddress(ModuleState.Module, METHOD_MIDISTREAMPAUSE_NAME);
    }

    return ModuleState.MidiStreamPause(hms);
}

// 0x6ad43c80
extern "C" MMRESULT WINAPI MidiStreamPosition(HMIDISTRM hms, LPMMTIME lpmmt, UINT cbmmt)
{
    if (ModuleState.MidiStreamPosition == NULL)
    {
        ModuleState.MidiStreamPosition = (LPMIDISTREAMPOSITION)GetProcAddress(ModuleState.Module, METHOD_MIDISTREAMPOSITION_NAME);
    }

    return ModuleState.MidiStreamPosition(hms, lpmmt, cbmmt);
}

// 0x6ad43cd0
extern "C" MMRESULT WINAPI MidiStreamProperty(HMIDISTRM hms, LPBYTE lppropdata, DWORD dwProperty)
{
    if (ModuleState.MidiStreamProperty == NULL)
    {
        ModuleState.MidiStreamProperty = (LPMIDISTREAMPROPERTY)GetProcAddress(ModuleState.Module, METHOD_MIDISTREAMPROPERTY_NAME);
    }

    return ModuleState.MidiStreamProperty(hms, lppropdata, dwProperty);
}

// 0x6ad43d20
extern "C" MMRESULT WINAPI MidiStreamRestart(HMIDISTRM hms)
{
    if (ModuleState.MidiStreamRestart == NULL)
    {
        ModuleState.MidiStreamRestart = (LPMIDISTREAMRESTART)GetProcAddress(ModuleState.Module, METHOD_MIDISTREAMRESTART_NAME);
    }

    return ModuleState.MidiStreamRestart(hms);
}

// 0x6ad43d70
extern "C" MMRESULT WINAPI MidiStreamStop(HMIDISTRM hms)
{
    if (ModuleState.MidiStreamStop == NULL)
    {
        ModuleState.MidiStreamStop = (LPMIDISTREAMSTOP)GetProcAddress(ModuleState.Module, METHOD_MIDISTREAMSTOP_NAME);
    }

    return ModuleState.MidiStreamStop(hms);
}

// 0x6ad43dc0
extern "C" MMRESULT WINAPI MixerClose(HMIXER hmx)
{
    if (ModuleState.MixerClose == NULL)
    {
        ModuleState.MixerClose = (LPMIXERCLOSE)GetProcAddress(ModuleState.Module, METHOD_MIXERCLOSE_NAME);
    }

    return ModuleState.MixerClose(hmx);
}

// 0x6ad43e10
extern "C" MMRESULT WINAPI MixerGetControlDetailsA(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails)
{
    if (ModuleState.MixerGetControlDetailsA == NULL)
    {
        ModuleState.MixerGetControlDetailsA = (LPMIXERGETCONTROLDETAILSA)GetProcAddress(ModuleState.Module, METHOD_MIXERGETCONTROLDETAILSA_NAME);
    }

    return ModuleState.MixerGetControlDetailsA(hmxobj, pmxcd, fdwDetails);
}

// 0x6ad43e60
extern "C" MMRESULT WINAPI MixerGetControlDetailsW(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails)
{
    if (ModuleState.MixerGetControlDetailsW == NULL)
    {
        ModuleState.MixerGetControlDetailsW = (LPMIXERGETCONTROLDETAILSW)GetProcAddress(ModuleState.Module, METHOD_MIXERGETCONTROLDETAILSW_NAME);
    }

    return ModuleState.MixerGetControlDetailsW(hmxobj, pmxcd, fdwDetails);
}

// 0x6ad43eb0
extern "C" MMRESULT WINAPI MixerGetDevCapsA(UINT_PTR uMxId, LPMIXERCAPSA pmxcaps, UINT cbmxcaps)
{
    if (ModuleState.MixerGetDevCapsA == NULL)
    {
        ModuleState.MixerGetDevCapsA = (LPMIXERGETDEVCAPSA)GetProcAddress(ModuleState.Module, METHOD_MIXERGETDEVCAPSA_NAME);
    }

    return ModuleState.MixerGetDevCapsA(uMxId, pmxcaps, cbmxcaps);
}

// 0x6ad43f00
extern "C" MMRESULT WINAPI MixerGetDevCapsW(UINT_PTR uMxId, LPMIXERCAPSW pmxcaps, UINT cbmxcaps)
{
    if (ModuleState.MixerGetDevCapsW == NULL)
    {
        ModuleState.MixerGetDevCapsW = (LPMIXERGETDEVCAPSW)GetProcAddress(ModuleState.Module, METHOD_MIXERGETDEVCAPSW_NAME);
    }

    return ModuleState.MixerGetDevCapsW(uMxId, pmxcaps, cbmxcaps);
}

// 0x6ad43f50
extern "C" MMRESULT WINAPI MixerGetID(HMIXEROBJ hmxobj, UINT * puMxId, DWORD fdwId)
{
    if (ModuleState.MixerGetID == NULL)
    {
        ModuleState.MixerGetID = (LPMIXERGETID)GetProcAddress(ModuleState.Module, METHOD_MIXERGETID_NAME);
    }

    return ModuleState.MixerGetID(hmxobj, puMxId, fdwId);
}

// 0x6ad42110
extern "C" MMRESULT WINAPI MixerGetLineControlsA(HMIXEROBJ hmxobj, LPMIXERLINECONTROLSA pmxlc, DWORD fdwControls)
{
    if (ModuleState.MixerGetLineControlsA == NULL)
    {
        ModuleState.MixerGetLineControlsA = (LPMIXERGETLINECONTROLSA)GetProcAddress(ModuleState.Module, METHOD_MIXERGETLINECONTROLSA_NAME);
    }

    return ModuleState.MixerGetLineControlsA(hmxobj, pmxlc, fdwControls);
}

// 0x6ad42160
extern "C" MMRESULT WINAPI MixerGetLineControlsW(HMIXEROBJ hmxobj, LPMIXERLINECONTROLSW pmxlc, DWORD fdwControls)
{
    if (ModuleState.MixerGetLineControlsW == NULL)
    {
        ModuleState.MixerGetLineControlsW = (LPMIXERGETLINECONTROLSW)GetProcAddress(ModuleState.Module, METHOD_MIXERGETLINECONTROLSW_NAME);
    }

    return ModuleState.MixerGetLineControlsW(hmxobj, pmxlc, fdwControls);
}

// 0x6ad42620
extern "C" MMRESULT WINAPI MixerGetLineInfoA(HMIXEROBJ hmxobj, LPMIXERLINEA pmxl, DWORD fdwInfo)
{
    if (ModuleState.MixerGetLineInfoA == NULL)
    {
        ModuleState.MixerGetLineInfoA = (LPMIXERGETLINEINFOA)GetProcAddress(ModuleState.Module, METHOD_MIXERGETLINEINFOA_NAME);
    }

    return ModuleState.MixerGetLineInfoA(hmxobj, pmxl, fdwInfo);
}

// 0x6ad42670
extern "C" MMRESULT WINAPI MixerGetLineInfoW(HMIXEROBJ hmxobj, LPMIXERLINEW pmxl, DWORD fdwInfo)
{
    if (ModuleState.MixerGetLineInfoW == NULL)
    {
        ModuleState.MixerGetLineInfoW = (LPMIXERGETLINEINFOW)GetProcAddress(ModuleState.Module, METHOD_MIXERGETLINEINFOW_NAME);
    }

    return ModuleState.MixerGetLineInfoW(hmxobj, pmxl, fdwInfo);
}

// 0x6ad42580
extern "C" UINT WINAPI MixerGetNumDevs(VOID)
{
    if (ModuleState.MixerGetNumDevs == NULL)
    {
        ModuleState.MixerGetNumDevs = (LPMIXERGETNUMDEVS)GetProcAddress(ModuleState.Module, METHOD_MIXERGETNUMDEVS_NAME);
    }

    return ModuleState.MixerGetNumDevs();
}

// 0x6ad43fa0
extern "C" DWORD WINAPI MixerMessage(HMIXER hmx, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    if (ModuleState.MixerMessage == NULL)
    {
        ModuleState.MixerMessage = (LPMIXERMESSAGE)GetProcAddress(ModuleState.Module, METHOD_MIXERMESSAGE_NAME);
    }

    return ModuleState.MixerMessage(hmx, uMsg, dwParam1, dwParam2);
}

// 0x6ad43ff0
extern "C" MMRESULT WINAPI MixerOpen(LPHMIXER phmx, UINT uMxId, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen)
{
    if (ModuleState.MixerOpen == NULL)
    {
        ModuleState.MixerOpen = (LPMIXEROPEN)GetProcAddress(ModuleState.Module, METHOD_MIXEROPEN_NAME);
    }

    return ModuleState.MixerOpen(phmx, uMxId, dwCallback, dwInstance, fdwOpen);
}

// 0x6ad423e0
extern "C" MMRESULT WINAPI MixerSetControlDetails(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails)
{
    if (((LPMIXERCONTROLDETAILS_SIGNED)pmxcd->paDetails)->lValue == 0) { return MMSYSERR_NOERROR; }

    if (ModuleState.MixerSetControlDetails == NULL)
    {
        ModuleState.MixerSetControlDetails = (LPMIXERSETCONTROLDETAILS)GetProcAddress(ModuleState.Module, METHOD_MIXERSETCONTROLDETAILS_NAME);
    }

    return ModuleState.MixerSetControlDetails(hmxobj, pmxcd, fdwDetails);
}

extern "C" UINT WINAPI MmDrvInstall(HDRVR hDriver, LPCWSTR wszDrvEntry, DRIVERMSGPROC drvMessage, UINT wFlags)
{
    if (ModuleState.MmDrvInstall == NULL)
    {
        ModuleState.MmDrvInstall = (LPMMDRVINSTALL)GetProcAddress(ModuleState.Module, METHOD_MMDRVINSTALL_NAME);
    }

    return ModuleState.MmDrvInstall(hDriver, wszDrvEntry, drvMessage, wFlags);
}

// 0x6ad453f0
extern "C" DWORD WINAPI MmGetCurrentTask(VOID)
{
    if (ModuleState.MmGetCurrentTask == NULL)
    {
        ModuleState.MmGetCurrentTask = (LPMMGETCURRENTTASK)GetProcAddress(ModuleState.Module, METHOD_MMGETCURRENTTASK_NAME);
    }

    return ModuleState.MmGetCurrentTask();
}

// 0x6ad44050
extern "C" MMRESULT WINAPI MmioAdvance(HMMIO hmmio, LPMMIOINFO pmmioinfo, UINT fuAdvance)
{
    if (ModuleState.MmioAdvance == NULL)
    {
        ModuleState.MmioAdvance = (LPMMIOADVANCE)GetProcAddress(ModuleState.Module, METHOD_MMIOADVANCE_NAME);
    }

    return ModuleState.MmioAdvance(hmmio, pmmioinfo, fuAdvance);
}

// 0x6ad440a0
extern "C" MMRESULT WINAPI MmioAscend(HMMIO hmmio, LPMMCKINFO pmmcki, UINT fuAscend)
{
    if (ModuleState.MmioAscend == NULL)
    {
        ModuleState.MmioAscend = (LPMMIOASCEND)GetProcAddress(ModuleState.Module, METHOD_MMIOASCEND_NAME);
    }

    return ModuleState.MmioAscend(hmmio, pmmcki, fuAscend);
}

// 0x6ad440f0
extern "C" MMRESULT WINAPI MmioClose(HMMIO hmmio, UINT fuClose)
{
    if (ModuleState.MmioClose == NULL)
    {
        ModuleState.MmioClose = (LPMMIOCLOSE)GetProcAddress(ModuleState.Module, METHOD_MMIOCLOSE_NAME);
    }

    return ModuleState.MmioClose(hmmio, fuClose);
}

// 0x6ad422f0
extern "C" MMRESULT WINAPI MmioCreateChunk(HMMIO hmmio, LPMMCKINFO pmmcki, UINT fuCreate)
{
    if (ModuleState.MmioCreateChunk == NULL)
    {
        ModuleState.MmioCreateChunk = (LPMMIOCREATECHUNK)GetProcAddress(ModuleState.Module, METHOD_MMIOCREATECHUNK_NAME);
    }

    return ModuleState.MmioCreateChunk(hmmio, pmmcki, fuCreate);
}

// 0x6ad421f0
extern "C" MMRESULT WINAPI MmioDescend(HMMIO hmmio, LPMMCKINFO pmmcki, CONST MMCKINFO* pmmckiParent, UINT fuDescend)
{
    if (ModuleState.MmioDescend == NULL)
    {
        ModuleState.MmioDescend = (LPMMIODESCEND)GetProcAddress(ModuleState.Module, METHOD_MMIODESCEND_NAME);
    }

    return ModuleState.MmioDescend(hmmio, pmmcki, pmmckiParent, fuDescend);
}

// 0x6ad44140
extern "C" MMRESULT WINAPI MmioFlush(HMMIO hmmio, UINT fuFlush)
{
    if (ModuleState.MmioFlush == NULL)
    {
        ModuleState.MmioFlush = (LPMMIOFLUSH)GetProcAddress(ModuleState.Module, METHOD_MMIOFLUSH_NAME);
    }

    return ModuleState.MmioFlush(hmmio, fuFlush);
}

// 0x6ad44190
extern "C" MMRESULT WINAPI MmioGetInfo(HMMIO hmmio, LPMMIOINFO pmmioinfo, UINT fuInfo)
{
    if (ModuleState.MmioGetInfo == NULL)
    {
        ModuleState.MmioGetInfo = (LPMMIOGETINFO)GetProcAddress(ModuleState.Module, METHOD_MMIOGETINFO_NAME);
    }

    return ModuleState.MmioGetInfo(hmmio, pmmioinfo, fuInfo);
}

// 0x6ad441e0
extern "C" LPMMIOPROC WINAPI MmioInstallIOProcA(FOURCC fccIOProc, LPMMIOPROC pIOProc, DWORD dwFlags)
{
    if (ModuleState.MmioInstallIOProcA == NULL)
    {
        ModuleState.MmioInstallIOProcA = (LPMMIOINSTALLIOPROCA)GetProcAddress(ModuleState.Module, METHOD_MMIOINSTALLIOPROCA_NAME);
    }

    return ModuleState.MmioInstallIOProcA(fccIOProc, pIOProc, dwFlags);
}

// 0x6ad44230
extern "C" LPMMIOPROC WINAPI MmioInstallIOProcW(FOURCC fccIOProc, LPMMIOPROC pIOProc, DWORD dwFlags)
{
    if (ModuleState.MmioInstallIOProcW == NULL)
    {
        ModuleState.MmioInstallIOProcW = (LPMMIOINSTALLIOPROCW)GetProcAddress(ModuleState.Module, METHOD_MMIOINSTALLIOPROCW_NAME);
    }

    return ModuleState.MmioInstallIOProcW(fccIOProc, pIOProc, dwFlags);
}

// 0x6ad44280
extern "C" HMMIO WINAPI MmioOpenA(LPSTR pszFileName, LPMMIOINFO pmmioinfo, DWORD fdwOpen)
{
    if (ModuleState.MmioOpenA == NULL)
    {
        ModuleState.MmioOpenA = (LPMMIOOPENA)GetProcAddress(ModuleState.Module, METHOD_MMIOOPENA_NAME);
    }

    return ModuleState.MmioOpenA(pszFileName, pmmioinfo, fdwOpen);
}

// 0x6ad442d0
extern "C" HMMIO WINAPI MmioOpenW(LPWSTR pszFileName, LPMMIOINFO pmmioinfo, DWORD fdwOpen)
{
    if (ModuleState.MmioOpenW == NULL)
    {
        ModuleState.MmioOpenW = (LPMMIOOPENW)GetProcAddress(ModuleState.Module, METHOD_MMIOOPENW_NAME);
    }

    return ModuleState.MmioOpenW(pszFileName, pmmioinfo, fdwOpen);
}

// 0x6ad42490
extern "C" LONG WINAPI MmioRead(HMMIO hmmio, HPSTR pch, LONG cch)
{
    if (ModuleState.MmioRead == NULL)
    {
        ModuleState.MmioRead = (LPMMIOREAD)GetProcAddress(ModuleState.Module, METHOD_MMIOREAD_NAME);
    }

    return ModuleState.MmioRead(hmmio, pch, cch);
}

// 0x6ad44320
extern "C" MMRESULT WINAPI MmioRenameA(LPCSTR pszFileName, LPCSTR pszNewFileName, LPCMMIOINFO pmmioinfo, DWORD fdwRename)
{
    if (ModuleState.MmioRenameA == NULL)
    {
        ModuleState.MmioRenameA = (LPMMIORENAMEA)GetProcAddress(ModuleState.Module, METHOD_MMIORENAMEA_NAME);
    }

    return ModuleState.MmioRenameA(pszFileName, pszNewFileName, pmmioinfo, fdwRename);
}

// 0x6ad44370
extern "C" MMRESULT WINAPI MmioRenameW(LPCWSTR pszFileName, LPCWSTR pszNewFileName, LPCMMIOINFO pmmioinfo, DWORD fdwRename)
{
    if (ModuleState.MmioRenameW == NULL)
    {
        ModuleState.MmioRenameW = (LPMMIORENAMEW)GetProcAddress(ModuleState.Module, METHOD_MMIORENAMEW_NAME);
    }

    return ModuleState.MmioRenameW(pszFileName, pszNewFileName, pmmioinfo, fdwRename);
}

// 0x6ad443c0
extern "C" LONG WINAPI MmioSeek(HMMIO hmmio, LONG lOffset, INT iOrigin)
{
    if (ModuleState.MmioSeek == NULL)
    {
        ModuleState.MmioSeek = (LPMMIOSEEK)GetProcAddress(ModuleState.Module, METHOD_MMIOSEEK_NAME);
    }

    return ModuleState.MmioSeek(hmmio, lOffset, iOrigin);
}

// 0x6ad44410
extern "C" LRESULT WINAPI MmioSendMessage(HMMIO hmmio, UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
    if (ModuleState.MmioSendMessage == NULL)
    {
        ModuleState.MmioSendMessage = (LPMMIOSENDMESSAGE)GetProcAddress(ModuleState.Module, METHOD_MMIOSENDMESSAGE_NAME);
    }

    return ModuleState.MmioSendMessage(hmmio, uMsg, lParam1, lParam2);
}

// 0x6ad44460
extern "C" MMRESULT WINAPI MmioSetBuffer(HMMIO hmmio, LPSTR pchBuffer, LONG cchBuffer, UINT fuBuffer)
{
    if (ModuleState.MmioSetBuffer == NULL)
    {
        ModuleState.MmioSetBuffer = (LPMMIOSETBUFFER)GetProcAddress(ModuleState.Module, METHOD_MMIOSETBUFFER_NAME);
    }

    return ModuleState.MmioSetBuffer(hmmio, pchBuffer, cchBuffer, fuBuffer);
}

// 0x6ad444b0
extern "C" MMRESULT WINAPI MmioSetInfo(HMMIO hmmio, LPCMMIOINFO pmmioinfo, UINT fuInfo)
{
    if (ModuleState.MmioSetInfo == NULL)
    {
        ModuleState.MmioSetInfo = (LPMMIOSETINFO)GetProcAddress(ModuleState.Module, METHOD_MMIOSETINFO_NAME);
    }

    return ModuleState.MmioSetInfo(hmmio, pmmioinfo, fuInfo);
}

// 0x6ad44500
extern "C" FOURCC WINAPI MmioStringToFOURCCA(LPCSTR sz, UINT uFlags)
{
    if (ModuleState.MmioStringToFOURCCA == NULL)
    {
        ModuleState.MmioStringToFOURCCA = (LPMMIOSTRINGTOFOURCCA)GetProcAddress(ModuleState.Module, METHOD_MMIOSTRINGTOFOURCCA_NAME);
    }

    return ModuleState.MmioStringToFOURCCA(sz, uFlags);
}

// 0x6ad44550
extern "C" FOURCC WINAPI MmioStringToFOURCCW(LPCWSTR sz, UINT uFlags)
{
    if (ModuleState.MmioStringToFOURCCW == NULL)
    {
        ModuleState.MmioStringToFOURCCW = (LPMMIOSTRINGTOFOURCCW)GetProcAddress(ModuleState.Module, METHOD_MMIOSTRINGTOFOURCCW_NAME);
    }

    return ModuleState.MmioStringToFOURCCW(sz, uFlags);
}

// 0x6ad42710
extern "C" LONG WINAPI MmioWrite(HMMIO hmmio, CONST CHAR* pch, LONG cch)
{
    if (ModuleState.MmioWrite == NULL)
    {
        ModuleState.MmioWrite = (LPMMIOWRITE)GetProcAddress(ModuleState.Module, METHOD_MMIOWRITE_NAME);
    }

    return ModuleState.MmioWrite(hmmio, pch, cch);
}

// 0x6ad445a0
extern "C" UINT WINAPI MmsystemGetVersion(VOID)
{
    if (ModuleState.MmsystemGetVersion == NULL)
    {
        ModuleState.MmsystemGetVersion = (LPMMSYSTEMGETVERSION)GetProcAddress(ModuleState.Module, METHOD_MMSYSTEMGETVERSION_NAME);
    }

    return ModuleState.MmsystemGetVersion();
}

// 0x6ad45310
extern "C" VOID WINAPI MmTaskBlock(DWORD h)
{
    if (ModuleState.MmTaskBlock == NULL)
    {
        ModuleState.MmTaskBlock = (LPMMTASKBLOCK)GetProcAddress(ModuleState.Module, METHOD_MMTASKBLOCK_NAME);
    }

    ModuleState.MmTaskBlock(h);
}

// 0x6ad452c0
extern "C" UINT WINAPI MmTaskCreate(LPTASKCALLBACK lpfn, HANDLE* lph, DWORD_PTR dwInst)
{
    if (ModuleState.MmTaskCreate == NULL)
    {
        ModuleState.MmTaskCreate = (LPMMTASKCREATE)GetProcAddress(ModuleState.Module, METHOD_MMTASKCREATE_NAME);
    }

    return ModuleState.MmTaskCreate(lpfn, lph, dwInst);
}

// 0x6ad45360
extern "C" BOOL WINAPI MmTaskSignal(DWORD h)
{
    if (ModuleState.MmTaskSignal == NULL)
    {
        ModuleState.MmTaskSignal = (LPMMTASKSIGNAL)GetProcAddress(ModuleState.Module, METHOD_MMTASKSIGNAL_NAME);
    }

    return ModuleState.MmTaskSignal(h);
}

// 0x6ad453b0
extern "C" VOID WINAPI MmTaskYield(VOID)
{
    if (ModuleState.MmTaskYield == NULL)
    {
        ModuleState.MmTaskYield = (LPMMTASKYIELD)GetProcAddress(ModuleState.Module, METHOD_MMTASKYIELD_NAME);
    }

    ModuleState.MmTaskYield();
}

extern "C" DWORD WINAPI Mod32Message(UINT uId, UINT uMsg, DWORD dwInstance, DWORD dwParam1, WORD dwParam2)
{
    if (ModuleState.Mod32Message == NULL)
    {
        ModuleState.Mod32Message = (LPMOD32MESSAGE)GetProcAddress(ModuleState.Module, METHOD_MOD32MESSAGE_NAME);
    }

    return ModuleState.Mod32Message(uId, uMsg, dwInstance, dwParam1, dwParam2);
}


extern "C" DWORD WINAPI Mxd32Message(UINT uId, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    if (ModuleState.Mxd32Message == NULL)
    {
        ModuleState.Mxd32Message = (LPMXD32MESSAGE)GetProcAddress(ModuleState.Module, METHOD_MXD32MESSAGE_NAME);
    }

    return ModuleState.Mxd32Message(uId, uMsg, dwInstance, dwParam1, dwParam2);
}

extern "C" DWORD WINAPI NotifyCallbackData(UINT uId, UINT uMsg, DWORD dwInstance, DWORD dwParam1, VPCALLBACK_DATA dwParam2)
{
    if (ModuleState.NotifyCallbackData == NULL)
    {
        ModuleState.NotifyCallbackData = (LPNOTIFYCALLBACKDATA)GetProcAddress(ModuleState.Module, METHOD_NOTIFYCALLBACKDATA_NAME);
    }

    return ModuleState.NotifyCallbackData(uId, uMsg, dwInstance, dwParam1, dwParam2);
}

// 0x6ad445e0
extern "C" HDRVR WINAPI OpenDriver(LPCWSTR szDriverName, LPCWSTR szSectionName, LPARAM lParam2)
{
    if (ModuleState.OpenDriver == NULL)
    {
        ModuleState.OpenDriver = (LPOPENDRIVER)GetProcAddress(ModuleState.Module, METHOD_OPENDRIVER_NAME);
    }

    return ModuleState.OpenDriver(szDriverName, szSectionName, lParam2);
}

// 0x6ad44630
extern "C" BOOL WINAPI PlaySound(LPCSTR pszSound, HMODULE hmod, DWORD fdwSound)
{
    if (ModuleState.PlaySound == NULL)
    {
        ModuleState.PlaySound = (LPPLAYSOUND)GetProcAddress(ModuleState.Module, METHOD_PLAYSOUND_NAME);
    }

    return ModuleState.PlaySound(pszSound, hmod, fdwSound);
}

// 0x6ad44680
extern "C" BOOL WINAPI PlaySoundA(LPCSTR pszSound, HMODULE hmod, DWORD fdwSound)
{
    if (ModuleState.PlaySoundA == NULL)
    {
        ModuleState.PlaySoundA = (LPPLAYSOUNDA)GetProcAddress(ModuleState.Module, METHOD_PLAYSOUNDA_NAME);
    }

    return ModuleState.PlaySoundA(pszSound, hmod, fdwSound);
}

// 0x6ad446d0
extern "C" BOOL WINAPI PlaySoundW(LPCWSTR pszSound, HMODULE hmod, DWORD fdwSound)
{
    if (ModuleState.PlaySoundW == NULL)
    {
        ModuleState.PlaySoundW = (LPPLAYSOUNDW)GetProcAddress(ModuleState.Module, METHOD_PLAYSOUNDW_NAME);
    }

    return ModuleState.PlaySoundW(pszSound, hmod, fdwSound);
}

// 0x6ad44720
extern "C" LRESULT WINAPI SendDriverMessage(HDRVR hDriver, UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
    if (ModuleState.SendDriverMessage == NULL)
    {
        ModuleState.SendDriverMessage = (LPSENDDRIVERMESSAGE)GetProcAddress(ModuleState.Module, METHOD_SENDDRIVERMESSAGE_NAME);
    }

    return ModuleState.SendDriverMessage(hDriver, uMsg, lParam1, lParam2);
}

// 0x6ad44770
extern "C" BOOL WINAPI SndPlaySoundA(LPCSTR pszSound, UINT fuSound)
{
    if (ModuleState.SndPlaySoundA == NULL)
    {
        ModuleState.SndPlaySoundA = (LPSNDPLAYSOUNDA)GetProcAddress(ModuleState.Module, METHOD_SNDPLAYSOUNDA_NAME);
    }

    return ModuleState.SndPlaySoundA(pszSound, fuSound);
}

// 0x6ad447c0
extern "C" BOOL WINAPI SndPlaySoundW(LPCWSTR pszSound, UINT fuSound)
{
    if (ModuleState.SndPlaySoundW == NULL)
    {
        ModuleState.SndPlaySoundW = (LPSNDPLAYSOUNDW)GetProcAddress(ModuleState.Module, METHOD_SNDPLAYSOUNDW_NAME);
    }

    return ModuleState.SndPlaySoundW(pszSound, fuSound);
}

extern "C" DWORD WINAPI Tid32Message(UINT uId, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    if (ModuleState.Tid32Message == NULL)
    {
        ModuleState.Tid32Message = (LPTID32MESSAGE)GetProcAddress(ModuleState.Module, METHOD_TID32MESSAGE_NAME);
    }

    return ModuleState.Tid32Message(uId, uMsg, dwInstance, dwParam1, dwParam2);
}

// 0x6ad426c0
extern "C" MMRESULT WINAPI TimeBeginPeriod(UINT uPeriod)
{
    if (ModuleState.TimeBeginPeriod == NULL)
    {
        ModuleState.TimeBeginPeriod = (LPTIMEBEGINPERIOD)GetProcAddress(ModuleState.Module, METHOD_TIMEBEGINPERIOD_NAME);
    }

    return ModuleState.TimeBeginPeriod(uPeriod);
}

// 0x6ad42760
extern "C" MMRESULT WINAPI TimeEndPeriod(UINT uPeriod)
{
    if (ModuleState.TimeEndPeriod == NULL)
    {
        ModuleState.TimeEndPeriod = (LPTIMEENDPERIOD)GetProcAddress(ModuleState.Module, METHOD_TIMEENDPERIOD_NAME);
    }

    return ModuleState.TimeEndPeriod(uPeriod);
}

// 0x6ad44810
extern "C" MMRESULT WINAPI TimeGetDevCaps(LPTIMECAPS ptc, INT cbtc)
{
    if (ModuleState.TimeGetDevCaps == NULL)
    {
        ModuleState.TimeGetDevCaps = (LPTIMEGETDEVCAPS)GetProcAddress(ModuleState.Module, METHOD_TIMEGETDEVCAPS_NAME);
    }

    return ModuleState.TimeGetDevCaps(ptc, cbtc);
}

// 0x6ad44860
extern "C" MMRESULT WINAPI TimeGetSystemTime(LPMMTIME pmmt,UINT cbmmt)
{
    if (ModuleState.TimeGetSystemTime == NULL)
    {
        ModuleState.TimeGetSystemTime = (LPTIMEGETSYSTEMTIME)GetProcAddress(ModuleState.Module, METHOD_TIMEGETSYSTEMTIME_NAME);
    }

    return ModuleState.TimeGetSystemTime(pmmt, cbmmt);
}

// 0x6ad422e0
extern "C" DWORD WINAPI TimeGetTime(VOID)
{
    return ModuleState.TimeGetTime();
}

// 0x6ad42440
extern "C" MMRESULT WINAPI TimeKillEvent(UINT uTimerID)
{
    if (ModuleState.TimeKillEvent == NULL)
    {
        ModuleState.TimeKillEvent = (LPTIMEKILLEVENT)GetProcAddress(ModuleState.Module, METHOD_TIMEKILLEVENT_NAME);
    }

    return ModuleState.TimeKillEvent(uTimerID);
}

// 0x6ad425c0
extern "C" MMRESULT WINAPI TimeSetEvent(UINT uDelay, UINT uResolution, LPTIMECALLBACK fptc, DWORD_PTR dwUser, UINT fuEvent)
{
    if (ModuleState.TimeSetEvent == NULL)
    {
        ModuleState.TimeSetEvent = (LPTIMESETEVENT)GetProcAddress(ModuleState.Module, METHOD_TIMESETEVENT_NAME);
    }

    return ModuleState.TimeSetEvent(uDelay, uResolution, fptc, dwUser, fuEvent);
}

// 0x6ad448b0
extern "C" MMRESULT WINAPI WaveInAddBuffer(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
    if (ModuleState.WaveInAddBuffer == NULL)
    {
        ModuleState.WaveInAddBuffer = (LPWAVEINADDBUFFER)GetProcAddress(ModuleState.Module, METHOD_WAVEINADDBUFFER_NAME);
    }

    return ModuleState.WaveInAddBuffer(hwi, pwh, cbwh);
}

// 0x6ad44900
extern "C" MMRESULT WINAPI WaveInClose(HWAVEIN hwi)
{
    if (ModuleState.WaveInClose == NULL)
    {
        ModuleState.WaveInClose = (LPWAVEINCLOSE)GetProcAddress(ModuleState.Module, METHOD_WAVEINCLOSE_NAME);
    }

    return ModuleState.WaveInClose(hwi);
}

// 0x6ad42340
extern "C" MMRESULT WINAPI WaveInGetDevCapsA(UINT_PTR uDeviceID, LPWAVEINCAPSA pwic, UINT cbwic)
{
    if (ModuleState.WaveInGetDevCapsA == NULL)
    {
        ModuleState.WaveInGetDevCapsA = (LPWAVEINGETDEVCAPSA)GetProcAddress(ModuleState.Module, METHOD_WAVEINGETDEVCAPSA_NAME);
    }

    return ModuleState.WaveInGetDevCapsA(uDeviceID, pwic, cbwic);
}

// 0x6ad42390
extern "C" MMRESULT WINAPI WaveInGetDevCapsW(UINT_PTR uDeviceID, LPWAVEINCAPSW pwic, UINT cbwic)
{
    if (ModuleState.WaveInGetDevCapsW == NULL)
    {
        ModuleState.WaveInGetDevCapsW = (LPWAVEINGETDEVCAPSW)GetProcAddress(ModuleState.Module, METHOD_WAVEINGETDEVCAPSW_NAME);
    }

    return ModuleState.WaveInGetDevCapsW(uDeviceID, pwic, cbwic);
}

// 0x6ad44950
extern "C" MMRESULT WINAPI WaveInGetErrorTextA(MMRESULT mmrError, LPSTR pszText, UINT cchText)
{
    if (ModuleState.WaveInGetErrorTextA == NULL)
    {
        ModuleState.WaveInGetErrorTextA = (LPWAVEINGETERRORTEXTA)GetProcAddress(ModuleState.Module, METHOD_WAVEINGETERRORTEXTA_NAME);
    }

    return ModuleState.WaveInGetErrorTextA(mmrError, pszText, cchText);
}

// 0x6ad449a0
extern "C" MMRESULT WINAPI WaveInGetErrorTextW(MMRESULT mmrError, LPWSTR pszText, UINT cchText)
{
    if (ModuleState.WaveInGetErrorTextW == NULL)
    {
        ModuleState.WaveInGetErrorTextW = (LPWAVEINGETERRORTEXTW)GetProcAddress(ModuleState.Module, METHOD_WAVEINGETERRORTEXTW_NAME);
    }

    return ModuleState.WaveInGetErrorTextW(mmrError, pszText, cchText);
}

// 0x6ad449f0
extern "C" MMRESULT WINAPI WaveInGetID(HWAVEIN hwi, LPUINT puDeviceID)
{
    if (ModuleState.WaveInGetID == NULL)
    {
        ModuleState.WaveInGetID = (LPWAVEINGETID)GetProcAddress(ModuleState.Module, METHOD_WAVEINGETID_NAME);
    }

    return ModuleState.WaveInGetID(hwi, puDeviceID);
}

// 0x6ad421b0
extern "C" UINT WINAPI WaveInGetNumDevs(VOID)
{
    if (ModuleState.WaveInGetNumDevs == NULL)
    {
        ModuleState.WaveInGetNumDevs = (LPWAVEINGETNUMDEVS)GetProcAddress(ModuleState.Module, METHOD_WAVEINGETNUMDEVS_NAME);
    }

    return ModuleState.WaveInGetNumDevs();
}

// 0x6ad44a40
extern "C" MMRESULT WINAPI WaveInGetPosition(HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt)
{
    if (ModuleState.WaveInGetPosition == NULL)
    {
        ModuleState.WaveInGetPosition = (LPWAVEINGETPOSITION)GetProcAddress(ModuleState.Module, METHOD_WAVEINGETPOSITION_NAME);
    }

    return ModuleState.WaveInGetPosition(hwi, pmmt, cbmmt);
}

// 0x6ad44a90
extern "C" MMRESULT WINAPI WaveInMessage(HWAVEIN hwi, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2)
{
    if (ModuleState.WaveInMessage == NULL)
    {
        ModuleState.WaveInMessage = (LPWAVEINMESSAGE)GetProcAddress(ModuleState.Module, METHOD_WAVEINMESSAGE_NAME);
    }

    return ModuleState.WaveInMessage(hwi, uMsg, dw1, dw2);
}

// 0x6ad44ae0
extern "C" MMRESULT WINAPI WaveInOpen(LPHWAVEIN phwi, UINT uDeviceID, LPCWAVEFORMATEX pwfx, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen)
{
    if (ModuleState.WaveInOpen == NULL)
    {
        ModuleState.WaveInOpen = (LPWAVEINOPEN)GetProcAddress(ModuleState.Module, METHOD_WAVEINOPEN_NAME);
    }

    return ModuleState.WaveInOpen(phwi, uDeviceID, pwfx, dwCallback, dwInstance, fdwOpen);
}

// 0x6ad44b40
extern "C" MMRESULT WINAPI WaveInPrepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
    if (ModuleState.WaveInPrepareHeader == NULL)
    {
        ModuleState.WaveInPrepareHeader = (LPWAVEINPREPAREHEADER)GetProcAddress(ModuleState.Module, METHOD_WAVEINPREPAREHEADER_NAME);
    }

    return ModuleState.WaveInPrepareHeader(hwi, pwh, cbwh);
}

// 0x6ad44b90
extern "C" MMRESULT WINAPI WaveInReset(HWAVEIN hwi)
{
    if (ModuleState.WaveInReset == NULL)
    {
        ModuleState.WaveInReset = (LPWAVEINRESET)GetProcAddress(ModuleState.Module, METHOD_WAVEINRESET_NAME);
    }

    return ModuleState.WaveInReset(hwi);
}

// 0x6ad44be0
extern "C" MMRESULT WINAPI WaveInStart(HWAVEIN hwi)
{
    if (ModuleState.WaveInStart == NULL)
    {
        ModuleState.WaveInStart = (LPWAVEINSTART)GetProcAddress(ModuleState.Module, METHOD_WAVEINSTART_NAME);
    }

    return ModuleState.WaveInStart(hwi);
}

// 0x6ad44c30
extern "C" MMRESULT WINAPI WaveInStop(HWAVEIN hwi)
{
    if (ModuleState.WaveInStop == NULL)
    {
        ModuleState.WaveInStop = (LPWAVEINSTOP)GetProcAddress(ModuleState.Module, METHOD_WAVEINSTOP_NAME);
    }

    return ModuleState.WaveInStop(hwi);
}

// 0x6ad44c80
extern "C" MMRESULT WINAPI WaveInUnprepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
    if (ModuleState.WaveInUnprepareHeader == NULL)
    {
        ModuleState.WaveInUnprepareHeader = (LPWAVEINUNPREPAREHEADER)GetProcAddress(ModuleState.Module, METHOD_WAVEINUNPREPAREHEADER_NAME);
    }

    return ModuleState.WaveInUnprepareHeader(hwi, pwh, cbwh);
}

// 0x6ad44cd0
extern "C" MMRESULT WINAPI WaveOutBreakLoop(HWAVEOUT hwo)
{
    if (ModuleState.WaveOutBreakLoop == NULL)
    {
        ModuleState.WaveOutBreakLoop = (LPWAVEOUTBREAKLOOP)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTBREAKLOOP_NAME);
    }

    return ModuleState.WaveOutBreakLoop(hwo);
}

// 0x6ad44d20
extern "C" MMRESULT WINAPI WaveOutClose(HWAVEOUT hwo)
{
    if (ModuleState.WaveOutClose == NULL)
    {
        ModuleState.WaveOutClose = (LPWAVEOUTCLOSE)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTCLOSE_NAME);
    }

    return ModuleState.WaveOutClose(hwo);
}

// 0x6ad424e0
extern "C" MMRESULT WINAPI WaveOutGetDevCapsA(UINT_PTR uDeviceID, LPWAVEOUTCAPSA pwoc, UINT cbwoc)
{
    if (ModuleState.WaveOutGetDevCapsA == NULL)
    {
        ModuleState.WaveOutGetDevCapsA = (LPWAVEOUTGETDEVCAPSA)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTGETDEVCAPSA_NAME);
    }

    return ModuleState.WaveOutGetDevCapsA(uDeviceID, pwoc, cbwoc);
}

// 0x6ad42530
extern "C" MMRESULT WINAPI WaveOutGetDevCapsW(UINT_PTR uDeviceID, LPWAVEOUTCAPSW pwoc, UINT cbwoc)
{
    if (ModuleState.WaveOutGetDevCapsW == NULL)
    {
        ModuleState.WaveOutGetDevCapsW = (LPWAVEOUTGETDEVCAPSW)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTGETDEVCAPSW_NAME);
    }

    return ModuleState.WaveOutGetDevCapsW(uDeviceID, pwoc, cbwoc);
}

// 0x6ad44d70
extern "C" MMRESULT WINAPI WaveOutGetErrorTextA(MMRESULT mmrError, LPSTR pszText, UINT cchText)
{
    if (ModuleState.WaveOutGetErrorTextA == NULL)
    {
        ModuleState.WaveOutGetErrorTextA = (LPWAVEOUTGETERRORTEXTA)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTGETERRORTEXTA_NAME);
    }

    return ModuleState.WaveOutGetErrorTextA(mmrError, pszText, cchText);
}

// 0x6ad44dc0
extern "C" MMRESULT WINAPI WaveOutGetErrorTextW(MMRESULT mmrError, LPWSTR pszText,UINT cchText)
{
    if (ModuleState.WaveOutGetErrorTextW == NULL)
    {
        ModuleState.WaveOutGetErrorTextW = (LPWAVEOUTGETERRORTEXTW)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTGETERRORTEXTW_NAME);
    }

    return ModuleState.WaveOutGetErrorTextW(mmrError, pszText, cchText);
}

// 0x6ad44e10
extern "C" MMRESULT WINAPI WaveOutGetID(HWAVEOUT hwo, LPUINT puDeviceID)
{
    if (ModuleState.WaveOutGetID == NULL)
    {
        ModuleState.WaveOutGetID = (LPWAVEOUTGETID)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTGETID_NAME);
    }

    return ModuleState.WaveOutGetID(hwo, puDeviceID);
}

// 0x6ad44e60
extern "C" UINT WINAPI WaveOutGetNumDevs(VOID)
{
    if (ModuleState.WaveOutGetNumDevs == NULL)
    {
        ModuleState.WaveOutGetNumDevs = (LPWAVEOUTGETNUMDEVS)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTGETNUMDEVS_NAME);
    }

    return ModuleState.WaveOutGetNumDevs();
}

// 0x6ad44ea0
extern "C" MMRESULT WINAPI WaveOutGetPitch(HWAVEOUT hwo, LPDWORD pdwPitch)
{
    if (ModuleState.WaveOutGetPitch == NULL)
    {
        ModuleState.WaveOutGetPitch = (LPWAVEOUTGETPITCH)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTGETPITCH_NAME);
    }

    return ModuleState.WaveOutGetPitch(hwo, pdwPitch);
}

// 0x6ad44ef0
extern "C" MMRESULT WINAPI WaveOutGetPlaybackRate(HWAVEOUT hwo, LPDWORD pdwRate)
{
    if (ModuleState.WaveOutGetPlaybackRate == NULL)
    {
        ModuleState.WaveOutGetPlaybackRate = (LPWAVEOUTGETPLAYBACKRATE)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTGETPLAYBACKRATE_NAME);
    }

    return ModuleState.WaveOutGetPlaybackRate(hwo, pdwRate);
}

// 0x6ad44f40
extern "C" MMRESULT WINAPI WaveOutGetPosition(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt)
{
    if (ModuleState.WaveOutGetPosition == NULL)
    {
        ModuleState.WaveOutGetPosition = (LPWAVEOUTGETPOSITION)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTGETPOSITION_NAME);
    }

    return ModuleState.WaveOutGetPosition(hwo, pmmt, cbmmt);
}

// 0x6ad420d0
extern "C" MMRESULT WINAPI WaveOutGetVolume(HWAVEOUT hwo, LPDWORD pdwVolume)
{
    *pdwVolume = State.Volume;

    return MMSYSERR_NOERROR;
}

// 0x6ad44f90
extern "C" MMRESULT WINAPI WaveOutMessage(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2)
{
    if (ModuleState.WaveOutMessage == NULL)
    {
        ModuleState.WaveOutMessage = (LPWAVEOUTMESSAGE)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTMESSAGE_NAME);
    }

    return ModuleState.WaveOutMessage(hwo, uMsg, dw1, dw2);
}

// 0x6ad44fe0
extern "C" MMRESULT WINAPI WaveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID, LPCWAVEFORMATEX pwfx, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen)
{
    if (ModuleState.WaveOutOpen == NULL)
    {
        ModuleState.WaveOutOpen = (LPWAVEOUTOPEN)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTOPEN_NAME);
    }

    return ModuleState.WaveOutOpen(phwo, uDeviceID, pwfx, dwCallback, dwInstance, fdwOpen);
}

// 0x6ad45040
extern "C" MMRESULT WINAPI WaveOutPause(HWAVEOUT hwo)
{
    if (ModuleState.WaveOutPause == NULL)
    {
        ModuleState.WaveOutPause = (LPWAVEOUTPAUSE)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTPAUSE_NAME);
    }

    return ModuleState.WaveOutPause(hwo);
}

// 0x6ad45090
extern "C" MMRESULT WINAPI WaveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
    if (ModuleState.WaveOutPrepareHeader == NULL)
    {
        ModuleState.WaveOutPrepareHeader = (LPWAVEOUTPREPAREHEADER)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTPREPAREHEADER_NAME);
    }

    return ModuleState.WaveOutPrepareHeader(hwo, pwh, cbwh);
}

// 0x6ad450e0
extern "C" MMRESULT WINAPI WaveOutReset(HWAVEOUT hwo)
{
    if (ModuleState.WaveOutReset == NULL)
    {
        ModuleState.WaveOutReset = (LPWAVEOUTRESET)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTRESET_NAME);
    }

    return ModuleState.WaveOutReset(hwo);
}

// 0x6ad45130
extern "C" MMRESULT WINAPI WaveOutRestart(HWAVEOUT hwo)
{
    if (ModuleState.WaveOutRestart == NULL)
    {
        ModuleState.WaveOutRestart = (LPWAVEOUTRESTART)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTRESTART_NAME);
    }

    return ModuleState.WaveOutRestart(hwo);
}

// 0x6ad45180
extern "C" MMRESULT WINAPI WaveOutSetPitch(HWAVEOUT hwo, DWORD dwPitch)
{
    if (ModuleState.WaveOutSetPitch == NULL)
    {
        ModuleState.WaveOutSetPitch = (LPWAVEOUTSETPITCH)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTSETPITCH_NAME);
    }

    return ModuleState.WaveOutSetPitch(hwo, dwPitch);
}

// 0x6ad451d0
extern "C" MMRESULT WINAPI WaveOutSetPlaybackRate(HWAVEOUT hwo, DWORD dwRate)
{
    if (ModuleState.WaveOutSetPlaybackRate == NULL)
    {
        ModuleState.WaveOutSetPlaybackRate = (LPWAVEOUTSETPLAYBACKRATE)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTSETPLAYBACKRATE_NAME);
    }

    return ModuleState.WaveOutSetPlaybackRate(hwo, dwRate);
}

// 0x6ad42100
extern "C" MMRESULT WINAPI WaveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume)
{
    return AuxSetVolume(AUDIO_DEVICE_ID, dwVolume);
}

// 0x6ad45220
extern "C" MMRESULT WINAPI WaveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
    if (ModuleState.WaveOutUnprepareHeader == NULL)
    {
        ModuleState.WaveOutUnprepareHeader = (LPWAVEOUTUNPREPAREHEADER)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTUNPREPAREHEADER_NAME);
    }

    return ModuleState.WaveOutUnprepareHeader(hwo, pwh, cbwh);
}

// 0x6ad45270
extern "C" MMRESULT WINAPI WaveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
    if (ModuleState.WaveOutWrite == NULL)
    {
        ModuleState.WaveOutWrite = (LPWAVEOUTWRITE)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTWRITE_NAME);
    }

    return ModuleState.WaveOutWrite(hwo, pwh, cbwh);
}

extern "C" DWORD WINAPI Wid32Message(UINT uId, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    if (ModuleState.Wid32Message == NULL)
    {
        ModuleState.Wid32Message = (LPWID32MESSAGE)GetProcAddress(ModuleState.Module, METHOD_WID32MESSAGE_NAME);
    }

    return ModuleState.Wid32Message(uId, uMsg, dwInstance, dwParam1, dwParam2);
}

extern "C" DWORD WINAPI Wod32Message(UINT uId, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    if (ModuleState.Wod32Message == NULL)
    {
        ModuleState.Wod32Message = (LPWOD32MESSAGE)GetProcAddress(ModuleState.Module, METHOD_WOD32MESSAGE_NAME);
    }

    return ModuleState.Wod32Message(uId, uMsg, dwInstance, dwParam1, dwParam2);
}

extern "C" BOOL WINAPI WOW32DriverCallback(DWORD dwCallback, DWORD dwFlags, WORD wID, WORD wMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
    if (ModuleState.WOW32DriverCallback == NULL)
    {
        ModuleState.WOW32DriverCallback = (LPWOW32DRIVERCALLBACK)GetProcAddress(ModuleState.Module, METHOD_WOW32DRIVERCALLBACK_NAME);
    }

    return ModuleState.WOW32DriverCallback(dwCallback, dwFlags, wID, wMsg, dwUser, dw1, dw2);
}

extern "C" BOOL WINAPI WOW32ResolveMultiMediaHandle(UINT uHandleType, UINT uMappingDirection, WORD wHandle16_In, LPWORD lpwHandle16_Out, DWORD dwHandle32_In, LPDWORD lpdwHandle32_Out)
{
    if (ModuleState.WOW32ResolveMultiMediaHandle == NULL)
    {
        ModuleState.WOW32ResolveMultiMediaHandle = (LPWOW32RESOLVEMULTIMEDIAHANDLE)GetProcAddress(ModuleState.Module, METHOD_WOW32RESOLVEMULTIMEDIAHANDLE_NAME);
    }

    return ModuleState.WOW32ResolveMultiMediaHandle(uHandleType, uMappingDirection, wHandle16_In, lpwHandle16_Out, dwHandle32_In, lpdwHandle32_Out);
}

extern "C" VOID WOWAppExit(HANDLE hTask)
{
    if (ModuleState.WOWAppExit == NULL)
    {
        ModuleState.WOWAppExit = (LPWOWAPPEXIT)GetProcAddress(ModuleState.Module, METHOD_WOWAPPEXIT_NAME);
    }

    ModuleState.WOWAppExit(hTask);
}
