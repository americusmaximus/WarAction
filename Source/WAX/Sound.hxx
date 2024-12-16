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

typedef enum SoundResult
{
    SOUNDRESULT_NONE                                = 0,
    SOUNDRESULT_INITIALIZE_ERROR                    = 1,
    SOUNDRESULT_SET_COOPERATIVE_LEVEL_ERROR         = 2,
    SOUNDRESULT_CREATE_MAIN_SOUND_BUFFER_ERROR      = 3,
    SOUNDRESULT_DUPLICATE_SOUND_BUFFER_ERROR        = 4,
    SOUNDRESULT_BUFFER_PLAY_ERROR                   = 5,
    SOUNDRESULT_NO_SOUND_BUFFER_ERROR               = 6,
    SOUNDRESULT_NO_SOUND_INSTANCE_ERROR             = 7,
    SOUNDRESULT_INVALID_SETTINGS_ERROR              = 8,
    SOUNDRESULT_CREATE_SOUND_BUFFER_ERROR           = 9,
    SOUNDRESULT_LOCK_SOUND_BUFFER_ERROR             = 10,
    SOUNDRESULT_NO_TRACKS_AVAILABLE                 = 11,
    SOUNDRESULT_FORCE_DWORD                         = 0x7FFFFFFF
} SOUNDRESULT, * SOUNDRESULTPTR;

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

/*
SoundDescriptor and SoundDescriptorEx Format:
    The structures are variable-sized structures.
    
    Offset 0x00 - Name.
    Offset 0x40 - Count. Number of sound chunks in the sound.
    Offset 0x44 - Chunks. An index to a chunk header in the sound file.
*/

typedef struct SoundDescriptor
{
    CHAR    Name[MAX_SOUND_NAME_LENGTH];
    U32     Count;
} SOUNDDESCRIPTOR, * SOUNDDESCRIPTORPTR;

typedef struct SoundDescriptorEx
{
    CHAR    Name[MAX_SOUND_NAME_LENGTH];
    U32     Count;
    U32     Chunks[1];
} SOUNDDESCRIPTOREX, * SOUNDDESCRIPTOREXPTR;

/*
SOUNDS.HDR File Format:
    Offset 0x0 - U32 - Count.
                    Number of elements in the file.
    Offset 0x4 - Array of SOUNDDESCRIPTOR - Sounds.
                    A contiguous array of variable-size structures SoundDescriptor.
*/

typedef struct SoundHeader
{
    U32                     Count;
    SOUNDDESCRIPTOR         Sounds[1];
} SOUNDHEADER, * SOUNDHEADERPTR;

typedef struct Sound
{
    LPDIRECTSOUND           Instance; // TODO Modern sound mode?
    LPDIRECTSOUNDBUFFER     Buffer; // TODO Modern sound mode?
    SOUNDRESULT             State;
    HRESULT                 Result;
    U32                     Count;
    SOUNDTRACKPTR           Tracks;
    BOOL                    IsReverseStereo;
    S32                     Unk07; // TODO
    S32                     Unk08; // TODO
} SOUND, * SOUNDPTR;

/*
SOUNDS.RUS File Format:
    Offset 0x0 - U32 - Count.
                    Number of sound chunks in the file.
    Offset 0x4  - Array of offsets to actual audio chunks.

    ------------------------

    Audio chunks start with 3 integers: # of smaples/second, # bits/second, # of channels (can be 0).
    Then the actual sound data...
*/