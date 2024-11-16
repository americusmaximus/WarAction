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

#include "AssetFile.hxx"
#include "Logger.hxx"
#include "Settings.hxx"
#include "Sound.hxx"
#include "Strings.hxx"

#include <..\Text\Resources.hxx>

#include <Mathematics.Basic.hxx>

#include <math.h>
#include <stdio.h>

using namespace Mathematics;

SOUNDSTATEMODULECONTAINER SoundState;

// 0x10023dc0
SOUNDBUFFERPTR CLASSCALL ActivateSoundBuffer(SOUNDBUFFERPTR self, LPDIRECTSOUNDBUFFER buffer, CONST BOOL active)
{
    self->IsActive = active;
    self->Buffer = buffer;

    return self;
}

// 0x10023de0
VOID CLASSCALL ReleaseSoundBuffer(SOUNDBUFFERPTR self)
{
    DIRECTSOUNDRELEASE(self->Buffer);
}

// 0x100240c0
SOUNDBUFFERPTR CLASSCALL InitializeSoundState(SOUNDPTR self, BINFILEPTR file, CONST U32 size, CONST U32 channels, CONST U32 bits, CONST U32 samples, CONST BOOL mode)
{
    if (self->Instance == NULL)
    {
        self->State = SOUNDRESULT_NO_SOUND_INSTANCE_ERROR;

        return NULL;
    }

    LPDIRECTSOUNDBUFFER buffer = ActivateSoundStateSoundBuffer(self, size, channels, bits, samples);

    if (buffer != NULL)
    {
        LPVOID audio1 = NULL;
        DWORD audio1size = 0;
        LPVOID audio2 = NULL;
        DWORD audio2size = 0;

        self->Result = buffer->Lock(0, size, &audio1, &audio1size, &audio2, &audio2size, DSBLOCK_NONE);

        if (FAILED(self->Result))
        {
            self->State = SOUNDRESULT_LOCK_SOUND_BUFFER_ERROR;

            return NULL;
        }

        if (audio1 != NULL) { ReadBinFile(file, audio1, audio1size); }
        if (audio2 != NULL) { ReadBinFile(file, audio2, audio2size); }

        buffer->Unlock(audio1, audio1size, audio2, audio2size);

        SOUNDBUFFERPTR result = ALLOCATE(SOUNDBUFFER);

        if (result != NULL) { return ActivateSoundBuffer(result, buffer, mode); }
    }

    return NULL;
}

// 0x10023fe0
LPDIRECTSOUNDBUFFER CLASSCALL ActivateSoundStateSoundBuffer(SOUNDPTR self, CONST U32 size, CONST U32 channels, CONST U32 bits, CONST U32 samples)
{
    if (self->Instance == NULL)
    {
        self->State = SOUNDRESULT_NO_SOUND_INSTANCE_ERROR;

        return NULL;
    }

    WAVEFORMATEX format;
    ZeroMemory(&format, sizeof(WAVEFORMATEX));

    CONST U32 alignement = channels * bits >> 3;

    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = channels;
    format.nSamplesPerSec = samples;
    format.nAvgBytesPerSec = (alignement & 0xFFFF) * samples;
    format.nBlockAlign = alignement;
    format.wBitsPerSample = bits;

    DSBUFFERDESC desc;
    ZeroMemory(&desc, sizeof(DSBUFFERDESC));

    desc.dwSize = sizeof(DSBUFFERDESC);
    desc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_LOCSOFTWARE | DSBCAPS_STATIC;
    desc.dwBufferBytes = size;
    desc.lpwfxFormat = &format;

    LPDIRECTSOUNDBUFFER buffer = NULL;
    self->Result = self->Instance->CreateSoundBuffer(&desc, &buffer, NULL);

    if (FAILED(self->Result))
    {
        self->State = SOUNDRESULT_CREATE_SECONDARY_SOUND_BUFFER_ERROR;

        return NULL;
    }

    return buffer;
}

// 0x100242d0
BOOL CLASSCALL InitializeSoundStateSoundTrack(SOUNDPTR self, SOUNDBUFFERPTR buffer, CONST F32 volume, CONST F32 pan)
{
    if (self->Instance == NULL)
    {
        self->State = SOUNDRESULT_NO_SOUND_INSTANCE_ERROR;

        return FALSE;
    }

    CONST S32 indx = AcquireSoundStateTrackIndex(self);

    if (indx == INVALID_SOUND_TRACK_INDEX)
    {
        self->State = SOUNDRESULT_NO_TRACKS_AVAILABLE;

        return FALSE;
    }

    return InitializeSoundTrack(&self->Tracks[indx], buffer, volume, pan);
}

