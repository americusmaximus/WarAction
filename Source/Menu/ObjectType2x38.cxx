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
#include "ObjectType2x38.hxx"
#include "VideoControl.hxx"

// 0x1003a290
CONTROLTYPE2X38SELF ObjectType2x38SelfState =
{
    (CONTROLTYPE2X38TYPEACTION)AcquireObjectType2,
    InitializeObjectType2x38,
    (CONTROLTYPE2X38DISABLEACTION)DisableButtonControl,
    (CONTROLTYPE2X38TICKACTION)TickButtonControl,
    ActionObjectType2x38,
    (CONTROLTYPE2X38RELEASEACTION)ReleaseVideoControl
};

// 0x10001e50
CONTROLTYPE2X38PTR CLASSCALL ActivateObjectType2x38(CONTROLTYPE2X38PTR self, BINASSETPTR asset, CONST U32 indx, CONST U32 action)
{
    ActivateButtonControl((BUTTONCONTROLPTR)self, asset, indx, indx * 0x1000 + action); // TODO

    self->Self = &ObjectType2x38SelfState;

    self->Click = "switch";
    self->ActualAction = action;

    return self;
}

// 0x10001ea0
VOID CLASSCALL InitializeObjectType2x38(CONTROLTYPE2X38PTR self)
{
    InitializeButtonControl((BUTTONCONTROLPTR)self);

    self->Unk11 = 1; // TODO

    FUN_10022580(self->Action, CONTROLACTION_JMULTI2_CANCEL);
}

// 0x10001ec0
U32 CLASSCALL ActionObjectType2x38(CONTROLTYPE2X38PTR self)
{
    if (!self->IsVisible || self->Unk11 == 0 /* TODO */) { return CONTROLACTION_NONE; }

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
            cVar3 = command.Action == self->Shortcut + 0x1000000 ? 1 : 0; // TODO

            if (command.Action == self->Shortcut + 0x2000000) { cVar3 = 2; }// TODO
        }
    }

    if (cVar3 == 1 /* TODO*/)
    {
        EnqueueControlCommand(CONTROLCOMMAND_TEXT_CONTROL, self->ActualAction, 4, self->Index); // TODO

        return CONTROLACTION_1;
    }
    else if (cVar3 == 2 /* TODO*/) { EnqueueControlCommand(CONTROLCOMMAND_TEXT_CONTROL, self->ActualAction, 6 /* TODO */, self->Index); }

    return CONTROLACTION_NONE;
}