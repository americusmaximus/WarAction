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

#include "ActionArea.hxx"
#include "Assets.hxx"
#include "ControlCommand.hxx"
#include "Graphics.hxx"
#include "MapControl.hxx"
#include "State.hxx"
#include "Strings.hxx"

#include <Extension.hxx>

#include <stdlib.h>

#define MAX_MAP_CONTROL_TEXT_LENGTH 256

// 0x1003a580
MAPCONTROLSELF MapControlSelfState =
{
    (MAPCONTROLTYPEACTION)AcquireControlTypeControl,
    InitializeMapControl,
    DisableMapControl,
    TickMapControl,
    ActionMapControl,
    ReleaseMapControl
};

// 0x1000e950
MAPCONTROLPTR CLASSCALL ActivateMapControl(MAPCONTROLPTR self, CONST S32 x, CONST S32 y, CONST S32 dx, CONST S32 dy, CONST S32 width, CONST S32 height, LABELCONTROLPTR size, LABELCONTROLPTR actors)
{
    self->Self = &MapControlSelfState;

    self->IsActive = FALSE;

    ActivateMap(&self->Map);

    self->Description = ActivateDescriptionControl(ALLOCATE(DESCRIPTIONCONTROL),
        dx, dy, width, height, &AssetsState.Fonts.Main, 3, 3);

    self->X = x;
    self->Y = y;

    self->Actors = actors;
    self->Size = size;

    return self;
}

// 0x1000eaa0
VOID CLASSCALL InitializeMapControl(MAPCONTROLPTR self)
{
    self->Description->Self->Initialize(self->Description);

    self->Map.Descriptor.Details.Height = 0;
    self->Map.Descriptor.Details.Width = 0;

    self->IsHover = FALSE;

    if (self->X != 0)
    {
        ActivateActionArea(self->X - 65, self->Y, 130, 65,
            CONTROLCOMMANDACTION_MOUSE_LEAVE | CONTROLCOMMANDACTION_MOUSE_ENTER,
            CONTROLACTION_MAP_HOVER, DEFAULT_ACTION_PRIORITY);
    }
}

// 0x1000eaf0
VOID CLASSCALL DisableMapControl(MAPCONTROLPTR self)
{
    self->Description->Self->Disable(self->Description);

    if (self->X != 0) { DequeueControlCommand(CONTROLACTION_MAP_HOVER); }
}

// 0x1000eb20
VOID CLASSCALL TickMapControl(MAPCONTROLPTR self)
{
    self->Description->Self->Tick(self->Description);

    if (self->Map.Descriptor.Details.Width != 0 && self->X != 0)
    {
        CONST BOOL normal =
            self->Map.Descriptor.Details.Width <= MAX_MAP_SIZE && self->Map.Descriptor.Details.Height <= MAX_MAP_SIZE;

        CONST S32 height = normal ? self->Map.Descriptor.Details.Height : self->Map.Descriptor.Details.Height / 2;
        CONST S32 width = normal ? self->Map.Descriptor.Details.Width : self->Map.Descriptor.Details.Width / 2;

        for (S32 x = 0; x < height; x = x + 2)
        {
            for (S32 xx = 0; xx < width; xx = xx + 2)
            {
                State.Renderer->Actions.DrawMainSurfaceColorPoint(self->X + (-x + xx) / 2,
                    self->Y + (x + xx) / 4, self->Map.Pixels[x * width + xx]);
            }
        }

        if (self->IsHover)
        {
            State.Renderer->Actions.DrawMainSurfaceSprite(0, 0,
                (IMAGESPRITEPTR)AcquireBinAssetContent(&AssetsState.Assets.KartaBK, 0));

            S32 ox = 236;

            for (S32 x = 0; x < height; x++)
            {
                S32 oy = x + 98;

                for (S32 xx = 0; xx < width; xx++)
                {
                    CONST PIXEL pixel = self->Map.Pixels[x * width + xx];

                    State.Renderer->Actions.DrawMainSurfaceColorPoint(ox + xx, oy, pixel);

                    if (ox < 236 && xx < width - 1)
                    {
                        State.Renderer->Actions.DrawMainSurfaceColorPoint(ox + xx + 1, oy, pixel);
                    }

                    oy = oy + 1;
                }

                ox = ox - 1;
            }
        }
    }
}

