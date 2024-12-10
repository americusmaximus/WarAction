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

#include "Control.hxx"

#include <stdlib.h>

CONTROLSTATE        ControlState;
CONTROLACTIONQUEUE  ControlActionQueueState;

// 0x1003a248
CONTROLSELF ControlSelfState =
{
    (CONTROLTYPEACTION)AcquireControlTypeControl,
    InitializeControl,
    DisableControl,
    TickControl,
    ActionControl,
    ReleaseControl
};

// 0x10001590
CONTROLTYPE CLASSCALL AcquireControlTypeControl(LPVOID) { return CONTROLTYPE_CONTROL; }

// 0x10001b60
CONTROLTYPE CLASSCALL AcquireControlTypeButton(LPVOID) { return CONTROLTYPE_BUTTON; }

// 0x10001e90
CONTROLTYPE CLASSCALL AcquireObjectType2(LPVOID) { return CONTROLTYPE_2; } // TODO

// 0x10001ae0
CONTROLTYPE CLASSCALL AcquireControlTypeImage(LPVOID) { return CONTROLTYPE_IMAGE; }

// 0x10002170
CONTROLTYPE CLASSCALL AcquireControlTypePanel(LPVOID) { return CONTROLTYPE_PANEL; }

// 0x100062a0
CONTROLTYPE CLASSCALL AcquireControlTypeScroll(LPVOID) { return CONTROLTYPE_SCROLL; }

// 0x100030d0
CONTROLTYPE CLASSCALL AcquireControlTypeList(LPVOID) { return CONTROLTYPE_LIST; }

// 0x10014d50
CONTROLTYPE CLASSCALL AcquireControlTypeLoadSave(LPVOID) { return CONTROLTYPE_LOAD_SAVE; }

// 0x10011c00
CONTROLTYPE CLASSCALL AcquireControlTypeDial(LPVOID) { return CONTROLTYPE_DIAL; }

// 0x10003f50
CONTROLTYPE CLASSCALL AcquireControlTypeInput(LPVOID) { return CONTROLTYPE_INPUT; }

// 0x10012050
CONTROLTYPE CLASSCALL AcquireControlTypeMultiSelect(LPVOID) { return CONTROLTYPE_MULTI_SELECT; }

// 0x100123e0
CONTROLTYPE CLASSCALL AcquireControlTypeMultiJoin(LPVOID) { return CONTROLTYPE_MULTI_JOIN; }

// 0x10013530
CONTROLTYPE CLASSCALL AcquireControlTypeMain(LPVOID) { return CONTROLTYPE_MAIN; }

// 0x10010450
CONTROLTYPE CLASSCALL AcquireControlTypeMultiStart(LPVOID) { return CONTROLTYPE_MULTI_START; }

// 0x100125e0
CONTROLTYPE CLASSCALL AcquireControlTypeMultiJoinResult(LPVOID) { return CONTROLTYPE_MULTI_JOIN_RESULT; }

// 0x10004df0
CONTROLTYPE CLASSCALL AcquireControlTypeMessage(LPVOID) { return CONTROLTYPE_MESSAGE; }

// 0x10001a00
VOID CLASSCALL InitializeControl(CONTROLPTR self)
{
    self->IsActive = TRUE;
}

// 0x10001a10
VOID CLASSCALL DisableControl(CONTROLPTR self)
{
    self->IsActive = FALSE;
}

// 0x100019f0
VOID CLASSCALL TickControl(CONTROLPTR self) { }

// 0x10001a20
U32 CLASSCALL ActionControl(CONTROLPTR self) { return CONTROLACTION_NONE; }

// 0x100015a0
CONTROLPTR CLASSCALL ReleaseControl(CONTROLPTR self, CONST OBJECTRELEASETYPE mode)
{
    self->Self = &ControlSelfState;

    if (mode & OBJECTRELEASETYPE_ALLOCATED) { free(self); }

    return self;
}

// 0x10016120
VOID CLASSCALL DisposeControl(CONTROLPTR self)
{
    self->Self = &ControlSelfState;
}

// 0x100136d0
VOID EnqueueControlActionQueue(CONST U32 value)
{
    ControlActionQueueState.Items[ControlActionQueueState.Count] = value;
    ControlActionQueueState.Count = ControlActionQueueState.Count + 1;
}

// 0x100136b0
U32 DequeueControlActionQueue(VOID)
{
    ControlActionQueueState.Count = ControlActionQueueState.Count - 1;

    return ControlActionQueueState.Items[ControlActionQueueState.Count];
}