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

#define BUTTON_CONTROL_ACTION_PRIORITY  0x8000

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
BUTTONCONTROLPTR CLASSCALL ActivateButtonControl(BUTTONCONTROLPTR self, BINASSETPTR asset, CONST U32 index, CONST U32 action)
{
    self->Self = &ButtonControlSelfState;

    self->IsActive = FALSE;

    self->Shortcut = SHORTCUT_NONE;

    self->Click = NULL;
    self->Unclick = NULL;

    self->Asset = asset;
    self->Index = index;
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

    IMAGESPRITEPTR image = (IMAGESPRITEPTR)AcquireBinAssetContent(self->Asset, self->Index);

    ActivateActionArea(self->X * image->Width / 100 + image->X,
        self->Y * image->Height / 100 + image->Y,
        ((50 - self->X) * image->Width * 2) / 100, ((50 - self->Y) * image->Height * 2) / 100,
        0x1e /* TODO */, self->Action, BUTTON_CONTROL_ACTION_PRIORITY); // TODO

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
            if (command.Action & (CONTROLCOMMANDACTION_10 | CONTROLCOMMANDACTION_8 | CONTROLCOMMANDACTION_2)) { self->IsAction = CursorState.IsLeft; }
            if (command.Action & CONTROLCOMMANDACTION_4) { self->IsAction = FALSE; }

            result = (!action || !(command.Action & CONTROLCOMMANDACTION_10)) ? CONTROLACTION_NONE : CONTROLACTION_1; // TODO

            DequeueControlCommand(TRUE);
        }
        else if (command.Command == CONTROLCOMMAND_UTF && !ControlState.Active)
        {
            if (command.Action == self->Shortcut + 0x1000000)// TODO
            {
                self->IsAction = TRUE;

                DequeueControlCommand(TRUE);
            }

            if (command.Action == self->Shortcut + 0x2000000)// TODO
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
            if (GetTickCount() - self->Ticks < (self->Unk0x2c == 0 ? 500 : 100))
            {
                if (action != self->IsAction)
                {
                    if (self->IsAction) { EnqueueControlCommand(CONTROLCOMMAND_TEXT_CONTROL, self->Action, 3, 0); } // TODO

                    PlaySoundStateSound(&SoundState.State, self->IsAction ? self->Click : self->Unclick);
                }

                return CONTROLACTION_NONE;
            }

            self->Ticks = GetTickCount();
            self->Unk0x2c = self->Unk0x2c + 1;
        }
    }

    if (result != CONTROLACTION_NONE) { EnqueueControlCommand(CONTROLCOMMAND_TEXT_CONTROL, self->Action, 4, 0); } // TODO

    if (action != self->IsAction)
    {
        if (self->IsAction) { EnqueueControlCommand(CONTROLCOMMAND_TEXT_CONTROL, self->Action, 3, 0); } // TODO

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