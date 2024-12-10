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
#include "Assets.hxx"
#include "ControlCommand.hxx"
#include "Cursor.hxx"
#include "ScrollControl.hxx"
#include "State.hxx"

#include <stdlib.h>

#define SCROLL_CONTROL_ACTION_PRIORITY_1  0x8001 /* TODO */
#define SCROLL_CONTROL_ACTION_PRIORITY_2  0x8002 /* TODO */

#define CONTROLCOMMANDACTION_SCROLL_UNKNONW1    (CONTROLCOMMANDACTION_UNKNOWN_400 | CONTROLCOMMANDACTION_MOUSE_RIGHT_DOUBLECLICK        \
                                                    | CONTROLCOMMANDACTION_MOUSE_LEFT_DOUBLECLICK | CONTROLCOMMANDACTION_MOUSE_RIGHT_UP \
                                                    | CONTROLCOMMANDACTION_MOUSE_RIGHT_DOWN  | CONTROLCOMMANDACTION_MOUSE_LEFT_UP       \
                                                    | CONTROLCOMMANDACTION_MOUSE_LEFT_DOWN  | CONTROLCOMMANDACTION_MOUSE_LEAVE | CONTROLCOMMANDACTION_MOUSE_ENTER) /* TODO */

#define CONTROLCOMMANDACTION_SCROLL_UNKNONW2    (CONTROLCOMMANDACTION_UNKNOWN_400 | CONTROLCOMMANDACTION_MOUSE_RIGHT_DOUBLECLICK        \
                                                    | CONTROLCOMMANDACTION_MOUSE_LEFT_DOUBLECLICK | CONTROLCOMMANDACTION_MOUSE_RIGHT_UP \
                                                    | CONTROLCOMMANDACTION_MOUSE_RIGHT_DOWN | CONTROLCOMMANDACTION_MOUSE_LEFT_UP        \
                                                    | CONTROLCOMMANDACTION_MOUSE_LEFT_DOWN | CONTROLCOMMANDACTION_MOUSE_LEAVE           \
                                                    | CONTROLCOMMANDACTION_MOUSE_ENTER | CONTROLCOMMANDACTION_MOUSE_SCROLL)  /* TODO */

// 0x1003a3a8
SCROLLCONTROLSELF ScrollControlSelfState =
{
    (SCROLLCONTROLTYPEACTION)AcquireControlTypeScroll,
    InitializeScrollControl,
    DisableScrollControl,
    TickScrollControl,
    ActionScrollControl,
    ReleaseScrollControl
};

// 0x100065e0
SCROLLCONTROLPTR CLASSCALL ActivateScrollControl(SCROLLCONTROLPTR self, BINASSETPTR asset, CONST U32 action)
{
    self->Self = &ScrollControlSelfState;

    self->IsActive = FALSE;
    self->Asset = asset;

    self->Up = ActivateButtonControl(ALLOCATE(BUTTONCONTROL), asset, 0, action + 2);
    self->Down = ActivateButtonControl(ALLOCATE(BUTTONCONTROL), asset, 2, action + 3);
    
    self->Action = action;

    return self;
}

// 0x10006780
VOID CLASSCALL InitializeScrollControl(SCROLLCONTROLPTR self)
{
    self->Up->Self->Initialize(self->Up);
    self->Up->Unk12 = 1; // TODO

    self->Down->Self->Initialize(self->Down);
    self->Down->Unk12 = 1; // TODO

    InitializeScrollerScrollControl(self);
}

// 0x100067b0
VOID CLASSCALL DisableScrollControl(SCROLLCONTROLPTR self)
{
    self->Up->Self->Disable(self->Up);
    self->Down->Self->Disable(self->Down);

    DisableScrollerScrollControl(self);
}

// 0x100067d0
VOID CLASSCALL TickScrollControl(SCROLLCONTROLPTR self)
{
    self->Up->Self->Tick(self->Up);
    self->Down->Self->Tick(self->Down);

    TickScrollerScrollControl(self);
}

