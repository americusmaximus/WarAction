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

#include "InputControl.hxx"
#include "FontAsset.hxx"

struct TimeControl;

typedef CONTROLTYPE(CLASSCALL* TIMECONTROLTYPEACTION)(TimeControl* self);
typedef VOID(CLASSCALL* TIMECONTROLINITIALIZEACTION)(TimeControl* self);
typedef VOID(CLASSCALL* TIMECONTROLDISABLEACTION)(TimeControl* self);
typedef VOID(CLASSCALL* TIMECONTROLTICKACTION)(TimeControl* self);
typedef U32(CLASSCALL* TIMECONTROLACTIONACTION)(TimeControl* self);
typedef TimeControl* (CLASSCALL* TIMECONTROLRELEASEACTION)(TimeControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: ControlSelf
typedef struct TimeControlSelf
{
    TIMECONTROLTYPEACTION       Type;
    TIMECONTROLINITIALIZEACTION Initialize;
    TIMECONTROLDISABLEACTION    Disable;
    TIMECONTROLTICKACTION       Tick;
    TIMECONTROLACTIONACTION     Action;
    TIMECONTROLRELEASEACTION    Release;
} TIMECONTROLSELF, * TIMECONTROLSELFPTR;

EXTERN TIMECONTROLSELF TimeControlSelfState;

typedef enum TimeControlFocus
{
    TIMECONTROLFOCUS_NONE           = 0,
    TIMECONTROLFOCUS_MINUTES        = 1,
    TIMECONTROLFOCUS_SECONDS        = 2,
    TIMECONTROLFOCUS_FORCE_DWORD    = 0x7FFFFFFF
} TIMECONTROLFOCUS, * TIMECONTROLFOCUSPTR;

#pragma pack(push, 1)
// INHERITANCE: Control
typedef struct TimeControl
{
    TIMECONTROLSELFPTR  Self;
    U8                  IsActive;
    INPUTCONTROLPTR     Minutes;
    INPUTCONTROLPTR     Seconds;
    U32                 Action;
    S32                 X;
    S32                 Y;
    S32                 Width;
    S32                 Height;
    FONTASSETPTR        Font;
} TIMECONTROL, * TIMECONTROLPTR;
#pragma pack(pop)

TIMECONTROLPTR CLASSCALL ActivateTimeControl(TIMECONTROLPTR self, CONST U32 action, CONST S32 x, CONST S32 y, CONST S32 width, CONST S32 height, FONTASSETPTR font);
TIMECONTROLPTR CLASSCALL ReleaseTimeControl(TIMECONTROLPTR self, CONST OBJECTRELEASETYPE mode);
U32 CLASSCALL AcquireTimeControlSeconds(TIMECONTROLPTR self);
U32 CLASSCALL ActionTimeControl(TIMECONTROLPTR self);
VOID CLASSCALL DisableTimeControl(TIMECONTROLPTR self);
VOID CLASSCALL DisposeTimeControl(TIMECONTROLPTR self);
VOID CLASSCALL InitializeTimeControl(TIMECONTROLPTR self);
VOID CLASSCALL SelectTimeControlFocus(TIMECONTROLPTR self, CONST TIMECONTROLFOCUS control);
VOID CLASSCALL SelectTimeControlModifiable(TIMECONTROLPTR self, CONST BOOL modifiable);
VOID CLASSCALL SelectTimeControlValue(TIMECONTROLPTR self, CONST U32 seconds);
VOID CLASSCALL TickTimeControl(TIMECONTROLPTR self);