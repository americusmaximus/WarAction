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

struct ToggleControl;

typedef CONTROLTYPE(CLASSCALL* TOGGLECONTROLTYPEACTION)(ToggleControl* self);
typedef VOID(CLASSCALL* TOGGLECONTROLINITIALIZEACTION)(ToggleControl* self);
typedef VOID(CLASSCALL* TOGGLECONTROLDISABLEACTION)(ToggleControl* self);
typedef VOID(CLASSCALL* TOGGLECONTROLTICKACTION)(ToggleControl* self);
typedef U32(CLASSCALL* TOGGLECONTROLACTIONACTION)(ToggleControl* self);
typedef ToggleControl* (CLASSCALL* TOGGLECONTROLRELEASEACTION)(ToggleControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: ButtonControlSelf
typedef struct ToggleControlSelf
{
    TOGGLECONTROLTYPEACTION         Type;
    TOGGLECONTROLINITIALIZEACTION   Initialize;
    TOGGLECONTROLDISABLEACTION      Disable;
    TOGGLECONTROLTICKACTION         Tick;
    TOGGLECONTROLACTIONACTION       Action;
    TOGGLECONTROLRELEASEACTION      Release;
} TOGGLECONTROLSELF, * TOGGLECONTROLSELFPTR;

EXTERN TOGGLECONTROLSELF ToggleControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: ButtonControl
typedef struct ToggleControl
{
    TOGGLECONTROLSELFPTR  Self;
    U8                          IsActive;
    BINASSETPTR                 Asset;
    U32                         Index;
    S32                         Shortcut;
    S32                         X;
    S32                         Y;
    LPCSTR                      Click;
    LPCSTR                      Unclick;
    U8                          IsAction;
    U8                          IsVisible;
    U8                          IsClickable;
    BOOL                        IsContinuous;
    U32                         Ticks;
    U32                         Iteration;
    U32                         Action;
    U32                         ActualAction;
} TOGGLECONTROL, * TOGGLECONTROLPTR;
#pragma pack(pop)

TOGGLECONTROLPTR CLASSCALL ActivateToggleControl(TOGGLECONTROLPTR self, BINASSETPTR asset, CONST U32 indx, CONST U32 action);
VOID CLASSCALL InitializeToggleControl(TOGGLECONTROLPTR self);
U32 CLASSCALL ActionToggleControl(TOGGLECONTROLPTR self);