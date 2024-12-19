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
#include "ControlCommand.hxx"
#include "Cursor.hxx"
#include "ImageControl.hxx"
#include "ObjectType3x3d.hxx"
#include "VideoControl.hxx"

// 0x1003a5b4
CONTROLTYPE3X3DSELF ObjectType3x3dSelfState =
{
    (CONTROLTYPE3X3DTYPEACTION)AcquireControlTypeImage,
    InitializeObjectType3x3d,
    DisableObjectType3x3d,
    TickObjectType3x3d,
    ActionObjectType3x3d,
    (CONTROLTYPE3X3DRELEASEACTION)ReleaseVideoControl
};

// 0x10046cac
CONTROLTYPE3X3DPTR ControlType3x3dState;

// 0x1000fbc0
CONTROLTYPE3X3DPTR CLASSCALL ActivateObjectType3x3d(CONTROLTYPE3X3DPTR self, BINASSETPTR asset, CONST U32 indx, CONST U32 action, U32 param_5, U32 param_6, U32 param_7, U32 param_8, U32 param_9, U32 param_10, U32 param_11, U32 param_12)
{
    ActivateImageControl((IMAGECONTROLPTR)self, asset, indx);

    self->Self = &ObjectType3x3dSelfState;

    self->Action = action;
    self->Unk07 = param_5;
    self->Unk08 = param_6;
    self->Unk10 = param_12;
    self->Unk09 = param_11;
    self->Unk11 = param_8;
    self->Unk13 = param_10;
    self->Unk12 = param_9;
    self->Unk14 = param_7;
    self->Unk15 = 1; // TODO

    return self;
}

// 0x1000fc30
VOID CLASSCALL InitializeObjectType3x3d(CONTROLTYPE3X3DPTR self)
{
    InitializeControl((CONTROLPTR)self);

    ActivateActionArea(self->Unk11 - self->Unk09, self->Unk12 - self->Unk13,
        self->Unk10 + self->Unk09, self->Unk13 * 2,
        CONTROLCOMMANDACTION_MOUSE_LEFT_UP | CONTROLCOMMANDACTION_MOUSE_LEFT_DOWN | CONTROLCOMMANDACTION_MOUSE_ENTER | CONTROLCOMMANDACTION_MOUSE_HOVER,
        self->Action, DEFAULT_ACTION_PRIORITY);
}

// 0x1000fc70
VOID CLASSCALL DisableObjectType3x3d(CONTROLTYPE3X3DPTR self)
{
    DequeueControlCommand(self->Action);
    DisableControl((CONTROLPTR)self);
}

// 0x1000fd30
VOID CLASSCALL TickObjectType3x3d(CONTROLTYPE3X3DPTR self)
{
    self->X = ((self->Unk10 + self->Unk09) * (self->Unk14 - self->Unk07)) / (self->Unk08 - self->Unk07) - self->Unk09; // TODO
    TickImageControl((IMAGECONTROLPTR)self);
}

// 0x1000fc90
U32 CLASSCALL ActionObjectType3x3d(CONTROLTYPE3X3DPTR self)
{
    if (self->Unk15 == 0) { return CONTROLACTION_NONE; } // TODO

    CONST CONTROLCOMMANDPTR command = DequeueControlCommand(FALSE);

    if (command != NULL && command->Command == self->Action)
    {
        U32 x = CursorState.X;

        // TODO variable names...
        if (CursorState.X < self->Unk11 - self->Unk09) { x = self->Unk11 - self->Unk09; }
        if (self->Unk10 + self->Unk11 < x) { x = self->Unk10 + self->Unk11; }

        if (CursorState.IsLeft)
        {
            self->Unk14 = ((((self->Unk08 - self->Unk07) * 1000) / (self->Unk10 + self->Unk09))
                * ((self->Unk09 - self->Unk11) + x) + 500) / 1000 + self->Unk07;
        }

        DequeueControlCommand(TRUE);
    }

    return ActionControl((CONTROLPTR)self);
}