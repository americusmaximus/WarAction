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

#include <DirectSound.hxx>

#include <stdio.h>

#define INVALID_MIXER_CONTROL_ID    (-1)
#define INVALID_MIXER_OBJECT_HANLDE (-1)

#define MAX_AUDIO_WAIT_TIME         10000

// 0x10046b60
AUDIOPLAYER AudioPlayerState;

// 0x1003a320
AUDIOPLAYERSELF AudioPlayerSelfState =
{
    ReleaseAudioPlayer
};

// 0x10002940
AUDIOPLAYERPTR CLASSCALL ActivateAudioPlayer(AUDIOPLAYERPTR self)
{
    self->Self = &AudioPlayerSelfState;
    self->Mixer = (HMIXEROBJ)INVALID_MIXER_OBJECT_HANLDE;
    self->Event = NULL;

    return self;
}

// 0x10002960
AUDIOPLAYERPTR CLASSCALL ReleaseAudioPlayer(AUDIOPLAYERPTR self, CONST OBJECTRELEASETYPE mode)
{
    DisposeAudioPlayer(self);

    if (mode & OBJECTRELEASETYPE_ALLOCATED) { free(self); }

    return self;
}

// 0x10002980
VOID CLASSCALL DisposeAudioPlayer(AUDIOPLAYERPTR self)
{
    self->Self = &AudioPlayerSelfState;
}

// 0x100029d0
VOID CLASSCALL InitializeAudioPlayer(AUDIOPLAYERPTR self)
{
    self->Event = CreateEventA(NULL, FALSE, FALSE, "CDDA_pulse");

    if (self->Event == NULL) { return; }

    InitializeAudioPlayerMixer(self);

    DWORD id = 0;
    self->Thread = CreateThread(NULL, 0, AudioPlayerWorker, self, 0, &id);

    if (self->Thread == NULL) { return; }

    self->IsActive = TRUE;
}

