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
#include "StatisticsControl.hxx"

#define SINGLE_RESULT_CONTINUE_BUTTON_INDEX 0
#define SINGLE_RESULT_REPLAY_BUTTON_INDEX   1
#define SINGLE_RESULT_EXIT_BUTTON_INDEX     2

struct SingleResultControl;

typedef CONTROLTYPE(CLASSCALL* SINGLERESULTCONTROLTYPEACTION)(SingleResultControl* self);
typedef VOID(CLASSCALL* SINGLERESULTCONTROLINITIALIZEACTION)(SingleResultControl* self);
typedef VOID(CLASSCALL* SINGLERESULTCONTROLDISABLEACTION)(SingleResultControl* self);
typedef VOID(CLASSCALL* SINGLERESULTCONTROLTICKACTION)(SingleResultControl* self);
typedef U32(CLASSCALL* SINGLERESULTCONTROLACTIONACTION)(SingleResultControl* self);
typedef SingleResultControl* (CLASSCALL* SINGLERESULTCONTROLRELEASEACTION)(SingleResultControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: SceneControlSelf
typedef struct SingleSingleResultControlSelf
{
    SINGLERESULTCONTROLTYPEACTION       Type;
    SINGLERESULTCONTROLINITIALIZEACTION Initialize;
    SINGLERESULTCONTROLDISABLEACTION    Disable;
    SINGLERESULTCONTROLTICKACTION       Tick;
    SINGLERESULTCONTROLACTIONACTION     Action;
    SINGLERESULTCONTROLRELEASEACTION    Release;
} SINGLERESULTCONTROLSELF, * SINGLERESULTCONTROLSELFPTR;

EXTERN SINGLERESULTCONTROLSELF SingleResultControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: SceneControl
typedef struct SingleResultControl
{
    SINGLERESULTCONTROLSELFPTR  Self;
    U8                          IsActive;
    CONTROLNODEPTR              Nodes;
    U32                         Unk03; // TODO
    U32                         Unk04; // TODO
    STATISTICSCONTROLPTR        Statistics;
} SINGLERESULTCONTROL, * SINGLERESULTCONTROLPTR;
#pragma pack(pop)

SINGLERESULTCONTROLPTR CLASSCALL ActivateSingleResultControl(SINGLERESULTCONTROLPTR self);
VOID CLASSCALL InitializeSingleResultControl(SINGLERESULTCONTROLPTR self);