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

#include "State.hxx"
#include "SoundState.hxx"

#include <stdlib.h>

// 0x004104d8
SOUNDSTATECONTAINER SoundState;

// 0x00401fa0
VOID InitializeDirectSoundState0x18(LPSOUNDSTATEUNK0X18 self)
{
    self->Unk00 = 0; // TODO
    self->Unk01 = -10000; // TODO
    self->Unk02 = 0; // TODO
    self->Unk03 = -10000; // TODO
    self->Unk04 = NULL;
}

// 0x00401fc0
// 0x00401fd0
VOID ReleaseDirectSoundState0x18(LPSOUNDSTATEUNK0X18 self)
{
    if (State.SoundState->Instance != NULL)
    {
        if (self->Unk04 != NULL)
        {
            // TODO
        }

        self->Unk00 = 0; // TODO
    }
}

// 0x00401400
VOID InitializeSoundStateContainer()
{
    InitializeSoundState(&SoundState);
}

// 0x00401410
VOID ActivateReleaseSoundStateContainer()
{
    atexit(ReleaseSoundStateContainer);
}

// 0x004013f0
VOID ActivateSoundStateContainer()
{
    InitializeSoundStateContainer();
    ActivateReleaseSoundStateContainer();
}

// 0x00402010
LPSOUNDSTATECONTAINER InitializeSoundState(LPSOUNDSTATECONTAINER self)
{
    self->Instance = NULL;
    self->Buffer = NULL;

    self->Items = NULL;
    self->Count = 0;

    self->Result = DS_OK;
    self->State = SOUND_STATE_NONE;

    self->Unk07 = 500; // TODO
    self->Unk08 = -10000; // TODO

    State.SoundState = self;

    return self;
}

// 0x00401420
VOID ReleaseSoundStateContainer()
{
    ReleaseSoundState(&SoundState);
}

// 0x00402200
VOID ReleaseSoundState(LPSOUNDSTATECONTAINER self)
{
    if (self->Items != NULL)
    {
        for (U32 x = 0; x < self->Count; x++)
        {
            ReleaseDirectSoundState0x18(&self->Items[x]);
        }

        free(self->Items);

        self->Items = NULL;
    }

    self->Count = 0;

    DIRECTSOUNDRELEASE(self->Buffer);
    DIRECTSOUNDRELEASE(self->Instance);
}

// 0x00402060
BOOL InitializeSoundState(LPSOUNDSTATECONTAINER self, HWND window, CONST U32 count)
{
    ReleaseSoundState(self);

    self->Result = DirectSoundCreate(NULL, &self->Instance, NULL);

    if (FAILED(self->Result)) { self->State = SOUND_STATE_INIT_ERROR; return FALSE; }

    self->Result = self->Instance->SetCooperativeLevel(window, DSSCL_PRIORITY);

    if (FAILED(self->Result))
    {
        DIRECTSOUNDRELEASE(self->Instance);

        self->State = SOUND_STATE_SETUP_ERROR;

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

        self->State = SOUND_STATE_BUFFER_ERROR;

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
        format.nChannels = WAVE_FORMAT_PCM;
        format.nBlockAlign = 2;
        format.wBitsPerSample = 16;
        format.nSamplesPerSec = 22050;
        format.nAvgBytesPerSec = 22050 * 2;

        self->Buffer->SetFormat(&format);
    }

    self->Buffer->Play(0, 0, DSBPLAY_LOOPING);

    self->Count = count;
    self->Items = (LPSOUNDSTATEUNK0X18)malloc(count * sizeof(SOUNDSTATEUNK0X18));

    if (self->Items == NULL) { self->Count = 0; }

    for (U32 x = 0; x < self->Count; x++)
    {
        InitializeDirectSoundState0x18(&self->Items[x]);
    }

    return TRUE;
}