// 0x100067f0
U32 CLASSCALL ActionScrollControl(SCROLLCONTROLPTR self)
{
    if (self->Max <= self->Min) { return CONTROLACTION_NONE; }

    CONST U32 current = self->Current;

    if (self->Up->Self->Action(self->Up) != CONTROLACTION_NONE && (self->Current != 0))
    {
        self->Current = self->Current - 1;

        ScrollScrollControl(self);
    }

    if (self->Down->Self->Action(self->Down) != CONTROLACTION_NONE && (self->Min + self->Current < self->Max))
    {
        self->Current = self->Current + 1;

        ScrollScrollControl(self);
    }

    ActionScrollerScrollControl(self);

    return current != self->Current;
}

// 0x100066a0
SCROLLCONTROLPTR CLASSCALL ReleaseScrollControl(SCROLLCONTROLPTR self, CONST OBJECTRELEASETYPE mode)
{
    DisposeScrollControl(self);

    if (mode & OBJECTRELEASETYPE_ALLOCATED) { free(self); }

    return self;
}

// 0x10006720
VOID CLASSCALL DisposeScrollControl(SCROLLCONTROLPTR self)
{
    self->Self = (ScrollControlSelf*)&ControlSelfState;

    if (self->Up != NULL) { self->Up->Self->Release(self->Up, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->Down != NULL) { self->Down->Self->Release(self->Down, OBJECTRELEASETYPE_ALLOCATED); }
}

// 0x10006340
VOID CLASSCALL InitializeScrollerScrollControl(SCROLLCONTROLPTR self)
{
    InitializeControl((CONTROLPTR)self);

    ActivateActionArea(0, 0, 1, 1, CONTROLCOMMANDACTION_SCROLL_UNKNONW1, self->Action, SCROLL_CONTROL_ACTION_PRIORITY_2);

    ScrollScrollControl(self);

    IMAGESPRITEPTR image = (IMAGESPRITEPTR)AcquireBinAssetContent(&AssetsState.Assets.ScrollBar, 0);

    image->X = 0;
    image->Y = 0;

    ActivateActionArea(self->X, self->Y, image->Width, self->Height - self->Y + 1,
        CONTROLCOMMANDACTION_SCROLL_UNKNONW2, self->Action + 1, SCROLL_CONTROL_ACTION_PRIORITY_1);
}

// 0x100062d0
VOID CLASSCALL ScrollScrollControl(SCROLLCONTROLPTR self)
{
    if (self->IsActive && self->Min < self->Max)
    {
        IMAGESPRITEPTR image = (IMAGESPRITEPTR)AcquireBinAssetContent(&AssetsState.Assets.ScrollBar, 0);

        S32 h = 0;
        S32 y = 0;

        if (self->Max == 0)
        {
            y = self->Y;
            h = self->Height;
        }
        else
        {
            h = self->Current;

            y = AcquireScrollControlVerticalOffset(self, h);
            h = AcquireScrollControlVerticalOffset(self, self->Min + h);

            h = h - 1;
        }

        AcquireScrollControlActionArea(self->Action, self->X, y, image->Width, h - y + 1);
    }
}

// 0x100062b0
S32 CLASSCALL AcquireScrollControlVerticalOffset(SCROLLCONTROLPTR self, CONST S32 offset)
{
    return ((self->Height - self->Y) * offset) / self->Max + self->Y;
}

// 0x100063c0
VOID CLASSCALL DisableScrollerScrollControl(SCROLLCONTROLPTR self)
{
    DisableControl((CONTROLPTR)self);

    DequeueControlCommand(self->Action);
    DequeueControlCommand(self->Action + 1);
}

// 0x100063e0
VOID CLASSCALL TickScrollerScrollControl(SCROLLCONTROLPTR self)
{
    if (self->Min < self->Max)
    {
        CONST ACTIONAREAPTR area = AcquireActionArea(self->Action);
        CONST IMAGESPRITEPTR image = (IMAGESPRITEPTR)AcquireBinAssetContent(&AssetsState.Assets.ScrollBar, 0);

        State.Renderer->Window.X = 0;
        State.Renderer->Window.Y = area->Y;
        State.Renderer->Window.Width = GRAPHICS_RESOLUTION_640 - 1;
        State.Renderer->Window.Height = area->Height / 2 - 1 + area->Y;

        State.Renderer->Actions.DrawMainSurfaceSprite(area->X, area->Y, image);

        State.Renderer->Window.X = 0;
        State.Renderer->Window.Y = area->Height / 2 + area->Y;
        State.Renderer->Window.Width = GRAPHICS_RESOLUTION_640 - 1;
        State.Renderer->Window.Height = area->Y - 1 + area->Height;

        State.Renderer->Actions.DrawMainSurfaceSprite(area->X, area->Height - image->Height + area->Y, image);

        State.Renderer->Window.X = 0;
        State.Renderer->Window.Y = 0;
        State.Renderer->Window.Width = GRAPHICS_RESOLUTION_640 - 1;
        State.Renderer->Window.Height = GRAPHICS_RESOLUTION_480 - 1;
    }
}

// 0x10006510
BOOL CLASSCALL ActionScrollerScrollControl(SCROLLCONTROLPTR self)
{
    ActionControl((CONTROLPTR)self);

    if (self->Max <= self->Min) { return FALSE; }

    CONST S32 value = self->Current;

    S32 current = (CursorState.Y - self->Y) * self->Max / (self->Height - self->Y) - self->Min / 2;

    if (current < 0) { current = 0; }

    if (self->Max < self->Min + current) { current = self->Max - self->Min; }

    if (!CursorState.IsLeft) { self->IsAction = FALSE; }

    if (self->IsAction)
    {
        self->Current = current;

        ScrollScrollControl(self);
    }

    CONTROLCOMMANDPTR command = DequeueControlCommand(FALSE);

    if (command != NULL)
    {
        if (command->Command == self->Action + 1)
        {
            if (command->Action & CONTROLCOMMANDACTION_MOUSE_LEFT_DOWN)
            {
                self->Current = current;

                ScrollScrollControl(self);
            }

            DequeueControlCommand(TRUE);
        }

        if (command->Command == self->Action)
        {
            if (command->Action & CONTROLCOMMANDACTION_MOUSE_LEFT_DOWN) { self->IsAction = TRUE; }

            DequeueControlCommand(TRUE);
        }
    }

    return value != self->Current;
}

// 0x10006270
VOID CLASSCALL InitializeScrollControlArea(SCROLLCONTROLPTR self, CONST U32 action, CONST S32 x, CONST S32 y, CONST S32 height)
{
    self->Action = action;

    self->X = x;
    self->Y = y;
    self->Height = height;

    self->Current = 0;
    self->Min = 0;
    self->Max = 0;

    self->IsAction = FALSE;
}

// 0x100066c0
VOID CLASSCALL InitializeScrollControlArea(SCROLLCONTROLPTR self, CONST S32 x, CONST S32 offsetUp, CONST S32 offsetDown)
{
    CONST IMAGESPRITEPTR up = (IMAGESPRITEPTR)AcquireBinAssetContent(self->Asset, 0);
    CONST IMAGESPRITEPTR down = (IMAGESPRITEPTR)AcquireBinAssetContent(self->Asset, 2);

    InitializeScrollControlArea(self, self->Action, up->X + x, up->Y + offsetUp + up->Height, down->Y - 1 + offsetDown);
}

// 0x10022540
BOOL AcquireScrollControlActionArea(CONST U32 action, CONST S32 x, CONST S32 y, CONST S32 width, CONST S32 height)
{
    ACTIONAREAPTR area = AcquireActionArea(action);

    if (area == NULL) { return FALSE; }

    SelectActionAreaDimensions(area, x, y, width, height);

    return TRUE;
}