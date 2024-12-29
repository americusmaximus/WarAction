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

#define MULTI_RESULT_DETAILS_BUTTON_INDEX               0
#define MULTI_RESULT_TOTAL_SCORE_BUTTON_INDEX           1
#define MULTI_RESULT_EXIT_BUTTON_INDEX                  2
#define MULTI_RESULT_BALANCE_BUTTON_INDEX               3
#define MULTI_RESULT_DESTROYED_LOST_BUTTON_INDEX        4
#define MULTI_RESULT_DESTROYED_LOST_BALANCE_LABEL_INDEX 5

struct MultiResultControl;

typedef CONTROLTYPE(CLASSCALL* MULTIRESULTCONTROLTYPEACTION)(MultiResultControl* self);
typedef VOID(CLASSCALL* MULTIRESULTCONTROLINITIALIZEACTION)(MultiResultControl* self);
typedef VOID(CLASSCALL* MULTIRESULTCONTROLDISABLEACTION)(MultiResultControl* self);
typedef VOID(CLASSCALL* MULTIRESULTCONTROLTICKACTION)(MultiResultControl* self);
typedef U32(CLASSCALL* MULTIRESULTCONTROLACTIONACTION)(MultiResultControl* self);
typedef MultiResultControl* (CLASSCALL* MULTIRESULTCONTROLRELEASEACTION)(MultiResultControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: PanelControlSelf
typedef struct ControlType4x15_2Self
{
    MULTIRESULTCONTROLTYPEACTION        Type;
    MULTIRESULTCONTROLINITIALIZEACTION  Initialize;
    MULTIRESULTCONTROLDISABLEACTION     Disable;
    MULTIRESULTCONTROLTICKACTION        Tick;
    MULTIRESULTCONTROLACTIONACTION      Action;
    MULTIRESULTCONTROLRELEASEACTION     Release;
} MULTIRESULTCONTROLSELF, * MULTIRESULTCONTROLSELFPTR;

EXTERN MULTIRESULTCONTROLSELF MultiResultControlSelfState;

typedef enum MultiResultStatisticsKind
{
    MULTIRESULTSTATISTICSKIND_LOSSES        = 0,
    MULTIRESULTSTATISTICSKIND_WINS          = 1,
    MULTIRESULTSTATISTICSKIND_FORCE_DWORD   = 0x7FFFFF
} MULTIRESULTSTATISTICSKIND, * MULTIRESULTSTATISTICSKINDPTR;

typedef enum MultiResultStatisticsType
{
    MULTIRESULTSTATISTICSTYPE_TOTAL         = 0,
    MULTIRESULTSTATISTICSTYPE_DETAIL        = 1,
    MULTIRESULTSTATISTICSTYPE_FORCE_DWORD   = 0x7FFFFF
} MULTIRESULTSTATISTICSTYPE, * MULTIRESULTSTATISTICSTYPEPTR;

#pragma pack(push, 1)
// INHERITANCE: PanelControl
typedef struct MultiResultControl
{
    MULTIRESULTCONTROLSELFPTR   Self;
    U8                          IsActive;
    CONTROLNODEPTR              Nodes;
    SCENECONTROLPTR             Scene;
    MULTIRESULTSTATISTICSKIND   Kind;
    MULTIRESULTSTATISTICSTYPE   Type;
} MULTIRESULTCONTROL, * MULTIRESULTCONTROLPTR;
#pragma pack(pop)

MULTIRESULTCONTROLPTR CLASSCALL ActivateMultiResultControl(MULTIRESULTCONTROLPTR self);
U32 CLASSCALL ActionMultiResultControl(MULTIRESULTCONTROLPTR self);
VOID CLASSCALL ActionMultiResultControlButtons(MULTIRESULTCONTROLPTR self);
VOID CLASSCALL DisableMultiResultControl(MULTIRESULTCONTROLPTR self);
VOID CLASSCALL InitializeMultiResultControl(MULTIRESULTCONTROLPTR self);
VOID CLASSCALL TickMultiResultControl(MULTIRESULTCONTROLPTR self);