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

#include "Assets.hxx"
#include "Cursor.hxx"
#include "Messages.hxx"
#include "ObjectType4x5d.hxx"
#include "Saves.hxx"
#include "Sound.hxx"
#include "State.hxx"

#include <stdlib.h>

// 0x1003a4e4
CONTROLTYPE4X5DSELF ObjectType4x5dSelfState =
{
    (CONTROLTYPE4X5DTYPEACTION)AcquireControlTypePanel,
    InitializeObjectType4x5d,
    DisableObjectType4x5d,
    TickObjectType4x5d,
    ActionObjectType4x5d,
    ReleaseObjectType4x5d,
};

// 0x1000d0b0
CONTROLTYPE4X5DPTR CLASSCALL ActivateObjectType4x5d(CONTROLTYPE4X5DPTR self)
{
    ActivateSceneControl((SCENECONTROLPTR)self, NULL, &AssetsState.Assets.BriefBT, CONTROLACTION_BRIEF_OK);

    self->Self = &ObjectType4x5dSelfState;

    self->AniFile = NULL;
    self->ArrColFile = NULL;
    self->SeqFile = NULL;
    self->BkgPckFile = NULL;
    self->BkgColFile = NULL;
    self->ColPckFile = NULL;
    self->ColFile = NULL;

    self->Description = ActivateDescriptionControl(ALLOCATE(DESCRIPTIONCONTROL),
        40, 21, 376, 68, &AssetsState.Fonts.Main, 0, -2);

    return self;
}

// 0x1000d180
VOID CLASSCALL InitializeObjectType4x5d(CONTROLTYPE4X5DPTR self)
{
    InitializePanelControl((PANELCONTROLPTR)self);

    self->SeqFileStart = 1; // TODO
    self->Ticks = GetTickCount() - 1000;

    for (U32 x = 0; x < 112 /* TODO */; x++)
    {
        memset(&State.Renderer->Sprites[x], 0x80, sizeof(SPRITE0X50)); // TODO
    }

    self->ArrColFile = NULL;

    CHAR map[MAX_FILE_NAME_LENGTH];
    CHAR mission[MAX_FILE_NAME_LENGTH];

    if (FUN_10018ba0(map, mission))
    {
        CHAR path[MAX_FILE_NAME_LENGTH];
        wsprintfA(path, "%sb_arr.col", mission);

        self->SpriteType = InitializeSprite(path, &self->ArrColFile);

        if (self->SpriteType != 0) // TODO
        {
            wsprintfA(path, "%sb_arr.ani", mission);
            OpenBinaryFile(path, &self->AniFile);

            wsprintfA(path, "%sb_arr.seq", mission);
            self->SequenceCount = OpenBinaryFile(path, &self->SeqFile) / 4; // TODO is this correct?

            {
                wsprintfA(path, "%sb_text.txt", mission);
                SelectAssetsStateIsActive(FALSE);

                LPSTR txt = NULL; // TODO name
                CONST U32 txtsize = OpenBinaryFile(path, (LPVOID*)&txt); // TODO name
                SelectAssetsStateIsActive(TRUE);

                // TODO make this block pretty
                LPSTR message = NULL;

                if (txtsize == 0) { message = (LPSTR)AcquireAssetMessage(ASSET_MESSAGE_NO_BRIEFING_AVAILABLE); }
                else
                {
                    txt[txtsize - 1] = NULL;
                    message = AcquireCleanUnicodeString(txt, TRUE);
                    txt = message;
                }

                SelectDescriptionControlText(self->Description, message);

                if (txt != NULL) { free(txt); }
            }

            wsprintfA(path, "%sb_bkg.col", map);
            self->Unk15 = InitializeSprite(path, &self->BkgColFile);

            wsprintfA(path, "%sb_bkg.pck", map);
            OpenBinaryFile(path, &self->BkgPckFile);

            self->Unk21 = 0;
            self->Unk20 = 0;
            self->Unk19 = 0;
            self->Unk18 = 0;

            TEXTASSET text;
            LPCSTR pcStack_420 = NULL; // TODO

            {
                ActivateTextAsset(&text);
                InitializeTextAsset(&text, "campaign.seq");

                U32 todo = 0; // TODO
                U32 iVar3 = 0; // TODO

                for (U32 x = 0; x < text.Count; x++)
                {
                    CHAR value[256]; // TODO
                    AcquireTextAssetStringValue(&text, x, 0, value);

                    if (value[0] == '>' && value[1] != NULL) { todo = todo + 1; }

                    // TODO NOT IMPLEMENTED

                    if (iVar3 == 0) // TODO
                    {
                        if (todo == 1) {
                            pcStack_420 = "b_rufly";
                        }
                        else {
                            if (todo == 2) {
                                pcStack_420 = "b_defire";
                                self->Unk17 = 1;
                                break;
                            }
                            if (todo != 3) break;
                            pcStack_420 = "b_ussmok";
                            self->Unk19 = 3;
                            self->Unk21 = 400;
                        }
                        self->Unk17 = 0;
                        break;
                    }
                }
            }

            InitializeTextAsset(&text, NULL);
            DisposeTextAsset(&text);

            if (pcStack_420 != NULL)
            {
                wsprintfA(path, "%s.col", pcStack_420);
                InitializeSprite(path, &self->ColFile);

                wsprintfA(path, "%s.pck", pcStack_420);
                OpenBinaryFile(path, &self->ColPckFile);
            }

            ZeroMemory(State.Renderer->Surface.Back,
                State.Renderer->Surface.Height * State.Renderer->Surface.Width * 2); // TODO

            State.Renderer->Actions.DrawBackSurfacePaletteShadeSprite(0, 0, 12 /* TODO */,
                (PIXEL*)self->BkgColFile,
                IMAGEPALETTESPRITEPTR((ADDR)self->BkgPckFile + (ADDR)((IMAGEPALETTESPRITEPTR)self->BkgPckFile)->Unk04)); // TODO

            ReleaseSoundStateTracks(&SoundState.State, TRUE);

            {
                STRINGVALUE setting;
                STRINGVALUEPTR actual = AcquireStringValue(&setting, "%s.apm", mission + 0x14); // TODO
                InitializeSoundStateActionBuffer(&SoundState.State, actual->Value);
                ReleaseStringValue(actual);
            }

            self->Description->Self->Initialize(self->Description);
        }
    }
}

