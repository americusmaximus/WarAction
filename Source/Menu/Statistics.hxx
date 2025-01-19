/*
Copyright (c) 2024 - 2025 Americus Maximus

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

#include "Game.hxx"
#include "StringList.hxx"

#define INVALID_STATISTICS_ITEM_INDEX   (-1)
#define MAX_STATISTICS_PLAYERS_COUNT      (MAX_PLAYER_COUNT + 1)

#pragma pack(push, 1)
typedef struct Statistics
{
    PLAYER      Players[MAX_STATISTICS_PLAYERS_COUNT];
    CHAR        Name[MAX_FILE_NAME_LENGTH]; // TODO Name
    CHAR        Path[MAX_FILE_NAME_LENGTH]; // TODO Name
} STATISTICS, * STATISTICSPTR;
#pragma pack(pop)

typedef struct StatisticsArea
{
    U32 Ticks;
    U32 Unk01; // TODO
    U32 Unk02; // TODO
    U32 Unk03; // TODO
    U32 Unk04; // TODO
} STATISTICSAREA, * STATISTICSAREAPTR;

typedef struct StatisticsModuleContainer // TODO Better name
{
    STATISTICS          State;                                  // 0x10046478

    U8                  IsActive;                               // 0x10046fa1
    STATISTICSAREA      Areas[MAX_STATISTICS_PLAYERS_COUNT];    // 0x10046fa8
    RECT                Rectangle;                              // 0x100470ac
} STATISTICSMODULECONTAINER, * STATISTICSMODULECONTAINERPTR;

EXTERN STATISTICSMODULECONTAINER StatisticsState;

STATISTICSPTR CLASSCALL ActivateStatistics(STATISTICSPTR self);
VOID CLASSCALL AcquireStatistics(STATISTICSPTR self, LPCSTR name);
VOID CLASSCALL SaveStatistics(STATISTICSPTR self);
U32 CLASSCALL AcquireStatisticsPlayerIndex(STATISTICSPTR self, PLAYERPTR player);

VOID ReleaseStatisticsControlActionRectangleAction(VOID);

LPCSTR AcquireStatisticsFileExtension(CONST CHAR game, CONST GAMEDIFFICULTY difficulty);
VOID CLASSCALL FUN_1000cac0(STATISTICSPTR self, LPCSTR path, STRINGLISTPTR list); // TODDO