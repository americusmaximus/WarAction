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

#define MAX_SOUND_NAME_LENGTH   64

typedef enum SoundState
{
    SOUNDSTATE_NONE                                 = 0,
    SOUNDSTATE_INITIALIZE_ERROR                     = 1,
    SOUNDSTATE_SET_COOPERATIVE_LEVEL_ERROR          = 2,
    SOUNDSTATE_CREATE_MAIN_SOUND_BUFFER_ERROR       = 3,
    SOUNDSTATE_DUPLICATE_SOUND_BUFFER_ERROR         = 4,
    SOUNDSTATE_BUFFER_PLAY_ERROR                    = 5,
    SOUNDSTATE_NO_SOUND_BUFFER_ERROR                = 6,
    SOUNDSTATE_NO_SOUND_INSTANCE_ERROR              = 7,
    SOUNDSTATE_INVALID_SETTINGS_ERROR               = 8,
    SOUNDSTATE_CREATE_SECONDARY_SOUND_BUFFER_ERROR  = 9,
    SOUNDSTATE_LOCK_SOUND_BUFFER_ERROR              = 10,
    SOUNDSTATE_NO_TRACKS_AVAILABLE                  = 11,
    SOUNDSTATE_FORCE_DWORD                          = 0x7FFFFFFF
} SOUNDSTATE;

typedef struct SoundBuffer
{
    BOOL                    IsActive;
    LPDIRECTSOUNDBUFFER     Buffer;
} SOUNDBUFFER, * SOUNDBUFFERPTR;

typedef struct SoundTrack
{
    SOUNDBUFFERPTR          State;
    S32                     Volume;
    S32                     Pan;
    S32                     MinVolume;
    LPDIRECTSOUNDBUFFER     Buffer;
    U32                     Ticks;
} SOUNDTRACK, * SOUNDTRACKPTR;

typedef struct SoundDescriptor
{
    CHAR    Name[MAX_SOUND_NAME_LENGTH];
    U32     Unk01; // TODO
    U32     Unk02; // TODO
    U32     Unk03; // TODO
    U32     Unk04; // TODO
    //U32     Unk05; // TODO
} SOUNDDESCRIPTOR, * SOUNDDESCRIPTORPTR;

typedef struct SoundHeader
{
    U32                     Count;
    SOUNDDESCRIPTOR         Sounds[1];
} SOUNDHEADER, * SOUNDHEADERPTR;

typedef struct SoundStateContainer
{
    LPDIRECTSOUND           Instance;
    LPDIRECTSOUNDBUFFER     Buffer;
    SOUNDSTATE              State;
    HRESULT                 Result;
    U32                     Count;
    SOUNDTRACKPTR           Tracks;
    BOOL                    IsReverseStereo;
    S32                     Unk07; // TODO
    S32                     Unk08; // TODO
} SOUNDSTATECONTAINER, * SOUNDSTATECONTAINERPTR;