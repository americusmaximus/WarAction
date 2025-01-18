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

#pragma once

#include "ButtonControl.hxx"

struct ScrollControl;

typedef CONTROLTYPE(CLASSCALL* SCROLLCONTROLTYPEACTION)(ScrollControl* self);
typedef VOID(CLASSCALL* SCROLLCONTROLINITIALIZEACTION)(ScrollControl* self);
typedef VOID(CLASSCALL* SCROLLCONTROLDISABLEACTION)(ScrollControl* self);
typedef VOID(CLASSCALL* SCROLLCONTROLTICKACTION)(ScrollControl* self);
typedef U32(CLASSCALL* SCROLLCONTROLACTIONACTION)(ScrollControl* self);
typedef ScrollControl* (CLASSCALL* SCROLLCONTROLRELEASEACTION)(ScrollControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: ControlSelf
typedef struct ScrollControlSelf
{
    SCROLLCONTROLTYPEACTION         Type;
    SCROLLCONTROLINITIALIZEACTION   Initialize;
    SCROLLCONTROLDISABLEACTION      Disable;
    SCROLLCONTROLTICKACTION         Tick;
    SCROLLCONTROLACTIONACTION       Action;
    SCROLLCONTROLRELEASEACTION      Release;
} SCROLLCONTROLSELF, * SCROLLCONTROLSELFPTR;

EXTERN SCROLLCONTROLSELF ScrollControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: Control
typedef struct ScrollControl
{
    SCROLLCONTROLSELFPTR    Self;
    U8                      IsActive;
    U32                     Action;
    S32                     X;
    S32                     Y;
    S32                     Height;
    S32                     Count;
    S32                     Visible;
    S32                     Current;
    U8                      IsAction;
    BUTTONCONTROLPTR        Up;
    BUTTONCONTROLPTR        Down;
    BINASSETPTR             Asset;
} SCROLLCONTROL, * SCROLLCONTROLPTR;
#pragma pack(pop)

SCROLLCONTROLPTR CLASSCALL ActivateScrollControl(SCROLLCONTROLPTR self, BINASSETPTR asset, CONST U32 action);
SCROLLCONTROLPTR CLASSCALL ReleaseScrollControl(SCROLLCONTROLPTR self, CONST OBJECTRELEASETYPE mode);
BOOL CLASSCALL ActionScrollerScrollControl(SCROLLCONTROLPTR self);
S32 CLASSCALL AcquireScrollControlVerticalOffset(SCROLLCONTROLPTR self, CONST S32 offset);
U32 CLASSCALL ActionScrollControl(SCROLLCONTROLPTR self);
VOID CLASSCALL DisableScrollControl(SCROLLCONTROLPTR self);
VOID CLASSCALL DisableScrollerScrollControl(SCROLLCONTROLPTR self);
VOID CLASSCALL DisposeScrollControl(SCROLLCONTROLPTR self);
VOID CLASSCALL InitializeScrollControl(SCROLLCONTROLPTR self);
VOID CLASSCALL InitializeScrollControlArea(SCROLLCONTROLPTR self, CONST S32 x, CONST S32 offsetUp, CONST S32 offsetDown);
VOID CLASSCALL InitializeScrollControlArea(SCROLLCONTROLPTR self, CONST U32 action, CONST S32 x, CONST S32 y, CONST S32 height);
VOID CLASSCALL InitializeScrollerScrollControl(SCROLLCONTROLPTR self);
VOID CLASSCALL ScrollScrollControl(SCROLLCONTROLPTR self);
VOID CLASSCALL TickScrollControl(SCROLLCONTROLPTR self);
VOID CLASSCALL TickScrollerScrollControl(SCROLLCONTROLPTR self);

BOOL AcquireScrollControlActionArea(CONST U32 action, CONST S32 x, CONST S32 y, CONST S32 width, CONST S32 height);