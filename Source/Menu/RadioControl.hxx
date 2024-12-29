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

#pragma once

#include "PanelControl.hxx"
#include "ToggleControl.hxx"

#define INVALID_RADIO_ITEM_INDEX    (-1)

struct RadioControl;

typedef CONTROLTYPE(CLASSCALL* CONTROLTYPE4X12TYPEACTION)(RadioControl* self);
typedef VOID(CLASSCALL* CONTROLTYPE4X12INITIALIZEACTION)(RadioControl* self);
typedef VOID(CLASSCALL* CONTROLTYPE4X12DISABLEACTION)(RadioControl* self);
typedef VOID(CLASSCALL* CONTROLTYPE4X12TICKACTION)(RadioControl* self);
typedef U32(CLASSCALL* CONTROLTYPE4X12ACTIONACTION)(RadioControl* self);
typedef RadioControl* (CLASSCALL* CONTROLTYPE4X12RELEASEACTION)(RadioControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: PanelControlSelf
typedef struct RadioControlSelf
{
    CONTROLTYPE4X12TYPEACTION Type;
    CONTROLTYPE4X12INITIALIZEACTION Initialize;
    CONTROLTYPE4X12DISABLEACTION Disable;
    CONTROLTYPE4X12TICKACTION Tick;
    CONTROLTYPE4X12ACTIONACTION Action;
    CONTROLTYPE4X12RELEASEACTION Release;
} RADIOCONTROLSELF, * RADIOCONTROLSELFPTR;

EXTERN RADIOCONTROLSELF RadioControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: PanelControl
typedef struct RadioControl
{
    RADIOCONTROLSELFPTR Self;
    U8                  IsActive;
    CONTROLNODEPTR      Nodes;
    U8                  IsSound;
    BINASSETPTR         Asset;
    U32                 Action;
} RADIOCONTROL, * RADIOCONTROLPTR;
#pragma pack(pop)

RADIOCONTROLPTR CLASSCALL ActivateRadioControl(RADIOCONTROLPTR self, CONST U32 action, BINASSETPTR asset, CONST U8 sound);
BOOL CLASSCALL SelectRadioControlItem(RADIOCONTROLPTR self, CONST S32 indx);
S32 CLASSCALL AcquireRadioControlItemIndex(RADIOCONTROLPTR self);
U32 CLASSCALL ActionRadioControl(RADIOCONTROLPTR self);
VOID CLASSCALL SelectRadioControlPosition(RADIOCONTROLPTR self, CONST S32 x, CONST S32 y);