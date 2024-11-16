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

#include "LabelControl.hxx"
#include "ListControl.hxx"
#include "MapControl.hxx"
#include "ObjectType4x12.hxx"
#include "PanelControl.hxx"
#include "ScrollControl.hxx"

#define MULTI_MAP_OK_BUTTON_INDEX           0
#define MULTI_MAP_CANCEL_BUTTON_INDEX       1

#define MULTI_MAP_GAME_MAPS_BUTTON_INDEX    0
#define MULTI_MAP_SAVED_GAMES_BUTTON_INDEX  1

struct MultiMapControl;

typedef CONTROLTYPE(CLASSCALL* MULTIMAPCONTROLTYPEACTION)(MultiMapControl* self);
typedef VOID(CLASSCALL* MULTIMAPCONTROLINITIALIZEACTION)(MultiMapControl* self);
typedef VOID(CLASSCALL* MULTIMAPCONTROLDISABLEACTION)(MultiMapControl* self);
typedef VOID(CLASSCALL* MULTIMAPCONTROLTICKACTION)(MultiMapControl* self);
typedef U32(CLASSCALL* MULTIMAPCONTROLACTIONACTION)(MultiMapControl* self);
typedef MultiMapControl* (CLASSCALL* MULTIMAPCONTROLRELEASEACTION)(MultiMapControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: SceneControlSelf
typedef struct MultiMapControlSelf
{
    MULTIMAPCONTROLTYPEACTION       Type;
    MULTIMAPCONTROLINITIALIZEACTION Initialize;
    MULTIMAPCONTROLDISABLEACTION    Disable;
    MULTIMAPCONTROLTICKACTION       Tick;
    MULTIMAPCONTROLACTIONACTION     Action;
    MULTIMAPCONTROLRELEASEACTION    Release;
} MULTIMAPCONTROLSELF, * MULTIMAPCONTROLSELFPTR;

EXTERN MULTIMAPCONTROLSELF MultiMapControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: SceneControl
typedef struct MultiMapControl
{
    MULTIMAPCONTROLSELFPTR  Self;
    U8                      IsActive;
    CONTROLNODEPTR          Nodes;
    BINASSETPTR             Background;
    BINASSETPTR             Buttons;
    CONTROLTYPE4X12PTR      Options;
    SCROLLCONTROLPTR        Scroll;
    LISTCONTROLPTR          List;
    STRINGLISTPTR           Items;
    LABELCONTROLPTR         Size;
    LABELCONTROLPTR         Actors;
    MAPCONTROLPTR           Map;
    U32                     Unk12; // TODO
} MULTIMAPCONTROL, * MULTIMAPCONTROLPTR;
#pragma pack(pop)

MULTIMAPCONTROLPTR CLASSCALL ActivateMultiMapControl(MULTIMAPCONTROLPTR self);
VOID CLASSCALL InitializeMultiMapControl(MULTIMAPCONTROLPTR self);
U32 CLASSCALL ActionMultiMapControl(MULTIMAPCONTROLPTR self);
VOID CLASSCALL FUN_1000f340(MULTIMAPCONTROLPTR self); // TODO name