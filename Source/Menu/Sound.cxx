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

// TODO
S32 DAT_100462c4; // 0x100462c4
U32 DAT_10046474; // 0x10046474
U32 DAT_100462c0; // 0x100462c0

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
SOUNDBUFFERPTR CLASSCALL InitializeSoundState(SOUNDPTR self, BINFILEPTR file, CONST U32 size, CONST U32 channels, CONST U32 bits, CONST U32 samples, CONST BOOL active)
{
    if (self->Instance == NULL)
    {
        self->State = SOUNDRESULT_NO_SOUND_INSTANCE_ERROR;

        return NULL;
    }

    LPDIRECTSOUNDBUFFER buffer = ActivateSoundStateSoundBuffer(self, size, channels, bits, samples);

    if (buffer != NULL)
    {
        LPVOID audio1 = NULL, audio2 = NULL;
        DWORD audio1size = 0, audio2size = 0;

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

        if (result != NULL) { return ActivateSoundBuffer(result, buffer, active); }
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

    CONST U32 alignement = channels * (bits >> 3);

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
        self->State = SOUNDRESULT_CREATE_SOUND_BUFFER_ERROR;

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

    SoundState.Sound = state;

    CHAR asset[MAX_FILE_NAME_LENGTH];

    {
        STRINGVALUE name, value;
        AcquireSettingsValue(&name, IDS_SOUND_RESOURCE);
        AcquireStringValue(&value, StringsState.Scratch);

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

    CONST U32 hash = AcquireSoundNameHash(name);

    U32 indx = 0;
    for (; indx < self->Count; indx++)
    {
        if (self->Tracks[indx].Hash == hash) { break; }
    }

    if (indx == self->Count && !InitializeSoundStateSample(self, name)) { return; }

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

    self->Tracks[self->Count].Hash = AcquireSoundNameHash(name);
    self->Tracks[self->Count].Buffer = NULL;

    LogMessage("SOUND : Loading sample %s\n", name);

    SOUNDDESCRIPTORPTR sound = &self->Header->Sounds[0];

    for (U32 x = 0; x < self->Header->Count; x++)
    {
        if (strcmp(sound->Name, name) == 0)
        {
            self->Tracks[self->Count].Hash = AcquireSoundNameHash(name);

            CONST SOUNDDESCRIPTOREXPTR ex = (SOUNDDESCRIPTOREXPTR)sound;

            {
                // Reading SOUNDS.RUS:
                // 1. 4 bytes - number of chunks in the file.
                // 2. Skip to the required sound chunk descriptor.

                CONST U32 offset = sizeof(U32) + ex->Chunks[0] * sizeof(U32);
                PointBinFile(&self->File, offset, FILE_BEGIN);
            }

            U32 start, end;
            ReadBinFile(&self->File, &start, sizeof(U32));
            ReadBinFile(&self->File, &end, sizeof(U32));

            PointBinFile(&self->File, start, FILE_BEGIN);

            // Sound chunk details are right in front of the actual sound data.
            U32 samples, bits, channels;
            ReadBinFile(&self->File, &samples, sizeof(U32));
            ReadBinFile(&self->File, &bits, sizeof(U32));
            ReadBinFile(&self->File, &channels, sizeof(U32));

            {
                CONST U32 length = end - start - (3 * sizeof(U32));

                // Given the sound chunk details just in fron of the actual data,
                // its size, 3 4-byte values, must be subtracted from the total chunk length.
                self->Tracks[self->Count].Buffer = InitializeSoundState(self->State,
                    &self->File, length, 1, bits, samples, FALSE);
            }

            break;
        }

        // SOUNDS.HDR contains a number of variable-size structures,
        // therefore, memory offset has to be calculated to accommodate for the size variation.
        sound = (SOUNDDESCRIPTORPTR)((ADDR)sound + (ADDR)(sound->Count * sizeof(U32) + sizeof(SOUNDDESCRIPTOR)));
    }

    if (self->Tracks[self->Count].Buffer == NULL) { LogMessage("SOUND : Cannot load %s\n", name); }

    CONST SOUNDBUFFERPTR buffer = self->Tracks[self->Count].Buffer;

    self->Count = self->Count + 1;

    return buffer != NULL;
}

// 0x1000c650
VOID CLASSCALL InitializeSoundStateActionBuffer(SOUNDSTATEPTR self, LPCSTR name)
{
    ASSETFILE file = { (BFH)INVALID_BINFILE_VALUE };
    if (OpenAssetFile(&file, name))
    {
        U16 magic = 0;
        U32 size = 0;
        ReadAssetFile(&file, &magic, sizeof(U16));
        ReadAssetFile(&file, &size, sizeof(U32));

        U8* source = (U8*)malloc(size);

        ReadAssetFile(&file, source, size);
        CloseAssetFile(&file);

        U32* content = (U32*)malloc(size * 4); // TODO

        FUN_1000c5a0();

        for (U32 x = 0; x < size; x++)
        {
            content[x] = (FUN_1000c600(source[x] >> 4) << 16) | FUN_1000c600(source[x] & 0xF);
        }

        free(source);

        self->Buffers.Action = ActivateSoundStateSoundBuffer(self->State, content,
            size * 4 /* TODO */, 1, 16, 22050, FALSE);

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
    if (SoundState.Sound->Instance == NULL)
    {
        SoundState.Sound->State = SOUNDRESULT_NO_SOUND_INSTANCE_ERROR;

        return FALSE;
    }

    ReleaseSoundTrack(self);

    if (buffer == NULL)
    {
        SoundState.Sound->State = SOUNDRESULT_NO_SOUND_BUFFER_ERROR;

        return FALSE;
    }

    if (SoundState.Sound->Unk07 <= SoundState.Sound->Unk08)
    {
        SoundState.Sound->State = SOUNDRESULT_INVALID_SETTINGS_ERROR;

        return FALSE;
    }

    // Volume
    {
        self->Volume = volume <= 0.0009765625 ? DSBPAN_LEFT : (S32)log2(volume / 1000.0);

        if (self->Volume <= self->MinVolume)
        {
            SoundState.Sound->State = SOUNDRESULT_INVALID_SETTINGS_ERROR;

            return FALSE;
        }
    }

    // Pan
    {
        CONST S32 value = SoundState.Sound->IsReverseStereo
            ? (S32)(pan * DSBPAN_LEFT) : (S32)(pan * DSBPAN_RIGHT);

        self->Pan = Max(DSBPAN_LEFT, Min(value, DSBPAN_RIGHT));
    }

    SoundState.Sound->Result = SoundState.Sound->Instance->DuplicateSoundBuffer(buffer->Buffer, &self->Buffer);

    if (FAILED(SoundState.Sound->Result))
    {
        SoundState.Sound->State = SOUNDRESULT_DUPLICATE_SOUND_BUFFER_ERROR;

        return FALSE;
    }

    self->Buffer->SetVolume(self->Volume);
    self->Buffer->SetPan(self->Pan);

    SoundState.Sound->Result = self->Buffer->Play(DSBPLAY_NONE, 0,
        buffer->IsActive ? DSBPLAY_LOOPING : DSBPLAY_NONE);

    if (FAILED(SoundState.Sound->Result))
    {
        DIRECTSOUNDRELEASE(self->Buffer);

        SoundState.Sound->State = SOUNDRESULT_BUFFER_PLAY_ERROR;

        return FALSE;
    }

    self->State = buffer;

    return TRUE;
}

// 0x10023fa0
VOID CLASSCALL ReleaseSoundTrack(SOUNDTRACKPTR self)
{
    if (SoundState.Sound->Instance != NULL)
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

// 0x1000c5a0
VOID FUN_1000c5a0() // TODO name
{
    DAT_100462c4 = 0;
    DAT_10046474 = 0;
    DAT_100462c0 = 0;
}

// 0x1003a420
STATIC S32 UnkSoundArray1[49 /* TODO */] = // TODO name
{
    0x00000010,
    0x00000011,
    0x00000013,
    0x00000015,
    0x00000017,
    0x00000019,
    0x0000001C,
    0x0000001F,
    0x00000022,
    0x00000025,
    0x00000029,
    0x0000002D,
    0x00000032,
    0x00000037,
    0x0000003C,
    0x00000042,
    0x00000049,
    0x00000050,
    0x00000058,
    0x00000061,
    0x0000006B,
    0x00000076,
    0x00000082,
    0x0000008F,
    0x0000009D,
    0x000000AD,
    0x000000BE,
    0x000000D1,
    0x000000E6,
    0x000000FD,
    0x00000117,
    0x00000133,
    0x00000151,
    0x00000173,
    0x00000198,
    0x000001C1,
    0x000001EE,
    0x00000220,
    0x00000256,
    0x00000292,
    0x000002D4,
    0x0000031C,
    0x0000036C,
    0x000003C3,
    0x00000424,
    0x0000048E,
    0x00000502,
    0x00000583,
    0x00000610
};

// 0x1000c600
U32 FUN_1000c600(CONST U32 value) // TODO name
{
    // TODO Not implemented

    int iVar1;
    int iVar2;

    iVar1 = UnkSoundArray1[DAT_100462c4];

    iVar2 = 0;

    if (value & 4) { iVar2 = iVar1; }
    if (value & 2) { iVar2 = iVar2 + (iVar1 >> 1); }
    if (value & 1) { iVar2 = iVar2 + (iVar1 >> 2); }
    if (value & 8) { iVar2 = -iVar2; }

    iVar2 = DAT_10046474 + iVar2;

    DAT_10046474 = iVar2;

    FUN_1000c5c0(value);

    return iVar2;
}

// 0x1003a3e0
STATIC S32 UnkSoundArray2[16 /* TODO */] = // TODO name
{
    -1, -1, -1, -1,
    2,  4,  6,  8,
    -1, -1, -1, -1,
    2,  4,  6,  8
};

// 0x1000c5c0
VOID FUN_1000c5c0(CONST U32 indx) // TODO name
{
    // TODO Not implemented

    int iVar1 = UnkSoundArray2[indx];

    if (iVar1 < 0) { if (0 < DAT_100462c4) { DAT_100462c4 = DAT_100462c4 + iVar1; } }
    else
    {
        DAT_100462c4 = DAT_100462c4 + iVar1;

        if (0x30 < DAT_100462c4) { DAT_100462c4 = 0x30; }
    }
}