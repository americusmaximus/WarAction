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

#include "AssetFile.hxx"
#include "Assets.hxx"
#include "BriefControl.hxx"
#include "Graphics.hxx"
#include "Messages.hxx"
#include "Saves.hxx"
#include "Sound.hxx"
#include "State.hxx"

#include <stdlib.h>

#define BRIEF_SPRITE_LEVEL              16
#define BRIEF_SPRITE_SHADE_LEVEL        12

#define MAX_RGB_COLOR_COUNT             (3 * MAX_PALETTE_SIZE)

#define MAX_BRIEF_CONTENT_SIZE          0x800

#define MAX_BRIEF_CONTROL_MESSAGE_LENTH 256

U32     AnimationIndex; // 0x10046ca8 // TODO Name LineNumber?

// 0x1003a4e4
BRIEFCONTROLSELF BriefControlSelfState =
{
    (BRIEFCONTROLTYPEACTION)AcquireControlTypePanel,
    InitializeBriefControl,
    DisableBriefControl,
    TickBriefControl,
    ActionBriefControl,
    ReleaseBriefControl,
};

// 0x1000d0b0
BRIEFCONTROLPTR CLASSCALL ActivateBriefControl(BRIEFCONTROLPTR self)
{
    ActivateSceneControl((SCENECONTROLPTR)self, NULL, &AssetsState.Assets.BriefBT, CONTROLACTION_BRIEF_OK);

    self->Self = &BriefControlSelfState;

    self->AnimationSprite = NULL;
    self->AnimationColors = NULL;

    self->Sequences = NULL;

    self->BkgPckFile = NULL;
    self->BkgColFile = NULL;

    self->ColPckFile = NULL;
    self->ColFile = NULL;

    self->Description =
        ActivateDescriptionControl(ALLOCATE(DESCRIPTIONCONTROL), 40, 21, 376, 68, &AssetsState.Fonts.Main, 0, -2);

    return self;
}