// 0x1000d580
VOID CLASSCALL DisableObjectType4x5d(CONTROLTYPE4X5DPTR self)
{
    DisablePanelControl((PANELCONTROLPTR)self);

    self->Description->Self->Disable(self->Description);

    if (self->ArrColFile != (void*)0x0)
    {
        free(self->ArrColFile);
        self->ArrColFile = NULL;

        free(self->AniFile);
        self->AniFile = NULL;

        free(self->SeqFile);
        self->SeqFile = NULL;

        free(self->BkgColFile);
        self->BkgColFile = NULL;

        free(self->BkgPckFile);
        self->BkgPckFile = NULL;

        free(self->ColFile);
        self->ColFile = NULL;

        free(self->ColPckFile);
        self->ColPckFile = NULL;
    }

    ReleaseSoundStateActionBuffer(&SoundState.State);
    ReleaseSoundStateTracks(&SoundState.State, TRUE);
    InitializeSoundStateBackBuffer(&SoundState.State, TRUE);
}

// 0x1000d770
VOID CLASSCALL TickObjectType4x5d(CONTROLTYPE4X5DPTR self)
{
    State.Renderer->Actions.WriteBackSurfaceMainSurfaceRectangle(0, 0, GRAPHICS_RESOLUTION_640, GRAPHICS_RESOLUTION_480);

    for (U32 x = 0; x < self->SequenceCount; x++)
    {
        if (0xFFFFF < ((U32*)self->SeqFile)[x]) { break; } // TODO

        CONST U32 offset = ((U32*)self->AniFile)[((U32*)self->SeqFile)[x]] + 8; // TODO

        if (self->SpriteType == 1) // TODO
        {
            State.Renderer->Actions.DrawMainSurfacePaletteSprite(0, 0,
                (PIXEL*)self->ArrColFile, (IMAGEPALETTESPRITEPTR)((ADDR)self->AniFile + offset)); // TODO
        }
        else
        {
            State.Renderer->Actions.DrawMainSurfaceAnimationSpriteVersion2(0, 0, 0x10 /* TODO */,
                self->ArrColFile, (IMAGEPALETTESPRITEPTR)((ADDR)self->AniFile + offset)); // TODO
        }
    }

    if (self->Unk17 != 0 && self->ColFile != NULL && self->Unk20 == 0) // TODO
    {
        State.Renderer->Actions.DrawMainSurfaceAnimationSpriteVersion2(0, 0, 0x10 /* TODO */,
            self->ColFile, (IMAGEPALETTESPRITEPTR)((ADDR)self->ColPckFile + ((U32*)self->ColPckFile)[CursorState.AnimationIndex] + 8)); // TODO
    }

    State.Renderer->Actions.DrawMainSurfaceSprite(0, 0,
        (IMAGESPRITEPTR)AcquireBinAssetContent(&AssetsState.Assets.BriefBK, 0));

    if (self->Unk17 == 0 && self->ColFile != NULL && self->Unk20 == 0) // TODO
    {
        State.Renderer->Actions.DrawMainSurfaceAnimationSpriteVersion2(0, 0, 0x10 /* TODO */,
            self->ColFile, (IMAGEPALETTESPRITEPTR)((ADDR)self->ColPckFile + ((U32*)self->ColPckFile)[CursorState.AnimationIndex] + 8)); // TODO
    }

    TickPanelControl((PANELCONTROLPTR)self);

    self->Description->Self->Tick(self->Description);
}

