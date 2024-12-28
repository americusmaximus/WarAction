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

#include "BinAsset.hxx"
#include "ControlCommand.hxx"
#include "ObjectType4x12.hxx"
#include "Sound.hxx"

#include <stdlib.h>

// 0x1003a2f0
CONTROLTYPE4X12SELF ObjectType4x12SelfState =
{
    (CONTROLTYPE4X12TYPEACTION)AcquireControlTypePanel,
    (CONTROLTYPE4X12INITIALIZEACTION)InitializePanelControl,
    (CONTROLTYPE4X12DISABLEACTION)DisablePanelControl,
    (CONTROLTYPE4X12TICKACTION)TickPanelControl,
    ActionObjectType4x12,
    (CONTROLTYPE4X12RELEASEACTION)ReleasePanelControl
};

// 0x10002300
CONTROLTYPE4X12PTR CLASSCALL ActivateObjectType4x12(CONTROLTYPE4X12PTR self, CONST U32 action, BINASSETPTR asset, CONST U8 sound)
{
    self->Self = &ObjectType4x12SelfState;

    self->IsActive = FALSE;
    self->Nodes = NULL;
    self->IsSound = sound;
    self->Asset = asset;
    self->Action = action;

    CONST U32 count = AcquireBinAssetItemCount(asset) / 2;

    for (U32 x = 0; x < count; x++)
    {
        CONTROLTYPE2X38PTR control =
            ActivateObjectType2x38(ALLOCATE(CONTROLTYPE2X38), asset, x * 2, action);

        if (self->Nodes == NULL)
        {
            self->Nodes = ALLOCATE(CONTROLNODE);
            InitControlNode(self->Nodes, (CONTROLPTR)control);
        }
        else { AppendControlNode(self->Nodes, (CONTROLPTR)control); }
    }

    return self;
}

// 0x100023e0
U32 CLASSCALL ActionObjectType4x12(CONTROLTYPE4X12PTR self)
{
    ActionPanelControl((PANELCONTROLPTR)self);

    CONST CONTROLCOMMANDPTR command = DequeueControlCommand(FALSE);

    if (command != NULL && command->Command == CONTROLCOMMAND_UI
        && command->Action == self->Action && command->Parameter1 == 4 /* TODO */)
    {
        if (!ClickObjectType4x12(self, command->Parameter2 / 2)) { DequeueControlCommand(TRUE); }
    }

    return CONTROLACTION_NONE;
}

// 0x10002430
// 0x10002490
BOOL CLASSCALL ClickObjectType4x12(CONTROLTYPE4X12PTR self, CONST S32 indx)
{
    if (!self->IsSound)
    {
        CONTROLTYPE2X38PTR control =
            (CONTROLTYPE2X38PTR)AcquirePanelControlNode((PANELCONTROLPTR)self, indx);

        control->IsAction = ~control->IsAction; // XOR 0x1 // TODO

        PlaySoundStateSound(&SoundState.State, control->Click);

        return TRUE;
    }

    BOOL result = FALSE;

    CONST U32 count = AcquireBinAssetItemCount(self->Asset) / 2;

    for (U32 x = 0; x < count; x++)
    {
        CONTROLTYPE2X38PTR control =
            (CONTROLTYPE2X38PTR)AcquirePanelControlNode((PANELCONTROLPTR)self, x);

        if (x == indx && !control->IsAction)
        {
            PlaySoundStateSound(&SoundState.State, control->Click);

            result = TRUE;
        }

        control->IsAction = x == indx;
    }

    return result;
}

// 0x10002550
VOID CLASSCALL SelectObjectType4x12XY(CONTROLTYPE4X12PTR self, CONST S32 x, CONST S32 y) // TODO
{
    CONST U32 count = AcquireBinAssetItemCount(self->Asset) / 2;

    for (U32 xx = 0; xx < count; xx++)
    {
        CONTROLTYPE2X38PTR control =
            (CONTROLTYPE2X38PTR)AcquirePanelControlNode((PANELCONTROLPTR)self, xx);

        control->X = x;
        control->Y = y;
    }
}

// 0x10002440
S32 CLASSCALL FUN_10002440(CONTROLTYPE4X12PTR self)
{
    S32 result = -1; // TODO

    CONST U32 count = AcquireBinAssetItemCount(self->Asset) / 2;

    for (U32 x = 0; x < count; x++)
    {
        CONTROLTYPE2X38PTR control =
            (CONTROLTYPE2X38PTR)AcquirePanelControlNode((PANELCONTROLPTR)self, x);

        if (control->IsAction != 0) { result = x; } // TODO
    }

    return result;
}