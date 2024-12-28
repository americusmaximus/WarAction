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

#include "ButtonControl.hxx"
#include "ControlCommand.hxx"
#include "ObjectType2x38.hxx"
#include "ObjectType2x39.hxx"
#include "VideoControl.hxx"

// 0x1003a2a8
CONTROLTYPE2X39SELF ObjectType2x39SelfState =
{
    (CONTROLTYPE2X39TYPEACTION)AcquireObjectType2,
    InitializeObjectType2x39,
    (CONTROLTYPE2X39DISABLEACTION)DisableButtonControl,
    (CONTROLTYPE2X39TICKACTION)TickButtonControl,
    ActionObjectType2x39,
    (CONTROLTYPE2X39RELEASEACTION)ReleaseVideoControl
};

// 0x10001fa0
CONTROLTYPE2X39PTR CLASSCALL ActivateObjectType2x39(CONTROLTYPE2X39PTR self, BINASSETPTR asset, CONST U32 indx, CONST U32 action)
{
    ActivateObjectType2x38((CONTROLTYPE2X38PTR)self, asset, indx, action);

    self->Self = &ObjectType2x39SelfState;

    self->Unk0x39 = FALSE;

    return self;
}

// 0x10001fd0
VOID CLASSCALL InitializeObjectType2x39(CONTROLTYPE2X39PTR self)
{
    InitializeObjectType2x38((CONTROLTYPE2X38PTR)self);

    self->Unk0x39 = FALSE;
}

// 0x10001fe0
U32 CLASSCALL ActionObjectType2x39(CONTROLTYPE2X39PTR self)
{
    U32 result = ActionObjectType2x38((CONTROLTYPE2X38PTR)self);

    if (result != CONTROLACTION_NONE) { self->IsAction = TRUE; }

    CONTROLCOMMAND command;
    if (DequeueControlCommand(&command, FALSE) && command.Command == CONTROLCOMMAND_UI
        && command.Action == self->Action && command.Parameter1 == 6 /* TODO */)
    {
        self->Unk0x39 = self->Unk0x39 == 0; // TODO
        self->IsAction = FALSE;

        DequeueControlCommand(TRUE);
        EnqueueControlCommand(CONTROLCOMMAND_UI, self->Action, 2, 0); // TODO
    }

    return result;
}