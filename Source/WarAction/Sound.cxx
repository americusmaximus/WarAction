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

#include "Sound.hxx"
#include "State.hxx"

#include <stdlib.h>

// 0x004104d8
SOUND SoundState;

// 0x00401fa0
VOID CLASSCALL ActivateSoundTrack(SOUNDTRACKPTR self)
{
    self->State = NULL;
    self->Volume = DSBVOLUME_MIN;
    self->MinVolume = DSBVOLUME_MIN;
    self->Pan = DSBPAN_CENTER;
    self->Buffer = NULL;
}

// 0x00401fc0
// 0x00401fd0
VOID CLASSCALL ReleaseSoundTrack(SOUNDTRACKPTR self)
{
    if (State.Sound->Instance != NULL)
    {
        if (self->Buffer != NULL)
        {
            self->Buffer->Stop();
            DIRECTSOUNDRELEASE(self->Buffer);
        }

        self->State = NULL;
    }
}

// 0x00401400
VOID ActivateSoundState(VOID)
{
    ActivateSoundState(&SoundState);
}

// 0x00401410
VOID ActivateReleaseSoundState(VOID)
{
    atexit(ReleaseSoundStateContainer);
}

// 0x004013f0
VOID ActivateSoundStateContainer(VOID)
{
    ActivateSoundState();
    ActivateReleaseSoundState();
}

// 0x00402010
SOUNDPTR CLASSCALL ActivateSoundState(SOUNDPTR self)
{
    self->Instance = NULL;
    self->Buffer = NULL;

    self->Tracks = NULL;
    self->Count = 0;

    self->Result = DS_OK;
    self->State = SOUNDRESULT_NONE;

    self->Unk07 = 500; // TODO
    self->Unk08 = -10000; // TODO

    State.Sound = self;

    return self;
}

// 0x00401420
VOID ReleaseSoundStateContainer(VOID)
{
    ReleaseSoundState(&SoundState);
}

// 0x00402200
VOID CLASSCALL ReleaseSoundState(SOUNDPTR self)
{
    if (self->Tracks != NULL)
    {
        for (U32 x = 0; x < self->Count; x++)
        {
            ReleaseSoundTrack(&self->Tracks[x]);
        }

        free(self->Tracks);

        self->Tracks = NULL;
    }

    self->Count = 0;

    DIRECTSOUNDRELEASE(self->Buffer);
    DIRECTSOUNDRELEASE(self->Instance);
}

// 0x00402060
BOOL CLASSCALL InitializeSoundState(SOUNDPTR self, HWND window, CONST U32 count)
{
    ReleaseSoundState(self);

    self->Result = DirectSoundCreate(NULL, &self->Instance, NULL);

    if (FAILED(self->Result)) { self->State = SOUNDRESULT_INITIALIZE_ERROR; return FALSE; }

    self->Result = self->Instance->SetCooperativeLevel(window, DSSCL_PRIORITY);

    if (FAILED(self->Result))
    {
        DIRECTSOUNDRELEASE(self->Instance);

        self->State = SOUNDRESULT_SET_COOPERATIVE_LEVEL_ERROR;

        return FALSE;
    }

    DSBUFFERDESC desc;
    ZeroMemory(&desc, sizeof(DSBUFFERDESC));

    desc.dwSize = sizeof(DSBUFFERDESC);
    desc.dwFlags = DSBCAPS_PRIMARYBUFFER;

    self->Result = self->Instance->CreateSoundBuffer(&desc, &self->Buffer, NULL);

    if (FAILED(self->Result))
    {
        DIRECTSOUNDRELEASE(self->Instance);

        self->State = SOUNDRESULT_CREATE_MAIN_SOUND_BUFFER_ERROR;

        return FALSE;
    }

    WAVEFORMATEX format;
    ZeroMemory(&format, sizeof(WAVEFORMATEX));

    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = 2;
    format.nBlockAlign = 4;
    format.wBitsPerSample = 16;
    format.nSamplesPerSec = 22050;
    format.nAvgBytesPerSec = 22050 * 4;

    if (FAILED(self->Buffer->SetFormat(&format)))
    {
        format.nChannels = 1;
        format.nBlockAlign = 2;
        format.wBitsPerSample = 16;
        format.nSamplesPerSec = 22050;
        format.nAvgBytesPerSec = 22050 * 2;

        self->Buffer->SetFormat(&format);
    }

    self->Buffer->Play(0, 0, DSBPLAY_LOOPING);

    self->Count = count;
    self->Tracks = (SOUNDTRACKPTR)malloc(count * sizeof(SOUNDTRACK));

    if (self->Tracks == NULL) { self->Count = 0; }

    for (U32 x = 0; x < self->Count; x++)
    {
        ActivateSoundTrack(&self->Tracks[x]);
    }

    return TRUE;
}