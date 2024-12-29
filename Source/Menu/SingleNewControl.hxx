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

#include "SceneControl.hxx"

#define SINGLE_NEW_ADDON_UK_BUTTON_INDEX            0
#define SINGLE_NEW_ADDON_GERMANY_BUTTON_INDEX       1
#define SINGLE_NEW_ADDON_RUSSIA_BUTTON_INDEX        2
#define SINGLE_NEW_ADDON_CANCEL_BUTTON_INDEX        3
#define SINGLE_NEW_ADDON_USA_BUTTON_INDEX           4

#define SINGLE_NEW_ORIGINAL_ALLIES_BUTTON_INDEX     0
#define SINGLE_NEW_ORIGINAL_GERMANY_BUTTON_INDEX    1
#define SINGLE_NEW_ORIGINAL_RUSSIA_BUTTON_INDEX     2
#define SINGLE_NEW_ORIGINAL_CANCEL_BUTTON_INDEX     3
#define SINGLE_NEW_ORIGINAL_NONE_BUTTON_INDEX       4

struct SingleNewControl;

typedef CONTROLTYPE(CLASSCALL* SINGLENEWCONTROLTYPEACTION)(SingleNewControl* self);
typedef VOID(CLASSCALL* SINGLENEWCONTROLINITIALIZEACTION)(SingleNewControl* self);
typedef VOID(CLASSCALL* SINGLENEWCONTROLDISABLEACTION)(SingleNewControl* self);
typedef VOID(CLASSCALL* SINGLENEWCONTROLTICKACTION)(SingleNewControl* self);
typedef U32(CLASSCALL* SINGLENEWCONTROLACTIONACTION)(SingleNewControl* self);
typedef SingleNewControl* (CLASSCALL* SINGLENEWCONTROLRELEASEACTION)(SingleNewControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: SceneControlSelf
typedef struct SingleNewControlSelf
{
    SINGLENEWCONTROLTYPEACTION        Type;
    SINGLENEWCONTROLINITIALIZEACTION  Initialize;
    SINGLENEWCONTROLDISABLEACTION     Disable;
    SINGLENEWCONTROLTICKACTION        Tick;
    SINGLENEWCONTROLACTIONACTION      Action;
    SINGLENEWCONTROLRELEASEACTION     Release;
} SINGLENEWCONTROLSELF, * SINGLENEWCONTROLSELFPTR;

EXTERN SINGLENEWCONTROLSELF SingleNewControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: SceneControl
typedef struct SingleNewControl
{
    SINGLENEWCONTROLSELFPTR     Self;
    U8                          IsActive;
    CONTROLNODEPTR              Nodes;
    U32                         Unk03; // TODO
    U32                         Unk04; // TODO
    BOOL                        IsAddon;
    S32                         Nation;
} SINGLENEWCONTROL, * SINGLENEWCONTROLPTR;
#pragma pack(pop)

SINGLENEWCONTROLPTR CLASSCALL ActivateSingleNewControl(SINGLENEWCONTROLPTR self, CONST BOOL addon);
BOOL CLASSCALL SaveSingleNewControlState(SINGLENEWCONTROLPTR self);
U32 CLASSCALL ActionSingleNewControl(SINGLENEWCONTROLPTR self);