// 0x10002a30
BOOL CLASSCALL InitializeAudioPlayerMixer(AUDIOPLAYERPTR self)
{
    self->Mixer = NULL;
    self->Control = INVALID_MIXER_CONTROL_ID;
    self->Channels.Count = 0;

    MIXERCAPSA caps;
    ZeroMemory(&caps, sizeof(MIXERCAPSA));

    if (mixerGetDevCapsA(0, &caps, sizeof(MIXERCAPSA)) != MMSYSERR_NOERROR) { return FALSE; }

    MIXERLINEA info;
    ZeroMemory(&info, sizeof(MIXERLINEA));

    info.cbStruct = sizeof(MIXERLINEA);
    info.dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC;

    if (mixerGetLineInfoA(NULL, &info, MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR) { return FALSE; }

    MIXERCONTROLA control;
    ZeroMemory(&control, sizeof(MIXERCONTROLA));

    control.cbStruct = sizeof(MIXERCONTROLA);

    MIXERLINECONTROLSA controls;
    ZeroMemory(&controls, sizeof(MIXERLINECONTROLSA));

    controls.cbStruct = sizeof(MIXERLINECONTROLSA);
    controls.dwLineID = info.dwLineID;
    controls.cControls = 1;
    controls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
    controls.cbmxctrl = sizeof(MIXERCONTROLA);
    controls.pamxctrl = &control;

    if (mixerGetLineControlsA(NULL, &controls, MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR) { return FALSE; }

    self->Control = control.dwControlID;
    self->Channels.Count = info.cChannels;

    return TRUE;
}

// 0x100029c0
DWORD WINAPI AudioPlayerWorker(LPVOID context)
{
    return AudioPlayerWorker((AUDIOPLAYERPTR)context);
}

// 0x10002c10
BOOL CLASSCALL AudioPlayerWorker(AUDIOPLAYERPTR self)
{
    self->Channels.Channels = (PMIXERCONTROLDETAILS_UNSIGNED)malloc(self->Channels.Count * sizeof(MIXERCONTROLDETAILS_UNSIGNED));

    SelectAudioPlayerMixerDetails(self, self->Channels.Count, self->Channels.Channels);

    if (!SendAudioPlayerMessage(self, "open cdaudio shareable alias cdx wait")) { return FALSE; }
    if (!SendAudioPlayerMessage(self, "set cdx time format tmsf wait")) { return FALSE; }

    self->Length.Hours = -1;
    self->Mode = AUDIOPLAYERMODE_STOPPED;

    while (TRUE)
    {
        BOOL exit = FALSE;

        if (WaitForSingleObject(self->Event, 800) == WAIT_OBJECT_0)
        {
            switch (self->Command)
            {
            case AUDIOCOMMAND_PLAY_TRACK:
            {
                self->IsSuccess = SendAudioPlayerMessage(self, "play cdx from %u to %u",
                    self->Tracks + AUDIO_TRACK_OFFSET, self->Tracks + (AUDIO_TRACK_OFFSET + 1)); break;
            }
            case AUDIOCOMMAND_STOP: { self->IsSuccess = SendAudioPlayerMessage(self, "stop cdx"); break; }
            case AUDIOCOMMAND_PAUSE:
            {
                self->IsSuccess = SendAudioPlayerMessage(self, "status cdx position");

                if (self->IsSuccess)
                {
                    sscanf(self->Message, "%d:%d:%d:%d",
                        &self->Length.Hours, &self->Length.Minutes, &self->Length.Seconds, &self->Length.Frames);
                }

                self->IsSuccess = SendAudioPlayerMessage(self, "pause cdx"); break;
            }
            case AUDIOCOMMAND_RESUME: { self->IsSuccess = SendAudioPlayerMessage(self, "resume cdx"); break; }
            case AUDIOCOMMAND_PLAY_TIME:
            {
                SendAudioPlayerMessage(self, "play cdx from %u to %u",
                    self->Length.Hours | (self->Length.Minutes << 8) | (self->Length.Seconds << 16) | (self->Length.Frames << 24),
                    self->Length.Hours + 1); break;
            }
            case AUDIOCOMMAND_CLOSE: { exit = TRUE; break; }
            }

            self->Command = AUDIOCOMMAND_NONE;
        }

        if (!self->IsActive)
        {
            if (!SendAudioPlayerMessage(self, "status cdx mode")) { self->Mode = AUDIOPLAYERMODE_ERROR; }
            else
            {
                if (strncmp(self->Message, "playing", 8) == 0) { self->Mode = AUDIOPLAYERMODE_PLAYING; }
                else
                {
                    if (strncmp(self->Message, "stopped", 8) == 0) { self->Mode = AUDIOPLAYERMODE_STOPPED; }
                    else { self->Mode = strncmp(self->Message, "paused", 7) == 0 ? AUDIOPLAYERMODE_PAUSED : AUDIOPLAYERMODE_OTHER; }
                }
            }
        }

        if (exit)
        {
            SendAudioPlayerMessage(self, "stop cdx");
            SelectAudioPlayerMixerDetails(self, self->Channels.Count, self->Channels.Channels);
            SendAudioPlayerMessage(self, "close cdx");

            return TRUE;
        }
    }
}

// 0x10002b20
BOOL CLASSCALL SelectAudioPlayerMixerDetails(AUDIOPLAYERPTR self, CONST U32 count, PMIXERCONTROLDETAILS_UNSIGNED channels)
{
    if (self->Control == INVALID_MIXER_CONTROL_ID) { return FALSE; }

    MIXERCONTROLDETAILS details;

    details.cbStruct = sizeof(MIXERCONTROLDETAILS);
    details.dwControlID = self->Control;
    details.cChannels = count;
    details.hwndOwner = NULL;
    details.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
    details.paDetails = channels;

    return mixerSetControlDetails(self->Mixer, &details, MIXER_OBJECTF_MIXER) == MMSYSERR_NOERROR;
}

// 0x10002b90
BOOL SendAudioPlayerMessage(AUDIOPLAYERPTR self, LPCSTR format, ...)
{
    CHAR message[MAX_AUDIOPLAYER_MESSAGE_LENGTH];

    va_list args;
    va_start(args, format);
    vsprintf(message, format, args);
    va_end(args);

    return mciSendStringA(message, self->Message, MAX_AUDIOPLAYER_MESSAGE_LENGTH, NULL) == MMSYSERR_NOERROR;
}

// 0x10002b80
U32 CLASSCALL AcquireAudioPlayerMode(AUDIOPLAYERPTR self)
{
    return self->Mode;
}

// 0x10002990
BOOL CLASSCALL InitializeAudioPlayerEvent(AUDIOPLAYERPTR self, CONST U32 count)
{
    self->Tracks = count;
    self->Command = AUDIOCOMMAND_PLAY_TRACK;

    SetEvent(self->Event);

    return TRUE;
}

// 0x10002be0
BOOL CLASSCALL ReleaseAudioPlayerEvent(AUDIOPLAYERPTR self)
{
    self->Command = AUDIOCOMMAND_CLOSE;

    SetEvent(self->Event);
    WaitForSingleObject(self->Thread, MAX_AUDIO_WAIT_TIME);

    return TRUE;
}

// 0x10002e70
BOOL CLASSCALL AcquireAudioPlayerPosition(AUDIOPLAYERPTR self, U32* hours, U32* minutes, U32* seconds, U32* frames)
{
    self->IsSuccess = SendAudioPlayerMessage(self, "status cdx position");

    if (self->IsSuccess)
    {
        sscanf(self->Message, "%d:%d:%d:%d",
            &self->Length.Hours, &self->Length.Minutes, &self->Length.Seconds, &self->Length.Frames);
    }

    *hours = self->Length.Hours;
    *minutes = self->Length.Minutes;
    *seconds = self->Length.Seconds;
    *frames = self->Length.Frames;
    *hours = *hours - 1;

    return TRUE;
}