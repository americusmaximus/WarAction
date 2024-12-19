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

#include "ActionArea.hxx"
#include "BinFile.hxx"
#include "Bink.hxx"
#include "ControlCommand.hxx"
#include "Settings.hxx"
#include "Sound.hxx"
#include "State.hxx"
#include "TextAsset.hxx"
#include "Video.hxx"
#include "VideoControl.hxx"

#include <..\Text\Resources.hxx>

#include <DirectDraw.hxx>

#define VIDEO_CONTROL_ACTION_PRIORITY   0

// 0x1003a230
VIDEOCONTROLSELF VideoControlSelfState =
{
    (VIDEOCONTROLTYPEACTION)AcquireControlTypeControl,
    InitializeVideoControl,
    DisableVideoControl,
    (VIDEOCONTROLTICKACTION)TickControl,
    ActionVideoControl,
    ReleaseVideoControl
};

// 0x10001000
VIDEOCONTROLPTR CLASSCALL ActivateVideoControl(VIDEOCONTROLPTR self)
{
    self->Self = &VideoControlSelfState;

    self->IsActive = FALSE;
    self->Name[0] = NULL;

    return self;
}

// 0x10001450
VOID CLASSCALL InitializeVideoControl(VIDEOCONTROLPTR self)
{
    State.Renderer->Actions.DrawMainSurfaceColorRectangle(0, 0,
        GRAPHICS_RESOLUTION_640, GRAPHICS_RESOLUTION_480, BLACK_PIXEL);
    State.Renderer->Actions.WriteMainSurfaceRendererSurfaceRectangle(0, 0,
        GRAPHICS_RESOLUTION_640, GRAPHICS_RESOLUTION_480);

    InitializeBink(self->Name, State.Module->Renderer, State.Module->Sound, self->Max, self->Current);

    InitializeControl((CONTROLPTR)self);

    ReleaseSoundStateTracks(&SoundState.State, TRUE);

    VideoState.IsActive = TRUE;

    ActivateActionArea(0, 0, GRAPHICS_RESOLUTION_640, GRAPHICS_RESOLUTION_480,
        CONTROLCOMMANDACTION_MOUSE_LEFT_DOWN, CONTROLCOMMAND_VIDEO_CONTROL, VIDEO_CONTROL_ACTION_PRIORITY);
}

// 0x10001500
VOID CLASSCALL DisableVideoControl(VIDEOCONTROLPTR self)
{
    DisableControl((CONTROLPTR)self);

    ReleaseBink();

    VideoState.IsActive = FALSE;

    DequeueControlCommand(CONTROLCOMMAND_VIDEO_CONTROL);
}

// 0x10001520
U32 CLASSCALL ActionVideoControl(VIDEOCONTROLPTR self)
{
    CONST CONTROLCOMMANDPTR command = DequeueControlCommand(FALSE);

    if (command != NULL)
    {
        if (command->Command == CONTROLCOMMAND_VIDEO_CONTROL)
        {
            DequeueControlCommand(TRUE);

            return CONTROLACTION_PLAY_COMPLETED;
        }

        if (command->Command == CONTROLCOMMAND_VIDEO_CONTROL
            && (command->Action == VK_ESCAPE || command->Action == VK_SPACE))
        {
            DequeueControlCommand(TRUE);

            return CONTROLACTION_PLAY_COMPLETED;
        }
    }

    CONST BINKVIDEORESULT result = PlayBinkVideo();

    if (result == BINKVIDEORESULT_CONTINUE) { return CONTROLACTION_NONE; }
    else if (result == BINKVIDEORESULT_COMPLETED) { return CONTROLACTION_PLAY_COMPLETED; }

    return CONTROLACTION_1116;
}

// 0x100025c0
VIDEOCONTROLPTR CLASSCALL ReleaseVideoControl(VIDEOCONTROLPTR self, CONST OBJECTRELEASETYPE mode)
{
    DisposeControl((CONTROLPTR)self);

    if (mode & OBJECTRELEASETYPE_ALLOCATED) { free(self); }

    return self;
}

