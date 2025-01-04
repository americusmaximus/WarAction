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

#include "ButtonControl.hxx"
#include "ControlCommand.hxx"
#include "SwapControl.hxx"
#include "VideoControl.hxx"

// 0x1003a2a8
SWAPCONTROLSELF SwapControlSelfState =
{
    (CONTROLTYPE2X39TYPEACTION)AcquireControlTypeToggle,
    InitializeSwapControl,
    (CONTROLTYPE2X39DISABLEACTION)DisableButtonControl,
    (CONTROLTYPE2X39TICKACTION)TickButtonControl,
    ActionSwapControl,
    (CONTROLTYPE2X39RELEASEACTION)ReleaseVideoControl
};

// 0x10001fa0
SWAPCONTROLPTR CLASSCALL ActivateSwapControl(SWAPCONTROLPTR self, BINASSETPTR asset, CONST U32 indx, CONST U32 action)
{
    ActivateToggleControl((TOGGLECONTROLPTR)self, asset, indx, action);

    self->Self = &SwapControlSelfState;

    self->IsToggle = FALSE;

    return self;
}

// 0x10001fd0
VOID CLASSCALL InitializeSwapControl(SWAPCONTROLPTR self)
{
    InitializeToggleControl((TOGGLECONTROLPTR)self);

    self->IsToggle = FALSE;
}

// 0x10001fe0
U32 CLASSCALL ActionSwapControl(SWAPCONTROLPTR self)
{
    U32 result = ActionToggleControl((TOGGLECONTROLPTR)self);

    if (result != CONTROLACTION_NONE) { self->IsAction = TRUE; }

    CONTROLCOMMAND command;
    if (DequeueControlCommand(&command, FALSE) && command.Command == CONTROLCOMMAND_UI
        && command.Action == self->Action && command.Parameter1 == CONTROLACTION_UI_TOGGLE)
    {
        self->IsToggle = ~self->IsToggle;
        self->IsAction = FALSE;

        DequeueControlCommand(TRUE);
        EnqueueControlCommand(CONTROLCOMMAND_UI, self->Action,
            CONTROLACTION_UI_CHANGE, DEFAULT_CONTROLACTION_UI_VALUE);
    }

    return result;
}