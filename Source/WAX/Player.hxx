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

#include "Basic.hxx"

#define MAX_PLAYER_NAME_LENGTH  32

typedef enum PlayerStatus
{
    PLAYERSTATUS_NONE           = 0,
    PLAYERSTATUS_SUCCESS        = 1,
    PLAYERSTATUS_FAIL           = 2,
    PLAYERSTATUS_DRAW           = 3,
    PLAYERSTATUS_FORCE_DWORD    = 0x7FFFFFFF
} PLAYERSTATUS, * PLAYERSTATUSPTR;

#pragma pack(push, 1)
typedef struct PlayerStatistics
{
    U32     Tanks;
    U32     Artillery;
    U32     Infantry;
    U32     Planes;
    U32     Vehicles;
} PLAYERSTATISTICS, * PLAYERSTATISTICSPTR;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct Player
{
    CHAR                Name[MAX_PLAYER_NAME_LENGTH];
    U32                 Unk01; // TODO
    U32                 Time;
    PLAYERSTATUS        Status;
    U32                 Index; // TODO Name
    PLAYERSTATISTICS    Wins;
    PLAYERSTATISTICS    Losses;
    U32                 Score;
    U32                 Stars;
} PLAYER, * PLAYERPTR;
#pragma pack(pop)