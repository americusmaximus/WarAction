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

#define MULTI_SELECT_CREATE_GAME_BUTTON_INDEX   0
#define MULTI_SELECT_JOIN_GAME_BUTTON_INDEX     1
#define MULTI_SELECT_CANCEL_BUTTON_INDEX        2
#define MULTI_SELECT_DELETE_BUTTON_INDEX        3

struct MultiSelectControl;

typedef CONTROLTYPE(CLASSCALL* MULTISELECTCONTROLTYPEACTION)(MultiSelectControl* self);
typedef VOID(CLASSCALL* MULTISELECTCONTROLINITIALIZEACTION)(MultiSelectControl* self);
typedef VOID(CLASSCALL* MULTISELECTCONTROLDISABLEACTION)(MultiSelectControl* self);
typedef VOID(CLASSCALL* MULTISELECTCONTROLTICKACTION)(MultiSelectControl* self);
typedef U32(CLASSCALL* MULTISELECTCONTROLACTIONACTION)(MultiSelectControl* self);
typedef MultiSelectControl* (CLASSCALL* MULTISELECTCONTROLRELEASEACTION)(MultiSelectControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: PanelControlSelf
typedef struct MultiSelectControlSelf
{
    MULTISELECTCONTROLTYPEACTION        Type;
    MULTISELECTCONTROLINITIALIZEACTION  Initialize;
    MULTISELECTCONTROLDISABLEACTION     Disable;
    MULTISELECTCONTROLTICKACTION        Tick;
    MULTISELECTCONTROLACTIONACTION      Action;
    MULTISELECTCONTROLRELEASEACTION     Release;
} MULTISELECTCONTROLSELF, * MULTISELECTCONTROLSELFPTR;

EXTERN MULTISELECTCONTROLSELF MultiSelectControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: PanelControl
typedef struct MultiSelectControl
{
    MULTISELECTCONTROLSELFPTR   Self;
    U8                          IsActive;
    CONTROLNODEPTR              Nodes;
    SCENECONTROLPTR             Scene;
    INPUTCONTROLPTR             Input;
    SCROLLCONTROLPTR            Scroll;
    LISTCONTROLPTR              List;
    STRINGLISTPTR               Items;
    BOOL                        IsMessage;
} MULTISELECTCONTROL, * MULTISELECTCONTROLPTR;
#pragma pack(pop)

MULTISELECTCONTROLPTR CLASSCALL ActivateMultiSelectControl(MULTISELECTCONTROLPTR self);
VOID CLASSCALL InitializeMultiSelectControl(MULTISELECTCONTROLPTR self);
U32 CLASSCALL ActionMultiSelectControl(MULTISELECTCONTROLPTR self);