// 0x1000d180
VOID CLASSCALL InitializeBriefControl(BRIEFCONTROLPTR self)
{
    InitializePanelControl((PANELCONTROLPTR)self);

    self->SequenceStart = 1;
    self->Ticks = GetTickCount() - 1000;

    for (U32 x = 0; x < MAX_FOG_SPRITE_COUNT; x++)
    {
        memset(&State.Renderer->Fog[x], 0x80, sizeof(FOGSPRITE));
    }

    self->AnimationColors = NULL;

    CHAR map[MAX_FILE_NAME_LENGTH];
    CHAR mission[MAX_FILE_NAME_LENGTH];

    if (AcquireCurrentGameMapMissionNames(map, mission))
    {
        CHAR path[MAX_FILE_NAME_LENGTH];
        wsprintfA(path, "%sb_arr.col", mission);

        self->AnimationSpriteType = InitializeBriefControlSprite(path, &self->AnimationColors);

        if (self->AnimationSpriteType != BRIEFCONTROLSPRITETYPE_NONE)
        {
            wsprintfA(path, "%sb_arr.ani", mission);
            AcquireBriefControlAssetContent(path, &self->AnimationSprite);

            wsprintfA(path, "%sb_arr.seq", mission);
            self->SequenceCount = AcquireBriefControlAssetContent(path, (LPVOID*)&self->Sequences) / sizeof(U32);

            {
                wsprintfA(path, "%sb_text.txt", mission);
                SelectAssetsStateIsActive(FALSE);

                LPSTR txt = NULL;
                CONST U32 length = AcquireBriefControlAssetContent(path, (LPVOID*)&txt);
                SelectAssetsStateIsActive(TRUE);

                LPSTR message = NULL;
                if (length == 0)
                {
                    message = (LPSTR)AcquireAssetMessage(ASSET_MESSAGE_NO_BRIEFING_AVAILABLE);
                }
                else
                {
                    txt[length - 1] = NULL;
                    message = AcquireCleanUnicodeString(txt, TRUE);
                    txt = message;
                }

                SelectDescriptionControlText(self->Description, message);

                if (txt != NULL) { free(txt); }
            }

            wsprintfA(path, "%sb_bkg.col", map);
            self->BackgroundSpriteType = InitializeBriefControlSprite(path, &self->BkgColFile);

            wsprintfA(path, "%sb_bkg.pck", map);
            AcquireBriefControlAssetContent(path, &self->BkgPckFile);

            self->Unk21 = 0; // TODO
            self->Unk20 = 0; // TODO
            self->Unk19 = 0; // TODO
            self->Unk18 = 0; // TODO

            TEXTASSET text;
            LPCSTR name = NULL; // TODO

            {
                ActivateTextAsset(&text);
                InitializeTextAsset(&text, "campaign.seq");

                /*
                >AMERICAN CAMPAIGN
                >
                >Add-On campaign
                100000 -		-		-		>   1. Commando
                100001 -		-		-		>   2. Boomerang
                100002 -		us_vic		-		>   3. Wounded Beast
                >
                >ENGLISH CAMPAIGN
                >
                >Add-On campaign
                012000 -		-		-		>   1. Shield and Sword
                005000 -		-		-		>   2. Trap
                005001 -		en_vic		-		>   3. Scorpion
                */

                // Colums:
                // 1 - mission id
                // 2 - start video
                // 3 - victory video
                // 4 - defeat video
                // 5 - mission name

                U32 category = 0;

                for (U32 x = 0; x < text.Count; x++)
                {
                    CHAR str[MAX_BRIEF_CONTROL_MESSAGE_LENTH];
                    AcquireTextAssetStringValue(&text, x, 0, str);

                    if (str[0] == '>' && str[1] != NULL) { category = category + 1; }

                    if (strncmp(str, mission, strlen(mission)) == 0)
                    {
                        // NOTE. This looks like logic from the past.
                        // These values does not match with anything in the new file.

                        if (category == 1)
                        {
                            name = "b_rufly";

                            self->Unk17 = 0;
                        }
                        else if (category == 2)
                        {
                            name = "b_defire";

                            self->Unk17 = 1;
                        }
                        else if (category == 3)
                        {
                            name = "b_ussmok";

                            self->Unk19 = 3;
                            self->Unk21 = 400;
                            self->Unk17 = 0;
                        }

                        break;
                    }
                }
            }

            InitializeTextAsset(&text, NULL);
            DisposeTextAsset(&text);

            if (name != NULL)
            {
                wsprintfA(path, "%s.col", name);
                InitializeBriefControlSprite(path, &self->ColFile);

                wsprintfA(path, "%s.pck", name);
                AcquireBriefControlAssetContent(path, &self->ColPckFile);
            }

            ZeroMemory(State.Renderer->Surface.Back,
                State.Renderer->Surface.Height * State.Renderer->Surface.Width * sizeof(PIXEL));

            {
                CONST ANIMATIONSPRITEHEADERPTR header = (ANIMATIONSPRITEHEADERPTR)self->BkgPckFile;
                IMAGEPALETTESPRITEPTR sprite =
                    (IMAGEPALETTESPRITEPTR)((ADDR)self->BkgPckFile + (ADDR)header->Offsets[0]);

                State.Renderer->Actions.DrawBackSurfacePaletteShadeSprite(0, 0,
                    BRIEF_SPRITE_SHADE_LEVEL, (PIXEL*)self->BkgColFile, sprite);
            }

            ReleaseSoundStateTracks(&SoundState.State, TRUE);

            {
                STRINGVALUE setting;
                STRINGVALUEPTR actual = AcquireStringValue(&setting, "%s.apm", mission);
                InitializeSoundStateActionBuffer(&SoundState.State, actual->Value);
                ReleaseStringValue(actual);
            }

            self->Description->Self->Initialize(self->Description);
        }
    }
}

