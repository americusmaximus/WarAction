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
#include "ObjectType2x39.hxx"
#include "SceneControl.hxx"
#include "ScrollControl.hxx"
#include "Settings.hxx"

#include <Game.hxx>

#define SINGLE_ORIGINAL_CAMPAIGNS_BUTTON_INDEX  0
#define SINGLE_SINGLE_MISSIONS_BUTTON_INDEX     1
#define SINGLE_LOAD_GAME_BUTTON_INDEX           2
#define SINGLE_CANCEL_BUTTON_INDEX              3
#define SINGLE_DELETE_BUTTON_INDEX              4
#define SINGLE_ADDON_CAMPAIGNS_BUTTON_INDEX     5


struct SingleControl;

typedef CONTROLTYPE(CLASSCALL* SINGLECONTROLTYPEACTION)(SingleControl* self);
typedef VOID(CLASSCALL* SINGLECONTROLINITIALIZEACTION)(SingleControl* self);
typedef VOID(CLASSCALL* SINGLECONTROLDISABLEACTION)(SingleControl* self);
typedef VOID(CLASSCALL* SINGLECONTROLTICKACTION)(SingleControl* self);
typedef U32(CLASSCALL* SINGLECONTROLACTIONACTION)(SingleControl* self);
typedef SingleControl* (CLASSCALL* SINGLECONTROLRELEASEACTION)(SingleControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: PanelControlSelf
typedef struct SingleControlSelf
{
    SINGLECONTROLTYPEACTION       Type;
    SINGLECONTROLINITIALIZEACTION Initialize;
    SINGLECONTROLDISABLEACTION    Disable;
    SINGLECONTROLTICKACTION       Tick;
    SINGLECONTROLACTIONACTION     Action;
    SINGLECONTROLRELEASEACTION    Release;
} SINGLECONTROLSELF, * SINGLECONTROLSELFPTR;

EXTERN SINGLECONTROLSELF SingleControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: PanelControl
typedef struct SingleControl
{
    SINGLECONTROLSELFPTR    Self;
    U8                      IsActive;
    CONTROLNODEPTR          Nodes;
    SCENECONTROLPTR         Scene;
    INPUTCONTROLPTR         Name;
    SCROLLCONTROLPTR        Scroll;
    LISTCONTROLPTR          Names;
    STRINGLISTPTR           Items;
    CONTROLTYPE2X39PTR      Difficulty;
    SETTINGS                Settings;
    BOOL                    IsMessage;
} SINGLECONTROL, * SINGLECONTROLPTR;
#pragma pack(pop)

SINGLECONTROLPTR CLASSCALL ActivateSingleControl(SINGLECONTROLPTR self);
U32 CLASSCALL ActionSingleControl(SINGLECONTROLPTR self);
VOID CLASSCALL DisableSingleControl(SINGLECONTROLPTR self);
VOID CLASSCALL InitializeSingleControl(SINGLECONTROLPTR self);
VOID CLASSCALL TickSingleControl(SINGLECONTROLPTR self);