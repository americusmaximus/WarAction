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
#include "ObjectType2x38.hxx"

#define INVALID_OBJECTTYPE4X12_INDEX    (-1) /* TODO */

struct ControlType4x12;

typedef CONTROLTYPE(CLASSCALL* CONTROLTYPE4X12TYPEACTION)(ControlType4x12* self);
typedef VOID(CLASSCALL* CONTROLTYPE4X12INITIALIZEACTION)(ControlType4x12* self);
typedef VOID(CLASSCALL* CONTROLTYPE4X12DISABLEACTION)(ControlType4x12* self);
typedef VOID(CLASSCALL* CONTROLTYPE4X12TICKACTION)(ControlType4x12* self);
typedef U32(CLASSCALL* CONTROLTYPE4X12ACTIONACTION)(ControlType4x12* self);
typedef ControlType4x12* (CLASSCALL* CONTROLTYPE4X12RELEASEACTION)(ControlType4x12* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: PanelControlSelf
typedef struct ControlType4x12Self
{
    CONTROLTYPE4X12TYPEACTION Type;
    CONTROLTYPE4X12INITIALIZEACTION Initialize;
    CONTROLTYPE4X12DISABLEACTION Disable;
    CONTROLTYPE4X12TICKACTION Tick;
    CONTROLTYPE4X12ACTIONACTION Action;
    CONTROLTYPE4X12RELEASEACTION Release;
} CONTROLTYPE4X12SELF, * CONTROLTYPE4X12SELFPTR;

EXTERN CONTROLTYPE4X12SELF ObjectType4x12SelfState;

#pragma pack(push, 1)
// INHERITANCE: PanelControl
typedef struct ControlType4x12
{
    CONTROLTYPE4X12SELFPTR  Self;
    U8                      IsActive;
    CONTROLNODEPTR          Nodes;
    U8                      IsSound;
    BINASSETPTR             Asset;
    U32                     Action;
} CONTROLTYPE4X12, * CONTROLTYPE4X12PTR;
#pragma pack(pop)

CONTROLTYPE4X12PTR CLASSCALL ActivateObjectType4x12(CONTROLTYPE4X12PTR self, CONST U32 action, BINASSETPTR asset, CONST U8 sound);
U32 CLASSCALL ActionObjectType4x12(CONTROLTYPE4X12PTR self);

BOOL CLASSCALL ClickObjectType4x12(CONTROLTYPE4X12PTR self, CONST S32 indx);
VOID CLASSCALL SelectObjectType4x12XY(CONTROLTYPE4X12PTR self, CONST S32 x, CONST S32 y);
S32 CLASSCALL FUN_10002440(CONTROLTYPE4X12PTR self); // TODO