// 0x1000ed40
U32 CLASSCALL ActionMapControl(MAPCONTROLPTR self)
{
    self->Description->Self->Action(self->Description);

    CONST CONTROLCOMMANDPTR command = DequeueControlCommand(FALSE);

    if (command != NULL && command->Command == CONTROLACTION_MAP_HOVER)
    {
        if (command->Action == CONTROLCOMMANDACTION_MOUSE_ENTER) { self->IsHover = TRUE; }
        else if (command->Action == CONTROLCOMMANDACTION_MOUSE_LEAVE) { self->IsHover = FALSE; }

        DequeueControlCommand(TRUE);
    }

    return CONTROLACTION_NONE;
}

// 0x1000ea20
MAPCONTROLPTR CLASSCALL ReleaseMapControl(MAPCONTROLPTR self, CONST OBJECTRELEASETYPE mode)
{
    DisposeMapControl(self);

    if (mode & OBJECTRELEASETYPE_ALLOCATED) { free(self); }

    return self;
}

// 0x1000ea40
VOID CLASSCALL DisposeMapControl(MAPCONTROLPTR self)
{
    if (self->Description != NULL) { self->Description->Self->Release(self->Description, OBJECTRELEASETYPE_ALLOCATED); }

    DisposeMap(&self->Map);

    self->Self = (MAPCONTROLSELFPTR)&ControlSelfState;
}

// 0x1000eda0
VOID CLASSCALL InitializeMapMapControl(MAPCONTROLPTR self, LPCSTR name)
{
    CHAR message[MAX_MAP_CONTROL_TEXT_LENGTH];

    if (name != NULL)
    {
        CONST U32 length = strlen(name);

        if (length >= 3)
        {
            BOOL loaded = FALSE;

            if (name[length - 2] == FILE_EXT_MULTI) { loaded = InitializeMultiMap(name, &self->Map); }
            else if (name[length - 2] == FILE_EXT_SINGLE) { loaded = InitializeSingleMap(name, &self->Map); }

            if (loaded)
            {
                self->Description->Self->Disable(self->Description);
                self->Description->Color = ADJUSTCOLORS(0x6400, 0xFF00, 0x6400);

                SelectDescriptionControlText(self->Description,
                    self->Map.Description == NULL ? StringsState.Scratch : self->Map.Description);

                self->Description->Self->Initialize(self->Description);

                if (self->Size != NULL)
                {
                    wsprintfA(message, "%dx%d", self->Map.Descriptor.Details.Width, self->Map.Descriptor.Details.Height);
                    SlectLabelControlText(self->Size, message);
                }

                if (self->Actors != NULL)
                {
                    if (self->Map.Descriptor.Actors.Min_Players == self->Map.Descriptor.Actors.Max_Players)
                    {
                        wsprintfA(message, "%d", self->Map.Descriptor.Actors.Min_Players);
                    }
                    else
                    {
                        wsprintfA(message, "%d-%d", self->Map.Descriptor.Actors.Min_Players, self->Map.Descriptor.Actors.Max_Players);
                    }

                    SlectLabelControlText(self->Actors, message);
                }

                CONST BOOL normal =
                    self->Map.Descriptor.Details.Width <= MAX_MAP_SIZE && self->Map.Descriptor.Details.Height <= MAX_MAP_SIZE;

                CONST U32 height = normal ? self->Map.Descriptor.Details.Height : self->Map.Descriptor.Details.Height / 2;
                CONST U32 width = normal ? self->Map.Descriptor.Details.Width : self->Map.Descriptor.Details.Width / 2;

                for (U32 x = 0; x < height; x++)
                {
                    for (U32 xx = 0; xx < width; xx++)
                    {
                        self->Map.Pixels[x * height + xx] = ADJUSTSPRITECOLOR(self->Map.Pixels[x * height + xx]);
                    }
                }

                return;
            }
        }
    }

    self->Map.Descriptor.Details.Height = 0;
    self->Map.Descriptor.Details.Width = 0;

    self->Map.Descriptor.Actors.Max_Players = 0;

    self->Description->Self->Disable(self->Description);

    SelectDescriptionControlText(self->Description, StringsState.Scratch);
    self->Description->Self->Initialize(self->Description);

    if (self->Size != NULL) { SlectLabelControlText(self->Size, StringsState.Scratch); }
    if (self->Actors != NULL) { SlectLabelControlText(self->Actors, StringsState.Scratch); }
}