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

#include "ListControl.hxx"
#include "SceneControl.hxx"
#include "ScrollControl.hxx"

#define LOAD_SAVE_LOAD_BUTTON_INDEX     0
#define LOAD_SAVE_CANCEL_BUTTON_INDEX   1

struct LoadSaveControl;

typedef CONTROLTYPE(CLASSCALL* LOADSAVECONTROLTYPEACTION)(LoadSaveControl* self);
typedef VOID(CLASSCALL* LOADSAVECONTROLINITIALIZEACTION)(LoadSaveControl* self);
typedef VOID(CLASSCALL* LOADSAVECONTROLDISABLEACTION)(LoadSaveControl* self);
typedef VOID(CLASSCALL* LOADSAVECONTROLTICKACTION)(LoadSaveControl* self);
typedef U32(CLASSCALL* LOADSAVECONTROLACTIONACTION)(LoadSaveControl* self);
typedef LoadSaveControl* (CLASSCALL* LOADSAVECONTROLRELEASEACTION)(LoadSaveControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: PanelControlSelf
typedef struct LoadSaveControlSelf
{
    LOADSAVECONTROLTYPEACTION       Type;
    LOADSAVECONTROLINITIALIZEACTION Initialize;
    LOADSAVECONTROLDISABLEACTION    Disable;
    LOADSAVECONTROLTICKACTION       Tick;
    LOADSAVECONTROLACTIONACTION     Action;
    LOADSAVECONTROLRELEASEACTION    Release;
} LOADSAVECONTROLSELF, * LOADSAVECONTROLSELFPTR;

EXTERN LOADSAVECONTROLSELF LoadSaveControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: PanelControl
typedef struct LoadSaveControl
{
    LOADSAVECONTROLSELFPTR  Self;
    U8                      IsActive;
    CONTROLNODEPTR          Nodes;
    SCENECONTROLPTR         Scene;
    SCROLLCONTROLPTR        Scroll;
    LISTCONTROLPTR          List;
    STRINGLISTPTR           Items;
} LOADSAVECONTROL, * LOADSAVECONTROLPTR;
#pragma pack(pop)

LOADSAVECONTROLPTR CLASSCALL ActivateLoadSaveControl(LOADSAVECONTROLPTR self);
U32 CLASSCALL ActionLoadSaveControl(LOADSAVECONTROLPTR self);
VOID CLASSCALL InitializeLoadSaveControl(LOADSAVECONTROLPTR self);