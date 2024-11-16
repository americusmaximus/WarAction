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

#include "Control.hxx"

struct StatisticsControl;

typedef CONTROLTYPE(CLASSCALL* STATISTICSCONTROLTYPEACTION)(StatisticsControl* self);
typedef VOID(CLASSCALL* STATISTICSCONTROLiNITIALIZEACTION)(StatisticsControl* self);
typedef VOID(CLASSCALL* STATISTICSCONTROLDISABLEACTION)(StatisticsControl* self);
typedef VOID(CLASSCALL* STATISTICSCONTROLTICKACTION)(StatisticsControl* self);
typedef U32(CLASSCALL* STATISTICSCONTROLACTIONACTION)(StatisticsControl* self);
typedef StatisticsControl* (CLASSCALL* STATISTICSCONTROLRELEASEACTION)(StatisticsControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: ControlSelf
typedef struct StatisticsControlSelf
{
    STATISTICSCONTROLTYPEACTION       Type;
    STATISTICSCONTROLiNITIALIZEACTION Initialize;
    STATISTICSCONTROLDISABLEACTION    Disable;
    STATISTICSCONTROLTICKACTION       Tick;
    STATISTICSCONTROLACTIONACTION     Action;
    STATISTICSCONTROLRELEASEACTION    Release;
} STATISTICSCONTROLSELF, * STATISTICSCONTROLSELFPTR;

EXTERN STATISTICSCONTROLSELF StatisticsControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: Control
typedef struct StatisticsControl
{
    STATISTICSCONTROLSELFPTR    Self;
    U8                          IsActive;
    U32                         Unk02; // TODO
    S32                         Index;
} STATISTICSCONTROL, * STATISTICSCONTROLPTR;
#pragma pack(pop)

STATISTICSCONTROLPTR CLASSCALL ActivateStatisticsControl(STATISTICSCONTROLPTR self);
U32 CLASSCALL ActionStatisticsControl(STATISTICSCONTROLPTR self);
VOID CLASSCALL DisableStatisticsControl(STATISTICSCONTROLPTR self);
VOID CLASSCALL InitializeStatisticsControl(STATISTICSCONTROLPTR self);
VOID CLASSCALL TickStatisticsControl(STATISTICSCONTROLPTR self);
VOID CLASSCALL AcquireStatisticsControlStatistics(STATISTICSCONTROLPTR self, LPCSTR path);

VOID AcquireStatisticsControlActionRectangle(CONST U32 indx, LPRECT rect);