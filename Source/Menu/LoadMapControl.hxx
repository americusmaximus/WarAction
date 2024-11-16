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
#include "PanelControl.hxx"
#include "ScrollControl.hxx"
#include "StringList.hxx"

#define LOAD_MAP_OK_BUTTON_INDEX        0
#define LOAD_MAP_CANCEL_BUTTON_INDEX    1

struct LoadMapControl;

typedef CONTROLTYPE(CLASSCALL* LOADMAPCONTROLTYPEACTION)(LoadMapControl* self);
typedef VOID(CLASSCALL* LOADMAPCONTROLINITIALIZEACTION)(LoadMapControl* self);
typedef VOID(CLASSCALL* LOADMAPCONTROLDISABLEACTION)(LoadMapControl* self);
typedef VOID(CLASSCALL* LOADMAPCONTROLTICKACTION)(LoadMapControl* self);
typedef U32(CLASSCALL* LOADMAPCONTROLACTIONACTION)(LoadMapControl* self);
typedef LoadMapControl* (CLASSCALL* LOADMAPCONTROLRELEASEACTION)(LoadMapControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: PanelControlSelf
typedef struct LoadMapControlSelf
{
    LOADMAPCONTROLTYPEACTION        Type;
    LOADMAPCONTROLINITIALIZEACTION  Initialize;
    LOADMAPCONTROLDISABLEACTION     Disable;
    LOADMAPCONTROLTICKACTION        Tick;
    LOADMAPCONTROLACTIONACTION      Action;
    LOADMAPCONTROLRELEASEACTION     Release;
} LOADMAPCONTROLSELF, * LOADMAPCONTROLSELFPTR;

EXTERN LOADMAPCONTROLSELF LoadMapControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: PanelControl
typedef struct LoadMapControl
{
    LOADMAPCONTROLSELFPTR   Self;
    U8                      IsActive;
    CONTROLNODEPTR          Nodes;
    U32                     Unk03; // TODO
    U32                     Unk04; // TODO
    SCROLLCONTROLPTR        Scroll;
    LISTCONTROLPTR          List;
    STRINGLISTPTR           Items;
    LABELCONTROLPTR         Size;
    MAPCONTROLPTR           Map;
    U32                     Unk10; // TODO
} LOADMAPCONTROL, * LOADMAPCONTROLPTR;
#pragma pack(pop)

LOADMAPCONTROLPTR CLASSCALL ActivateLoadMapControl(LOADMAPCONTROLPTR self);
VOID CLASSCALL InitializeLoadMapControl(LOADMAPCONTROLPTR self);
VOID CLASSCALL DisableLoadMapControl(LOADMAPCONTROLPTR self);
U32 CLASSCALL ActionLoadMapControl(LOADMAPCONTROLPTR self);
VOID CLASSCALL FUN_10015620(LOADMAPCONTROLPTR self);// TODO name