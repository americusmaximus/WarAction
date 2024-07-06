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

#define MAX_GAME_STATE_PLAYER_COUNT             12
#define MAX_GAME_STATE_PLAYER_NAME_LENGTH       32 /* TODO */
#define MAX_GAME_STATE_SAVE_FILE_NAME_LENGTH    256

#define INVALID_GAME_NATION                     (-1)
#define GAME_NATION_RUSSIAN                     0
#define GAME_NATION_GERMAN                      1
#define GAME_NATION_BRITISH                     2
#define GAME_NATION_AMERICAN                    3

typedef enum GameCommand
{
    GAMECOMMAND_NONE            = 0,
    GAMECOMMAND_VICTORY         = 1,
    GAMECOMMAND_UNKNOWN_2       = 2,
    GAMECOMMAND_UNKNOWN_3       = 3,
    GAMECOMMAND_LOAD_GAME       = 4,
    GAMECOMMAND_QUIT            = 5, // RK_EXITTOOS
    GAMECOMMAND_RESTART         = 6, // RK_RESTART
    GAMECOMMAND_FORCE_DWORD     = 0x7FFFFFFF
} GAMECOMMAND;

typedef enum GameDifficulty
{
    GAMEDIFFICULTY_NONE         = 0,
    GAMEDIFFICULTY_EASY         = 1,
    GAMEDIFFICULTY_MEDIUM       = 2,
    GAMEDIFFICULTY_HARD         = 3,
    GAMEDIFFICULTY_FORCE_DWORD  = 0x7FFFFFFF
} GAMEDIFFICULTY;

typedef struct GamePlayerStateContainer
{
    CHAR    Name[MAX_GAME_STATE_PLAYER_NAME_LENGTH];
    U32     Unk01; // TODO
    U32     Unk02; // TODO
    U32     Unk03; // TODO
    U32     Unk04; // TODO
    U32     Unk05; // TODO
    U32     Unk06; // TODO
    U32     Unk07; // TODO
    U32     Unk08; // TODO
    U32     Unk09; // TODO
    U32     Unk10; // TODO
    U32     Unk11; // TODO
    U32     Unk12; // TODO
    U32     Unk13; // TODO
    U32     Unk14; // TODO
    U32     Unk15; // TODO
    U32     Unk16; // TODO
} GAMEPLAYERSTATECONTAINER, * GAMEPLAYERSTATECONTAINERPTR;

typedef struct GameStateContainer
{
    BOOL                        IsNetwork;
    GAMEDIFFICULTY              Difficulty;
    S32                         Unknown0x0c; // TODO
    S32                         Unknown0x10; // TODO
    S32                         Unknown0x14; // TODO
    S32                         Unknown0x18; // TODO
    S32                         Unknown0x1c; // TODO
    S32                         Unknown0x20; // TODO
    U32                         TurnDelay;
    S32                         Unknown0x28; // TODO
    U32                         Ticks;
    GAMECOMMAND                 Command;
    S32                         NextMap;
    S32                         NextMission;
    GAMEPLAYERSTATECONTAINER    Players[MAX_GAME_STATE_PLAYER_COUNT];
    U32                         Time;
    CHAR                        SaveFile[MAX_GAME_STATE_SAVE_FILE_NAME_LENGTH];
} GAMESTATECONTAINER, * GAMESTATECONTAINERPTR;