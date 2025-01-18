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
#include "ControlCommand.hxx"
#include "Graphics.hxx"
#include "ListControl.hxx"
#include "Sound.hxx"
#include "State.hxx"

#include <stdlib.h>

#define MAX_LIST_CONTROL_ITEM_LENGTH    100

// 0x1003a3c0
LISTCONTROLSELF ListControlSelfState =
{
    (LISTCONTROLTYPEACTION)AcquireControlTypeList,
    InitializeListControl,
    DisableListControl,
    TickListControl,
    ActionListControl,
    ReleaseListControl
};

// 0x10006920
LISTCONTROLPTR CLASSCALL ActivateListControl(LISTCONTROLPTR self, CONST U32 action, CONST S32 x, CONST S32 y, CONST S32 width, CONST S32 height, SCROLLCONTROLPTR scroll, FONTASSETPTR font, STRINGLISTPTR items)
{
    self->Self = &ListControlSelfState;

    self->IsActive = FALSE;

    self->Action = action;

    self->X = x;
    self->Y = y;
    self->Width = width;
    self->Height = height;

    self->Scroll = scroll;
    self->Font = font;
    self->Items = items;

    self->Index = INVALID_LIST_CONTROL_INDEX;

    return self;
}

// 0x10006a00
VOID CLASSCALL InitializeListControl(LISTCONTROLPTR self)
{
    self->Scroll->Current = 0;
    ScrollScrollControl(self->Scroll);

    self->Scroll->Count = self->Items->Count;
    ScrollScrollControl(self->Scroll);

    if (self->Items->Count == 1) { self->Index = 0; }

    self->Scroll->Visible = self->Height / AcquireFontAssetHeight(self->Font);
    ScrollScrollControl(self->Scroll);

    self->Height = self->Scroll->Visible * AcquireFontAssetHeight(self->Font);

    self->Scroll->Self->Initialize(self->Scroll);

    ActivateActionArea(self->X, self->Y, self->Width, self->Height,
        CONTROLCOMMANDACTION_MOUSE_LEFT_DOUBLECLICK | CONTROLCOMMANDACTION_MOUSE_LEFT_DOWN,
        self->Action, DEFAULT_ACTION_PRIORITY);

    InitializeControl((CONTROLPTR)self);

    self->Iteration = 0;
}

// 0x10006ae0
VOID CLASSCALL DisableListControl(LISTCONTROLPTR self)
{
    DisableControl((CONTROLPTR)self);
    DequeueControlCommand(self->Action);
    self->Scroll->Self->Disable(self->Scroll);
}

// 0x10006b00
VOID CLASSCALL TickListControl(LISTCONTROLPTR self)
{
    State.Renderer->Window.X = self->X;
    State.Renderer->Window.Y = self->Y;
    State.Renderer->Window.Width = self->X - 1 + self->Width;
    State.Renderer->Window.Height = self->Y - 1 + self->Height;

    for (S32 x = 0; x < self->Scroll->Visible; x++)
    {
        CONST S32 current = self->Scroll->Current + x;

        if (current < self->Items->Count)
        {
            SelectFontAssetColor(self->Font,
                current == self->Index ? ADJUSTCOLORS(0xFF00, 0xFF00, 0x0000) : ADJUSTCOLORS(0x8000, 0x8000, 0x0000));

            CHAR item[MAX_LIST_CONTROL_ITEM_LENGTH];
            self->Items->Self->AcquireValue(self->Items, current, item);
            DrawFontAssetText(self->Font, self->X, AcquireFontAssetHeight(self->Font) * x + self->Y, item, FONTALIGNMENT_LEFT);
        }
    }

    State.Renderer->Window.X = 0;
    State.Renderer->Window.Y = 0;
    State.Renderer->Window.Width = GRAPHICS_RESOLUTION_640 - 1;
    State.Renderer->Window.Height = GRAPHICS_RESOLUTION_480 - 1;

    self->Scroll->Self->Tick(self->Scroll);
}

