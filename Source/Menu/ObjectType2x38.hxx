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

#include "Control.hxx"

struct ControlType2x38;

typedef CONTROLTYPE(CLASSCALL* CONTROLTYPE2X38TYPEACTION)(ControlType2x38* self);
typedef VOID(CLASSCALL* CONTROLTYPE2X38INITIALIZEACTION)(ControlType2x38* self);
typedef VOID(CLASSCALL* CONTROLTYPE2X38DISABLEACTION)(ControlType2x38* self);
typedef VOID(CLASSCALL* CONTROLTYPE2X38TICKACTION)(ControlType2x38* self);
typedef U32(CLASSCALL* CONTROLTYPE2X38ACTIONACTION)(ControlType2x38* self);
typedef ControlType2x38* (CLASSCALL* CONTROLTYPE2X38RELEASEACTION)(ControlType2x38* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: ControlSelf
typedef struct ControlType2x38Self
{
    CONTROLTYPE2X38TYPEACTION Type;
    CONTROLTYPE2X38INITIALIZEACTION Initialize;
    CONTROLTYPE2X38DISABLEACTION Disable;
    CONTROLTYPE2X38TICKACTION Tick;
    CONTROLTYPE2X38ACTIONACTION Action;
    CONTROLTYPE2X38RELEASEACTION Release;
} CONTROLTYPE2X38SELF, * CONTROLTYPE2X38SELFPTR;

EXTERN CONTROLTYPE2X38SELF ObjectType2x38SelfState;

#pragma pack(push, 1)
// INHERITANCE: ButtonControl
typedef struct ControlType2x38
{
    CONTROLTYPE2X38SELFPTR  Self;
    U8                      IsActive;
    BINASSETPTR             Asset;
    U32                     Index;
    S32                     Shortcut;
    S32                     X;
    S32                     Y;
    LPCSTR                  Click;
    LPCSTR                  Unclick;
    U8                      IsAction;
    U8                      IsVisible;
    U8                      IsClickable;
    U32                     Unk12; // TODO
    U32                     Ticks;
    U32                     Unk0x2c; // TODO
    U32                     Action;
    U32                     ActualAction;
} CONTROLTYPE2X38, * CONTROLTYPE2X38PTR;
#pragma pack(pop)

CONTROLTYPE2X38PTR CLASSCALL ActivateObjectType2x38(CONTROLTYPE2X38PTR self, BINASSETPTR asset, CONST U32 indx, CONST U32 action);
VOID CLASSCALL InitializeObjectType2x38(CONTROLTYPE2X38PTR self);
U32 CLASSCALL ActionObjectType2x38(CONTROLTYPE2X38PTR self);