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

#include "ChatControl.hxx"
#include "InputControl.hxx"
#include "LabelControl.hxx"
#include "MapControl.hxx"
#include "PanelControl.hxx"
#include "RadioControl.hxx"
#include "SceneControl.hxx"
#include "SwapControl.hxx"
#include "TimeControl.hxx"

#include <Game.hxx>

#define MULTI_START_START_BUTTON_INDEX          0
#define MULTI_START_CANCEL_BUTTON_INDEX         1
#define MULTI_START_INACTIVE_START_BUTTON_INDEX 2
#define MULTI_START_READY_BUTTON_INDEX          3

struct MultiStartControl;

typedef CONTROLTYPE(CLASSCALL* MULTISTARTCONTROLTYPEACTION)(MultiStartControl* self);
typedef VOID(CLASSCALL* MULTISTARTCONTROLINITIALIZEACTION)(MultiStartControl* self);
typedef VOID(CLASSCALL* MULTISTARTCONTROLDISABLEACTION)(MultiStartControl* self);
typedef VOID(CLASSCALL* MULTISTARTCONTROLTICKACTION)(MultiStartControl* self);
typedef U32(CLASSCALL* MULTISTARTCONTROLACTIONACTION)(MultiStartControl* self);
typedef MultiStartControl* (CLASSCALL* MULTISTARTCONTROLRELEASEACTION)(MultiStartControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: PanelControlSelf
typedef struct MultiStartControlSelf
{
    MULTISTARTCONTROLTYPEACTION       Type;
    MULTISTARTCONTROLINITIALIZEACTION Initialize;
    MULTISTARTCONTROLDISABLEACTION    Disable;
    MULTISTARTCONTROLTICKACTION       Tick;
    MULTISTARTCONTROLACTIONACTION     Action;
    MULTISTARTCONTROLRELEASEACTION    Release;
} MULTISTARTCONTROLSELF, * MULTISTARTCONTROLSELFPTR;

EXTERN MULTISTARTCONTROLSELF MultiStartControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: PanelControl
typedef struct MultiStartControl
{
    MULTISTARTCONTROLSELFPTR    Self;
    U8                          IsActive;
    CONTROLNODEPTR              Nodes;
    SCENECONTROLPTR             Scene;
    RADIOCONTROLPTR             Colors; // TODO
    RADIOCONTROLPTR             Team; // TODO
    RADIOCONTROLPTR             Nation; // TODO
    RADIOCONTROLPTR             ObjectType4x12_4; // TODO
    RADIOCONTROLPTR             ObjectType4x12_5; // TODO
    RADIOCONTROLPTR             ObjectType4x12_6; // TODO
    SWAPCONTROLPTR              ObjectType2x39; // TODO
    INPUTCONTROLPTR             Input;
    TIMECONTROLPTR              Inactivity;
    TIMECONTROLPTR              Capture;
    TIMECONTROLPTR              TimeOut;
    LABELCONTROLPTR             Label1; // TODO
    LABELCONTROLPTR             MapSize; // TODO
    LABELCONTROLPTR             MapActors; // TODO
    LABELCONTROLPTR             Players[MAX_PLAYER_COUNT];
    U32                         Unk0x71; // TODO
    MAPCONTROLPTR               Map;
    U32                         Unk0x79; // TODO
    U32                         Unk0x7d; // TODO
    CHATCONTROLPTR              Chat;
} MULTISTARTCONTROL, * MULTISTARTCONTROLPTR;
#pragma pack(pop)

MULTISTARTCONTROLPTR CLASSCALL ActivateMultiStartControl(MULTISTARTCONTROLPTR self);
MULTISTARTCONTROLPTR CLASSCALL ReleaseMultiStartControl(MULTISTARTCONTROLPTR self, CONST OBJECTRELEASETYPE mode);
U32 CLASSCALL ActionMultiStartControl(MULTISTARTCONTROLPTR self);
VOID CLASSCALL DisableMultiStartControl(MULTISTARTCONTROLPTR self);
VOID CLASSCALL DisposeMultiStartControl(MULTISTARTCONTROLPTR self);
VOID CLASSCALL InitializeMultiStartControl(MULTISTARTCONTROLPTR self);
VOID CLASSCALL MultiStartControlUnknown1(MULTISTARTCONTROLPTR self); // TODO
VOID CLASSCALL MultiStartControlUnknown2(MULTISTARTCONTROLPTR self); // TODO
VOID CLASSCALL TickMultiStartControl(MULTISTARTCONTROLPTR self);