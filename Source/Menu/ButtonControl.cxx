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
#include "ButtonControl.hxx"
#include "ControlCommand.hxx"
#include "Cursor.hxx"
#include "Settings.hxx"
#include "Shortcuts.hxx"
#include "Sound.hxx"
#include "State.hxx"
#include "VideoControl.hxx"

#define CONTROLCOMMANDACTION_BUTTON_ACTION  (CONTROLCOMMANDACTION_MOUSE_LEFT_UP | CONTROLCOMMANDACTION_MOUSE_LEFT_DOWN | CONTROLCOMMANDACTION_MOUSE_LEAVE | CONTROLCOMMANDACTION_MOUSE_ENTER)
#define CONTROLCOMMANDACTION_BUTTON_CLICK   (CONTROLCOMMANDACTION_MOUSE_LEFT_UP | CONTROLCOMMANDACTION_MOUSE_LEFT_DOWN | CONTROLCOMMANDACTION_MOUSE_ENTER)

// 0x1003a278
BUTTONCONTROLSELF ButtonControlSelfState =
{
    (BUTTONCONTROLTYPEACTION)AcquireControlTypeButton,
    InitializeButtonControl,
    DisableButtonControl,
    TickButtonControl,
    ActionButtonControl,
    (BUTTONCONTROLRELEASEACTION)ReleaseVideoControl
};

// 0x10001b20
BUTTONCONTROLPTR CLASSCALL ActivateButtonControl(BUTTONCONTROLPTR self, BINASSETPTR asset, CONST U32 indx, CONST U32 action)
{
    self->Self = &ButtonControlSelfState;

    self->IsActive = FALSE;

    self->Shortcut = SHORTCUT_NONE;

    self->Click = NULL;
    self->Unclick = NULL;

    self->Asset = asset;
    self->Index = indx;
    self->Action = action;

    self->X = 5;
    self->Y = 15;

    return self;
}

// 0x10001b70
VOID CLASSCALL InitializeButtonControl(BUTTONCONTROLPTR self)
{
    InitializeControl((CONTROLPTR)self);

    self->IsVisible = TRUE;
    self->Unk11 = TRUE;
    self->IsAction = FALSE;

    CONST IMAGESPRITEPTR image = (IMAGESPRITEPTR)AcquireBinAssetContent(self->Asset, self->Index);

    ActivateActionArea(self->X * image->Width / 100 + image->X,
        self->Y * image->Height / 100 + image->Y,
        ((50 - self->X) * image->Width * 2) / 100, ((50 - self->Y) * image->Height * 2) / 100,
        CONTROLCOMMANDACTION_BUTTON_ACTION, self->Action, DEFAULT_ACTION_PRIORITY);

    self->Unk12 = 0; // TODO
    self->Shortcut = AcquireShortcut(&ShortcutsState.State, self->Action);
}

// 0x10001c50
VOID CLASSCALL DisableButtonControl(BUTTONCONTROLPTR self)
{
    DisableControl((CONTROLPTR)self);
    DequeueControlCommand(self->Action);
}

// 0x10001e10
VOID CLASSCALL TickButtonControl(BUTTONCONTROLPTR self)
{
    if (self->IsVisible)
    {
        State.Renderer->Actions.DrawMainSurfaceSprite(0, 0,
            (IMAGESPRITEPTR)AcquireBinAssetContent(self->Asset, self->IsAction + self->Index));
    }
}

// 0x10001c60
U32 CLASSCALL ActionButtonControl(BUTTONCONTROLPTR self)
{
    if (!self->IsVisible)
    {
        self->IsAction = FALSE;

        return CONTROLACTION_NONE;
    }

    if (!self->Unk11) { return CONTROLACTION_NONE; }

    CONST U8 action = self->IsAction;

    U32 result = CONTROLACTION_NONE;

    CONTROLCOMMAND command;
    if (DequeueControlCommand(&command, FALSE))
    {
        if (command.Command == self->Action)
        {
            if (command.Action & CONTROLCOMMANDACTION_BUTTON_CLICK) { self->IsAction = CursorState.IsLeft; }
            else if (command.Action & CONTROLCOMMANDACTION_MOUSE_LEAVE) { self->IsAction = FALSE; }

            result = (!action || !(command.Action & CONTROLCOMMANDACTION_MOUSE_LEFT_UP))
                ? CONTROLACTION_NONE : CONTROLACTION_1;

            DequeueControlCommand(TRUE);
        }
        else if (command.Command == CONTROLCOMMAND_UTF && !ControlState.Active)
        {
            if (command.Action == self->Shortcut + CONTROLCOMMANDACTION_UTF_KEY_DOWN)
            {
                self->IsAction = TRUE;

                DequeueControlCommand(TRUE);
            }

            if (command.Action == self->Shortcut + CONTROLCOMMANDACTION_UTF_KEY_UP)
            {
                self->IsAction = FALSE;
                result = CONTROLACTION_1;

                DequeueControlCommand(TRUE);
            }
        }
    }

    if (self->Unk12 != 0)
    {
        if (!self->IsAction) { self->Ticks = 0; }
        else if (!action)
        {
            self->Ticks = GetTickCount();
            self->Unk0x2c = 0;
        }
        else
        {
            CONST U32 ticks = GetTickCount();

            if (ticks - self->Ticks < (self->Unk0x2c == 0 ? 500 : 100))
            {
                if (action != self->IsAction)
                {
                    if (self->IsAction) { EnqueueControlCommand(CONTROLCOMMAND_UI, self->Action, 3, 0); } // TODO

                    PlaySoundStateSound(&SoundState.State, self->IsAction ? self->Click : self->Unclick);
                }

                return CONTROLACTION_NONE;
            }

            self->Ticks = ticks;
            self->Unk0x2c = self->Unk0x2c + 1;
        }
    }

    if (result != CONTROLACTION_NONE) { EnqueueControlCommand(CONTROLCOMMAND_UI, self->Action, 4, 0); } // TODO

    if (action != self->IsAction)
    {
        if (self->IsAction) { EnqueueControlCommand(CONTROLCOMMAND_UI, self->Action, 3, 0); } // TODO

        PlaySoundStateSound(&SoundState.State, self->IsAction ? self->Click : self->Unclick);
    }

    return result;
}

// 0x10001df0
VOID CLASSCALL SelectButtonControlVisibility(BUTTONCONTROLPTR self, CONST BOOL visible)
{
    if (visible != self->IsVisible)
    {
        self->IsVisible = visible;
        self->IsAction = FALSE;
    }
}