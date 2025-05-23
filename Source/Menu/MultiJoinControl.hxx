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
#include "SceneControl.hxx"

#define MULTI_JOIN_OK_BUTTON_INDEX      0
#define MULTI_JOIN_CANCEL_BUTTON_INDEX  1

struct MultiJoinControl;

typedef CONTROLTYPE(CLASSCALL* MULTIJOINCONTROLTYPEACTION)(MultiJoinControl* self);
typedef VOID(CLASSCALL* MULTIJOINCONTROLINITIALIZEACTION)(MultiJoinControl* self);
typedef VOID(CLASSCALL* MULTIJOINCONTROLDISABLEACTION)(MultiJoinControl* self);
typedef VOID(CLASSCALL* MULTIJOINCONTROLTICKACTION)(MultiJoinControl* self);
typedef U32(CLASSCALL* MULTIJOINCONTROLACTIONACTION)(MultiJoinControl* self);
typedef MultiJoinControl* (CLASSCALL* MULTIJOINCONTROLRELEASEACTION)(MultiJoinControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: PanelControlSelf
typedef struct MultiJoinControlSelf
{
    MULTIJOINCONTROLTYPEACTION          Type;
    MULTIJOINCONTROLINITIALIZEACTION    Initialize;
    MULTIJOINCONTROLDISABLEACTION       Disable;
    MULTIJOINCONTROLTICKACTION          Tick;
    MULTIJOINCONTROLACTIONACTION        Action;
    MULTIJOINCONTROLRELEASEACTION       Release;
} MULTIJOINCONTROLSELF, * MULTIJOINCONTROLSELFPTR;

EXTERN MULTIJOINCONTROLSELF MultiJoinControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: PanelControl
typedef struct MultiJoinControl
{
    MULTIJOINCONTROLSELFPTR Self;
    U8                      IsActive;
    CONTROLNODEPTR          Nodes;
    SCENECONTROLPTR         SceneControl;
    INPUTCONTROLPTR         Input;
} MULTIJOINCONTROL, * MULTIJOINCONTROLPTR;
#pragma pack(pop)

MULTIJOINCONTROLPTR CLASSCALL ActivateMultiJoinControl(MULTIJOINCONTROLPTR self);
LPCSTR CLASSCALL AcquireMultiJoinControlInputValue(MULTIJOINCONTROLPTR self);
U32 CLASSCALL ActionMultiJoinControl(MULTIJOINCONTROLPTR self);
VOID CLASSCALL SelectMultiJoinControlInputValue(MULTIJOINCONTROLPTR self, LPCSTR value);