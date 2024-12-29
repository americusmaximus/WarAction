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

#include "BinAsset.hxx"
#include "Control.hxx"

struct ButtonControl;

typedef CONTROLTYPE(CLASSCALL* BUTTONCONTROLTYPEACTION)(ButtonControl* self);
typedef VOID(CLASSCALL* BUTTONCONTROLINITIALIZEACTION)(ButtonControl* self);
typedef VOID(CLASSCALL* BUTTONCONTROLDISABLEACTION)(ButtonControl* self);
typedef VOID(CLASSCALL* BUTTONCONTROLTICKACTION)(ButtonControl* self);
typedef U32(CLASSCALL* BUTTONCONTROLACTIONACTION)(ButtonControl* self);
typedef ButtonControl* (CLASSCALL* BUTTONCONTROLRELEASEACTION)(ButtonControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: ControlSelf
typedef struct ButtonControlSelf
{
    BUTTONCONTROLTYPEACTION         Type;
    BUTTONCONTROLINITIALIZEACTION   Initialize;
    BUTTONCONTROLDISABLEACTION      Disable;
    BUTTONCONTROLTICKACTION         Tick;
    BUTTONCONTROLACTIONACTION       Action;
    BUTTONCONTROLRELEASEACTION      Release;
} BUTTONCONTROLSELF, * BUTTONCONTROLSELFPTR;

EXTERN BUTTONCONTROLSELF ButtonControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: Control
typedef struct ButtonControl
{
    BUTTONCONTROLSELFPTR    Self;
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
    BOOL                    IsContinuous;
    U32                     Ticks;
    U32                     Iteration;
    U32                     Action;
} BUTTONCONTROL, * BUTTONCONTROLPTR;
#pragma pack(pop)

BUTTONCONTROLPTR CLASSCALL ActivateButtonControl(BUTTONCONTROLPTR self, BINASSETPTR asset, CONST U32 indx, CONST U32 action);
U32 CLASSCALL ActionButtonControl(BUTTONCONTROLPTR self);
VOID CLASSCALL DisableButtonControl(BUTTONCONTROLPTR self);
VOID CLASSCALL InitializeButtonControl(BUTTONCONTROLPTR self);
VOID CLASSCALL SelectButtonControlVisibility(BUTTONCONTROLPTR self, CONST BOOL visible);
VOID CLASSCALL TickButtonControl(BUTTONCONTROLPTR self);