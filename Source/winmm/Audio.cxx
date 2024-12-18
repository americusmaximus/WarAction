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

#define Min(x,y) ((x)>(y)?(y):(x))
#define Max(x,y) ((x)<(y)?(y):(x))

AudioStateContainer AudioState;

// 0x6ad45610
VOID InitializeAudio()
{
    InitializeCriticalSection(&AudioState.Mutex);
}

// 0x6ad45670
// a.k.a. plr_stop
VOID StopAudio()
{
    AudioState.Wave.Count = 0;

    EnterCriticalSection(&AudioState.Mutex);

    if (AudioState.Vorbis.datasource != NULL) { ov_clear(&AudioState.Vorbis); }

    LeaveCriticalSection(&AudioState.Mutex);

    if (AudioState.Wave.Callback != NULL)
    {
        CloseHandle(AudioState.Wave.Callback);
        
        AudioState.Wave.Callback = NULL;
    }

    if (AudioState.Wave.Out != NULL)
    {
        if (ModuleState.WaveOutReset == NULL)
        {
            ModuleState.WaveOutReset = (LPWAVEOUTRESET)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTRESET_NAME);
        }

        ModuleState.WaveOutReset(AudioState.Wave.Out);

        for (INT x = 0; x < MAX_AUDIO_WAVE_HEADER_COUNT; x++)
        {
            LPWAVEHDR pwh = AudioState.Wave.Headers[x];

            if (pwh != NULL && (pwh->dwFlags & WHDR_DONE))
            {
                if (ModuleState.WaveOutUnprepareHeader == NULL)
                {
                    ModuleState.WaveOutUnprepareHeader = (LPWAVEOUTUNPREPAREHEADER)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTUNPREPAREHEADER_NAME);
                }

                ModuleState.WaveOutUnprepareHeader(AudioState.Wave.Out, pwh, sizeof(WAVEHDR));

                free(AudioState.Wave.Headers[x]->lpData);
                free(AudioState.Wave.Headers[x]);

                AudioState.Wave.Headers[x] = NULL;
            }
        }

        if (ModuleState.WaveOutClose == NULL)
        {
            ModuleState.WaveOutClose = (LPWAVEOUTCLOSE)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTCLOSE_NAME);
        }

        ModuleState.WaveOutClose(AudioState.Wave.Out);

        AudioState.Wave.Out = NULL;
    }
}

// 0x6ad45810
// a.k.a. plr_play
BOOL OpenAudio(SoundTrack* track, DOUBLE position)
{
    StopAudio();

    EnterCriticalSection(&AudioState.Mutex);

    INT result = ov_fopen(track->Name, &AudioState.Vorbis);

    LeaveCriticalSection(&AudioState.Mutex);

    if (result != 0) { return FALSE; }

    EnterCriticalSection(&AudioState.Mutex);

    ov_time_seek(&AudioState.Vorbis, position);

    vorbis_info* vi = ov_info(&AudioState.Vorbis, -1);

    LeaveCriticalSection(&AudioState.Mutex);

    if (vi != NULL)
    {
        AudioState.Wave.Format.wFormatTag = WAVE_FORMAT_PCM;
        AudioState.Wave.Format.nChannels = (WORD)vi->channels;
        AudioState.Wave.Format.nSamplesPerSec = vi->rate;
        AudioState.Wave.Format.nBlockAlign = AudioState.Wave.Format.nChannels * 2;
        AudioState.Wave.Format.nAvgBytesPerSec =
            AudioState.Wave.Format.nBlockAlign * AudioState.Wave.Format.nSamplesPerSec;
        AudioState.Wave.Format.wBitsPerSample = 16;
        AudioState.Wave.Format.cbSize = 0;

        if (AudioState.Wave.Callback != NULL)
        {
            CloseHandle(AudioState.Wave.Callback);

            AudioState.Wave.Callback = NULL;
        }

        AudioState.Wave.Callback = CreateEventA(NULL, FALSE, TRUE, NULL);

        if (ModuleState.WaveOutOpen == NULL)
        {
            ModuleState.WaveOutOpen = (LPWAVEOUTOPEN)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTOPEN_NAME);
        }

        if (ModuleState.WaveOutOpen(&AudioState.Wave.Out, WAVE_MAPPER,
            &AudioState.Wave.Format, (DWORD_PTR)AudioState.Wave.Callback, NULL, CALLBACK_EVENT) == MMSYSERR_NOERROR) { return TRUE; }
    }

    EnterCriticalSection(&AudioState.Mutex);

    ov_clear(&AudioState.Vorbis);

    LeaveCriticalSection(&AudioState.Mutex);

    return FALSE;
}