// 0x1000d580
VOID CLASSCALL DisableBriefControl(BRIEFCONTROLPTR self)
{
    DisablePanelControl((PANELCONTROLPTR)self);

    self->Description->Self->Disable(self->Description);

    if (self->AnimationColors != NULL)
    {
        free(self->AnimationColors);
        self->AnimationColors = NULL;

        free(self->AnimationSprite);
        self->AnimationSprite = NULL;

        free(self->Sequences);
        self->Sequences = NULL;

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
VOID CLASSCALL TickBriefControl(BRIEFCONTROLPTR self)
{
    State.Renderer->Actions.WriteBackSurfaceMainSurfaceRectangle(0, 0, GRAPHICS_RESOLUTION_640, GRAPHICS_RESOLUTION_480);

    for (U32 x = self->SequenceStart; x < self->SequenceCount; x++)
    {
        CONST U32 index = self->Sequences[x];

        if (0xFFFF < index) { break; } // TODO

        CONST ANIMATIONSPRITEHEADERPTR header = (ANIMATIONSPRITEHEADERPTR)self->AnimationSprite;
        IMAGEPALETTESPRITEPTR sprite =
            (IMAGEPALETTESPRITEPTR)((ADDR)self->AnimationSprite + (ADDR)header->Offsets[index]);

        if (self->AnimationSpriteType == BRIEFCONTROLSPRITETYPE_UNKNOWN_1)
        {
            State.Renderer->Actions.DrawMainSurfacePaletteSprite(0, 0,
                (PIXEL*)self->AnimationColors, sprite);
        }
        else
        {
            State.Renderer->Actions.DrawMainSurfaceAnimationSpriteStencil(0, 0,
                BRIEF_SPRITE_LEVEL, (ANIMATIONPIXEL*)self->AnimationColors, sprite);
        }
    }

    if (self->Unk17 != 0 && self->ColFile != NULL && self->Unk20 == 0) // TODO
    {
        CONST ANIMATIONSPRITEHEADERPTR header = (ANIMATIONSPRITEHEADERPTR)self->ColPckFile;
        IMAGEPALETTESPRITEPTR sprite =
            (IMAGEPALETTESPRITEPTR)((ADDR)self->ColPckFile + (ADDR)header->Offsets[AnimationIndex]);

        State.Renderer->Actions.DrawMainSurfaceAnimationSpriteStencil(0, 0,
            BRIEF_SPRITE_LEVEL, (ANIMATIONPIXEL*)self->ColFile, sprite);
    }

    State.Renderer->Actions.DrawMainSurfaceSprite(0, 0,
        (IMAGESPRITEPTR)AcquireBinAssetContent(&AssetsState.Assets.BriefBK, 0));

    if (self->Unk17 == 0 && self->ColFile != NULL && self->Unk20 == 0) // TODO
    {
        CONST ANIMATIONSPRITEHEADERPTR header = (ANIMATIONSPRITEHEADERPTR)self->ColPckFile;
        IMAGEPALETTESPRITEPTR sprite =
            (IMAGEPALETTESPRITEPTR)((ADDR)self->ColPckFile + (ADDR)header->Offsets[AnimationIndex]);

        State.Renderer->Actions.DrawMainSurfaceAnimationSpriteStencil(0, 0,
            BRIEF_SPRITE_LEVEL, (ANIMATIONPIXEL*)self->ColFile, sprite);
    }

    TickPanelControl((PANELCONTROLPTR)self);

    self->Description->Self->Tick(self->Description);
}

// 0x1000d680
U32 CLASSCALL ActionBriefControl(BRIEFCONTROLPTR self)
{
    if (self->AnimationColors == NULL) { return CONTROLACTION_BRIEF_OK; }

    CONST U32 action = ActionPanelControl((PANELCONTROLPTR)self);

    if (action == CONTROLACTION_BRIEF_THIRDBUTTON) {
        // When pause button is hit, expected behavior:
        // 1. Description is paused, no new text is rendered or scrolled.
        // 2. The visual map animations are still happening as usual.

        FUN_1000e910(self->Description); // TODO
    }

    U32 ticks = GetTickCount();
    self->Description->Self->Action(self->Description);

    if (self->Ticks + 67 <= ticks)
    {
        self->Ticks = ticks < self->Ticks + 132 ? self->Ticks + 66 : ticks;

        if (self->SequenceStart < self->SequenceCount)
        {
            do
            {
                if (0xFFFFF < self->Sequences[self->SequenceStart]) { break; } // TODO

                self->SequenceStart = self->SequenceStart + 1;
            } while (self->SequenceStart < self->SequenceCount);

            if (self->SequenceCount <= self->SequenceStart) { self->SequenceStart = 0; }
        }
        else { self->SequenceStart = 0; }

        self->SequenceStart = self->SequenceStart + 1;
    }

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
            AnimationIndex = (AnimationIndex + 1) % (*(int*)((int)self->ColPckFile + 4) + -1); // TODO

            self->Unk18 = 0;

            if (AnimationIndex == 0) { self->Unk20 = self->Unk21; }
        }
    }

    return action;
}

// 0x1000d160
BRIEFCONTROLPTR CLASSCALL ReleaseBriefControl(BRIEFCONTROLPTR self, CONST OBJECTRELEASETYPE mode)
{
    DisposeBriefControl(self);

    if (mode & OBJECTRELEASETYPE_ALLOCATED) { free(self); }

    return self;
}

