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

#include "BinAsset.hxx"
#include "ControlCommand.hxx"
#include "RadioControl.hxx"
#include "Sound.hxx"

#include <stdlib.h>

// 0x1003a2f0
RADIOCONTROLSELF RadioControlSelfState =
{
    (CONTROLTYPE4X12TYPEACTION)AcquireControlTypePanel,
    (CONTROLTYPE4X12INITIALIZEACTION)InitializePanelControl,
    (CONTROLTYPE4X12DISABLEACTION)DisablePanelControl,
    (CONTROLTYPE4X12TICKACTION)TickPanelControl,
    ActionRadioControl,
    (CONTROLTYPE4X12RELEASEACTION)ReleasePanelControl
};

// 0x10002300
RADIOCONTROLPTR CLASSCALL ActivateRadioControl(RADIOCONTROLPTR self, CONST U32 action, BINASSETPTR asset, CONST U8 sound)
{
    self->Self = &RadioControlSelfState;

    self->IsActive = FALSE;
    self->Nodes = NULL;
    self->IsSound = sound;
    self->Asset = asset;
    self->Action = action;

    CONST U32 count = AcquireBinAssetItemCount(asset) / 2;

    for (U32 x = 0; x < count; x++)
    {
        TOGGLECONTROLPTR control =
            ActivateToggleControl(ALLOCATE(TOGGLECONTROL), asset, x * 2, action);

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
U32 CLASSCALL ActionRadioControl(RADIOCONTROLPTR self)
{
    ActionPanelControl((PANELCONTROLPTR)self);

    CONST CONTROLCOMMANDPTR command = DequeueControlCommand(FALSE);

    if (command != NULL && command->Command == CONTROLCOMMAND_UI
        && command->Action == self->Action && command->Parameter1 == CONTROLACTION_UI_CLICK)
    {
        if (!SelectRadioControlItem(self, command->Parameter2 / 2)) { DequeueControlCommand(TRUE); }
    }

    return CONTROLACTION_NONE;
}

// 0x10002430
// 0x10002490
BOOL CLASSCALL SelectRadioControlItem(RADIOCONTROLPTR self, CONST S32 indx)
{
    if (!self->IsSound)
    {
        TOGGLECONTROLPTR control =
            (TOGGLECONTROLPTR)AcquirePanelControlNode((PANELCONTROLPTR)self, indx);

        control->IsAction = ~control->IsAction;

        PlaySoundStateSound(&SoundState.State, control->Click);

        return TRUE;
    }

    BOOL result = FALSE;

    CONST U32 count = AcquireBinAssetItemCount(self->Asset) / 2;

    for (U32 x = 0; x < count; x++)
    {
        TOGGLECONTROLPTR control =
            (TOGGLECONTROLPTR)AcquirePanelControlNode((PANELCONTROLPTR)self, x);

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
VOID CLASSCALL SelectRadioControlPosition(RADIOCONTROLPTR self, CONST S32 x, CONST S32 y)
{
    CONST U32 count = AcquireBinAssetItemCount(self->Asset) / 2;

    for (U32 xx = 0; xx < count; xx++)
    {
        TOGGLECONTROLPTR control =
            (TOGGLECONTROLPTR)AcquirePanelControlNode((PANELCONTROLPTR)self, xx);

        control->X = x;
        control->Y = y;
    }
}

// 0x10002440
S32 CLASSCALL AcquireRadioControlItemIndex(RADIOCONTROLPTR self)
{
    S32 result = INVALID_RADIO_ITEM_INDEX;

    CONST U32 count = AcquireBinAssetItemCount(self->Asset) / 2;

    for (U32 x = 0; x < count; x++)
    {
        CONST TOGGLECONTROLPTR control =
            (TOGGLECONTROLPTR)AcquirePanelControlNode((PANELCONTROLPTR)self, x);

        if (control->IsAction) { result = x; }
    }

    return result;
}