// 0x1000d680
U32 CLASSCALL ActionObjectType4x5d(CONTROLTYPE4X5DPTR self)
{
    if (self->ArrColFile == NULL) { return CONTROLACTION_BRIEF_OK; }

    U32 action = ActionPanelControl((PANELCONTROLPTR)self);

    if (action == CONTROLACTION_BRIEF_THIRDBUTTON) { FUN_1000e910(self->Description); }

    {
        U32 ticks = GetTickCount();
        self->Description->Self->Action(self->Description);

        if (ticks - self->Ticks < 67) goto LAB_1000d70b; // TODO

        if (ticks < self->Ticks + 132) { self->Ticks = self->Ticks + 66; } // TODO
        else { self->Ticks = ticks; }
    }

    if (self->SeqFileStart < self->SequenceCount)
    {
        // TODO make this pretty
        U32 iVar2 = 0; // TODO
        do
        {
            if (0xFFFFF < ((U32*)self->SeqFile)[self->SeqFileStart]) { break; }

            iVar2 = self->SeqFileStart + 1;
            self->SeqFileStart = iVar2;
        } while (iVar2 < self->SequenceCount);

        if (self->SequenceCount <= self->SeqFileStart) { self->SeqFileStart = 0; }
    }
    else { self->SeqFileStart = 0; }

    self->SeqFileStart = self->SeqFileStart + 1;

LAB_1000d70b:
    if (self->ColFile != NULL)
    {
        if (self->Unk20 != 0)
        {
            self->Unk20 = self->Unk20 - 1;

            return action;
        }

        self->Unk18 = self->Unk18 + 1;

        if (self->Unk19 <= self->Unk18)
        {
            CursorState.AnimationIndex =
                (CursorState.AnimationIndex + 1) % (*(int*)((int)self->ColPckFile + 4) + -1); // TODO
            self->Unk18 = 0;

            if (CursorState.AnimationIndex == 0) { self->Unk20 = self->Unk21; }
        }
    }

    return action;
}

// 0x1000d160
CONTROLTYPE4X5DPTR CLASSCALL ReleaseObjectType4x5d(CONTROLTYPE4X5DPTR self, CONST OBJECTRELEASETYPE mode)
{
    DisposeObjectType4x5d(self);

    if (mode & OBJECTRELEASETYPE_ALLOCATED) { free(self); }

    return self;
}

// 0x1000d620
VOID CLASSCALL DisposeObjectType4x5d(CONTROLTYPE4X5DPTR self)
{
    self->Self = (CONTROLTYPE4X5DSELFPTR)&ObjectType4x5dSelfState;

    if (self->Description != NULL) { self->Description->Self->Release(self->Description, OBJECTRELEASETYPE_ALLOCATED); }

    DisposePanelControl((PANELCONTROLPTR)self);
}

// 0x10018ba0
BOOL FUN_10018ba0(LPSTR param_1, LPSTR param_2) // TODO name, should it be in Saves?
{
    CONST S32 map = AcquireCurrentGameMap();

    if (map == DEFAULT_GAME_MAP_INDEX) { return FALSE; }

    CONST S32 mission = AcquireCurrentGameMission();

    if (mission == DEFAULT_GAME_MISSION_INDEX) { return FALSE; }

    wsprintfA(param_1, "%03i", map);
    wsprintfA(param_2, "%03i%03i", map, mission);

    return TRUE;
}

// 0x1000d8d0
U32 InitializeSprite(LPCSTR name, LPVOID* content) // TODO name, types
{
    // TODO NOT IMPLEMENTED

    return 0;
}

// 0x1000d8a0
S32 OpenBinaryFile(LPCSTR name, LPVOID* content) // TODO name, types
{
    if (*content != NULL) { return -1; } // TodO

    return AcquireAssetContent(name, content, 0);
}