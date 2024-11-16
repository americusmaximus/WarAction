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
#include "FontAsset.hxx"

#define MAX_INPUT_CONTROL_TEXT_LENGTH           256
#define MAX_INPUT_CONTROL_VALUE_LENGTH          256

#define MAX_INPUT_CONTROL_RESTRICTION_COUNT     256

struct InputControl;

typedef CONTROLTYPE(CLASSCALL* INPUTCONTROLTYPEACTION)(InputControl* self);
typedef VOID(CLASSCALL* INPUTCONTROLINITIALIZEACTION)(InputControl* self);
typedef VOID(CLASSCALL* INPUTCONTROLDISABLEACTION)(InputControl* self);
typedef VOID(CLASSCALL* INPUTCONTROLTICKACTION)(InputControl* self);
typedef U32(CLASSCALL* INPUTCONTROLACTIONACTION)(InputControl* self);
typedef InputControl* (CLASSCALL* INPUTCONTROLRELEASEACTION)(InputControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: ControlSelf
typedef struct InputControlSelf
{
    INPUTCONTROLTYPEACTION          Type;
    INPUTCONTROLINITIALIZEACTION    Initialize;
    INPUTCONTROLDISABLEACTION       Disable;
    INPUTCONTROLTICKACTION          Tick;
    INPUTCONTROLACTIONACTION        Action;
    INPUTCONTROLRELEASEACTION       Release;
} INPUTCONTROLSELF, * INPUTCONTROLSELFPTR;

EXTERN INPUTCONTROLSELF InputControlSelfState;

typedef struct InputControlAllowItemRange
{
    U32 Min;
    U32 Max;
} INPUTCONTROLALLOWITEMRANGE, * INPUTCONTROLALLOWITEMRANGEPTR;

typedef struct InputControlAllowItems
{
    INPUTCONTROLALLOWITEMRANGE  Items[MAX_INPUT_CONTROL_RESTRICTION_COUNT];
    U32                         Count;
} INPUTCONTROLALLOWITEMS, * INPUTCONTROLALLOWITEMSPTR;

VOID CLASSCALL InitializeInputControlAllowItems(INPUTCONTROLALLOWITEMSPTR self, LPCSTR allowed);
BOOL CLASSCALL IsInputControlAllowItemsAllowed(INPUTCONTROLALLOWITEMSPTR self, CONST U32 action);

#pragma pack(push, 1)
// INHERITANCE: Control
typedef struct InputControl
{
    INPUTCONTROLSELFPTR         Self;
    U8                          IsActive;
    U32                         Action;
    S32                         X;
    S32                         Y;
    S32                         Width;
    S32                         Height;
    S32                         Margin;
    LPSTR                       Value;
    LPSTR                       Text;
    S32                         Length;
    S32                         Input;
    S32                         Capacity;
    FONTASSETPTR                Font;
    LPSTR                       Invalid;
    INPUTCONTROLALLOWITEMS      Allowed;
    U32                         Alignment;
    BOOL                        IsModifiable;
    U8                          IsFocus;
} INPUTCONTROL, * INPUTCONTROLPTR;
#pragma pack(pop)

INPUTCONTROLPTR CLASSCALL ActivateInputControl(INPUTCONTROLPTR self, CONST U32 action, CONST S32 x, CONST S32 y, CONST S32 width, CONST S32 height, FONTASSETPTR font, CONST S32 capacity);
INPUTCONTROLPTR CLASSCALL ReleaseInputControl(INPUTCONTROLPTR self, CONST OBJECTRELEASETYPE mode);
LPCSTR CLASSCALL AcquireInputControlValue(INPUTCONTROLPTR self);
U32 CLASSCALL ActionInputControl(INPUTCONTROLPTR self);
VOID CLASSCALL DisableInputControl(INPUTCONTROLPTR self);
VOID CLASSCALL DisposeInputControl(INPUTCONTROLPTR self);
VOID CLASSCALL InitializeInputControl(INPUTCONTROLPTR self);
VOID CLASSCALL InitializeInputControlAllowItems(INPUTCONTROLPTR self, LPCSTR allowed);
VOID CLASSCALL RemoveInputControlFocus(INPUTCONTROLPTR self);
VOID CLASSCALL SelectInputControlFocus(INPUTCONTROLPTR self);
VOID CLASSCALL SelectInputControlFocus(INPUTCONTROLPTR self, CONST BOOL focus);
VOID CLASSCALL SelectInputControlInvalidValue(INPUTCONTROLPTR self, LPCSTR value);
VOID CLASSCALL SelectInputControlValue(INPUTCONTROLPTR self, LPCSTR value);
VOID CLASSCALL TickInputControl(INPUTCONTROLPTR self);
BOOL CLASSCALL IsInputControlInputAllowed(INPUTCONTROLPTR self, CONST U32 action);