// 0x10006c90
U32 CLASSCALL ActionListControl(LISTCONTROLPTR self)
{
    ActionControl((CONTROLPTR)self);

    self->Scroll->Self->Action(self->Scroll);

    CONST S32 indx = self->Index;

    CONST CONTROLCOMMANDPTR command = DequeueControlCommand(FALSE);

    if (command == NULL) { return CONTROLACTION_NONE; }

    if (command->Command != CONTROLCOMMAND_KBD)
    {
        if (command->Command != self->Action) { return CONTROLACTION_NONE; }

        if (command->Action & CONTROLCOMMANDACTION_MOUSE_LEFT_DOWN)
        {
            CONST U32 item = command->Parameter2 / AcquireFontAssetHeight(self->Font) + self->Scroll->Current;

            if (item < self->Items->Count)
            {
                if (self->Index == item) { self->Iteration = self->Iteration + 1; }
                else
                {
                    self->Index = item;
                    EnqueueControlCommand(CONTROLCOMMAND_UI, self->Action,
                        CONTROLACTION_UI_CHANGE, DEFAULT_CONTROLACTION_UI_VALUE);

                    self->Iteration = 1;
                    PlaySoundStateSound(&SoundState.State, "mouse_click");
                }
            }
        }

        if ((command->Action & VK_INPUT) == CONTROLCOMMANDACTION_MOUSE_LEFT_DOUBLECLICK && 1 < self->Iteration)
        {
            EnqueueControlCommand(CONTROLCOMMAND_UI, self->Action,
                CONTROLACTION_UI_SELECT, DEFAULT_CONTROLACTION_UI_VALUE);
        }

        DequeueControlCommand(TRUE);

        return indx != self->Index;
    }

    if (self->Index == INVALID_LIST_CONTROL_INDEX) { return indx != self->Index; }

    if (command->Action == (VK_RETURN + CONTROLCOMMANDACTION_KBD_KEY_DOWN))
    {
        if (self->Index == INVALID_LIST_CONTROL_INDEX)
        {
            EnqueueControlCommand(CONTROLCOMMAND_UI, self->Action,
                CONTROLACTION_UI_SELECT, DEFAULT_CONTROLACTION_UI_VALUE);

            DequeueControlCommand(TRUE);
        }

        return indx != self->Index;
    }

    if (command->Action == (VK_UP + CONTROLCOMMANDACTION_KBD_KEY_DOWN))
    {
        if (self->Index > 0)
        {
            self->Index = self->Index - 1;

            if (self->Index < self->Scroll->Current)
            {
                self->Scroll->Current = self->Index;
                ScrollScrollControl(self->Scroll);
            }

            while (self->Scroll->Visible - 1 + self->Scroll->Current < self->Index)
            {
                self->Scroll->Current = self->Scroll->Current + 1;
                ScrollScrollControl(self->Scroll);
            }

            EnqueueControlCommand(CONTROLCOMMAND_UI, self->Action,
                CONTROLACTION_UI_CHANGE, DEFAULT_CONTROLACTION_UI_VALUE);

            AdjustScrollControlListControl(self);
        }

        DequeueControlCommand(TRUE);
    }
    else if (command->Action == (VK_DOWN + CONTROLCOMMANDACTION_KBD_KEY_DOWN))
    {
        if (self->Index + 1 < self->Items->Count)
        {
            self->Index = self->Index + 1;

            if (self->Index < self->Scroll->Current)
            {
                self->Scroll->Current = self->Index;
                ScrollScrollControl(self->Scroll);
            }

            while (self->Scroll->Visible - 1 + self->Scroll->Current < self->Index)
            {
                self->Scroll->Current = self->Scroll->Current + 1;
                ScrollScrollControl(self->Scroll);
            }

            EnqueueControlCommand(CONTROLCOMMAND_UI, self->Action,
                CONTROLACTION_UI_CHANGE, DEFAULT_CONTROLACTION_UI_VALUE);

            AdjustScrollControlListControl(self);
        }

        DequeueControlCommand(TRUE);
    }

    return indx != self->Index;
}

// 0x10006970
LISTCONTROLPTR CLASSCALL ReleaseListControl(LISTCONTROLPTR self, CONST OBJECTRELEASETYPE mode)
{
    DisposeListControl(self);

    if (mode & OBJECTRELEASETYPE_ALLOCATED) { free(self); }

    return self;
}

// 0x10006990
VOID CLASSCALL DisposeListControl(LISTCONTROLPTR self)
{
    self->Self = (LISTCONTROLSELFPTR)&ControlSelfState;

    if (self->Items != NULL)
    {
        ReleaseStringList(self->Items);

        free(self->Items);
    }

    if (self->Scroll != NULL) { self->Scroll->Self->Release(self->Scroll, OBJECTRELEASETYPE_ALLOCATED); }
}

// 0x10006ab0
BOOL CLASSCALL SelectListControlItem(LISTCONTROLPTR self, CONST S32 indx)
{
    if (INVALID_LIST_CONTROL_INDEX < indx && indx < self->Items->Count)
    {
        self->Index = indx;

        return TRUE;
    }

    return FALSE;
}

// 0x10006ea0
VOID CLASSCALL AdjustScrollControlListControl(LISTCONTROLPTR self)
{
    if (!self->IsActive) { return; }

    self->Scroll->Count = self->Items->Count;

    ScrollScrollControl(self->Scroll);

    if (self->Items->Count <= self->Index) { self->Index = INVALID_LIST_CONTROL_INDEX; }

    if (self->Items->Count == 1)
    {
        EnqueueControlCommand(CONTROLCOMMAND_UI, self->Action,
            CONTROLACTION_UI_CHANGE, DEFAULT_CONTROLACTION_UI_VALUE);

        self->Index = 0;
    }

    if (self->Items->Count < self->Scroll->Current + self->Scroll->Visible)
    {
        CONST S32 indx = self->Items->Count - self->Scroll->Visible;
        self->Scroll->Current = indx < 0 ? 0 : indx;

        ScrollScrollControl(self->Scroll);
    }
}