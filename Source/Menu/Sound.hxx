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

#include "BinFile.hxx"

#include <Sound.hxx>

#define MAX_SOUND_TRACK_COUNT       50

#define INVALID_SOUND_TRACK_INDEX   (-1)

typedef struct SoundModuleTrack
{
    U32             Hash;
    SOUNDBUFFERPTR  Buffer;
} SOUNDMODULETRACK, * SOUNDMODULETRACKPTR;

typedef struct SoundState
{
    SOUNDPTR                    State;
    U32                         Count;
    BOOL                        IsReleased;
    SOUNDMODULETRACK            Tracks[MAX_SOUND_TRACK_COUNT];
    struct
    {
        SOUNDBUFFERPTR          Back;
        SOUNDBUFFERPTR          Action;
    } Buffers;
    SOUNDHEADERPTR              Header;
    BINFILE                     File;
} SOUNDSTATE, * SOUNDSTATEPTR;

typedef struct SoundStateModuleContainer
{
    SOUNDSTATE      State;      // 0x100462c8

    SOUNDPTR        Sound;      // 0x1005ae58
} SOUNDSTATEMODULECONTAINER, * SOUNDSTATEMODULECONTAINERPTR;

EXTERN SOUNDSTATEMODULECONTAINER SoundState;

SOUNDBUFFERPTR CLASSCALL ActivateSoundBuffer(SOUNDBUFFERPTR self, LPDIRECTSOUNDBUFFER buffer, CONST BOOL mode);
VOID CLASSCALL ReleaseSoundBuffer(SOUNDBUFFERPTR self);

BOOL CLASSCALL InitializeSoundTrack(SOUNDTRACKPTR self, SOUNDBUFFERPTR buffer, CONST F32 volume, CONST F32 pan);
VOID CLASSCALL ReleaseSoundTrack(SOUNDTRACKPTR self);

BOOL CLASSCALL InitializeSoundStateSample(SOUNDSTATEPTR self, LPCSTR name);
SOUNDBUFFERPTR CLASSCALL ActivateSoundStateSoundBuffer(SOUNDPTR self, LPCVOID content, CONST U32 size, CONST U32 channels, CONST U32 bits, CONST U32 samples, CONST BOOL active);
VOID CLASSCALL InitializeSoundState(SOUNDSTATEPTR self, SOUNDPTR state);
VOID CLASSCALL InitializeSoundStateActionBuffer(SOUNDSTATEPTR self, LPCSTR name);
VOID CLASSCALL InitializeSoundStateBackBuffer(SOUNDSTATEPTR self, CONST BOOL activate);
VOID CLASSCALL InitializeSoundStateBackBuffer(SOUNDSTATEPTR self, LPCSTR name);
VOID CLASSCALL PlaySoundStateSound(SOUNDSTATEPTR self, LPCSTR name);
VOID CLASSCALL ReleaseSoundState(SOUNDSTATEPTR self);
VOID CLASSCALL ReleaseSoundStateActionBuffer(SOUNDSTATEPTR self);
VOID CLASSCALL ReleaseSoundStateInactiveSoundTracks(SOUNDSTATEPTR self);
VOID CLASSCALL ReleaseSoundStateTracks(SOUNDSTATEPTR self, CONST BOOL release);

BOOL CLASSCALL InitializeSoundStateSoundTrack(SOUNDPTR self, SOUNDBUFFERPTR buffer, CONST F32 volume, CONST F32 pan);
LPDIRECTSOUNDBUFFER CLASSCALL ActivateSoundStateSoundBuffer(SOUNDPTR self, CONST U32 size, CONST U32 channels, CONST U32 bits, CONST U32 samples);
S32 CLASSCALL AcquireSoundStateTrackIndex(SOUNDPTR self);
SOUNDBUFFERPTR CLASSCALL InitializeSoundState(SOUNDPTR self, BINFILEPTR file, CONST U32 size, CONST U32 channels, CONST U32 bits, CONST U32 samples, CONST BOOL active);
VOID CLASSCALL ReleaseSoundStateInactiveSoundTracks(SOUNDPTR self);
VOID CLASSCALL ReleaseSoundStateSoundTracks(SOUNDPTR self);

U32 AcquireSoundNameHash(LPCSTR name);

VOID FUN_1000c5a0(); // TODO name
U32 FUN_1000c600(CONST U32 value); // TODO name
VOID FUN_1000c5c0(CONST U32 indx); // TODO name