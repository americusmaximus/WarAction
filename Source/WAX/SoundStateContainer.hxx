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

#include "DirectSound.hxx"

typedef enum SoundState
{
    SOUND_STATE_NONE = 0,
    SOUND_STATE_INIT_ERROR = 1,
    SOUND_STATE_SETUP_ERROR = 2,
    SOUND_STATE_BUFFER_ERROR = 3,
    SOUND_STATE_FORCE_DWORD = 0x7fffffff
} SOUNDSTATE;

typedef struct SoundStateUnk0x18 /* TODO */
{
    U32 Unk00; // TODO
    S32 Unk01; // TODO
    U32 Unk02; // TODO
    S32 Unk03; // TODO
    LPVOID Unk04; // TODO
    U32 Unk05; // TODO
} SOUNDSTATEUNK0X18, * LPSOUNDSTATEUNK0X18;

typedef struct SoundStateContainer
{
    LPDIRECTSOUND Instance;
    LPDIRECTSOUNDBUFFER Buffer;
    SOUNDSTATE State;
    HRESULT Result;
    U32 Count;
    LPSOUNDSTATEUNK0X18 Items;
    BOOL IsReverseStereo;
    U32 Unk07; // TODO
    S32 Unk08; // TODO
} SOUNDSTATECONTAINER, * LPSOUNDSTATECONTAINER;