// 0x10024290
S32 CLASSCALL AcquireSoundStateTrackIndex(SOUNDPTR self)
{
    if (self->Instance == NULL)
    {
        self->State = SOUNDRESULT_NO_SOUND_INSTANCE_ERROR;

        return INVALID_SOUND_TRACK_INDEX;
    }

    for (U32 x = 0; x < self->Count; x++)
    {
        if (self->Tracks[x].State == NULL) { return x; }
    }

    self->State = SOUNDRESULT_NO_TRACKS_AVAILABLE;

    return INVALID_SOUND_TRACK_INDEX;
}

// 0x10024320
VOID CLASSCALL ReleaseSoundStateSoundTracks(SOUNDPTR self)
{
    if (self->Instance != NULL)
    {
        for (U32 x = 0; x < self->Count; x++)
        {
            ReleaseSoundTrack(&self->Tracks[x]);
        }
    }
}

// 0x1000c0b0
VOID CLASSCALL InitializeSoundState(SOUNDSTATEPTR self, SOUNDPTR state)
{
    self->Count = 0;
    self->State = state;

    SoundState.SoundState = state;

    CHAR asset[MAX_FILE_NAME_LENGTH];

    {
        STRINGVALUE name, value;
        AcquireStringValue(&name, StringsState.Scratch);
        AcquireSettingsValue(value, IDS_SOUND_RESOURCE);

        STRINGVALUE setting;
        STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value);

        strcpy(asset, actual->Value);

        ReleaseStringValue(actual);
    }

    CHAR name[MAX_FILE_NAME_LENGTH];

    {
        wsprintfA(name, "%shdr", asset);

        BINFILE file = { (BFH)INVALID_BINFILE_VALUE };
        OpenBinFile(&file, name, BINFILEOPENTYPE_READ);

        CONST U32 size = AcquireBinFileSize(&file);

        self->Header = (SOUNDHEADERPTR)malloc(size);

        ReadBinFile(&file, self->Header, size);
        CloseBinFile(&file);
    }

    {
        wsprintfA(name, "%srus", asset);
        OpenBinFile(&self->File, name, BINFILEOPENTYPE_READ);
    }

    self->IsReleased = FALSE;

    self->Buffers.Back = NULL;
    self->Buffers.Action = NULL;
}

// 0x1000c240
VOID CLASSCALL InitializeSoundStateBackBuffer(SOUNDSTATEPTR self, LPCSTR name)
{
    LogMessage("SOUND : Loading back `%s`\n", name);

    BINFILE file = { (BFH)INVALID_BINFILE_VALUE };

    if (OpenBinFile(&file, name, BINFILEOPENTYPE_READ))
    {
        self->Buffers.Back = InitializeSoundState(self->State, &file, AcquireBinFileSize(&file), 2, 16, 22050, TRUE);
    }
}

// 0x1000c540
VOID CLASSCALL InitializeSoundStateBackBuffer(SOUNDSTATEPTR self, CONST BOOL active)
{
    if (self->Buffers.Back != NULL && active)
    {
        InitializeSoundStateSoundTrack(self->State, self->Buffers.Back, 1.0f, 0.0f);
    }

    self->IsReleased = FALSE;
}

// 0x1000c580
VOID CLASSCALL ReleaseSoundStateTracks(SOUNDSTATEPTR self, CONST BOOL release)
{
    self->IsReleased = TRUE;

    if (release) { ReleaseSoundStateSoundTracks(self->State); }
}

// 0x1000c4a0
VOID CLASSCALL ReleaseSoundState(SOUNDSTATEPTR self)
{
    if (self->State != NULL)
    {
        ReleaseSoundStateTracks(self, TRUE);

        for (U32 x = 0; x < self->Count; x++)
        {
            if (self->Tracks[x].Buffer != NULL)
            {
                ReleaseSoundBuffer(self->Tracks[x].Buffer);

                free(self->Tracks[x].Buffer);
            }
        }

        self->Count = 0;

        free(self->Header);

        if (self->Buffers.Back != NULL)
        {
            ReleaseSoundBuffer(self->Buffers.Back);

            free(self->Buffers.Back);
        }

        ReleaseSoundStateActionBuffer(self);

        self->Buffers.Back = NULL;

        CloseBinFile(&self->File);
    }
}

// 0x1000c430
VOID CLASSCALL PlaySoundStateSound(SOUNDSTATEPTR self, LPCSTR name)
{
    if (name == NULL) { return; }

    U32 indx = 0;
    BOOL found = FALSE;
    CONST U32 hash = AcquireSoundNameHash(name);

    for (U32 x = 0; x < self->Count; x++)
    {
        if (self->Tracks[x].Hash == hash) { indx = x; found = TRUE; break; }
    }

    if (!found && !InitializeSoundStateSample(self, name)) { return; }

    LogMessage("SOUND : Playing %s\n", name);

    if (self->Tracks[indx].Buffer != NULL && !self->IsReleased)
    {
        InitializeSoundStateSoundTrack(self->State, self->Tracks[indx].Buffer, 1.0f, 0.0f);
    }
}