// 0x6ad459c0
// a.k.a. plr_pump
BOOL PlayAudio()
{
    if (AudioState.Vorbis.datasource == NULL)
    {
        EnterCriticalSection(&AudioState.Mutex);

        ov_clear(&AudioState.Vorbis);

        LeaveCriticalSection(&AudioState.Mutex);

        return FALSE;
    }

    CONST UINT size = AudioState.Wave.Format.nAvgBytesPerSec >> 2;

    if (size == 0) { return TRUE; }

    LPSTR buffer = (LPSTR)malloc(size);

    if (buffer == NULL) { return TRUE; }

    UINT length = 0;

    while (length < size)
    {
        EnterCriticalSection(&AudioState.Mutex);

        CONST LONG result = ov_read(&AudioState.Vorbis,
            (CHAR*)((size_t)buffer + (size_t)length), size - length, 0, sizeof(SHORT), 1, NULL);

        LeaveCriticalSection(&AudioState.Mutex);

        if (result == OV_HOLE) { continue; }
        else if (result == OV_EBADLINK || result == OV_EINVAL)
        {
            free(buffer);

            EnterCriticalSection(&AudioState.Mutex);

            ov_clear(&AudioState.Vorbis);

            LeaveCriticalSection(&AudioState.Mutex);

            return FALSE;
        }
        else if (result == 0 /* EOF */)
        {
            free(buffer);

            for (INT x = 0; x < MAX_AUDIO_WAVE_HEADER_COUNT; x++)
            {
                LPWAVEHDR pvh = AudioState.Wave.Headers[x];

                if (pvh != NULL && (pvh->dwFlags & WHDR_DONE))
                {
                    if (ModuleState.WaveOutUnprepareHeader == NULL)
                    {
                        ModuleState.WaveOutUnprepareHeader = (LPWAVEOUTUNPREPAREHEADER)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTUNPREPAREHEADER_NAME);
                    }

                    ModuleState.WaveOutUnprepareHeader(AudioState.Wave.Out, pvh, sizeof(WAVEHDR));

                    free(AudioState.Wave.Headers[x]->lpData);
                    free(AudioState.Wave.Headers[x]);

                    AudioState.Wave.Headers[x] = NULL;
                }
            }

            Sleep(100);

            EnterCriticalSection(&AudioState.Mutex);

            ov_clear(&AudioState.Vorbis);

            LeaveCriticalSection(&AudioState.Mutex);

            return FALSE;
        }

        length = length + result;
    }

    for (INT x = 0; x < (length / sizeof(SHORT)); x++)
    {
        ((SHORT*)buffer)[x] = (SHORT)((SHORT*)buffer)[x] * ((FLOAT)AudioState.Volume / 100.0f);
    }

    LPWAVEHDR header = (LPWAVEHDR)malloc(sizeof(WAVEHDR));

    if (header == NULL) { free(buffer); return TRUE; }

    ZeroMemory(header, sizeof(WAVEHDR));

    header->lpData = buffer;
    header->dwBufferLength = length;
    header->dwFlags = AudioState.Wave.Count == 0 ? 0 : WHDR_BEGINLOOP;

    if (ModuleState.WaveOutPrepareHeader == NULL)
    {
        ModuleState.WaveOutPrepareHeader = (LPWAVEOUTPREPAREHEADER)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTPREPAREHEADER_NAME);
    }

    ModuleState.WaveOutPrepareHeader(AudioState.Wave.Out, header, sizeof(WAVEHDR));

    if (AudioState.Wave.Count != 0) { WaitForSingleObject(AudioState.Wave.Callback, INFINITE); }

    BOOL queued = FALSE;

    for (INT xx = 0; xx < MAX_AUDIO_WAVE_HEADER_COUNT; xx++)
    {
        LPWAVEHDR pwh = AudioState.Wave.Headers[xx];

        if (pwh != NULL && (pwh->dwFlags & WHDR_DONE))
        {
            if (ModuleState.WaveOutUnprepareHeader == NULL)
            {
                ModuleState.WaveOutUnprepareHeader = (LPWAVEOUTUNPREPAREHEADER)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTUNPREPAREHEADER_NAME);
            }

            ModuleState.WaveOutUnprepareHeader(AudioState.Wave.Out, pwh, sizeof(WAVEHDR));

            free(AudioState.Wave.Headers[xx]->lpData);
            free(AudioState.Wave.Headers[xx]);

            AudioState.Wave.Headers[xx] = NULL;
        }

        if (!queued && AudioState.Wave.Headers[xx] == NULL)
        {
            if (ModuleState.WaveOutWrite == NULL)
            {
                ModuleState.WaveOutWrite = (LPWAVEOUTWRITE)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTWRITE_NAME);
            }

            ModuleState.WaveOutWrite(AudioState.Wave.Out, header, sizeof(WAVEHDR));

            queued = TRUE;

            AudioState.Wave.Headers[xx] = header;
        }
    }

    if (!queued)
    {
        free(header);
        free(buffer);
    }

    AudioState.Wave.Count = AudioState.Wave.Count + 1;

    return TRUE;
}

