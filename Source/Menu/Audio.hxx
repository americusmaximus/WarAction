/*
Copyright (c) 2024 - 2025 Americus Maximus

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
#include "Objects.hxx"

#include <mmeapi.h>

#define MAX_AUDIOPLAYER_MESSAGE_LENGTH      256

#define AUDIO_TRACK_OFFSET                  2

// Returns the current mode of the device.
// All devices can return the "not ready", "paused", "playing", and "stopped" values.
// Some devices can return the additional "open", "parked", "recording", and "seeking" values.
#define AUDIOPLAYERMODE_ERROR               (-1)
#define AUDIOPLAYERMODE_IDLE                0
#define AUDIOPLAYERMODE_PLAYING             1
#define AUDIOPLAYERMODE_STOPPED             2
#define AUDIOPLAYERMODE_PAUSED              3
#define AUDIOPLAYERMODE_OTHER               4

struct AudioPlayer;

typedef AudioPlayer* (CLASSCALL* AUDIOPLAYERRELEASEACTION)(AudioPlayer* self, CONST OBJECTRELEASETYPE mode);

typedef struct AudioPlayerSelf
{
    AUDIOPLAYERRELEASEACTION Release;
} AUDIOPLAYERSELF, * AUDIOPLAYERSELFPTR;

EXTERN AUDIOPLAYERSELF AudioPlayerSelfState;

typedef enum AudioCommand
{
    AUDIOCOMMAND_NONE           = 0,
    AUDIOCOMMAND_PLAY_TRACK     = 1,
    AUDIOCOMMAND_STOP           = 2,
    AUDIOCOMMAND_PAUSE          = 3,
    AUDIOCOMMAND_RESUME         = 4,
    AUDIOCOMMAND_PLAY_TIME      = 5,
    AUDIOCOMMAND_CLOSE          = 6,
    AUDIOCOMMAND_FORCE_DWORD    = 0x7FFFFFFF
} AUDIOCOMMAND, * AUDIOCOMMANDPTR;

typedef struct AudioPlayer
{
    AUDIOPLAYERSELFPTR                  Self;
    HANDLE                              Thread;
    HMIXEROBJ                           Mixer;
    U32                                 Control;
    struct
    {
        U32                             Count;
        PMIXERCONTROLDETAILS_UNSIGNED   Channels;
    } Channels;
    U32                                 Tracks;
    U32                                 Mode;
    BOOL                                IsSuccess;
    struct
    {
        U32                             Hours;
        U32                             Minutes;
        U32                             Seconds;
        U32                             Frames;
    } Length;
    BOOL                                IsActive;
    CHAR                                Message[MAX_AUDIOPLAYER_MESSAGE_LENGTH];
    HANDLE                              Event;
    AUDIOCOMMAND                        Command;
    U32                                 Unknown;
    U32                                 Ticks;
} AUDIOPLAYER, * AUDIOPLAYERPTR;

EXTERN AUDIOPLAYER AudioPlayerState;

AUDIOPLAYERPTR CLASSCALL ActivateAudioPlayer(AUDIOPLAYERPTR self);
AUDIOPLAYERPTR CLASSCALL ReleaseAudioPlayer(AUDIOPLAYERPTR self, CONST OBJECTRELEASETYPE mode);
BOOL CLASSCALL AcquireAudioPlayerPosition(AUDIOPLAYERPTR self, U32* hours, U32* minutes, U32* seconds, U32* frames);
BOOL CLASSCALL AudioPlayerWorker(AUDIOPLAYERPTR self);
BOOL CLASSCALL InitializeAudioPlayerEvent(AUDIOPLAYERPTR self, CONST U32 count);
BOOL CLASSCALL InitializeAudioPlayerMixer(AUDIOPLAYERPTR self);
BOOL CLASSCALL ReleaseAudioPlayerEvent(AUDIOPLAYERPTR self);
BOOL CLASSCALL SelectAudioPlayerMixerDetails(AUDIOPLAYERPTR self, CONST U32 count, PMIXERCONTROLDETAILS_UNSIGNED channels);
U32 CLASSCALL AcquireAudioPlayerMode(AUDIOPLAYERPTR self);
VOID CLASSCALL DisposeAudioPlayer(AUDIOPLAYERPTR self);
VOID CLASSCALL InitializeAudioPlayer(AUDIOPLAYERPTR self);

BOOL SendAudioPlayerMessage(AUDIOPLAYERPTR self, LPCSTR format, ...);
DWORD WINAPI AudioPlayerWorker(LPVOID context);