// 0x1000d620
VOID CLASSCALL DisposeBriefControl(BRIEFCONTROLPTR self)
{
    self->Self = (BRIEFCONTROLSELFPTR)&BriefControlSelfState;

    if (self->Description != NULL) { self->Description->Self->Release(self->Description, OBJECTRELEASETYPE_ALLOCATED); }

    DisposePanelControl((PANELCONTROLPTR)self);
}

// 0x10018ba0
BOOL AcquireCurrentGameMapMissionNames(LPSTR mapName, LPSTR missionName)
{
    CONST S32 map = AcquireCurrentGameMap();

    if (map == DEFAULT_GAME_MAP_INDEX) { return FALSE; }

    CONST S32 mission = AcquireCurrentGameMission();

    if (mission == DEFAULT_GAME_MISSION_INDEX) { return FALSE; }

    wsprintfA(mapName, "%03i", map);
    wsprintfA(missionName, "%03i%03i", map, mission);

    return TRUE;
}

// 0x1000d8d0
BRIEFCONTROLSPRITETYPE InitializeBriefControlSprite(LPCSTR name, LPVOID* content)
{
    if (*content != NULL) { return BRIEFCONTROLSPRITETYPE_INVALID; }

    ASSETFILE file = { (BFH)INVALID_BINFILE_VALUE };
    if (!OpenAssetFile(&file, name)) { return BRIEFCONTROLSPRITETYPE_NONE; }

    *content = malloc(MAX_BRIEF_CONTENT_SIZE);
    ZeroMemory(*content, MAX_BRIEF_CONTENT_SIZE);

    ReadAssetFile(&file, *content, 2 * sizeof(U32));

    CONST U32* header = (U32*)*content;

    if (header[0] == 0x308 && header[1] == 0xB123) // TODO
    {
        U8 colors[MAX_RGB_COLOR_COUNT];

        if (ReadAssetFile(&file, colors, MAX_RGB_COLOR_COUNT) != MAX_RGB_COLOR_COUNT)
        {
            CloseAssetFile(&file);

            return BRIEFCONTROLSPRITETYPE_NONE;
        }

        PIXEL* pixels = (PIXEL*)*content;

        for (U32 x = 0; x < MAX_PALETTE_SIZE; x++)
        {
            CONST U32 r = ((colors[3 * x + 0] << 8) >> State.Renderer->RedOffset) & State.Renderer->ActualRedMask;
            CONST U32 g = ((colors[3 * x + 1] << 8) >> State.Renderer->GreenOffset) & State.Renderer->ActualGreenMask;
            CONST U32 b = ((colors[3 * x + 2] << 8) >> State.Renderer->BlueOffset) & State.Renderer->ActualBlueMask;

            pixels[x] = r | g | b;
        }

        CloseAssetFile(&file);

        return BRIEFCONTROLSPRITETYPE_UNKNOWN_1;
    }
    else
    {
        PIXEL* pixels = (PIXEL*)*content;
        CONST U32 length = ReadAssetFile(&file, &pixels[4], 0x3F8); // TODO

        // NOTE. Used in the American add-on campaign animated background - the fireplace in the background.

        if (length == 0x3F8) // TODO
        {
            for (U32 x = 0; x < MAX_PALETTE_SIZE; x++) { pixels[x * 2] = ADJUSTSPRITECOLOR(pixels[x * 2]); }

            CloseAssetFile(&file);

            return BRIEFCONTROLSPRITETYPE_UNKNOWN_2;
        }

        // NOTE. Used for the backgrounds for the briefs, such as tables, glasses, flags, etc.

        if (length == 0x1F8) // TODO
        {
            for (U32 x = 0; x < MAX_PALETTE_SIZE; x++) { pixels[x] = ADJUSTSPRITECOLOR(pixels[x]); }

            CloseAssetFile(&file);

            return BRIEFCONTROLSPRITETYPE_UNKNOWN_1;
        }
    }

    CloseAssetFile(&file);

    return BRIEFCONTROLSPRITETYPE_NONE;
}

// 0x1000d8a0
S32 AcquireBriefControlAssetContent(LPCSTR name, LPVOID* content)
{
    if (*content != NULL) { return INVALID_ASSET_FILE_SIZE; }

    return AcquireAssetContent(name, content, 0);
}