// 0x10001310
BOOL CLASSCALL PlayVideoControl(VIDEOCONTROLPTR self, CONST S32 map, CONST S32 mission, CONST U32 param)
{
    TEXTASSET text;
    ActivateTextAsset(&text);
    InitializeTextAsset(&text, "campaign.seq");

    CHAR name[MAX_FILE_NAME_LENGTH];
    wsprintfA(name, "%03d%03d", map, mission);

    CHAR buffer[MAX_FILE_NAME_LENGTH]; // TODO name

    for (U32 x = 0; x < text.Count; x++)
    {
        AcquireTextAssetStringValue(&text, x, MAP_MISSION_TEXT_ASSET_PARAM, buffer);

        if (strcmp(name, buffer) == 0)
        {
            AcquireTextAssetStringValue(&text, x, param, buffer);

            if (buffer[0] != '-')
            {
                CONST BOOL result = PlayVideoControl(self, buffer);

                DisposeTextAsset(&text);

                return result;
            }
        }
    }

    DisposeTextAsset(&text);

    return FALSE;
}

// 0x10001020
BOOL CLASSCALL PlayVideoControl(VIDEOCONTROLPTR self, LPCSTR video)
{
    if (video[0] == '@')
    {
        TEXTASSET text;
        ActivateTextAsset(&text);
        InitializeTextAsset(&text, "campaign.seq");

        CHAR buffer[MAX_FILE_NAME_LENGTH]; // TODO name

        for (U32 x = 0; x < text.Count; x++)
        {
            AcquireTextAssetStringValue(&text, x, VIDEO_NAME_ALIAS_TEXT_ASSET_PARAM, buffer);

            if (strcmp(video, buffer) == 0)
            {
                AcquireTextAssetStringValue(&text, x, VIDEO_NAME_ACTUAL_TEXT_ASSET_PARAM, buffer);

                CONST BOOL result = PlayVideoControl(self, buffer);

                DisposeTextAsset(&text);

                return result;
            }
        }

        DisposeTextAsset(&text);

        return FALSE;
    }

    {
        STRINGVALUE name, value;
        AcquireSettingsValue(&name, IDS_MOVIES_PATH);
        AcquireStringValue(&value, StringsState.Scratch);

        STRINGVALUE setting;
        STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value);

        wsprintfA(self->Name, "%s%s.bik", actual->Value, video);

        ReleaseStringValue(actual);
    }

    BINFILE file = { (BFH)INVALID_BINFILE_VALUE };
    CONST BOOL exists = OpenBinFile(&file, self->Name, BINFILEOPENTYPE_READ);
    CloseBinFile(&file);

    if (exists)
    {
        self->Current = 0;
        self->Max = 0;

        TEXTASSET text;
        ActivateTextAsset(&text);
        InitializeTextAsset(&text, "campaign.seq");

        CHAR buffer[MAX_FILE_NAME_LENGTH]; // TODO name

        for (U32 x = 0; x < text.Count; x++)
        {
            AcquireTextAssetStringValue(&text, x, VIDEO_RECORD_SIZE_TEXT_ASSET_PARAM, buffer);

            if (strcmp(buffer, "size") == 0)
            {
                AcquireTextAssetStringValue(&text, x, VIDEO_RECORD_NAME_TEXT_ASSET_PARAM, buffer);

                if (strcmp(video, buffer) == 0)
                {
                    AcquireTextAssetStringValue(&text, x, VIDEO_RECORD_UNKNOWN1_TEXT_ASSET_PARAM, buffer);

                    self->Max = atoi(buffer);

                    AcquireTextAssetStringValue(&text, x, VIDEO_RECORD_UNKNOWN2_TEXT_ASSET_PARAM, buffer);

                    self->Current = atoi(buffer);
                }
            }
        }

        DisposeTextAsset(&text);
    }

    return exists;
}