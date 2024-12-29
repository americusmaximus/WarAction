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

#include "ToggleControl.hxx"

struct SwapControl;

typedef CONTROLTYPE(CLASSCALL* CONTROLTYPE2X39TYPEACTION)(SwapControl* self);
typedef VOID(CLASSCALL* CONTROLTYPE2X39INITIALIZEACTION)(SwapControl* self);
typedef VOID(CLASSCALL* CONTROLTYPE2X39DISABLEACTION)(SwapControl* self);
typedef VOID(CLASSCALL* CONTROLTYPE2X39TICKACTION)(SwapControl* self);
typedef U32(CLASSCALL* CONTROLTYPE2X39ACTIONACTION)(SwapControl* self);
typedef SwapControl* (CLASSCALL* CONTROLTYPE2X39RELEASEACTION)(SwapControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: ToggleControlSelf
typedef struct SwapControlSelf
{
    CONTROLTYPE2X39TYPEACTION Type;
    CONTROLTYPE2X39INITIALIZEACTION Initialize;
    CONTROLTYPE2X39DISABLEACTION Disable;
    CONTROLTYPE2X39TICKACTION Tick;
    CONTROLTYPE2X39ACTIONACTION Action;
    CONTROLTYPE2X39RELEASEACTION Release;
} SWAPCONTROLSELF, * SWAPCONTROLSELFPTR;

EXTERN SWAPCONTROLSELF SwapControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: ToggleControl
typedef struct SwapControl
{
    SWAPCONTROLSELFPTR  Self;
    U8                  IsActive;
    BINASSETPTR         Asset;
    U32                 Index;
    S32                 Shortcut;
    S32                 X;
    S32                 Y;
    LPCSTR              Click;
    LPCSTR              Unclick;
    U8                  IsAction;
    U8                  IsVisible;
    U8                  IsClickable;
    BOOL                IsContinuous;
    U32                 Ticks;
    U32                 Iteration;
    U32                 Action;
    U32                 ActualAction;
    U8                  IsToggle;
} SWAPCONTROL, * SWAPCONTROLPTR;
#pragma pack(pop)

SWAPCONTROLPTR CLASSCALL ActivateSwapControl(SWAPCONTROLPTR self, BINASSETPTR asset, CONST U32 indx, CONST U32 action);
VOID CLASSCALL InitializeSwapControl(SWAPCONTROLPTR self);
U32 CLASSCALL ActionSwapControl(SWAPCONTROLPTR self);