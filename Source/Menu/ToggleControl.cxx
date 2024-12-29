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
#include "ToggleControl.hxx"
#include "VideoControl.hxx"

// 0x1003a290
TOGGLECONTROLSELF ToggleControlSelfState =
{
    (TOGGLECONTROLTYPEACTION)AcquireControlTypeToggle,
    InitializeToggleControl,
    (TOGGLECONTROLDISABLEACTION)DisableButtonControl,
    (TOGGLECONTROLTICKACTION)TickButtonControl,
    ActionToggleControl,
    (TOGGLECONTROLRELEASEACTION)ReleaseVideoControl
};

// 0x10001e50
TOGGLECONTROLPTR CLASSCALL ActivateToggleControl(TOGGLECONTROLPTR self, BINASSETPTR asset, CONST U32 indx, CONST U32 action)
{
    ActivateButtonControl((BUTTONCONTROLPTR)self, asset, indx, indx * 0x1000 + action); // TODO

    self->Self = &ToggleControlSelfState;

    self->Click = "switch";
    self->ActualAction = action;

    return self;
}

// 0x10001ea0
VOID CLASSCALL InitializeToggleControl(TOGGLECONTROLPTR self)
{
    InitializeButtonControl((BUTTONCONTROLPTR)self);

    self->IsClickable = TRUE;

    ReplaceActionAreaAction(self->Action,
        CONTROLCOMMANDACTION_SCROLL | CONTROLCOMMANDACTION_MOUSE_LEFT_UP | CONTROLCOMMANDACTION_MOUSE_LEFT_DOWN | CONTROLCOMMANDACTION_MOUSE_LEAVE);
}

// 0x10001ec0
U32 CLASSCALL ActionToggleControl(TOGGLECONTROLPTR self)
{
    if (!self->IsVisible || !self->IsClickable) { return CONTROLACTION_NONE; }

    U32 cVar3 = 0; // TODO

    CONTROLCOMMAND command;
    if (DequeueControlCommand(&command, FALSE))
    {
        if (command.Command == self->Action)
        {
            if (command.Action & CONTROLCOMMANDACTION_MOUSE_LEFT_DOWN) { cVar3 = 1; } // TODO

            if ((command.Action & CONTROLCOMMANDACTION_MOUSE_LEFT_UP)
                || ((command.Action & CONTROLCOMMANDACTION_MOUSE_LEAVE) && CursorState.IsLeft))
            {
                cVar3 = 2;// TODO
            }

            DequeueControlCommand(TRUE);
        }
        else if (command.Command == CONTROLCOMMAND_UTF && !ControlState.Active)
        {
            cVar3 = command.Action == self->Shortcut + CONTROLCOMMANDACTION_UTF_KEY_DOWN ? 1 : 0; // TODO

            if (command.Action == self->Shortcut + CONTROLCOMMANDACTION_UTF_KEY_UP) { cVar3 = 2; }// TODO
        }
    }

    if (cVar3 == 1 /* TODO*/)
    {
        EnqueueControlCommand(CONTROLCOMMAND_UI, self->ActualAction, 4, self->Index); // TODO

        return CONTROLACTION_1;
    }
    else if (cVar3 == 2 /* TODO*/) { EnqueueControlCommand(CONTROLCOMMAND_UI, self->ActualAction, 6 /* TODO */, self->Index); }

    return CONTROLACTION_NONE;
}