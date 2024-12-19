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

#include "ControlCommand.hxx"
#include "TimeControl.hxx"

#include <stdlib.h>

#define MAX_TIME_CONTROL_INPUT_LENGTH           3
#define MAX_TIME_CONTROL_INPUT_MESSAGE_LENGTH   12

// 0x1003a360
TIMECONTROLSELF TimeControlSelfState =
{
    (TIMECONTROLTYPEACTION)AcquireControlTypeControl,
    InitializeTimeControl,
    DisableTimeControl,
    TickTimeControl,
    ActionTimeControl,
    ReleaseTimeControl
};

// 0x100048a0
TIMECONTROLPTR CLASSCALL ActivateTimeControl(TIMECONTROLPTR self, CONST U32 action, CONST S32 x, CONST S32 y, CONST S32 width, CONST S32 height, FONTASSETPTR font)
{
    self->Self = &TimeControlSelfState;

    self->IsActive = FALSE;

    self->Action = action;

    self->X = x;
    self->Y = y;
    self->Width = width;
    self->Height = height;

    self->Font = font;

    self->Minutes = ActivateInputControl(ALLOCATE(INPUTCONTROL),
        self->Action + 1, self->X, self->Y, self->Width * 2 / 5, self->Height,
        self->Font, MAX_TIME_CONTROL_INPUT_LENGTH);

    self->Seconds = ActivateInputControl(ALLOCATE(INPUTCONTROL),
        self->Action + 2, self->X + self->Width * 3 / 5, self->Y, self->Width * 2 / 5, self->Height,
        self->Font, MAX_TIME_CONTROL_INPUT_LENGTH);

    self->Minutes->Alignment = FONTALIGNMENT_CENTER;
    self->Seconds->Alignment = FONTALIGNMENT_CENTER;

    return self;
}

// 0x10004b90
VOID CLASSCALL InitializeTimeControl(TIMECONTROLPTR self)
{
    self->Minutes->Self->Initialize(self->Minutes);
    self->Seconds->Self->Initialize(self->Seconds);

    InitializeInputControlAllowItems(self->Minutes, "30-39");
    InitializeInputControlAllowItems(self->Seconds, "30-39");

    SelectTimeControlValue(self, 0);
}

// 0x10004bf0
VOID CLASSCALL DisableTimeControl(TIMECONTROLPTR self)
{
    self->Minutes->Self->Disable(self->Minutes);
    self->Seconds->Self->Disable(self->Seconds);
}

// 0x10004c10
VOID CLASSCALL TickTimeControl(TIMECONTROLPTR self)
{
    self->Minutes->Self->Tick(self->Minutes);
    self->Seconds->Self->Tick(self->Seconds);

    SelectFontAssetColor(self->Font, 0xC8, 0xC8, 0xC8);

    DrawFontAssetText(self->Font, self->X + self->Width / 2,
        (self->Height - AcquireFontAssetHeight(self->Font)) / 2 + self->Y, ":");
}

// 0x10004c80
U32 CLASSCALL ActionTimeControl(TIMECONTROLPTR self)
{
    self->Minutes->Self->Action(self->Minutes);
    self->Seconds->Self->Action(self->Seconds);

    CONST CONTROLCOMMANDPTR command = DequeueControlCommand(FALSE);

    if (command != NULL && command->Command == CONTROLCOMMAND_TEXT_CONTROL)
    {
        if (command->Action == self->Action + 1 && command->Parameter1 == 5 /* TODO */)
        {
            SelectTimeControlFocus(self, TIMECONTROLFOCUS_SECONDS);

            DequeueControlCommand(TRUE);
            EnqueueControlCommand(CONTROLCOMMAND_TEXT_CONTROL, self->Action, 2, 0); // TODO
        }
        else if (command->Action == self->Action + 2 && command->Parameter1 == 5 /* TODO */)
        {
            SelectTimeControlFocus(self, TIMECONTROLFOCUS_NONE);

            DequeueControlCommand(TRUE);
            EnqueueControlCommand(CONTROLCOMMAND_TEXT_CONTROL, self->Action, 2, 0); // TODO
        }
    }

    return CONTROLACTION_NONE;
}

// 0x100049f0
TIMECONTROLPTR CLASSCALL ReleaseTimeControl(TIMECONTROLPTR self, CONST OBJECTRELEASETYPE mode)
{
    DisposeTimeControl(self);

    if (mode & OBJECTRELEASETYPE_ALLOCATED) { free(self); }

    return self;
}

// 0x10004a10
VOID CLASSCALL DisposeTimeControl(TIMECONTROLPTR self)
{
    if (self->Minutes != NULL) { self->Minutes->Self->Release(self->Minutes, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->Seconds != NULL) { self->Seconds->Self->Release(self->Seconds, OBJECTRELEASETYPE_ALLOCATED); }

    self->Self = (TIMECONTROLSELFPTR)&ControlSelfState;
}

// 0x10004ad0
VOID CLASSCALL SelectTimeControlValue(TIMECONTROLPTR self, CONST U32 seconds)
{
    CHAR value[MAX_TIME_CONTROL_INPUT_MESSAGE_LENGTH];

    wsprintfA(value, "%02d", (seconds / 60) % 60);
    SelectInputControlValue(self->Minutes, value);

    wsprintfA(value, "%02d", seconds % 60);
    SelectInputControlValue(self->Seconds, value);
}

// 0x10004a70
VOID CLASSCALL SelectTimeControlFocus(TIMECONTROLPTR self, CONST TIMECONTROLFOCUS control)
{
    SelectTimeControlValue(self, AcquireTimeControlSeconds(self));

    if (control == TIMECONTROLFOCUS_MINUTES) { SelectInputControlFocus(self->Minutes); }
    else if (control == TIMECONTROLFOCUS_SECONDS) { SelectInputControlFocus(self->Seconds); }
}

// 0x10004b40
U32 CLASSCALL AcquireTimeControlSeconds(TIMECONTROLPTR self)
{
    S32 minutes = atoi(AcquireInputControlValue(self->Minutes));
    S32 seconds = atoi(AcquireInputControlValue(self->Seconds));

    if (minutes < 0 || 59 < minutes) { minutes = 0; }

    if (seconds < 0 || 59 < seconds) { seconds = 0; }

    return minutes * 60 + seconds;
}

// 0x10004ab0
VOID CLASSCALL SelectTimeControlModifiable(TIMECONTROLPTR self, CONST BOOL modifiable)
{
    self->Minutes->IsModifiable = modifiable;
    self->Seconds->IsModifiable = modifiable;
}