// 0x1000c2a0
BOOL CLASSCALL InitializeSoundStateSample(SOUNDSTATEPTR self, LPCSTR name)
{
    if (name == NULL) { return FALSE; }

    CONST U32 hash = AcquireSoundNameHash(name);

    self->Tracks[self->Count].Hash = hash;
    self->Tracks[self->Count].Buffer = NULL;

    LogMessage("SOUND : Loading sample %s\n", name);

    for (U32 x = 0; x < self->Header->Count; x++)
    {
        SOUNDDESCRIPTORPTR sound = &self->Header->Sounds[x];

        if (strcmp(sound->Name, name) == 0)
        {
            self->Tracks[self->Count].Hash = AcquireSoundNameHash(name);

            PointBinFile(&self->File, sound->Unk02 * 4 + 4, FILE_BEGIN); // TODO, is this correct?

            U32 start, end;
            ReadBinFile(&self->File, &start, sizeof(U32)); // TODO, is this correct?
            ReadBinFile(&self->File, &end, sizeof(U32)); // TODO, is this correct?

            PointBinFile(&self->File, start, FILE_BEGIN); // TODO, is this correct?

            U32 samples, bits, channels;
            ReadBinFile(&self->File, &samples, sizeof(U32)); // TODO, is this correct?
            ReadBinFile(&self->File, &bits, sizeof(U32)); // TODO, is this correct?
            ReadBinFile(&self->File, &channels, sizeof(U32)); // TODO, is this correct?

            self->Tracks[self->Count].Buffer = InitializeSoundState(self->State, &self->File,
                end - start - 0xc /* TODO */, 1, bits, samples, FALSE);

            break;
        }

        // TODO ?
    }

    if (self->Tracks[self->Count].Buffer == NULL) { LogMessage("SOUND : Cannot load %s\n", name); }

    SOUNDBUFFERPTR buffer = self->Tracks[self->Count].Buffer;

    self->Count = self->Count + 1;

    if (buffer != NULL) { return TRUE; }

    return FALSE;
}

// 0x1000c650
VOID CLASSCALL InitializeSoundStateActionBuffer(SOUNDSTATEPTR self, LPCSTR name)
{
    ASSETFILE file = { (BFH)INVALID_BINFILE_VALUE };

    if (OpenAssetFile(&file, name))
    {
        U16 magic = 0, count = 0;
        ReadAssetFile(&file, &magic, sizeof(U16));
        ReadAssetFile(&file, &count, sizeof(U32));

        LPVOID value = malloc(count);

        ReadAssetFile(&file, value, count);
        CloseAssetFile(&file);

        LPVOID content = malloc(count * 4); // TODO

        // TODO NOT IMPLEMENTED
        // FUN_1000c5a0();

        for (U32 x = 0; x < count; x++)
        {
            // TODO NOT IMPLEMENTED 
            //content[x].Unk00 = FUN_1000c600((uint)(*(byte*)(x + (int)value) >> 4)); // TODO
            //content[x].Unk01 = FUN_1000c600(*(byte*)(x + (int)value) & 0xf); // TODO
        }

        free(value);

        self->Buffers.Action = ActivateSoundStateSoundBuffer(self->State, content, count * 4 /* TODO */, 1, 16, 22050, FALSE);

        InitializeSoundStateSoundTrack(self->State, self->Buffers.Action, 1.0f, 0.0f);

        free(content);
    }
}

// 0x1000c770
VOID CLASSCALL ReleaseSoundStateActionBuffer(SOUNDSTATEPTR self)
{
    if (self->Buffers.Action != NULL)
    {
        ReleaseSoundBuffer(self->Buffers.Action);

        free(self->Buffers.Action);

        self->Buffers.Action = NULL;
    }
}

// 0x1000c530
VOID CLASSCALL ReleaseSoundStateInactiveSoundTracks(SOUNDSTATEPTR self)
{
    ReleaseSoundStateInactiveSoundTracks(self->State);
}

