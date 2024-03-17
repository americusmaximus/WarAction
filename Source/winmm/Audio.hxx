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

#include <vorbis/vorbisfile.h>

#define AUDIO_DEVICE_ID 257

#define MIN_AUDIO_VOLUME 0
#define MAX_AUDIO_VOLUME 100

#define MAX_AUDIO_WAVE_HEADER_COUNT 3

struct AudioWorkerContext
{
    INT First;
    INT Last;
};

struct AudioStateContainer
{
    BOOL IsActive; // 0x6ad52034

    OggVorbis_File Vorbis; // 0x6ad59d80

    struct
    {
        BOOL IsActive; // 0x6ad52024
        DWORD_PTR Callback; // 0x6ad59518
    } Alerts;

    CRITICAL_SECTION Mutex; // 0x6ad59d54

    INT Track; // 0x6ad52030

    AudioWorkerContext Context; // 0x6ad4c008

    DOUBLE Position; // 0x6ad52028

    INT Volume; // 0x6ad4c020

    HANDLE Worker; // 0x6ad5203c

    struct
    {
        HANDLE Callback; // 0x6ad58af0

        WAVEFORMATEX Format; // 0x6ad59d40

        UINT Count; // 0x6ad58aec

        LPWAVEHDR Headers[MAX_AUDIO_WAVE_HEADER_COUNT]; // 0x6ad58ae0

        HWAVEOUT Out; // 0x6ad58af4
    } Wave;
};

extern AudioStateContainer AudioState;

DWORD WINAPI AudioWorker(LPVOID params);

FLOAT AcquireAudioLength(LPCSTR track);
FLOAT AcquireAudioPosition(VOID);

VOID InitializeAudio(VOID);
VOID PauseAudio(VOID);
VOID SelectAudioVolume(CONST INT volume);
VOID StartAudio(VOID);