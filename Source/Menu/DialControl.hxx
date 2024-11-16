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
#include "ListControl.hxx"
#include "SceneControl.hxx"
#include "ScrollControl.hxx"
#include "StringList.hxx"

#define DIAL_DIAL_BUTTON_INDEX      0
#define DIAL_CANCEL_BUTTON_INDEX    1
#define DIAL_ADD_BUTTON_INDEX       2
#define DIAL_DELETE_BUTTON_INDEX    3

struct DialControl;

typedef CONTROLTYPE(CLASSCALL* DIALCONTROLTYPEACTION)(DialControl* self);
typedef VOID(CLASSCALL* DIALCONTROLINITIALIZEACTION)(DialControl* self);
typedef VOID(CLASSCALL* DIALCONTROLDISABLEACTION)(DialControl* self);
typedef VOID(CLASSCALL* DIALCONTROLTICKACTION)(DialControl* self);
typedef U32(CLASSCALL* DIALCONTROLACTIONACTION)(DialControl* self);
typedef DialControl* (CLASSCALL* DIALCONTROLRELEASEACTION)(DialControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: PanelControlSelf
typedef struct DialControlSelf
{
    DIALCONTROLTYPEACTION       Type;
    DIALCONTROLINITIALIZEACTION Initialize;
    DIALCONTROLDISABLEACTION    Disable;
    DIALCONTROLTICKACTION       Tick;
    DIALCONTROLACTIONACTION     Action;
    DIALCONTROLRELEASEACTION    Release;
} DIALCONTROLSELF, * DIALCONTROLSELFPTR;

EXTERN DIALCONTROLSELF DialControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: PanelControl
typedef struct DialControl
{
    DIALCONTROLSELFPTR  Self;
    U8                  IsActive;
    CONTROLNODEPTR      Nodes;
    SCENECONTROLPTR     Scene;
    SCROLLCONTROLPTR    Scroll;
    LISTCONTROLPTR      List;
    STRINGLISTPTR       Items;
    INPUTCONTROLPTR     Phone;
    INPUTCONTROLPTR     Name;
} DIALCONTROL, * DIALCONTROLPTR;
#pragma pack(pop)

DIALCONTROLPTR CLASSCALL ActivateDialControl(DIALCONTROLPTR self);
VOID CLASSCALL InitializeDialControl(DIALCONTROLPTR self);
VOID CLASSCALL DisableDialControl(DIALCONTROLPTR self);
U32 CLASSCALL ActionDialControl(DIALCONTROLPTR self);