// 0x6ad41280
// a.k.a. player_main
DWORD WINAPI AudioWorker(LPVOID params)
{
    AudioWorkerContext* context = (AudioWorkerContext*)params;

    AudioState.Track = context->First;
    INT last = context->Last;

    while (TRUE)
    {
        while (TRUE)
        {
            if (last < AudioState.Track)
            {
                EnterCriticalSection(&State.Mutex);

                if (!AudioState.IsExit)
                {
                    AudioState.Worker = NULL;
                    AudioState.State = FALSE;

                    StopAudio();

                    if (AudioState.Alerts.IsActive)
                    {
                        AudioState.Alerts.IsActive = FALSE;

                        SendMessageA((HWND)AudioState.Alerts.Callback,
                            MM_MCINOTIFY, MCI_NOTIFY_SUCCESSFUL, AUDIO_DEVICE_ID);
                    }

                    LeaveCriticalSection(&State.Mutex);

                    ExitThread(EXIT_SUCCESS);
                }

                LeaveCriticalSection(&State.Mutex);
            }

            while (!AudioState.State) { Sleep(200); }

            if (!AudioState.IsExit) { break; }

            AudioState.IsExit = FALSE;

            last = AudioState.Context.Last;

            AudioState.Track = AudioState.Context.First;
        }

        AudioState.State = OpenAudio(&State.Tracks.Tracks[AudioState.Track], AudioState.Position);

        do
        {
            if (!AudioState.State || !PlayAudio())
            {
                if (!AudioState.IsExit) { AudioState.Track = AudioState.Track + 2; } break;
            }

            Sleep(200);
        } while (!AudioState.IsExit);
    }
}

// 0x6ad45d20
FLOAT AcquireAudioPosition(VOID)
{
    if (AudioState.Vorbis.datasource == NULL) { return 0.0f; }

    EnterCriticalSection(&AudioState.Mutex);

    CONST FLOAT result = (FLOAT)ov_time_tell(&AudioState.Vorbis);

    LeaveCriticalSection(&AudioState.Mutex);

    return result;
}

// 0x6ad45630
VOID PauseAudio(VOID)
{
    if (AudioState.Wave.Out != NULL)
    {
        if (ModuleState.WaveOutPause == NULL)
        {
            ModuleState.WaveOutPause = (LPWAVEOUTPAUSE)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTPAUSE_NAME);
        }

        ModuleState.WaveOutPause(AudioState.Wave.Out);
    }
}

// 0x6ad45770
// a.k.a. plr_volume
VOID SelectAudioVolume(CONST INT volume)
{
    AudioState.Volume = Min(Max(volume, MIN_AUDIO_VOLUME), MAX_AUDIO_VOLUME);
}

// 0x6ad45650
VOID StartAudio(VOID)
{
    if (AudioState.Wave.Out != NULL)
    {
        if (ModuleState.WaveOutRestart == NULL)
        {
            ModuleState.WaveOutRestart = (LPWAVEOUTRESTART)GetProcAddress(ModuleState.Module, METHOD_WAVEOUTRESTART_NAME);
        }

        ModuleState.WaveOutRestart(AudioState.Wave.Out);
    }
}

// 0x6ad457b0
// a.k.a. plr_length
FLOAT AcquireAudioLength(LPCSTR track)
{
    OggVorbis_File vf;

    if (ov_fopen(track, &vf) != 0) { return 0.0f; }

    CONST FLOAT result = (FLOAT)ov_time_total(&vf, -1);

    ov_clear(&vf);

    return result;
}