// 0x100241a0
SOUNDBUFFERPTR CLASSCALL ActivateSoundStateSoundBuffer(SOUNDPTR self, LPCVOID content, CONST U32 size, CONST U32 channels, CONST U32 bits, CONST U32 samples, CONST BOOL active)
{
    if (self->Instance == NULL)
    {
        self->State = SOUNDRESULT_NO_SOUND_INSTANCE_ERROR;

        return NULL;
    }

    LPDIRECTSOUNDBUFFER buffer = ActivateSoundStateSoundBuffer(self, size, channels, bits, samples);

    if (buffer != NULL)
    {
        LPVOID audio1 = NULL;
        DWORD audio1size = 0;
        LPVOID audio2 = NULL;
        DWORD audio2size = 0;

        self->Result = buffer->Lock(0, size, &audio1, &audio1size, &audio2, &audio2size, DSBLOCK_NONE);

        if (FAILED(self->Result))
        {
            self->State = SOUNDRESULT_LOCK_SOUND_BUFFER_ERROR;

            return FALSE;
        }

        if (audio1size != 0) { CopyMemory(audio1, content, audio1size); }

        if (audio2size != 0) { CopyMemory(audio2, (LPVOID)((ADDR)content + audio1size), audio2size); }

        buffer->Unlock(audio1, audio1size, audio2, audio2size);

        SOUNDBUFFERPTR result = ALLOCATE(SOUNDBUFFER);

        if (result != NULL) { return ActivateSoundBuffer(result, buffer, active); }
    }

    return NULL;
}

// 0x10024350
VOID CLASSCALL ReleaseSoundStateInactiveSoundTracks(SOUNDPTR self)
{
    if (self->Instance == NULL) { return; }

    U32 ticks = GetTickCount();

    for (U32 x = 0; x < self->Count; x++)
    {
        if (self->Tracks[x].State != NULL && 299 < ticks - self->Tracks[x].Ticks)
        {
            DWORD status = DSBSTATUS_NONE;
            self->Tracks[x].Buffer->GetStatus(&status);

            if (!(status & DSBSTATUS_PLAYING)) { ReleaseSoundTrack(&self->Tracks[x]); }
        }
    }
}

// 0x10023df0
BOOL CLASSCALL InitializeSoundTrack(SOUNDTRACKPTR self, SOUNDBUFFERPTR buffer, CONST F32 volume, CONST F32 pan)
{
    if (SoundState.SoundState->Instance == NULL)
    {
        SoundState.SoundState->State = SOUNDRESULT_NO_SOUND_INSTANCE_ERROR;

        return FALSE;
    }

    ReleaseSoundTrack(self);

    if (buffer == NULL)
    {
        SoundState.SoundState->State = SOUNDRESULT_NO_SOUND_BUFFER_ERROR;

        return FALSE;
    }

    if (SoundState.SoundState->Unk07 <= SoundState.SoundState->Unk08)
    {
        SoundState.SoundState->State = SOUNDRESULT_INVALID_SETTINGS_ERROR;

        return FALSE;
    }

    // Volume
    {
        self->Volume = volume <= 0.0009765625 ? DSBPAN_LEFT : (S32)log2(volume / 1000.0);

        if (self->Volume <= self->MinVolume)
        {
            SoundState.SoundState->State = SOUNDRESULT_INVALID_SETTINGS_ERROR;

            return FALSE;
        }
    }

    // Pan
    {
        CONST S32 value = SoundState.SoundState->IsReverseStereo
            ? (S32)(pan * DSBPAN_LEFT) : (S32)(pan * DSBPAN_RIGHT);

        self->Pan = Max(DSBPAN_LEFT, Min(value, DSBPAN_RIGHT));
    }

    SoundState.SoundState->Result = SoundState.SoundState->Instance->DuplicateSoundBuffer(buffer->Buffer, &self->Buffer);

    if (FAILED(SoundState.SoundState->Result))
    {
        SoundState.SoundState->State = SOUNDRESULT_DUPLICATE_SOUND_BUFFER_ERROR;

        return FALSE;
    }

    self->Buffer->SetVolume(self->Volume);
    self->Buffer->SetPan(self->Pan);

    SoundState.SoundState->Result = self->Buffer->Play(0, 0, buffer->IsActive ? DSBPLAY_LOOPING : DSBPLAY_NONE);

    if (FAILED(SoundState.SoundState->Result))
    {
        DIRECTSOUNDRELEASE(self->Buffer);

        SoundState.SoundState->State = SOUNDRESULT_BUFFER_PLAY_ERROR;

        return FALSE;
    }

    self->State = buffer;

    return TRUE;
}

// 0x10023fa0
VOID CLASSCALL ReleaseSoundTrack(SOUNDTRACKPTR self)
{
    if (SoundState.SoundState->Instance != NULL)
    {
        if (self->Buffer != NULL)
        {
            self->Buffer->Stop();
            DIRECTSOUNDRELEASE(self->Buffer);
        }

        self->State = NULL;
    }
}

// 0x1000c210
U32 AcquireSoundNameHash(LPCSTR name)
{
    U32 result = 0x12345678;

    U32 indx = 0;

    while (name[indx] != NULL)
    {
        result = (U32)name[indx] + (result >> 28) + result * 16;

        indx = indx + 1;
    }

    return result;
}